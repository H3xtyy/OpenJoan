// Original library sources (re/attribution.csv original_file, under lib/): jpeg/jcdctmgr.c, jpeg/jchuff.c,
//   jpeg/jcmarker.c, jpeg/jcphuff.c, jpeg/jfdctflt.c, jpeg/jfdctfst.c, jpeg/jfdctint.c, jpeg/jutils.c,
//   d3dx8/jpeg/jcomapi.c.
//
// IJG libjpeg 6b compressor internals, part 2 (the game's own /Zp1 copy, 1-byte `boolean`; its only user is the
// screenshot writer 0x005a9040 through compress_api.cpp), typed C++ over the structures of jpeglib.hpp (packed like
// the original on x86, natural elsewhere). Module "lib/b115_06". Every function below is also registered at its
// original address (entry patches; no thunks in library code). The four jinit_* functions install our methods
// into cinfo. Booleans are 1 byte.
//
//   jcmarker.c (my_marker_writer = 0x20 bytes on x86: 7 methods + last_restart_interval)
//     0x007ab730 jinit_marker_writer   0x007ab790 write_marker_header   0x007ab7d0 emit_marker
//     0x007ab7f0 emit_byte             0x007ab830 emit_2bytes           0x007ab860 write_marker_byte (= emit_byte)
//     0x007ab870 write_file_header     0x007ab8c0 emit_jfif_app0        0x007ab970 emit_adobe_app14
//     0x007aba10 write_frame_header (TRACEMS JTRC_16BIT_TABLES)         0x007abb10 emit_dqt -> int prec
//     0x007abbe0 emit_sof              0x007abca0 write_scan_header     0x007abd80 emit_dht
//     0x007abe40 emit_dac (empty: arithmetic coding not compiled in)    0x007abe50 emit_dri
//     0x007abe80 emit_sos              0x007abf60 write_file_trailer    0x007abf80 write_tables_only
//   jutils.c
//     0x007ac150 jdiv_round_up  0x007ac160 jround_up  0x007ac180 jcopy_sample_rows  0x007ac1d0 jcopy_block_row (no
//     caller in joan.exe)  0x007ac200 jzero_far (the copies keep the original rep movsd + rep movsb split, so
//     overlapping buffers give the same bytes)
//   jchuff.c (huff_entropy_encoder = 0x6c bytes on x86; working_state on the stack)
//     0x007acba0 jpeg_make_c_derived_tbl  0x007acda0 jpeg_gen_optimal_table  0x007acfc0 jinit_huff_encoder
//     0x007ad000 start_pass_huff  0x007ad170 encode_mcu_huff  0x007ad2a0 encode_one_block  0x007ad430 emit_bits
//     0x007ad500 dump_buffer  0x007ad530 emit_restart  0x007ad5c0 flush_bits  0x007ad5f0 finish_pass_huff
//     0x007ad670 encode_mcu_gather  0x007ad740 htest_one_block  0x007ad820 finish_pass_gather
//   jcphuff.c (phuff_entropy_encoder = 0x69 bytes on x86, packed)
//     0x007ad910 jinit_phuff_encoder  0x007ad950 start_pass_phuff  0x007adad0 encode_mcu_DC_first
//     0x007adf20 encode_mcu_AC_first  0x007ae0e0 encode_mcu_DC_refine  0x007ae1a0 encode_mcu_AC_refine
//     0x007adc20 emit_bits  0x007add00 dump_buffer  0x007add40 emit_symbol  0x007add90 emit_restart
//     0x007ade40 flush_bits  0x007ade60 emit_eobrun  0x007adee0 emit_buffered_bits  0x007ae3b0 finish_pass_phuff
//     0x007ae3f0 finish_pass_gather_phuff
//   jcdctmgr.c (my_fdct_controller = 0x30 bytes on x86)
//     0x007ae4e0 jinit_forward_dct  0x007ae570 start_pass_fdctmgr  0x007ae720 forward_DCT
//     0x007ae890 forward_DCT_float
//   jfdctint.c / jfdctfst.c / jfdctflt.c
//     0x007b0cc0 jpeg_fdct_islow  0x007b10c0 jpeg_fdct_ifast  0x007b1310 jpeg_fdct_float (x87 in the original)
//   jcomapi.c (D3DX8's copy, shared by the game's compressor and D3DX8's own JPEG decoder)
//     0x007888f7 jpeg_abort  0x00788919 jpeg_destroy  0x00788934 jpeg_alloc_quant_table  0x00788950
//     jpeg_alloc_huff_table
//
// x87 exactness: the game runs the FPU at 24-bit precision (D3D, round to nearest), and the three float sites
// reproduce the original's x87 sequences at PC24 in portable C++ (src/fp: plain float operations where the x87 has
// float operands, joan::fp::Fpu for the double-operand divisor, joan::fp::ftol for _ftol 0x007b26cc): see the
// comments at jpeg_fdct_float, float_divisor and float_quantize. Built without FP contraction (no FMA fusing), the
// float code gives the same bits on x86 and x86_64. The constant tables (jpeg_natural_order 0x009e6da8, aanscales
// 0x009e6ee8, aanscalefactor 0x009e6f68, 8.0 0x009e6fa8, 1.0 0x009e3400, floats 0x009e6fb0..0x009e6fc0) are our own
// copies, bit-identical to joan.exe's .rdata.
//
// Error paths: ERREXIT / ERREXIT1 / TRACEMS write err->msg_code / msg_parm.i[0] and call err->error_exit /
// emit_message exactly as the original; code after an ERREXIT is reproduced as written (the game's error_exit never
// returns). Shift counts: every variable shift uses the x86 rule (count & 31), like the original's shl / sar cl.
//
// Verified bit-exact against joan.exe's own code in re/p3/jpeg (whole compressions: output bytes, heap and cinfo
// contents, malloc / free sequence, error codes; plus the FDCT / divisor / quantiser unit tests of re/p1/pixel).
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "jpeglib.hpp"
#include "../../registry.hpp"
#include "../../x87.hpp"

