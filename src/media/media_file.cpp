// See media_file.hpp.
#include "media_file.hpp"

#include <cstring>
#include <deque>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace joan::media {

namespace {

constexpr AVRational kHns{1, 10000000};  // 100 ns

struct Decoder {
    int index = -1;  // AVStream index
    StreamInfo info;
    AVCodecContext* ctx = nullptr;
    SwsContext* sws = nullptr;
    SwrContext* swr = nullptr;
    AVRational time_base{1, 1000};
    std::int64_t start = 0;      // the stream's start time (time_base), subtracted from every timestamp
    std::int64_t next_time = 0;  // 100 ns, for frames without a timestamp
    bool drained = false;
};

}  // namespace

struct MediaFile::Impl {
    AVFormatContext* fmt = nullptr;
    std::vector<Decoder> decoders;
    std::vector<StreamInfo> infos;
    std::vector<std::deque<Sample>> queues;  // per decoder: decoded, not yet delivered
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
    bool eof = false;
    bool started = false;  // a sample was delivered (a seek to 0 before that is a no-op)
    std::int64_t skip_before = 0;  // after a seek: samples ending before this time (100 ns) are decoded but dropped
    std::string error;

    ~Impl() { close(); }

    void close() {
        for (Decoder& dec : decoders) {
            avcodec_free_context(&dec.ctx);
            sws_freeContext(dec.sws);
            swr_free(&dec.swr);
        }
        decoders.clear();
        infos.clear();
        queues.clear();
        av_packet_free(&packet);
        av_frame_free(&frame);
        avformat_close_input(&fmt);
        if (avio) {
            av_freep(&avio->buffer);
            avio_context_free(&avio);
        }
        memory.reset();
        memory_pos = 0;
    }

    // open_memory: FFmpeg reads the file from `memory` through this I/O context instead of opening it.
    std::shared_ptr<const std::vector<std::uint8_t>> memory;
    std::int64_t memory_pos = 0;
    AVIOContext* avio = nullptr;

    static int memory_read(void* opaque, std::uint8_t* buf, int size) {
        Impl& m = *static_cast<Impl*>(opaque);
        const std::int64_t left = static_cast<std::int64_t>(m.memory->size()) - m.memory_pos;
        if (left <= 0) return AVERROR_EOF;
        const int n = static_cast<int>(left < size ? left : size);
        std::memcpy(buf, m.memory->data() + m.memory_pos, static_cast<std::size_t>(n));
        m.memory_pos += n;
        return n;
    }

    static std::int64_t memory_seek(void* opaque, std::int64_t offset, int whence) {
        Impl& m = *static_cast<Impl*>(opaque);
        const auto size = static_cast<std::int64_t>(m.memory->size());
        if (whence & AVSEEK_SIZE) return size;
        std::int64_t pos;
        switch (whence & ~AVSEEK_FORCE) {
        case SEEK_SET: pos = offset; break;
        case SEEK_CUR: pos = m.memory_pos + offset; break;
        case SEEK_END: pos = size + offset; break;
        default: return AVERROR(EINVAL);
        }
        if (pos < 0 || pos > size) return AVERROR(EINVAL);
        m.memory_pos = pos;
        return pos;
    }

    bool fail(const char* what, int err = 0) {
        error = what;
        if (err < 0) {
            char buf[AV_ERROR_MAX_STRING_SIZE] = {};
            av_strerror(err, buf, sizeof buf);
            error += ": ";
            error += buf;
        }
        return false;
    }

    Decoder* decoder_for(int stream_index) {
        for (Decoder& dec : decoders)
            if (dec.index == stream_index) return &dec;
        return nullptr;
    }

    std::int64_t to_hns(const Decoder& dec, std::int64_t ts) const {
        const std::int64_t t = av_rescale_q(ts - dec.start, dec.time_base, kHns);
        return t < 0 ? 0 : t;
    }

