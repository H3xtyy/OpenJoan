// The game-side definitions of jpeg_crt.hpp (what the libjpeg compressor uses of the game CRT and of D3DX8's data),
// and the facades through which the older raw-offset jcapi / jcparam copies in lib/d3dx8/objects.cpp and
// lib/crt/string.cpp still reach the typed modules (by the original addresses' names).
#include <cstddef>
#include <cstdint>

#include "jpeg_crt.hpp"
#include "jpeglib.hpp"
#include "../../call.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../original/direct.hpp"

namespace joan::jpeg::crt {

void* malloc(jsize size) { return joan::orig::malloc_(size); }  // 0x007b6c54
void free(void* p) { joan::orig::free_(p); }                     // 0x007b4e1d
const char* getenv(const char* name) { return joan::call::cdecl_call<const char*>(fn::getenv, name); }  // 0x007b7eb3
int sscanf_long_char(const char* text, const char* format, jlong* value, char* ch) {
    return joan::orig::libc::sscanf(text, format, value, ch);  // 0x007b3ec2
}
jsize fwrite(const void* buffer, jsize size, jsize count, void* file) {
    return joan::orig::libc::fwrite(const_cast<void*>(buffer), size, count, file);  // 0x007b7c14
}
int fflush(void* file) { return joan::orig::libc::fflush(file); }  // 0x007b7d4d
// ferror(stream) of VC6's stdio.h is the macro ((stream)->_flag & _IOERR), _IOERR 0x20. The FILE as lib/crt/stdio.cpp
// lays it out (its Iobuf: VC6 _iobuf, natural layout - 0x20 bytes, _flag at +0xc on x86).
namespace {
struct Iobuf {
    char* ptr;
    int cnt;
    char* base;
    std::uint32_t flag;
};
#if UINTPTR_MAX == 0xffffffffu
static_assert(offsetof(Iobuf, flag) == 0xc);
#endif
}  // namespace
bool ferror(const void* file) { return (static_cast<const Iobuf*>(file)->flag & 0x20u) != 0; }
int sprintf_s(char* buffer, const char* format, const char* s) {
    return joan::orig::libc::sprintf(buffer, format, s);  // 0x007b2952
}
int sprintf_i8(char* buffer, const char* format, const int (&i)[8]) {
    return joan::orig::libc::sprintf(buffer, format, i[0], i[1], i[2], i[3], i[4], i[5], i[6], i[7]);
}
void exit_failure() { joan::orig::libc::exit(1u); }  // 0x007b47fc
// jpeg_std_message_table (D3DX8 .rdata 0x009e1c68): IJG's own messages, in message-code order, null-terminated.
const char* const* message_table() {
    static const char* const kMessages[] = {
        "Bogus message code %d",
        "Sorry, there are legal restrictions on arithmetic coding",
        "ALIGN_TYPE is wrong, please fix",
        "MAX_ALLOC_CHUNK is wrong, please fix",
        "Bogus buffer control mode",
        "Invalid component ID %d in SOS",
        "IDCT output block size %d not supported",
        "Bogus input colorspace",
        "Bogus JPEG colorspace",
        "Bogus marker length",
        "Wrong JPEG library version: library is %d, caller expects %d",
        "Sampling factors too large for interleaved scan",
        "Invalid memory pool code %d",
        "Unsupported JPEG data precision %d",
        "Invalid progressive parameters Ss=%d Se=%d Ah=%d Al=%d",
        "Invalid progressive parameters at scan script entry %d",
        "Bogus sampling factors",
        "Invalid scan script at entry %d",
        "Improper call to JPEG library in state %d",
        "JPEG parameter struct mismatch: library thinks size is %u, caller expects %u",
        "Bogus virtual array access",
        "Buffer passed to JPEG library is too small",
        "Suspension not allowed here",
        "CCIR601 sampling not implemented yet",
        "Too many color components: %d, max %d",
        "Unsupported color conversion request",
        "Bogus DAC index %d",
        "Bogus DAC value 0x%x",
        "Bogus DHT counts",
        "Bogus DHT index %d",
        "Bogus DQT index %d",
        "Empty JPEG image (DNL not supported)",
        "Read from EMS failed",
        "Write to EMS failed",
        "Didn't expect more than one scan",
        "Input file read error",
        "Output file write error --- out of disk space?",
        "Fractional sampling not implemented yet",
        "Huffman code size table overflow",
        "Missing Huffman code table entry",
        "Maximum supported image dimension is %u pixels",
        "Empty input file",
        "Premature end of input file",
        "Cannot transcode due to multiple use of quantization table %d",
        "Scan script does not transmit all data",
        "Invalid color quantization mode change",
        "Not implemented yet",
        "Requested feature was omitted at compile time",
        "Backing store not supported",
        "Huffman table 0x%02x was not defined",
        "JPEG datastream contains no image",
        "Quantization table 0x%02x was not defined",
        "Not a JPEG file: starts with 0x%02x 0x%02x",
        "Insufficient memory (case %d)",
        "Cannot quantize more than %d color components",
        "Cannot quantize to fewer than %d colors",
        "Cannot quantize to more than %d colors",
        "Invalid JPEG file structure: two SOF markers",
        "Invalid JPEG file structure: missing SOS marker",
        "Unsupported JPEG process: SOF type 0x%02x",
        "Invalid JPEG file structure: two SOI markers",
        "Invalid JPEG file structure: SOS before SOF",
        "Failed to create temporary file %s",
        "Read failed on temporary file",
        "Seek failed on temporary file",
        "Write failed on temporary file --- out of disk space?",
        "Application transferred too few scanlines",
        "Unsupported marker type 0x%02x",
        "Virtual array controller messed up",
        "Image too wide for this implementation",
        "Read from XMS failed",
        "Write to XMS failed",
        "Copyright (C) 1996, Thomas G. Lane",
        "6a  7-Feb-96",
        "Caution: quantization tables are too coarse for baseline JPEG",
        "Adobe APP14 marker: version %d, flags 0x%04x 0x%04x, transform %d",
        "Unknown APP0 marker (not JFIF), length %u",
        "Unknown APP14 marker (not Adobe), length %u",
        "Define Arithmetic Table 0x%02x: 0x%02x",
        "Define Huffman Table 0x%02x",
        "Define Quantization Table %d  precision %d",
        "Define Restart Interval %u",
        "Freed EMS handle %u",
        "Obtained EMS handle %u",
        "End Of Image",
        "        %3d %3d %3d %3d %3d %3d %3d %3d",
        "JFIF APP0 marker, density %dx%d  %d",
        "Warning: thumbnail image size does not match data length %u",
        "Unknown JFIF minor revision number %d.%02d",
        "    with %d x %d thumbnail image",
        "Skipping marker 0x%02x, length %u",
        "Unexpected marker 0x%02x",
        "        %4u %4u %4u %4u %4u %4u %4u %4u",
        "Quantizing to %d = %d*%d*%d colors",
        "Quantizing to %d colors",
        "Selected %d colors for quantization",
        "At marker 0x%02x, recovery action %d",
        "RST%d",
        "Smoothing not supported with nonstandard sampling ratios",
        "Start Of Frame 0x%02x: width=%u, height=%u, components=%d",
        "    Component %d: %dhx%dv q=%d",
        "Start of Image",
        "Start Of Scan: %d components",
        "    Component %d: dc=%d ac=%d",
        "  Ss=%d, Se=%d, Ah=%d, Al=%d",
        "Closed temporary file %s",
        "Opened temporary file %s",
        "Unrecognized component IDs %d %d %d, assuming YCbCr",
        "Freed XMS handle %u",
        "Obtained XMS handle %u",
        "Unknown Adobe color transform code %d",
        "Inconsistent progression sequence for component %d coefficient %d",
        "Corrupt JPEG data: %u extraneous bytes before marker 0x%02x",
        "Corrupt JPEG data: premature end of data segment",
        "Corrupt JPEG data: bad Huffman code",
        "Warning: unknown JFIF revision number %d.%02d",
        "Premature end of JPEG file",
        "Corrupt JPEG data: found marker 0x%02x instead of RST%d",
        "Invalid SOS parameters for sequential JPEG",
        "Application transferred too many scanlines",
        nullptr,
    };
    return kMessages;
}

}  // namespace joan::jpeg::crt