namespace joan::jpeg {
namespace {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

// x86 variable shifts (count & 31)
i32 shl32(i32 v, i32 n) { return static_cast<i32>(static_cast<u32>(v) << (n & 31)); }
i32 sar32(i32 v, i32 n) { return v >> (n & 31); }

// jpeg_natural_order (0x009e6da8): 64 entries + 16 extra 63s
constexpr int kNaturalOrder[DCTSIZE2 + 16] = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33,
    40, 48, 41, 34, 27, 20, 13, 6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54,
    47, 55, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};

// ---------------------------------------------------------------------------------------------
// Private structures (packed on x86 like the original)
// ---------------------------------------------------------------------------------------------
struct DerivedTbl {  // c_derived_tbl (0x500 bytes)
    unsigned int ehufco[256];
    signed char ehufsi[256];  // `char` of MSVC (signed)
};
struct SavableState {  // savable_state of jchuff.c (0x18 bytes)
    i32 put_buffer;
    int put_bits;
    int last_dc_val[MAX_COMPS_IN_SCAN];
};
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(push, 1)
#endif
struct MarkerWriter {  // my_marker_writer
    jpeg_marker_writer pub;
    unsigned int last_restart_interval;
};
struct HuffEncoder {  // huff_entropy_encoder
    jpeg_entropy_encoder pub;
    SavableState saved;
    unsigned int restarts_to_go;
    int next_restart_num;
    DerivedTbl* dc_derived_tbls[NUM_HUFF_TBLS];
    DerivedTbl* ac_derived_tbls[NUM_HUFF_TBLS];
    jlong* dc_count_ptrs[NUM_HUFF_TBLS];
    jlong* ac_count_ptrs[NUM_HUFF_TBLS];
};
struct PhuffEncoder {  // phuff_entropy_encoder
    jpeg_entropy_encoder pub;
    boolean gather_statistics;
    JOCTET* next_output_byte;
    jsize free_in_buffer;
    i32 put_buffer;
    int put_bits;
    j_compress_ptr cinfo;
    int last_dc_val[MAX_COMPS_IN_SCAN];
    int ac_tbl_no;
    unsigned int EOBRUN;
    unsigned int BE;
    char* bit_buffer;
    unsigned int restarts_to_go;
    int next_restart_num;
    DerivedTbl* derived_tbls[NUM_HUFF_TBLS];
    jlong* count_ptrs[NUM_HUFF_TBLS];
};
using DCTELEM = i32;
using FAST_FLOAT = float;
struct FdctController {  // my_fdct_controller
    jpeg_forward_dct pub;
    void (*do_dct)(DCTELEM* data);
    DCTELEM* divisors[NUM_QUANT_TBLS];
    void (*do_float_dct)(FAST_FLOAT* data);
    FAST_FLOAT* float_divisors[NUM_QUANT_TBLS];
};
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(pop)
#endif
// working_state of jchuff.c: on the stack (its layout is not observable); the savable part is copied as a block.
struct WorkingState {
    JOCTET* next_output_byte;
    jsize free_in_buffer;
    SavableState cur;
    j_compress_ptr cinfo;
};

static_assert(sizeof(DerivedTbl) == 0x500 && sizeof(SavableState) == 0x18);
#if JOAN_JPEG_X86_LAYOUT  // the original layouts
static_assert(sizeof(MarkerWriter) == 0x20, "my_marker_writer");
static_assert(sizeof(HuffEncoder) == 0x6c && offsetof(HuffEncoder, saved) == 0xc &&
                  offsetof(HuffEncoder, restarts_to_go) == 0x24 && offsetof(HuffEncoder, dc_derived_tbls) == 0x2c &&
                  offsetof(HuffEncoder, dc_count_ptrs) == 0x4c,
              "huff_entropy_encoder");
static_assert(sizeof(PhuffEncoder) == 0x69 && offsetof(PhuffEncoder, next_output_byte) == 0xd &&
                  offsetof(PhuffEncoder, cinfo) == 0x1d && offsetof(PhuffEncoder, ac_tbl_no) == 0x31 &&
                  offsetof(PhuffEncoder, bit_buffer) == 0x3d && offsetof(PhuffEncoder, derived_tbls) == 0x49 &&
                  offsetof(PhuffEncoder, count_ptrs) == 0x59,
              "phuff_entropy_encoder");
static_assert(sizeof(FdctController) == 0x30 && offsetof(FdctController, do_float_dct) == 0x1c,
              "my_fdct_controller");
#endif

template <class T, class P>
T* priv(P* pub) {
    return reinterpret_cast<T*>(pub);
}

JOAN_COUNTER(c_marker_init, "jinit_marker_writer");
JOAN_COUNTER(c_huff_init, "jinit_huff_encoder");
JOAN_COUNTER(c_phuff_init, "jinit_phuff_encoder");
JOAN_COUNTER(c_fdct_init, "jinit_forward_dct");

// =============================================================================================
// jcmarker.c
// =============================================================================================
// 0x007ab7f0 emit_byte / 0x007ab860 write_marker_byte
void emit_byte(j_compress_ptr c, int val) {
    jpeg_destination_mgr* dest = c->dest;
    *dest->next_output_byte++ = static_cast<JOCTET>(val);
    if (--dest->free_in_buffer == 0) {
        if (dest->empty_output_buffer(c) == 0) errexit(c, JERR_CANT_SUSPEND);
    }
}
// 0x007ab7d0
void emit_marker(j_compress_ptr c, int mark) {
    emit_byte(c, 0xff);
    emit_byte(c, mark);
}
// 0x007ab830
void emit_2bytes(j_compress_ptr c, int value) {
    emit_byte(c, (value >> 8) & 0xff);
    emit_byte(c, value & 0xff);
}
// 0x007abb10
int emit_dqt(j_compress_ptr c, int index) {
    JQUANT_TBL* qtbl = c->quant_tbl_ptrs[index];
    if (qtbl == nullptr) errexit1(c, JERR_NO_QUANT_TABLE, index);
    int prec = 0;
    for (int i = 0; i < DCTSIZE2; i++)
        if (qtbl->quantval[i] > 255) prec = 1;
    if (qtbl->sent_table == 0) {
        emit_marker(c, 0xdb);
        emit_2bytes(c, prec ? DCTSIZE2 * 2 + 1 + 2 : DCTSIZE2 + 1 + 2);
        emit_byte(c, index + (prec << 4));
        for (int i = 0; i < DCTSIZE2; i++) {
            const unsigned int qval = qtbl->quantval[kNaturalOrder[i]];
            if (prec) emit_byte(c, static_cast<int>(qval >> 8));
            emit_byte(c, static_cast<int>(qval & 0xff));
        }
        qtbl->sent_table = 1;
    }
    return prec;
}
// 0x007abd80
void emit_dht(j_compress_ptr c, int index, boolean is_ac) {
    JHUFF_TBL* htbl;
    if (is_ac) {
        htbl = c->ac_huff_tbl_ptrs[index];
        index += 0x10;
    } else {
        htbl = c->dc_huff_tbl_ptrs[index];
    }
    if (htbl == nullptr) errexit1(c, JERR_NO_HUFF_TABLE, index);
    if (htbl->sent_table == 0) {
        emit_marker(c, 0xc4);
        int length = 0;
        for (int i = 1; i <= 16; i++) length += htbl->bits[i];
        emit_2bytes(c, length + 2 + 1 + 16);
        emit_byte(c, index);
        for (int i = 1; i <= 16; i++) emit_byte(c, htbl->bits[i]);
        for (int i = 0; i < length; i++) emit_byte(c, htbl->huffval[i]);
        htbl->sent_table = 1;
    }
}
// 0x007abe40 (arithmetic coding not supported)
void emit_dac(j_compress_ptr) {}
// 0x007abe50
void emit_dri(j_compress_ptr c) {
    emit_marker(c, 0xdd);
    emit_2bytes(c, 4);
    emit_2bytes(c, static_cast<int>(c->restart_interval));
}
// 0x007abbe0
void emit_sof(j_compress_ptr c, int code) {
    emit_marker(c, code);
    emit_2bytes(c, 3 * c->num_components + 2 + 5 + 1);
    if (static_cast<jlong>(c->image_height) > 65535 || static_cast<jlong>(c->image_width) > 65535)
        errexit1(c, JERR_IMAGE_TOO_BIG, 65535);
    emit_byte(c, c->data_precision);
    emit_2bytes(c, static_cast<int>(c->image_height));
    emit_2bytes(c, static_cast<int>(c->image_width));
    emit_byte(c, c->num_components);
    jpeg_component_info* compptr = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, compptr++) {
        emit_byte(c, compptr->component_id);
        emit_byte(c, (compptr->h_samp_factor << 4) + compptr->v_samp_factor);
        emit_byte(c, compptr->quant_tbl_no);
    }
}
// 0x007abe80
void emit_sos(j_compress_ptr c) {
    emit_marker(c, 0xda);
    emit_2bytes(c, c->comps_in_scan * 2 + 2 + 1 + 3);
    emit_byte(c, c->comps_in_scan);
    for (int i = 0; i < c->comps_in_scan; i++) {
        jpeg_component_info* compptr = c->cur_comp_info[i];
        emit_byte(c, compptr->component_id);
        int td = compptr->dc_tbl_no;
        int ta = compptr->ac_tbl_no;
        if (c->progressive_mode != 0) {
            if (c->Ss == 0) {
                ta = 0;
                if (c->Ah != 0 && c->arith_code == 0) td = 0;
            } else {
                td = 0;
            }
        }
        emit_byte(c, (td << 4) + ta);
    }
    emit_byte(c, c->Ss);
    emit_byte(c, c->Se);
    emit_byte(c, (c->Ah << 4) + c->Al);
}
// 0x007ab8c0
void emit_jfif_app0(j_compress_ptr c) {
    emit_marker(c, 0xe0);
    emit_2bytes(c, 2 + 4 + 1 + 2 + 1 + 2 + 2 + 1 + 1);
    emit_byte(c, 0x4a);
    emit_byte(c, 0x46);
    emit_byte(c, 0x49);
    emit_byte(c, 0x46);
    emit_byte(c, 0);
    emit_byte(c, c->JFIF_major_version);
    emit_byte(c, c->JFIF_minor_version);
    emit_byte(c, c->density_unit);
    emit_2bytes(c, c->X_density);
    emit_2bytes(c, c->Y_density);
    emit_byte(c, 0);
    emit_byte(c, 0);
}
// 0x007ab970
void emit_adobe_app14(j_compress_ptr c) {
    emit_marker(c, 0xee);
    emit_2bytes(c, 2 + 5 + 2 + 2 + 2 + 1);
    emit_byte(c, 0x41);
    emit_byte(c, 0x64);
    emit_byte(c, 0x6f);
    emit_byte(c, 0x62);
    emit_byte(c, 0x65);
    emit_2bytes(c, 100);
    emit_2bytes(c, 0);
    emit_2bytes(c, 0);
    switch (c->jpeg_color_space) {
        case JCS_YCbCr: emit_byte(c, 1); break;
        case JCS_YCCK: emit_byte(c, 2); break;
        default: emit_byte(c, 0); break;
    }
}
// 0x007ab790
void write_marker_header(j_compress_ptr c, int marker, unsigned int datalen) {
    if (datalen > 65533u) errexit(c, JERR_BAD_LENGTH);
    emit_marker(c, marker);
    emit_2bytes(c, static_cast<int>(datalen + 2));
}
// 0x007ab870
void write_file_header(j_compress_ptr c) {
    MarkerWriter* marker = priv<MarkerWriter>(c->marker);
    emit_marker(c, 0xd8);
    marker->last_restart_interval = 0;
    if (c->write_JFIF_header != 0) emit_jfif_app0(c);
    if (c->write_Adobe_marker != 0) emit_adobe_app14(c);
}
// 0x007aba10
void write_frame_header(j_compress_ptr c) {
    int prec = 0;
    jpeg_component_info* compptr = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, compptr++) prec += emit_dqt(c, compptr->quant_tbl_no);
    boolean is_baseline;
    if (c->arith_code != 0 || c->progressive_mode != 0 || c->data_precision != 8) {
        is_baseline = 0;
    } else {
        is_baseline = 1;
        compptr = c->comp_info;
        for (int ci = 0; ci < c->num_components; ci++, compptr++)
            if (compptr->dc_tbl_no > 1 || compptr->ac_tbl_no > 1) is_baseline = 0;
        if (prec != 0 && is_baseline != 0) {
            is_baseline = 0;
            emit_msg(c, JTRC_16BIT_TABLES, 0);  // TRACEMS(cinfo, 0, ...)
        }
    }
    if (c->arith_code != 0) emit_sof(c, 0xc9);
    else if (c->progressive_mode != 0) emit_sof(c, 0xc2);
    else if (is_baseline != 0) emit_sof(c, 0xc0);
    else emit_sof(c, 0xc1);
}
// 0x007abca0
void write_scan_header(j_compress_ptr c) {
    MarkerWriter* marker = priv<MarkerWriter>(c->marker);
    if (c->arith_code != 0) {
        emit_dac(c);
    } else {
        for (int i = 0; i < c->comps_in_scan; i++) {
            jpeg_component_info* compptr = c->cur_comp_info[i];
            if (c->progressive_mode != 0) {
                if (c->Ss == 0) {
                    if (c->Ah == 0) emit_dht(c, compptr->dc_tbl_no, 0);
                } else {
                    emit_dht(c, compptr->ac_tbl_no, 1);
                }
            } else {
                emit_dht(c, compptr->dc_tbl_no, 0);
                emit_dht(c, compptr->ac_tbl_no, 1);
            }
        }
    }
    if (c->restart_interval != marker->last_restart_interval) {
        emit_dri(c);
        marker->last_restart_interval = c->restart_interval;
    }
    emit_sos(c);
}
// 0x007abf60
void write_file_trailer(j_compress_ptr c) { emit_marker(c, 0xd9); }
// 0x007abf80
void write_tables_only(j_compress_ptr c) {
    emit_marker(c, 0xd8);
    for (int i = 0; i < NUM_QUANT_TBLS; i++)
        if (c->quant_tbl_ptrs[i] != nullptr) emit_dqt(c, i);
    if (c->arith_code == 0) {
        for (int i = 0; i < NUM_HUFF_TBLS; i++) {
            if (c->dc_huff_tbl_ptrs[i] != nullptr) emit_dht(c, i, 0);
            if (c->ac_huff_tbl_ptrs[i] != nullptr) emit_dht(c, i, 1);
        }
    }
    emit_marker(c, 0xd9);
}

}  // namespace