    void convert_video(Decoder& dec) {
        const int w = dec.info.width, h = dec.info.height;
        dec.sws = sws_getCachedContext(dec.sws, frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
                                       w, h, AV_PIX_FMT_RGB565LE, SWS_BILINEAR, nullptr, nullptr, nullptr);
        if (!dec.sws) return;
        Sample s;
        s.stream = dec.info.number;
        s.data.resize(static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 2u);
        // Bottom-up rows (a DIB, as Windows Media delivers RGB): start at the last row, negative stride.
        std::uint8_t* dst[4] = {s.data.data() + static_cast<std::size_t>(h - 1) * static_cast<std::size_t>(w) * 2u,
                                nullptr, nullptr, nullptr};
        const int stride[4] = {-w * 2, 0, 0, 0};
        sws_scale(dec.sws, frame->data, frame->linesize, 0, frame->height, dst, stride);
        const std::int64_t ts = frame->best_effort_timestamp;
        s.time = ts != AV_NOPTS_VALUE ? to_hns(dec, ts) : dec.next_time;
        s.duration = dec.info.frame_duration;
        dec.next_time = s.time + s.duration;
        queues[static_cast<std::size_t>(&dec - decoders.data())].push_back(std::move(s));
    }

    void convert_audio(Decoder& dec) {
        if (!dec.swr) {
            AVChannelLayout out{};
            av_channel_layout_default(&out, dec.info.channels);
            if (swr_alloc_set_opts2(&dec.swr, &out, AV_SAMPLE_FMT_S16, dec.info.rate, &frame->ch_layout,
                                    static_cast<AVSampleFormat>(frame->format), frame->sample_rate, 0, nullptr) < 0 ||
                swr_init(dec.swr) < 0) {
                swr_free(&dec.swr);
                av_channel_layout_uninit(&out);
                return;
            }
            av_channel_layout_uninit(&out);
        }
        const int max_out = swr_get_out_samples(dec.swr, frame->nb_samples);
        if (max_out <= 0) return;
        Sample s;
        s.stream = dec.info.number;
        s.data.resize(static_cast<std::size_t>(max_out) * static_cast<std::size_t>(dec.info.channels) * 2u);
        std::uint8_t* out[1] = {s.data.data()};
        const int n = swr_convert(dec.swr, out, max_out, const_cast<const std::uint8_t**>(frame->extended_data),
                                  frame->nb_samples);
        if (n <= 0) return;
        const std::int64_t ts = frame->best_effort_timestamp;
        const std::int64_t start = ts != AV_NOPTS_VALUE ? to_hns(dec, ts) : dec.next_time;
        // Delivered in pieces of at most kAudioChunk sample frames (~46 ms): a WMA v2 packet decodes to up to 16384
        // frames (371 ms), and the game's movie loop, which reads until the last audio sample's start time passes its
        // clock, then read every video frame up to the next audio packet in one update and showed only the first:
        // the picture stood still for ~0.3 s and jumped (Windows Media hands out small audio samples).
        constexpr int kAudioChunk = 2048;
        const std::size_t frame_bytes = static_cast<std::size_t>(dec.info.channels) * 2u;
        auto& q = queues[static_cast<std::size_t>(&dec - decoders.data())];
        for (int at = 0; at < n; at += kAudioChunk) {
            const int count = n - at < kAudioChunk ? n - at : kAudioChunk;
            Sample piece;
            piece.stream = dec.info.number;
            piece.data.assign(s.data.begin() + static_cast<std::ptrdiff_t>(static_cast<std::size_t>(at) * frame_bytes),
                              s.data.begin() + static_cast<std::ptrdiff_t>(static_cast<std::size_t>(at + count) * frame_bytes));
            piece.time = start + static_cast<std::int64_t>(at) * 10000000 / dec.info.rate;
            piece.duration = static_cast<std::int64_t>(count) * 10000000 / dec.info.rate;
            q.push_back(std::move(piece));
        }
        dec.next_time = start + static_cast<std::int64_t>(n) * 10000000 / dec.info.rate;
    }

    void receive(Decoder& dec) {
        for (;;) {
            const int r = avcodec_receive_frame(dec.ctx, frame);
            if (r == AVERROR_EOF) {
                dec.drained = true;
                return;
            }
            if (r < 0) return;  // EAGAIN or a decoding error: next packet
            if (dec.info.video)
                convert_video(dec);
            else
                convert_audio(dec);
            av_frame_unref(frame);
        }
    }