// ---------------------------------------- facades of the typed modules (callers: lib/d3dx8/objects.cpp,
// lib/crt/string.cpp - raw-offset copies of jcapi / jcparam that no game code reaches any more)
namespace {
joan::jpeg::j_compress_ptr cinfo(void* p) { return static_cast<joan::jpeg::j_compress_ptr>(p); }
}  // namespace
auto joan::orig::jinit_memory_mgr(void* a0) -> void { joan::jpeg::jinit_memory_mgr(cinfo(a0)); }  // 0x7aab10
auto joan::orig::jinit_compress_master(void* a0) -> void { joan::jpeg::jinit_compress_master(cinfo(a0)); }  // 0x7ac000
auto joan::orig::jpeg_fns::jpeg_abort(void* a0) -> void { joan::jpeg::jpeg_abort(cinfo(a0)); }  // 0x7888f7
auto joan::orig::jpeg_fns::jpeg_destroy(void* a0) -> void { joan::jpeg::jpeg_destroy(cinfo(a0)); }  // 0x788919
auto joan::orig::jpeg_fns::jpeg_alloc_quant_table(void* a0) -> void* {  // 0x788934
    return joan::jpeg::jpeg_alloc_quant_table(cinfo(a0));
}
auto joan::orig::jpeg_fns::jpeg_alloc_huff_table(void* a0) -> void* {  // 0x788950
    return joan::jpeg::jpeg_alloc_huff_table(cinfo(a0));
}