// 0x007ab730
void jinit_marker_writer(j_compress_ptr c) {
    c_marker_init.hit();
    auto* m = static_cast<MarkerWriter*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<MarkerWriter>));
    c->marker = &m->pub;
    m->pub.write_file_header = &write_file_header;
    m->pub.write_frame_header = &write_frame_header;
    m->pub.write_scan_header = &write_scan_header;
    m->pub.write_file_trailer = &write_file_trailer;
    m->pub.write_tables_only = &write_tables_only;
    m->pub.write_marker_header = &write_marker_header;
    m->pub.write_marker_byte = &emit_byte;
    m->last_restart_interval = 0;
}

// =============================================================================================
// jutils.c
// =============================================================================================
// 0x007ac150
jlong jdiv_round_up(jlong a, jlong b) { return (a + b - 1) / b; }
// 0x007ac160
jlong jround_up(jlong a, jlong b) {
    a += b - 1;
    return a - a % b;
}

namespace {
// rep movsd (count >> 2) then rep movsb (count & 3), forward.
void movs_copy(u8* d, const u8* s, jsize n) {
    for (jsize k = n >> 2; k != 0; --k, d += 4, s += 4) {
        u32 w;
        std::memcpy(&w, s, 4);
        std::memcpy(d, &w, 4);
    }
    for (jsize k = n & 3; k != 0; --k) *d++ = *s++;
}
}  // namespace

// 0x007ac180
void jcopy_sample_rows(JSAMPARRAY input_array, int source_row, JSAMPARRAY output_array, int dest_row, int num_rows,
                       JDIMENSION num_cols) {
    JSAMPARRAY in = input_array + source_row;
    JSAMPARRAY out = output_array + dest_row;
    for (int row = num_rows; row > 0; --row) movs_copy(*out++, *in++, num_cols);
}

namespace {
// 0x007ac1d0
void jcopy_block_row(JBLOCKROW input_row, JBLOCKROW output_row, JDIMENSION num_blocks) {
    movs_copy(reinterpret_cast<u8*>(output_row), reinterpret_cast<const u8*>(input_row), num_blocks * SIZEOF<JBLOCK>);
}
}  // namespace

// 0x007ac200
void jzero_far(void* target, jsize bytestozero) {
    u8* p = static_cast<u8*>(target);
    for (jsize k = bytestozero >> 2; k != 0; --k, p += 4) std::memset(p, 0, 4);
    for (jsize k = bytestozero & 3; k != 0; --k) *p++ = 0;
}

// =============================================================================================
// jcomapi.c (D3DX8's copy)
// =============================================================================================
namespace {
// D3DX8's jcomapi is built for its own (aligned, 6a) struct: is_decompressor at +0xc, global_state at +0x10. On the
// game's packed struct its "is_decompressor" test reads the low byte of client_data (+0xc) and its global_state
// store writes a dword at +0x10, i.e. is_decompressor (low byte) and the low three bytes of global_state (the top
// byte, +0x14, stays). Reproduced field by field (the same bytes on x86, the same values everywhere).
bool d3dx_is_decompressor(j_compress_ptr c) { return (reinterpret_cast<std::uintptr_t>(c->client_data) & 0xffu) != 0; }
void d3dx_store_global_state(j_compress_ptr c, u32 state) {
    c->is_decompressor = static_cast<boolean>(state & 0xffu);
    c->global_state = static_cast<int>((static_cast<u32>(c->global_state) & 0xff000000u) | (state >> 8));
}
}  // namespace

// 0x007888f7 jpeg_abort: free_pool(JPOOL_IMAGE); global_state = is_decompressor ? DSTATE_START : CSTATE_START.
void jpeg_abort(j_compress_ptr c) {
    c->mem->free_pool(c, JPOOL_IMAGE);
    d3dx_store_global_state(c, d3dx_is_decompressor(c) ? DSTATE_START : CSTATE_START);
}
// 0x00788919 jpeg_destroy
void jpeg_destroy(j_compress_ptr c) {
    if (c->mem != nullptr) c->mem->self_destruct(c);
    c->mem = nullptr;
    d3dx_store_global_state(c, 0);
}
// 0x00788934 jpeg_alloc_quant_table
JQUANT_TBL* jpeg_alloc_quant_table(j_compress_ptr c) {
    auto* tbl = static_cast<JQUANT_TBL*>(c->mem->alloc_small(c, JPOOL_PERMANENT, SIZEOF<JQUANT_TBL>));
    tbl->sent_table = 0;
    return tbl;
}
// 0x00788950 jpeg_alloc_huff_table
JHUFF_TBL* jpeg_alloc_huff_table(j_compress_ptr c) {
    auto* tbl = static_cast<JHUFF_TBL*>(c->mem->alloc_small(c, JPOOL_PERMANENT, SIZEOF<JHUFF_TBL>));
    tbl->sent_table = 0;
    return tbl;
}

// =============================================================================================
// jchuff.c
// =============================================================================================
namespace {

// 0x007acba0
void jpeg_make_c_derived_tbl(j_compress_ptr c, boolean isDC, int tblno, DerivedTbl** pdtbl) {
    if (tblno < 0 || tblno >= NUM_HUFF_TBLS) errexit1(c, JERR_NO_HUFF_TABLE, tblno);
    JHUFF_TBL* htbl = isDC ? c->dc_huff_tbl_ptrs[tblno] : c->ac_huff_tbl_ptrs[tblno];
    if (htbl == nullptr) errexit1(c, JERR_NO_HUFF_TABLE, tblno);
    if (*pdtbl == nullptr) *pdtbl = static_cast<DerivedTbl*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<DerivedTbl>));
    DerivedTbl* dtbl = *pdtbl;
    signed char huffsize[257];
    unsigned int huffcode[257];
    int p = 0;
    for (int l = 1; l <= 16; l++) {
        int i = htbl->bits[l];
        if (i < 0 || p + i > 256) errexit(c, JERR_BAD_HUFF_TABLE);
        while (i--) huffsize[p++] = static_cast<signed char>(l);
    }
    huffsize[p] = 0;
    const int lastp = p;
    unsigned int code = 0;
    int si = huffsize[0];
    p = 0;
    while (huffsize[p] != 0) {
        while (static_cast<int>(huffsize[p]) == si) {
            huffcode[p++] = code;
            code++;
        }
        if (static_cast<i32>(code) >= shl32(1, si)) errexit(c, JERR_BAD_HUFF_TABLE);
        code <<= 1;
        si++;
    }
    std::memset(dtbl->ehufsi, 0, sizeof(dtbl->ehufsi));
    const int maxsymbol = isDC ? 15 : 255;
    for (p = 0; p < lastp; p++) {
        const int i = htbl->huffval[p];
        if (i < 0 || i > maxsymbol || dtbl->ehufsi[i] != 0) errexit(c, JERR_BAD_HUFF_TABLE);
        dtbl->ehufco[i] = huffcode[p];
        dtbl->ehufsi[i] = huffsize[p];
    }
}

// 0x007ad500
boolean huff_dump_buffer(WorkingState* state) {
    jpeg_destination_mgr* dest = state->cinfo->dest;
    if (dest->empty_output_buffer(state->cinfo) == 0) return 0;
    state->next_output_byte = dest->next_output_byte;
    state->free_in_buffer = dest->free_in_buffer;
    return 1;
}
// 0x007ad430
boolean huff_emit_bits(WorkingState* state, unsigned int code, int size) {
    i32 put_buffer = static_cast<i32>(code);
    int put_bits = state->cur.put_bits;
    if (size == 0) errexit(state->cinfo, JERR_HUFF_MISSING_CODE);
    put_buffer &= shl32(1, size) - 1;
    put_bits += size;
    put_buffer = shl32(put_buffer, 24 - put_bits);
    put_buffer |= state->cur.put_buffer;
    while (put_bits >= 8) {
        const int cbyte = (put_buffer >> 16) & 0xff;
        *state->next_output_byte++ = static_cast<JOCTET>(cbyte);
        if (--state->free_in_buffer == 0)
            if (!huff_dump_buffer(state)) return 0;
        if (cbyte == 0xff) {
            *state->next_output_byte++ = 0;
            if (--state->free_in_buffer == 0)
                if (!huff_dump_buffer(state)) return 0;
        }
        put_buffer = shl32(put_buffer, 8);
        put_bits -= 8;
    }
    state->cur.put_buffer = put_buffer;
    state->cur.put_bits = put_bits;
    return 1;
}
// 0x007ad5c0
boolean huff_flush_bits(WorkingState* state) {
    if (!huff_emit_bits(state, 0x7f, 7)) return 0;
    state->cur.put_buffer = 0;
    state->cur.put_bits = 0;
    return 1;
}