    // Samples go out in time order across the streams, as Windows Media's synchronous reader delivers them: an ASF
    // file stores video up to ~3 s ahead of the audio of the same time (enlight.wmv: 39 frames before the first
    // audio packet), and the game's movie loop reads until both streams reach the clock, showing only the first video
    // frame it reads per update - in file order it skipped frames (stutter) and ran ahead of the sound.
    // Every stream that has not ended needs a decoded sample queued before the earliest one can be chosen.
    // The audio must also be decoded kAudioLead past the earliest queued video frame, or the lead in earliest()
    // cannot take effect (the audio packets of a time come ~3 s later in the file than its video).
    static constexpr std::int64_t kAudioLead = 2000000;  // 200 ms in 100 ns
    bool ready() const {
        std::int64_t video_front = -1;
        for (std::size_t i = 0; i < decoders.size(); ++i) {
            if (queues[i].empty() && !decoders[i].drained) return false;
            if (decoders[i].info.video && !queues[i].empty() &&
                (video_front < 0 || queues[i].front().time < video_front))
                video_front = queues[i].front().time;
        }
        if (video_front >= 0)
            for (std::size_t i = 0; i < decoders.size(); ++i)
                if (!decoders[i].info.video && !decoders[i].drained &&
                    (queues[i].empty() || queues[i].back().time < video_front + kAudioLead))
                    return false;
        return true;
    }
    bool fill() {
        while (!ready()) {
            if (eof) {
                bool any = false;
                for (Decoder& dec : decoders) {
                    if (dec.drained) continue;
                    avcodec_send_packet(dec.ctx, nullptr);
                    receive(dec);
                    dec.drained = true;
                    any = true;
                }
                if (!any) break;
                continue;
            }
            const int r = av_read_frame(fmt, packet);
            if (r < 0) {
                eof = true;
                continue;
            }
            if (Decoder* dec = decoder_for(packet->stream_index)) {
                if (avcodec_send_packet(dec->ctx, packet) >= 0) receive(*dec);
            }
            av_packet_unref(packet);
        }
        for (const auto& q : queues)
            if (!q.empty()) return true;
        return false;
    }

    /// The next sample to hand out: the earliest, with audio counted kAudioLead early. The game's movie loop
    /// (Video::update 0x00685a80) draws the frame only when the last sample it read in an update was video; with the
    /// audio exactly interleaved by time, an update whose video was current but whose last audio piece had just
    /// fallen behind the clock read one audio piece only and showed no picture (flashing; [Video] Trace at ~165
    /// updates/s: "reads 1 frames 0 last 2 -> 1"). Windows Media hands the audio out ahead (~93 ms pieces, ~0.15 s
    /// before the video of the same time): every audio piece is then read together with an earlier video frame.
    std::deque<Sample>* earliest() {
        std::deque<Sample>* best = nullptr;
        std::int64_t best_key = 0;
        for (std::size_t i = 0; i < queues.size(); ++i) {
            auto& q = queues[i];
            if (q.empty()) continue;
            const std::int64_t key = q.front().time - (decoders[i].info.video ? 0 : kAudioLead);
            if (!best || key < best_key) {
                best = &q;
                best_key = key;
            }
        }
        return best;
    }
};

MediaFile::MediaFile() : d_(std::make_unique<Impl>()) {}
MediaFile::~MediaFile() = default;

const std::vector<StreamInfo>& MediaFile::streams() const { return d_->infos; }
const std::string& MediaFile::error() const { return d_->error; }

bool MediaFile::open(const std::string& utf8_path) {
    Impl& m = *d_;
    m.close();
    m.error.clear();
    m.eof = false;
    m.started = false;
    m.skip_before = 0;
    const int r = avformat_open_input(&m.fmt, utf8_path.c_str(), nullptr, nullptr);
    if (r < 0) return m.fail("cannot open the file", r);
    return open_streams();
}

bool MediaFile::open_memory(std::shared_ptr<const std::vector<std::uint8_t>> bytes, const std::string& name) {
    Impl& m = *d_;
    m.close();
    m.error.clear();
    m.eof = false;
    m.started = false;
    m.skip_before = 0;
    if (!bytes || bytes->empty()) return m.fail("no file data");
    m.memory = std::move(bytes);
    constexpr int kBufferBytes = 64 * 1024;
    auto* const buffer = static_cast<std::uint8_t*>(av_malloc(kBufferBytes));
    if (!buffer) return m.fail("out of memory");
    m.avio = avio_alloc_context(buffer, kBufferBytes, 0, &m, &Impl::memory_read, nullptr, &Impl::memory_seek);
    if (!m.avio) {
        av_free(buffer);
        return m.fail("out of memory");
    }
    m.fmt = avformat_alloc_context();
    if (!m.fmt) return m.fail("out of memory");
    m.fmt->pb = m.avio;
    m.fmt->flags |= AVFMT_FLAG_CUSTOM_IO;
    const int r = avformat_open_input(&m.fmt, name.c_str(), nullptr, nullptr);  // frees fmt on failure
    if (r < 0) return m.fail("cannot open the file", r);
    return open_streams();
}

