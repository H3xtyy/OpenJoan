// A movie / music file decoded with FFmpeg into the sample formats the game asks Windows Media for (phase C3,
// docs/RUNTIME.md): video as RGB565 frames (bottom-up rows, as a DIB), audio as 16-bit interleaved PCM at the
// file's own rate and channel count, delivered in file order with times in 100 ns units like IWMSyncReader.
// Portable (no Windows types): media/wm_reader.cpp puts the game's Windows Media interfaces on top of it.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// Movie and music decoding with FFmpeg, presented in the sample formats the game asks Windows Media for.
namespace joan::media {

/// What MediaFile::streams() reports for one decodable stream of an opened file.
struct StreamInfo {
    std::uint16_t number = 0;  ///< the file's stream number (ASF: 1..127), what the game passes around
    bool video = false;        ///< otherwise audio
    // video
    int width = 0 /**< video: frame width in pixels */, height = 0 /**< video: frame height in pixels */;
    std::int64_t frame_duration = 0;  ///< 100 ns
    // audio (always 16-bit PCM)
    int channels = 0 /**< audio: channel count */, rate = 0 /**< audio: sample rate in Hz */;
};

/// One decoded video frame or audio chunk as returned by MediaFile::next().
struct Sample {
    std::uint16_t stream = 0;     ///< StreamInfo::number
    std::int64_t time = 0;        ///< 100 ns from the start of the file
    std::int64_t duration = 0;    ///< 100 ns
    std::vector<std::uint8_t> data;  ///< Video: one RGB565 frame, bottom-up rows. Audio: 16-bit interleaved PCM.
};

/// A movie or music file opened with FFmpeg and read sample by sample. Not copyable. Used by media/wm_reader.cpp; not
/// thread-safe (one reader per object). Decoder state is private (Impl).
class MediaFile {
public:
    MediaFile();
    ~MediaFile();
    MediaFile(const MediaFile&) = delete;
    MediaFile& operator=(const MediaFile&) = delete;

    /// Opens a file (UTF-8 path). False (with error()) when it cannot be read or has no decodable stream.
    bool open(const std::string& utf8_path);
    /// The same for a file already in memory (kept alive while open); `name` only for FFmpeg's messages.
    bool open_memory(std::shared_ptr<const std::vector<std::uint8_t>> bytes, const std::string& name);
    /// The streams that can be delivered, in the file's order; valid until the file is closed. Empty before a
    /// successful open.
    const std::vector<StreamInfo>& streams() const;
    /// Restarts delivery at `time` (100 ns; the last key frame at or before it for video).
    bool seek(std::int64_t time);
    /// The next decoded sample of any stream; false at the end of the file.
    bool next(Sample& out);
    /// Text of the last failure of open(), open_memory() or seek(); empty when nothing failed.
    const std::string& error() const;

private:
    bool open_streams();  // after avformat_open_input: the decoders

    struct Impl;
    std::unique_ptr<Impl> d_;
};

}  // namespace joan::media