// 0x007ad2a0
boolean encode_one_block(WorkingState* state, JCOEF* block, int last_dc_val, DerivedTbl* dctbl, DerivedTbl* actbl) {
    int temp = block[0] - last_dc_val;
    int temp2 = temp;
    if (temp < 0) {
        temp = -temp;
        temp2--;
    }
    int nbits = 0;
    while (temp) {
        nbits++;
        temp >>= 1;
    }
    if (nbits > 11) errexit(state->cinfo, JERR_BAD_DCT_COEF);
    if (!huff_emit_bits(state, dctbl->ehufco[nbits], dctbl->ehufsi[nbits])) return 0;
    if (nbits)
        if (!huff_emit_bits(state, static_cast<unsigned int>(temp2), nbits)) return 0;
    int r = 0;
    for (int k = 1; k < DCTSIZE2; k++) {
        temp = block[kNaturalOrder[k]];
        if (temp == 0) {
            r++;
        } else {
            while (r > 15) {
                if (!huff_emit_bits(state, actbl->ehufco[0xf0], actbl->ehufsi[0xf0])) return 0;
                r -= 16;
            }
            temp2 = temp;
            if (temp < 0) {
                temp = -temp;
                temp2--;
            }
            nbits = 1;
            while ((temp >>= 1)) nbits++;
            if (nbits > 10) errexit(state->cinfo, JERR_BAD_DCT_COEF);
            const int i = (r << 4) + nbits;
            if (!huff_emit_bits(state, actbl->ehufco[i], actbl->ehufsi[i])) return 0;
            if (!huff_emit_bits(state, static_cast<unsigned int>(temp2), nbits)) return 0;
            r = 0;
        }
    }
    if (r > 0)
        if (!huff_emit_bits(state, actbl->ehufco[0], actbl->ehufsi[0])) return 0;
    return 1;
}
// 0x007ad530
boolean huff_emit_restart(WorkingState* state, int restart_num) {
    if (!huff_flush_bits(state)) return 0;
    *state->next_output_byte++ = 0xff;
    if (--state->free_in_buffer == 0)
        if (!huff_dump_buffer(state)) return 0;
    *state->next_output_byte++ = static_cast<JOCTET>(0xd0 + restart_num);
    if (--state->free_in_buffer == 0)
        if (!huff_dump_buffer(state)) return 0;
    for (int ci = 0; ci < state->cinfo->comps_in_scan; ci++) state->cur.last_dc_val[ci] = 0;
    return 1;
}

void load_state(WorkingState& s, j_compress_ptr c, const HuffEncoder* entropy) {
    s.next_output_byte = c->dest->next_output_byte;
    s.free_in_buffer = c->dest->free_in_buffer;
    s.cur = entropy->saved;
    s.cinfo = c;
}
void store_state(const WorkingState& s, j_compress_ptr c, HuffEncoder* entropy) {
    c->dest->next_output_byte = s.next_output_byte;
    c->dest->free_in_buffer = s.free_in_buffer;
    entropy->saved = s.cur;
}

// 0x007ad170
boolean encode_mcu_huff(j_compress_ptr c, JBLOCKROW* mcu_data) {
    HuffEncoder* entropy = priv<HuffEncoder>(c->entropy);
    WorkingState state;
    load_state(state, c, entropy);
    if (c->restart_interval != 0) {
        if (entropy->restarts_to_go == 0)
            if (!huff_emit_restart(&state, entropy->next_restart_num)) return 0;
    }
    for (int blkn = 0; blkn < c->blocks_in_MCU; blkn++) {
        const int ci = c->MCU_membership[blkn];
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        if (!encode_one_block(&state, mcu_data[blkn][0], state.cur.last_dc_val[ci],
                              entropy->dc_derived_tbls[compptr->dc_tbl_no],
                              entropy->ac_derived_tbls[compptr->ac_tbl_no]))
            return 0;
        state.cur.last_dc_val[ci] = mcu_data[blkn][0][0];
    }
    store_state(state, c, entropy);
    if (c->restart_interval != 0) {
        if (entropy->restarts_to_go == 0) {
            entropy->restarts_to_go = c->restart_interval;
            entropy->next_restart_num++;
            entropy->next_restart_num &= 7;
        }
        entropy->restarts_to_go--;
    }
    return 1;
}
// 0x007ad5f0
void finish_pass_huff(j_compress_ptr c) {
    HuffEncoder* entropy = priv<HuffEncoder>(c->entropy);
    WorkingState state;
    load_state(state, c, entropy);
    if (!huff_flush_bits(&state)) errexit(c, JERR_CANT_SUSPEND);
    store_state(state, c, entropy);
}
// 0x007ad740
void htest_one_block(j_compress_ptr c, JCOEF* block, int last_dc_val, jlong* dc_counts, jlong* ac_counts) {
    int temp = block[0] - last_dc_val;
    if (temp < 0) temp = -temp;
    int nbits = 0;
    while (temp) {
        nbits++;
        temp >>= 1;
    }
    if (nbits > 11) errexit(c, JERR_BAD_DCT_COEF);
    dc_counts[nbits]++;
    int r = 0;
    for (int k = 1; k < DCTSIZE2; k++) {
        temp = block[kNaturalOrder[k]];
        if (temp == 0) {
            r++;
        } else {
            while (r > 15) {
                ac_counts[0xf0]++;
                r -= 16;
            }
            if (temp < 0) temp = -temp;
            nbits = 1;
            while ((temp >>= 1)) nbits++;
            if (nbits > 10) errexit(c, JERR_BAD_DCT_COEF);
            ac_counts[(r << 4) + nbits]++;
            r = 0;
        }
    }
    if (r > 0) ac_counts[0]++;
}
// 0x007ad670
boolean encode_mcu_gather(j_compress_ptr c, JBLOCKROW* mcu_data) {
    HuffEncoder* entropy = priv<HuffEncoder>(c->entropy);
    if (c->restart_interval != 0) {
        if (entropy->restarts_to_go == 0) {
            for (int ci = 0; ci < c->comps_in_scan; ci++) entropy->saved.last_dc_val[ci] = 0;
            entropy->restarts_to_go = c->restart_interval;
        }
        entropy->restarts_to_go--;
    }
    for (int blkn = 0; blkn < c->blocks_in_MCU; blkn++) {
        const int ci = c->MCU_membership[blkn];
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        htest_one_block(c, mcu_data[blkn][0], entropy->saved.last_dc_val[ci],
                        entropy->dc_count_ptrs[compptr->dc_tbl_no], entropy->ac_count_ptrs[compptr->ac_tbl_no]);
        entropy->saved.last_dc_val[ci] = mcu_data[blkn][0][0];
    }
    return 1;
}
// 0x007acda0
void jpeg_gen_optimal_table(j_compress_ptr c, JHUFF_TBL* htbl, jlong freq[]) {
    std::uint8_t bits[32 + 1];
    int codesize[257];
    int others[257];
    std::memset(bits, 0, sizeof(bits));
    std::memset(codesize, 0, sizeof(codesize));
    for (int i = 0; i < 257; i++) others[i] = -1;
    freq[256] = 1;
    for (;;) {
        int c1 = -1;
        jlong v = 1000000000;
        for (int i = 0; i <= 256; i++)
            if (freq[i] != 0 && freq[i] <= v) {
                v = freq[i];
                c1 = i;
            }
        int c2 = -1;
        v = 1000000000;
        for (int i = 0; i <= 256; i++)
            if (freq[i] != 0 && freq[i] <= v && i != c1) {
                v = freq[i];
                c2 = i;
            }
        if (c2 < 0) break;
        freq[c1] += freq[c2];
        freq[c2] = 0;
        codesize[c1]++;
        while (others[c1] >= 0) {
            c1 = others[c1];
            codesize[c1]++;
        }
        others[c1] = c2;
        codesize[c2]++;
        while (others[c2] >= 0) {
            c2 = others[c2];
            codesize[c2]++;
        }
    }
    for (int i = 0; i <= 256; i++) {
        if (codesize[i] != 0) {
            if (codesize[i] > 32) errexit(c, JERR_HUFF_CLEN_OVERFLOW);
            bits[codesize[i]]++;
        }
    }
    int i = 32;
    for (; i > 16; i--) {
        while (bits[i] > 0) {
            int j = i - 2;
            while (bits[j] == 0) j--;
            bits[i] -= 2;
            bits[i - 1]++;
            bits[j + 1] += 2;
            bits[j]--;
        }
    }
    while (bits[i] == 0) i--;
    bits[i]--;
    std::memcpy(htbl->bits, bits, sizeof(htbl->bits));
    int p = 0;
    for (i = 1; i <= 32; i++)
        for (int j = 0; j <= 255; j++)
            if (codesize[j] == i) htbl->huffval[p++] = static_cast<std::uint8_t>(j);
    htbl->sent_table = 0;
}
// 0x007ad820
void finish_pass_gather(j_compress_ptr c) {
    HuffEncoder* entropy = priv<HuffEncoder>(c->entropy);
    boolean did_dc[NUM_HUFF_TBLS] = {0, 0, 0, 0};
    boolean did_ac[NUM_HUFF_TBLS] = {0, 0, 0, 0};
    for (int ci = 0; ci < c->comps_in_scan; ci++) {
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        const int dctbl = compptr->dc_tbl_no;
        const int actbl = compptr->ac_tbl_no;
        if (!did_dc[dctbl]) {
            if (c->dc_huff_tbl_ptrs[dctbl] == nullptr) c->dc_huff_tbl_ptrs[dctbl] = jpeg_alloc_huff_table(c);
            jpeg_gen_optimal_table(c, c->dc_huff_tbl_ptrs[dctbl], entropy->dc_count_ptrs[dctbl]);
            did_dc[dctbl] = 1;
        }
        if (!did_ac[actbl]) {
            if (c->ac_huff_tbl_ptrs[actbl] == nullptr) c->ac_huff_tbl_ptrs[actbl] = jpeg_alloc_huff_table(c);
            jpeg_gen_optimal_table(c, c->ac_huff_tbl_ptrs[actbl], entropy->ac_count_ptrs[actbl]);
            did_ac[actbl] = 1;
        }
    }
}
// 0x007ad000
void start_pass_huff(j_compress_ptr c, boolean gather_statistics) {
    HuffEncoder* entropy = priv<HuffEncoder>(c->entropy);
    if (gather_statistics) {
        entropy->pub.encode_mcu = &encode_mcu_gather;
        entropy->pub.finish_pass = &finish_pass_gather;
    } else {
        entropy->pub.encode_mcu = &encode_mcu_huff;
        entropy->pub.finish_pass = &finish_pass_huff;
    }
    for (int ci = 0; ci < c->comps_in_scan; ci++) {
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        const int dctbl = compptr->dc_tbl_no;
        const int actbl = compptr->ac_tbl_no;
        if (gather_statistics) {
            if (dctbl < 0 || dctbl >= NUM_HUFF_TBLS) errexit1(c, JERR_NO_HUFF_TABLE, dctbl);
            if (actbl < 0 || actbl >= NUM_HUFF_TBLS) errexit1(c, JERR_NO_HUFF_TABLE, actbl);
            if (entropy->dc_count_ptrs[dctbl] == nullptr)
                entropy->dc_count_ptrs[dctbl] =
                    static_cast<jlong*>(c->mem->alloc_small(c, JPOOL_IMAGE, 257 * SIZEOF<jlong>));
            std::memset(entropy->dc_count_ptrs[dctbl], 0, 257 * sizeof(jlong));
            if (entropy->ac_count_ptrs[actbl] == nullptr)
                entropy->ac_count_ptrs[actbl] =
                    static_cast<jlong*>(c->mem->alloc_small(c, JPOOL_IMAGE, 257 * SIZEOF<jlong>));
            std::memset(entropy->ac_count_ptrs[actbl], 0, 257 * sizeof(jlong));
        } else {
            jpeg_make_c_derived_tbl(c, 1, dctbl, &entropy->dc_derived_tbls[dctbl]);
            jpeg_make_c_derived_tbl(c, 0, actbl, &entropy->ac_derived_tbls[actbl]);
        }
        entropy->saved.last_dc_val[ci] = 0;
    }
    entropy->saved.put_buffer = 0;
    entropy->saved.put_bits = 0;
    entropy->restarts_to_go = c->restart_interval;
    entropy->next_restart_num = 0;
}

}  // namespace