bool MediaFile::open_streams() {
    Impl& m = *d_;
    int r = avformat_find_stream_info(m.fmt, nullptr);
    if (r < 0) return m.fail("cannot read the stream headers", r);
    for (unsigned i = 0; i < m.fmt->nb_streams; ++i) {
        AVStream* st = m.fmt->streams[i];
        const AVCodecParameters* par = st->codecpar;
        if (par->codec_type != AVMEDIA_TYPE_VIDEO && par->codec_type != AVMEDIA_TYPE_AUDIO) continue;
        const AVCodec* codec = avcodec_find_decoder(par->codec_id);
        if (!codec) continue;  // a stream the game cannot use without a decoder either
        Decoder dec;
        dec.index = static_cast<int>(i);
        dec.ctx = avcodec_alloc_context3(codec);
        if (!dec.ctx || avcodec_parameters_to_context(dec.ctx, par) < 0 || avcodec_open2(dec.ctx, codec, nullptr) < 0) {
            avcodec_free_context(&dec.ctx);
            continue;
        }
        dec.time_base = st->time_base;
        dec.start = st->start_time != AV_NOPTS_VALUE ? st->start_time : 0;
        // ASF stream numbers (1..127) are the stream ids; anything else gets its index + 1.
        dec.info.number = static_cast<std::uint16_t>(st->id > 0 && st->id < 0x10000 ? st->id : static_cast<int>(i) + 1);
        dec.info.video = par->codec_type == AVMEDIA_TYPE_VIDEO;
        if (dec.info.video) {
            dec.info.width = par->width;
            dec.info.height = par->height;
            const AVRational fr = st->avg_frame_rate.num > 0 ? st->avg_frame_rate : st->r_frame_rate;
            dec.info.frame_duration = fr.num > 0 ? av_rescale_q(1, AVRational{fr.den, fr.num}, kHns) : 333333;
        } else {
            dec.info.channels = par->ch_layout.nb_channels > 0 ? par->ch_layout.nb_channels : 2;
            dec.info.rate = par->sample_rate > 0 ? par->sample_rate : 44100;
        }
        m.infos.push_back(dec.info);
        m.decoders.push_back(dec);
        m.queues.emplace_back();
    }
    if (m.decoders.empty()) return m.fail("no decodable audio or video stream");
    m.packet = av_packet_alloc();
    m.frame = av_frame_alloc();
    return true;
}

bool MediaFile::seek(std::int64_t time) {
    Impl& m = *d_;
    if (!m.fmt) return false;
    if (time <= 0 && !m.started) return true;  // already at the start (the game's SetRange(0, 0) after Open)
    const std::int64_t base = m.fmt->start_time != AV_NOPTS_VALUE ? m.fmt->start_time : 0;
    const std::int64_t ts = base + av_rescale_q(time < 0 ? 0 : time, kHns, AV_TIME_BASE_Q);
    if (av_seek_frame(m.fmt, -1, ts, AVSEEK_FLAG_BACKWARD) < 0) return false;
    for (Decoder& dec : m.decoders) {
        avcodec_flush_buffers(dec.ctx);
        dec.drained = false;
        dec.next_time = time;
    }
    for (auto& q : m.queues) q.clear();
    m.skip_before = time;
    m.eof = false;
    m.started = false;
    return true;
}

bool MediaFile::next(Sample& out) {
    Impl& m = *d_;
    // A seek lands on the key frame / packet before the target; like Windows Media's SetRange, delivery starts at
    // the target: what ends before it is decoded (the frames after a key frame need it) and dropped.
    for (;;) {
        if (!m.fmt || !m.fill()) return false;
        std::deque<Sample>* q = m.earliest();
        if (!q) return false;
        out = std::move(q->front());
        q->pop_front();
        if (out.time + out.duration > m.skip_before) break;
    }
    m.started = true;
    return true;
}

}  // namespace joan::media