// 0x007acfc0
void jinit_huff_encoder(j_compress_ptr c) {
    c_huff_init.hit();
    auto* entropy = static_cast<HuffEncoder*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<HuffEncoder>));
    c->entropy = &entropy->pub;
    entropy->pub.start_pass = &start_pass_huff;
    for (int i = 0; i < NUM_HUFF_TBLS; i++) {
        entropy->ac_derived_tbls[i] = nullptr;
        entropy->dc_derived_tbls[i] = nullptr;
        entropy->ac_count_ptrs[i] = nullptr;
        entropy->dc_count_ptrs[i] = nullptr;
    }
}

// =============================================================================================
// jcphuff.c
// =============================================================================================
namespace {

constexpr unsigned int MAX_CORR_BITS = 1000;

// 0x007add00
void phuff_dump_buffer(PhuffEncoder* e) {
    jpeg_destination_mgr* dest = e->cinfo->dest;
    if (dest->empty_output_buffer(e->cinfo) == 0) errexit(e->cinfo, JERR_CANT_SUSPEND);
    e->next_output_byte = dest->next_output_byte;
    e->free_in_buffer = dest->free_in_buffer;
}
void phuff_emit_byte(PhuffEncoder* e, int val) {
    *e->next_output_byte++ = static_cast<JOCTET>(val);
    if (--e->free_in_buffer == 0) phuff_dump_buffer(e);
}
// 0x007adc20
void phuff_emit_bits(PhuffEncoder* e, unsigned int code, int size) {
    i32 put_buffer = static_cast<i32>(code);
    int put_bits = e->put_bits;
    if (size == 0) errexit(e->cinfo, JERR_HUFF_MISSING_CODE);
    if (e->gather_statistics != 0) return;
    put_buffer &= shl32(1, size) - 1;
    put_bits += size;
    put_buffer = shl32(put_buffer, 24 - put_bits);
    put_buffer |= e->put_buffer;
    while (put_bits >= 8) {
        const int cbyte = (put_buffer >> 16) & 0xff;
        phuff_emit_byte(e, cbyte);
        if (cbyte == 0xff) phuff_emit_byte(e, 0);
        put_buffer = shl32(put_buffer, 8);
        put_bits -= 8;
    }
    e->put_buffer = put_buffer;
    e->put_bits = put_bits;
}
// 0x007ade40
void phuff_flush_bits(PhuffEncoder* e) {
    phuff_emit_bits(e, 0x7f, 7);
    e->put_buffer = 0;
    e->put_bits = 0;
}
// 0x007add40
void emit_symbol(PhuffEncoder* e, int tbl_no, int symbol) {
    if (e->gather_statistics != 0) {
        e->count_ptrs[tbl_no][symbol]++;
    } else {
        DerivedTbl* tbl = e->derived_tbls[tbl_no];
        phuff_emit_bits(e, tbl->ehufco[symbol], tbl->ehufsi[symbol]);
    }
}
// 0x007adee0
void emit_buffered_bits(PhuffEncoder* e, char* bufstart, unsigned int nbits) {
    if (e->gather_statistics != 0) return;
    while (nbits > 0) {
        phuff_emit_bits(e, static_cast<unsigned int>(static_cast<int>(static_cast<signed char>(*bufstart))), 1);
        bufstart++;
        nbits--;
    }
}
// 0x007ade60
void emit_eobrun(PhuffEncoder* e) {
    if (e->EOBRUN > 0) {
        int temp = static_cast<int>(e->EOBRUN);
        int nbits = 0;
        while ((temp >>= 1)) nbits++;
        if (nbits > 14) errexit(e->cinfo, JERR_HUFF_MISSING_CODE);
        emit_symbol(e, e->ac_tbl_no, nbits << 4);
        if (nbits) phuff_emit_bits(e, e->EOBRUN, nbits);
        e->EOBRUN = 0;
        emit_buffered_bits(e, e->bit_buffer, e->BE);
        e->BE = 0;
    }
}
// 0x007add90
void phuff_emit_restart(PhuffEncoder* e, int restart_num) {
    emit_eobrun(e);
    if (e->gather_statistics == 0) {
        phuff_flush_bits(e);
        phuff_emit_byte(e, 0xff);
        phuff_emit_byte(e, 0xd0 + restart_num);
    }
    if (e->cinfo->Ss == 0) {
        for (int ci = 0; ci < e->cinfo->comps_in_scan; ci++) e->last_dc_val[ci] = 0;
    } else {
        e->EOBRUN = 0;
        e->BE = 0;
    }
}
// common head / tail of the four encode_mcu methods
PhuffEncoder* phuff_begin(j_compress_ptr c) {
    PhuffEncoder* e = priv<PhuffEncoder>(c->entropy);
    e->next_output_byte = c->dest->next_output_byte;
    e->free_in_buffer = c->dest->free_in_buffer;
    if (c->restart_interval != 0)
        if (e->restarts_to_go == 0) phuff_emit_restart(e, e->next_restart_num);
    return e;
}
boolean phuff_end(j_compress_ptr c, PhuffEncoder* e) {
    c->dest->next_output_byte = e->next_output_byte;
    c->dest->free_in_buffer = e->free_in_buffer;
    if (c->restart_interval != 0) {
        if (e->restarts_to_go == 0) {
            e->restarts_to_go = c->restart_interval;
            e->next_restart_num++;
            e->next_restart_num &= 7;
        }
        e->restarts_to_go--;
    }
    return 1;
}
// 0x007adad0
boolean encode_mcu_DC_first(j_compress_ptr c, JBLOCKROW* mcu_data) {
    const int al = c->Al;
    PhuffEncoder* e = phuff_begin(c);
    for (int blkn = 0; blkn < c->blocks_in_MCU; blkn++) {
        const JCOEF* block = mcu_data[blkn][0];
        const int ci = c->MCU_membership[blkn];
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        int temp2 = sar32(block[0], al);
        int temp = temp2 - e->last_dc_val[ci];
        e->last_dc_val[ci] = temp2;
        temp2 = temp;
        if (temp < 0) {
            temp = -temp;
            temp2--;
        }
        int nbits = 0;
        while (temp) {
            nbits++;
            temp >>= 1;
        }
        if (nbits > 11) errexit(c, JERR_BAD_DCT_COEF);
        emit_symbol(e, compptr->dc_tbl_no, nbits);
        if (nbits) phuff_emit_bits(e, static_cast<unsigned int>(temp2), nbits);
    }
    return phuff_end(c, e);
}
// 0x007adf20
boolean encode_mcu_AC_first(j_compress_ptr c, JBLOCKROW* mcu_data) {
    const int se = c->Se;
    const int al = c->Al;
    PhuffEncoder* e = phuff_begin(c);
    const JCOEF* block = mcu_data[0][0];
    int r = 0;
    for (int k = c->Ss; k <= se; k++) {
        int temp = block[kNaturalOrder[k]];
        if (temp == 0) {
            r++;
            continue;
        }
        int temp2;
        if (temp < 0) {
            temp = sar32(-temp, al);
            temp2 = ~temp;
        } else {
            temp = sar32(temp, al);
            temp2 = temp;
        }
        if (temp == 0) {
            r++;
            continue;
        }
        if (e->EOBRUN > 0) emit_eobrun(e);
        while (r > 15) {
            emit_symbol(e, e->ac_tbl_no, 0xf0);
            r -= 16;
        }
        int nbits = 1;
        while ((temp >>= 1)) nbits++;
        if (nbits > 10) errexit(c, JERR_BAD_DCT_COEF);
        emit_symbol(e, e->ac_tbl_no, (r << 4) + nbits);
        phuff_emit_bits(e, static_cast<unsigned int>(temp2), nbits);
        r = 0;
    }
    if (r > 0) {
        e->EOBRUN++;
        if (e->EOBRUN == 0x7fff) emit_eobrun(e);
    }
    return phuff_end(c, e);
}
// 0x007ae0e0
boolean encode_mcu_DC_refine(j_compress_ptr c, JBLOCKROW* mcu_data) {
    const int al = c->Al;
    PhuffEncoder* e = phuff_begin(c);
    for (int blkn = 0; blkn < c->blocks_in_MCU; blkn++)
        phuff_emit_bits(e, static_cast<unsigned int>(sar32(mcu_data[blkn][0][0], al)), 1);
    return phuff_end(c, e);
}
// 0x007ae1a0
boolean encode_mcu_AC_refine(j_compress_ptr c, JBLOCKROW* mcu_data) {
    const int se = c->Se;
    const int al = c->Al;
    PhuffEncoder* e = phuff_begin(c);
    const JCOEF* block = mcu_data[0][0];
    int absvalues[DCTSIZE2];
    int eob = 0;
    for (int k = c->Ss; k <= se; k++) {
        int temp = block[kNaturalOrder[k]];
        if (temp < 0) temp = -temp;
        temp = sar32(temp, al);
        absvalues[k] = temp;
        if (temp == 1) eob = k;
    }
    int r = 0;
    unsigned int br = 0;
    char* br_buffer = e->bit_buffer + e->BE;
    for (int k = c->Ss; k <= se; k++) {
        int temp = absvalues[k];
        if (temp == 0) {
            r++;
            continue;
        }
        while (r > 15 && k <= eob) {
            emit_eobrun(e);
            emit_symbol(e, e->ac_tbl_no, 0xf0);
            r -= 16;
            emit_buffered_bits(e, br_buffer, br);
            br_buffer = e->bit_buffer;
            br = 0;
        }
        if (temp > 1) {
            br_buffer[br++] = static_cast<char>(temp & 1);
            continue;
        }
        emit_eobrun(e);
        emit_symbol(e, e->ac_tbl_no, (r << 4) + 1);
        temp = block[kNaturalOrder[k]] < 0 ? 0 : 1;
        phuff_emit_bits(e, static_cast<unsigned int>(temp), 1);
        emit_buffered_bits(e, br_buffer, br);
        br_buffer = e->bit_buffer;
        br = 0;
        r = 0;
    }
    if (r > 0 || br > 0) {
        e->EOBRUN++;
        e->BE += br;
        if (e->EOBRUN == 0x7fff || e->BE > MAX_CORR_BITS - DCTSIZE2 + 1) emit_eobrun(e);
    }
    return phuff_end(c, e);
}
// 0x007ae3b0
void finish_pass_phuff(j_compress_ptr c) {
    PhuffEncoder* e = priv<PhuffEncoder>(c->entropy);
    e->next_output_byte = c->dest->next_output_byte;
    e->free_in_buffer = c->dest->free_in_buffer;
    emit_eobrun(e);
    phuff_flush_bits(e);
    c->dest->next_output_byte = e->next_output_byte;
    c->dest->free_in_buffer = e->free_in_buffer;
}
// 0x007ae3f0
void finish_pass_gather_phuff(j_compress_ptr c) {
    PhuffEncoder* e = priv<PhuffEncoder>(c->entropy);
    emit_eobrun(e);
    const bool is_DC_band = c->Ss == 0;
    boolean did[NUM_HUFF_TBLS] = {0, 0, 0, 0};
    for (int ci = 0; ci < c->comps_in_scan; ci++) {
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        int tbl;
        if (is_DC_band) {
            if (c->Ah != 0) continue;
            tbl = compptr->dc_tbl_no;
        } else {
            tbl = compptr->ac_tbl_no;
        }
        if (!did[tbl]) {
            JHUFF_TBL* htbl = is_DC_band ? c->dc_huff_tbl_ptrs[tbl] : c->ac_huff_tbl_ptrs[tbl];
            if (htbl == nullptr) {
                htbl = jpeg_alloc_huff_table(c);
                if (is_DC_band)
                    c->dc_huff_tbl_ptrs[tbl] = htbl;
                else
                    c->ac_huff_tbl_ptrs[tbl] = htbl;
            }
            jpeg_gen_optimal_table(c, htbl, e->count_ptrs[tbl]);
            did[tbl] = 1;
        }
    }
}
// 0x007ad950
void start_pass_phuff(j_compress_ptr c, boolean gather_statistics) {
    PhuffEncoder* e = priv<PhuffEncoder>(c->entropy);
    e->cinfo = c;
    e->gather_statistics = gather_statistics;
    const boolean is_DC_band = c->Ss == 0 ? 1 : 0;
    if (c->Ah == 0) {
        if (is_DC_band) e->pub.encode_mcu = &encode_mcu_DC_first;
        else e->pub.encode_mcu = &encode_mcu_AC_first;
    } else {
        if (is_DC_band) {
            e->pub.encode_mcu = &encode_mcu_DC_refine;
        } else {
            e->pub.encode_mcu = &encode_mcu_AC_refine;
            if (e->bit_buffer == nullptr)
                e->bit_buffer = static_cast<char*>(c->mem->alloc_small(c, JPOOL_IMAGE, MAX_CORR_BITS * SIZEOF<char>));
        }
    }
    if (gather_statistics) e->pub.finish_pass = &finish_pass_gather_phuff;
    else e->pub.finish_pass = &finish_pass_phuff;
    for (int ci = 0; ci < c->comps_in_scan; ci++) {
        jpeg_component_info* compptr = c->cur_comp_info[ci];
        e->last_dc_val[ci] = 0;
        int tbl;
        if (is_DC_band) {
            if (c->Ah != 0) continue;
            tbl = compptr->dc_tbl_no;
        } else {
            tbl = compptr->ac_tbl_no;
            e->ac_tbl_no = tbl;
        }
        if (gather_statistics) {
            if (tbl < 0 || tbl >= NUM_HUFF_TBLS) errexit1(c, JERR_NO_HUFF_TABLE, tbl);
            if (e->count_ptrs[tbl] == nullptr)
                e->count_ptrs[tbl] = static_cast<jlong*>(c->mem->alloc_small(c, JPOOL_IMAGE, 257 * SIZEOF<jlong>));
            std::memset(e->count_ptrs[tbl], 0, 257 * sizeof(jlong));
        } else {
            jpeg_make_c_derived_tbl(c, is_DC_band, tbl, &e->derived_tbls[tbl]);
        }
    }
    e->EOBRUN = 0;
    e->BE = 0;
    e->put_buffer = 0;
    e->put_bits = 0;
    e->restarts_to_go = c->restart_interval;
    e->next_restart_num = 0;
}

}  // namespace

// 0x007ad910
void jinit_phuff_encoder(j_compress_ptr c) {
    c_phuff_init.hit();
    auto* e = static_cast<PhuffEncoder*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<PhuffEncoder>));
    c->entropy = &e->pub;
    e->pub.start_pass = &start_pass_phuff;
    for (int i = 0; i < NUM_HUFF_TBLS; i++) {
        e->derived_tbls[i] = nullptr;
        e->count_ptrs[i] = nullptr;
    }
    e->bit_buffer = nullptr;
}

namespace {
// =============================================================================================
// jfdctint.c / jfdctfst.c / jfdctflt.c
// =============================================================================================
// 0x007b0cc0 jpeg_fdct_islow (CONST_BITS 13, PASS1_BITS 2)
void jpeg_fdct_islow(i32* data) {
    constexpr i32 F0298 = 2446, F0390 = 3196, F0541 = 4433, F0765 = 6270, F0899 = 7373, F1175 = 9633,
                  F1501 = 12299, F1847 = 15137, F1961 = 16069, F2053 = 16819, F2562 = 20995, F3072 = 25172;
    for (int pass = 0; pass < 2; ++pass) {
        const i32 step = pass == 0 ? 1 : 8;     // element stride inside a row / column
        const i32 next = pass == 0 ? 8 : 1;     // stride between rows / columns
        const i32 shift = pass == 0 ? 11 : 15;  // CONST_BITS -/+ PASS1_BITS
        i32* d = data;
        for (int ctr = 0; ctr < 8; ++ctr, d += next) {
            const i32 tmp0 = d[0 * step] + d[7 * step];
            i32 tmp7 = d[0 * step] - d[7 * step];
            const i32 tmp1 = d[1 * step] + d[6 * step];
            i32 tmp6 = d[1 * step] - d[6 * step];
            const i32 tmp2 = d[2 * step] + d[5 * step];
            i32 tmp5 = d[2 * step] - d[5 * step];
            const i32 tmp3 = d[3 * step] + d[4 * step];
            i32 tmp4 = d[3 * step] - d[4 * step];
            const i32 tmp10 = tmp0 + tmp3;
            const i32 tmp13 = tmp0 - tmp3;
            const i32 tmp11 = tmp1 + tmp2;
            const i32 tmp12 = tmp1 - tmp2;
            if (pass == 0) {
                d[0] = (tmp10 + tmp11) << 2;
                d[4] = (tmp10 - tmp11) << 2;
            } else {
                d[0 * step] = (tmp10 + tmp11 + 2) >> 2;
                d[4 * step] = (tmp10 - tmp11 + 2) >> 2;
            }
            const i32 round = 1 << (shift - 1);
            i32 z1 = (tmp12 + tmp13) * F0541;
            d[2 * step] = (z1 + tmp13 * F0765 + round) >> shift;
            d[6 * step] = (z1 + tmp12 * -F1847 + round) >> shift;
            z1 = tmp4 + tmp7;
            i32 z2 = tmp5 + tmp6;
            i32 z3 = tmp4 + tmp6;
            i32 z4 = tmp5 + tmp7;
            const i32 z5 = (z3 + z4) * F1175;
            tmp4 *= F0298;
            tmp5 *= F2053;
            tmp6 *= F3072;
            tmp7 *= F1501;
            z1 *= -F0899;
            z2 *= -F2562;
            z3 *= -F1961;
            z4 *= -F0390;
            z3 += z5;
            z4 += z5;
            d[7 * step] = (tmp4 + z1 + z3 + round) >> shift;
            d[5 * step] = (tmp5 + z2 + z4 + round) >> shift;
            d[3 * step] = (tmp6 + z2 + z3 + round) >> shift;
            d[1 * step] = (tmp7 + z1 + z4 + round) >> shift;
        }
    }
}
// 0x007b10c0 jpeg_fdct_ifast (CONST_BITS 8, truncating DESCALE)
void jpeg_fdct_ifast(i32* data) {
    for (int pass = 0; pass < 2; ++pass) {
        const i32 step = pass == 0 ? 1 : 8;
        const i32 next = pass == 0 ? 8 : 1;
        i32* d = data;
        for (int ctr = 0; ctr < 8; ++ctr, d += next) {
            const i32 tmp0 = d[0 * step] + d[7 * step];
            const i32 tmp7 = d[0 * step] - d[7 * step];
            const i32 tmp1 = d[1 * step] + d[6 * step];
            const i32 tmp6 = d[1 * step] - d[6 * step];
            const i32 tmp2 = d[2 * step] + d[5 * step];
            const i32 tmp5 = d[2 * step] - d[5 * step];
            const i32 tmp3 = d[3 * step] + d[4 * step];
            const i32 tmp4 = d[3 * step] - d[4 * step];
            i32 tmp10 = tmp0 + tmp3;
            const i32 tmp13 = tmp0 - tmp3;
            i32 tmp11 = tmp1 + tmp2;
            i32 tmp12 = tmp1 - tmp2;
            d[0 * step] = tmp10 + tmp11;
            d[4 * step] = tmp10 - tmp11;
            const i32 z1 = ((tmp12 + tmp13) * 181) >> 8;
            d[2 * step] = tmp13 + z1;
            d[6 * step] = tmp13 - z1;
            tmp10 = tmp4 + tmp5;
            tmp11 = tmp5 + tmp6;
            tmp12 = tmp6 + tmp7;
            const i32 z5 = ((tmp10 - tmp12) * 98) >> 8;
            const i32 z2 = ((tmp10 * 139) >> 8) + z5;
            const i32 z4 = ((tmp12 * 334) >> 8) + z5;
            const i32 z3 = (tmp11 * 181) >> 8;
            const i32 z11 = tmp7 + z3;
            const i32 z13 = tmp7 - z3;
            d[5 * step] = z13 + z2;
            d[3 * step] = z13 - z2;
            d[1 * step] = z11 + z4;
            d[7 * step] = z11 - z4;
        }
    }
}

// float constants of jfdctflt / jcdctmgr (bit patterns of joan.exe's .rdata)
constexpr float kF0_707106781 = std::bit_cast<float>(0x3f3504f3u);  // 0x009e6fc0
constexpr float kF0_382683433 = std::bit_cast<float>(0x3ec3ef15u);  // 0x009e6fbc
constexpr float kF0_541196100 = std::bit_cast<float>(0x3f0a8bd4u);  // 0x009e6fb8
constexpr float kF1_306562965 = std::bit_cast<float>(0x3fa73d75u);  // 0x009e6fb4
constexpr float kF16384_5 = std::bit_cast<float>(0x46800100u);      // 0x009e6fb0
constexpr double kD8 = 8.0;                                         // 0x009e6fa8
constexpr double kD1 = 1.0;                                         // 0x009e3400
constexpr std::uint64_t kAanScaleFactor[8] = {  // 0x009e6f68 (doubles)
    0x3ff0000000000000ull, 0x3ff63150b14861efull, 0x3ff4e7ae914d6fcaull, 0x3ff2d062ef6c11aaull,
    0x3ff0000000000000ull, 0x3fe92469c0a7bf3bull, 0x3fe1517a7bc720bbull, 0x3fd1a855de72ab5dull};
constexpr i16 kAanScales[64] = {  // 0x009e6ee8
    16384, 22725, 21407, 19266, 16384, 12873, 8867,  4520,  22725, 31521, 29692, 26722, 22725,
    17855, 12299, 6270,  21407, 29692, 27969, 25172, 21407, 16819, 11585, 5906,  19266, 26722,
    25172, 22654, 19266, 15137, 10426, 5315,  16384, 22725, 21407, 19266, 16384, 12873, 8867,
    4520,  12873, 17855, 16819, 15137, 12873, 10114, 6967,  3552,  8867,  12299, 11585, 10426,
    8867,  6967,  4799,  2446,  4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247};

// One 8-point pass of jpeg_fdct_float over p[0], p[step], .., p[7 * step]. The original's x87 sequence (jfdctflt.c
// in source order; operands of some additions swapped, which is exact) only has float operands (fld / fadd / fsub /
// fmul dword) and its stack temporaries are rounded to 24 bits at PC24 like every other step, so each step is one
// float operation here, one per statement (no contraction). Equal to the x87 as long as no intermediate leaves the
// float exponent range (the x87 registers keep a 15-bit exponent): the game's input is sample - 128 (integers
// -128..127), whose intermediates stay below 2^16 in magnitude and are 0 or at least 2^-48.
void fdct_float_pass(float* p, u32 step) {
    float* const d0 = p;
    float* const d1 = p + step;
    float* const d2 = p + 2 * step;
    float* const d3 = p + 3 * step;
    float* const d4 = p + 4 * step;
    float* const d5 = p + 5 * step;
    float* const d6 = p + 6 * step;
    float* const d7 = p + 7 * step;
    const float tmp0 = *d0 + *d7;
    const float tmp7 = *d0 - *d7;
    const float tmp1 = *d1 + *d6;
    const float tmp6 = *d1 - *d6;
    const float tmp2 = *d2 + *d5;
    const float tmp5 = *d2 - *d5;
    const float tmp3 = *d4 + *d3;
    const float tmp4 = *d3 - *d4;
    // even part
    const float tmp10 = tmp3 + tmp0;
    const float tmp13 = tmp0 - tmp3;
    const float tmp11 = tmp2 + tmp1;
    const float tmp12 = tmp1 - tmp2;
    *d0 = tmp11 + tmp10;
    *d4 = tmp10 - tmp11;
    const float s1 = tmp12 + tmp13;
    const float z1 = s1 * kF0_707106781;
    *d2 = tmp13 + z1;
    *d6 = tmp13 - z1;
    // odd part
    const float o10 = tmp4 + tmp5;
    const float o12 = tmp6 + tmp7;
    const float o10m12 = o10 - o12;
    const float z5 = o10m12 * kF0_382683433;
    const float m2 = o10 * kF0_541196100;
    const float z2 = m2 + z5;
    const float m4 = o12 * kF1_306562965;
    const float z4 = m4 + z5;
    const float o11 = tmp5 + tmp6;
    const float z3 = o11 * kF0_707106781;
    const float z11 = z3 + tmp7;
    const float z13 = tmp7 - z3;
    *d5 = z2 + z13;
    *d3 = z13 - z2;
    *d1 = z11 + z4;
    *d7 = z11 - z4;
}
// 0x007b1310 jpeg_fdct_float(FAST_FLOAT* data): rows, then columns.
void jpeg_fdct_float(float* data) {
    for (u32 r = 0; r < 8; ++r) fdct_float_pass(data + r * 8, 1);
    for (u32 c = 0; c < 8; ++c) fdct_float_pass(data + c, 8);
}

// float divisor of start_pass_fdctmgr: (float)(1.0 / ((double)q * col * row * 8.0)), the original's fild, fmul qword
// (the COLUMN factor first), fmul qword, fmul qword 8.0, fdivr qword 1.0, fstp dword: double operands at PC24, so
// each step is an x87::Fpu operation (exact product / quotient rounded once to 24 bits); the result is a float.
float float_divisor(i32 q, u32 col, u32 row) {
    const joan::hook::x87::Fpu fpu{true};
    double t = fpu.mul(static_cast<double>(q), std::bit_cast<double>(kAanScaleFactor[col]));
    t = fpu.mul(t, std::bit_cast<double>(kAanScaleFactor[row]));
    t = fpu.mul(t, kD8);
    return static_cast<float>(fpu.div(kD1, t));
}
// float quantiser of forward_DCT_float: _ftol(divisor * workspace + 16384.5f) (fld / fmul / fadd dword, _ftol on
// st0). Float operands: at PC24 the product and the sum are float operations. Out of the float exponent range the
// x87 keeps a finite value where SSE gets inf / a denormal, but _ftol gives the same for both: |values| >= 2^63 and
// inf / NaN all give 0 (integer indefinite, low dword), and a product below 2^-126 vanishes in + 16384.5 either way.
i32 float_quantize(float divisor, float ws) {
    const float p = divisor * ws;
    const float s = p + kF16384_5;
    return joan::hook::x87::ftol(s);
}

// =============================================================================================
// jcdctmgr.c
// =============================================================================================
// 0x007ae570
void start_pass_fdctmgr(j_compress_ptr c) {
    FdctController* fdct = priv<FdctController>(c->fdct);
    jpeg_component_info* compptr = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, compptr++) {
        const int qtblno = compptr->quant_tbl_no;
        if (qtblno < 0 || qtblno >= NUM_QUANT_TBLS || c->quant_tbl_ptrs[qtblno] == nullptr)
            errexit1(c, JERR_NO_QUANT_TABLE, qtblno);
        const JQUANT_TBL* qtbl = c->quant_tbl_ptrs[qtblno];
        switch (c->dct_method) {
            case JDCT_ISLOW: {
                if (fdct->divisors[qtblno] == nullptr)
                    fdct->divisors[qtblno] =
                        static_cast<DCTELEM*>(c->mem->alloc_small(c, JPOOL_IMAGE, DCTSIZE2 * SIZEOF<DCTELEM>));
                DCTELEM* dtbl = fdct->divisors[qtblno];
                for (int i = 0; i < DCTSIZE2; i++) dtbl[i] = static_cast<DCTELEM>(qtbl->quantval[i]) << 3;
                break;
            }
            case JDCT_IFAST: {
                if (fdct->divisors[qtblno] == nullptr)
                    fdct->divisors[qtblno] =
                        static_cast<DCTELEM*>(c->mem->alloc_small(c, JPOOL_IMAGE, DCTSIZE2 * SIZEOF<DCTELEM>));
                DCTELEM* dtbl = fdct->divisors[qtblno];
                for (int i = 0; i < DCTSIZE2; i++)
                    dtbl[i] = (static_cast<i32>(qtbl->quantval[i]) * kAanScales[i] + 0x400) >> 11;
                break;
            }
            case JDCT_FLOAT: {
                if (fdct->float_divisors[qtblno] == nullptr)
                    fdct->float_divisors[qtblno] =
                        static_cast<FAST_FLOAT*>(c->mem->alloc_small(c, JPOOL_IMAGE, DCTSIZE2 * SIZEOF<FAST_FLOAT>));
                FAST_FLOAT* fdtbl = fdct->float_divisors[qtblno];
                int i = 0;
                for (u32 row = 0; row < 8u; row++)
                    for (u32 col = 0; col < 8u; col++, i++)
                        fdtbl[i] = float_divisor(qtbl->quantval[i], col, row);
                break;
            }
            default: errexit(c, JERR_NOT_COMPILED); break;
        }
    }
}
// 0x007ae720
void forward_DCT(j_compress_ptr c, jpeg_component_info* compptr, JSAMPARRAY sample_data, JBLOCKROW coef_blocks,
                 JDIMENSION start_row, JDIMENSION start_col, JDIMENSION num_blocks) {
    FdctController* fdct = priv<FdctController>(c->fdct);
    const auto do_dct = fdct->do_dct;
    const DCTELEM* divisors = fdct->divisors[compptr->quant_tbl_no];
    sample_data += start_row;
    DCTELEM workspace[DCTSIZE2];
    for (JDIMENSION bi = 0; bi < num_blocks; bi++, start_col += DCTSIZE) {
        for (int r = 0; r < DCTSIZE; r++) {
            const JSAMPLE* elem = sample_data[r] + start_col;
            for (int k = 0; k < DCTSIZE; k++) workspace[r * DCTSIZE + k] = static_cast<DCTELEM>(elem[k]) - 128;
        }
        do_dct(workspace);
        JCOEF* output_ptr = coef_blocks[bi];
        for (int i = 0; i < DCTSIZE2; i++) {
            const DCTELEM qval = divisors[i];
            DCTELEM temp = workspace[i];
            if (temp < 0) {
                temp = -temp + (qval >> 1);
                temp = temp >= qval ? temp / qval : 0;
                temp = -temp;
            } else {
                temp += qval >> 1;
                temp = temp >= qval ? temp / qval : 0;
            }
            output_ptr[i] = static_cast<JCOEF>(temp);
        }
    }
}
// 0x007ae890
void forward_DCT_float(j_compress_ptr c, jpeg_component_info* compptr, JSAMPARRAY sample_data, JBLOCKROW coef_blocks,
                       JDIMENSION start_row, JDIMENSION start_col, JDIMENSION num_blocks) {
    FdctController* fdct = priv<FdctController>(c->fdct);
    const auto do_dct = fdct->do_float_dct;
    const FAST_FLOAT* divisors = fdct->float_divisors[compptr->quant_tbl_no];
    sample_data += start_row;
    FAST_FLOAT workspace[DCTSIZE2];
    for (JDIMENSION bi = 0; bi < num_blocks; bi++, start_col += DCTSIZE) {
        for (int r = 0; r < DCTSIZE; r++) {
            const JSAMPLE* elem = sample_data[r] + start_col;
            for (int k = 0; k < DCTSIZE; k++)
                workspace[r * DCTSIZE + k] = static_cast<FAST_FLOAT>(static_cast<int>(elem[k]) - 128);
        }
        do_dct(workspace);
        JCOEF* output_ptr = coef_blocks[bi];
        for (int i = 0; i < DCTSIZE2; i++)
            output_ptr[i] = static_cast<JCOEF>(float_quantize(divisors[i], workspace[i]) - 0x4000);
    }
}

}  // namespace

// 0x007ae4e0
void jinit_forward_dct(j_compress_ptr c) {
    c_fdct_init.hit();
    auto* fdct = static_cast<FdctController*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<FdctController>));
    c->fdct = &fdct->pub;
    fdct->pub.start_pass = &start_pass_fdctmgr;
    switch (c->dct_method) {
        case JDCT_ISLOW:
            fdct->do_dct = &jpeg_fdct_islow;
            fdct->pub.forward_DCT = &forward_DCT;
            break;
        case JDCT_IFAST:
            fdct->do_dct = &jpeg_fdct_ifast;
            fdct->pub.forward_DCT = &forward_DCT;
            break;
        case JDCT_FLOAT:
            fdct->pub.forward_DCT = &forward_DCT_float;
            fdct->do_float_dct = &jpeg_fdct_float;
            break;
        default: errexit(c, JERR_NOT_COMPILED); break;
    }
    for (int i = 0; i < NUM_QUANT_TBLS; i++) {
        fdct->divisors[i] = nullptr;
        fdct->float_divisors[i] = nullptr;
    }
}

#define LIB(name, addr, f) JOAN_REPLACE("lib/b115_06", name, addr, f)
// jcmarker
LIB("jpeg::jinit_marker_writer", 0x007ab730, &jinit_marker_writer);
LIB("jpeg::write_marker_header", 0x007ab790, &write_marker_header);
LIB("jpeg::emit_marker", 0x007ab7d0, &emit_marker);
LIB("jpeg::emit_byte", 0x007ab7f0, &emit_byte);
LIB("jpeg::emit_2bytes", 0x007ab830, &emit_2bytes);
LIB("jpeg::write_marker_byte", 0x007ab860, &emit_byte);
LIB("jpeg::write_file_header", 0x007ab870, &write_file_header);
LIB("jpeg::emit_jfif_app0", 0x007ab8c0, &emit_jfif_app0);
LIB("jpeg::emit_adobe_app14", 0x007ab970, &emit_adobe_app14);
LIB("jpeg::write_frame_header", 0x007aba10, &write_frame_header);
LIB("jpeg::emit_dqt", 0x007abb10, &emit_dqt);
LIB("jpeg::emit_sof", 0x007abbe0, &emit_sof);
LIB("jpeg::write_scan_header", 0x007abca0, &write_scan_header);
LIB("jpeg::emit_dht", 0x007abd80, &emit_dht);
LIB("jpeg::emit_dac", 0x007abe40, &emit_dac);
LIB("jpeg::emit_dri", 0x007abe50, &emit_dri);
LIB("jpeg::emit_sos", 0x007abe80, &emit_sos);
LIB("jpeg::write_file_trailer", 0x007abf60, &write_file_trailer);
LIB("jpeg::write_tables_only", 0x007abf80, &write_tables_only);
// jutils
LIB("jpeg::jdiv_round_up", 0x007ac150, &jdiv_round_up);
LIB("jpeg::jround_up", 0x007ac160, &jround_up);
LIB("jpeg::jcopy_sample_rows", 0x007ac180, &jcopy_sample_rows);
LIB("jpeg::jcopy_block_row", 0x007ac1d0, &jcopy_block_row);
LIB("jpeg::jzero_far", 0x007ac200, &jzero_far);
// jchuff
LIB("jpeg::jpeg_make_c_derived_tbl", 0x007acba0, &jpeg_make_c_derived_tbl);
LIB("jpeg::jpeg_gen_optimal_table", 0x007acda0, &jpeg_gen_optimal_table);
LIB("jpeg::jinit_huff_encoder", 0x007acfc0, &jinit_huff_encoder);
LIB("jpeg::start_pass_huff", 0x007ad000, &start_pass_huff);
LIB("jpeg::encode_mcu_huff", 0x007ad170, &encode_mcu_huff);
LIB("jpeg::encode_one_block", 0x007ad2a0, &encode_one_block);
LIB("jpeg::huff_emit_bits", 0x007ad430, &huff_emit_bits);
LIB("jpeg::huff_dump_buffer", 0x007ad500, &huff_dump_buffer);
LIB("jpeg::huff_emit_restart", 0x007ad530, &huff_emit_restart);
LIB("jpeg::huff_flush_bits", 0x007ad5c0, &huff_flush_bits);
LIB("jpeg::finish_pass_huff", 0x007ad5f0, &finish_pass_huff);
LIB("jpeg::encode_mcu_gather", 0x007ad670, &encode_mcu_gather);
LIB("jpeg::htest_one_block", 0x007ad740, &htest_one_block);
LIB("jpeg::finish_pass_gather", 0x007ad820, &finish_pass_gather);
// jcphuff
LIB("jpeg::jinit_phuff_encoder", 0x007ad910, &jinit_phuff_encoder);
LIB("jpeg::start_pass_phuff", 0x007ad950, &start_pass_phuff);
LIB("jpeg::encode_mcu_DC_first", 0x007adad0, &encode_mcu_DC_first);
LIB("jpeg::phuff_emit_bits", 0x007adc20, &phuff_emit_bits);
LIB("jpeg::phuff_dump_buffer", 0x007add00, &phuff_dump_buffer);
LIB("jpeg::emit_symbol", 0x007add40, &emit_symbol);
LIB("jpeg::phuff_emit_restart", 0x007add90, &phuff_emit_restart);
LIB("jpeg::phuff_flush_bits", 0x007ade40, &phuff_flush_bits);
LIB("jpeg::emit_eobrun", 0x007ade60, &emit_eobrun);
LIB("jpeg::emit_buffered_bits", 0x007adee0, &emit_buffered_bits);
LIB("jpeg::encode_mcu_AC_first", 0x007adf20, &encode_mcu_AC_first);
LIB("jpeg::encode_mcu_DC_refine", 0x007ae0e0, &encode_mcu_DC_refine);
LIB("jpeg::encode_mcu_AC_refine", 0x007ae1a0, &encode_mcu_AC_refine);
LIB("jpeg::finish_pass_phuff", 0x007ae3b0, &finish_pass_phuff);
LIB("jpeg::finish_pass_gather_phuff", 0x007ae3f0, &finish_pass_gather_phuff);
// jcdctmgr / jfdct
LIB("jpeg::jinit_forward_dct", 0x007ae4e0, &jinit_forward_dct);
LIB("jpeg::start_pass_fdctmgr", 0x007ae570, &start_pass_fdctmgr);
LIB("jpeg::forward_DCT", 0x007ae720, &forward_DCT);
LIB("jpeg::forward_DCT_float", 0x007ae890, &forward_DCT_float);
LIB("jpeg::jpeg_fdct_islow", 0x007b0cc0, &jpeg_fdct_islow);
LIB("jpeg::jpeg_fdct_ifast", 0x007b10c0, &jpeg_fdct_ifast);
LIB("jpeg::jpeg_fdct_float", 0x007b1310, &jpeg_fdct_float);
// jcomapi (D3DX8's copy)
LIB("jpeg::jpeg_abort", 0x007888f7, &jpeg_abort);
LIB("jpeg::jpeg_destroy", 0x00788919, &jpeg_destroy);
LIB("jpeg::jpeg_alloc_quant_table", 0x00788934, &jpeg_alloc_quant_table);
LIB("jpeg::jpeg_alloc_huff_table", 0x00788950, &jpeg_alloc_huff_table);
#undef LIB

}  // namespace joan::jpeg
