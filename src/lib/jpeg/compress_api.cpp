// Original library sources (re/attribution.csv original_file, under lib/): jpeg/jcapimin.c, jpeg/jcapistd.c,
//   jpeg/jcparam.c, jpeg/jdatadst.c, d3dx8/jpeg/jerror.c.
//
// The compression API of the game's libjpeg (IJG 6b, /Zp1, 1-byte boolean) and D3DX8's error manager, typed C++
// over jpeglib.hpp: everything the game's Jpeg object and screenshot writer (elib/jpeg.cpp) call, directly. Every
// function is also registered at its original address (modules "lib/misc" and "lib/b113_06", the names of the
// raw-offset copies this file replaces, which lived in lib/d3dx8/objects.cpp and lib/crt/string.cpp): the method
// pointers it stores (the error manager's, the stdio destination's) are then the code slots' pointers of those
// addresses, as before (the replay normalises code pointers in the heap by their slots). Not registered:
// std_huff_tables 0x007aa2b0 / add_huff_table 0x007aa310 (their only callers were the original jpeg_set_defaults and
// std_huff_tables; this add_huff_table names the table slot by index instead of by a pointer into the packed cinfo).
//   jcapimin.c 0x007a9a60 jpeg_CreateCompress / 0x007a9b30 jpeg_destroy_compress (`jmp` to D3DX8's jpeg_destroy) /
//              0x007a9b40 jpeg_abort_compress (`jmp` to D3DX8's jpeg_abort) / 0x007a9b50 jpeg_suppress_tables /
//              0x007a9ba0 jpeg_finish_compress (jpeg_write_marker / jpeg_write_tables follow it: no caller)
//   jcapistd.c 0x007a9e00 jpeg_start_compress / 0x007a9e80 jpeg_write_scanlines
//   jcparam.c  0x007aa020 jpeg_add_quant_table / 0x007aa100 jpeg_set_linear_quality / 0x007aa140
//              jpeg_quality_scaling / 0x007aa180 jpeg_set_quality / 0x007aa1b0 jpeg_set_defaults / 0x007aa2b0
//              std_huff_tables / 0x007aa310 add_huff_table / 0x007aa3b0 jpeg_default_colorspace / 0x007aa430
//              jpeg_set_colorspace (jpeg_simple_progression and its fill_* helpers after it: no caller)
//   jdatadst.c 0x007aa9e0 jpeg_stdio_dest / 0x007aaa20 init_destination / 0x007aaa50 empty_output_buffer /
//              0x007aaaa0 term_destination (fwrite / fflush and the VC6 ferror macro on the CRT's FILE)
//   jerror.c   0x00765f43 jpeg_std_error / 0x00765e32 error_exit / 0x00765e4d output_message (D3DX8 build:
//              formats into a 200-byte buffer and discards it) / 0x00765e6a emit_message / 0x00765e9b
//              format_message (sprintf) / 0x00765f32 reset_error_mgr; message table 0x009e1c68 (last 0x77)
// The standard quantisation / Huffman tables (.rdata 0x009e69e0 / 0x009e6ae0 / 0x009e6be0..0x009e6d8f) are our own
// constants (JPEG Annex K, the bytes of joan.exe).
//
// Proof: re/p3/jpeg runs this file with compress_core.cpp / compress_entropy.cpp against joan.exe's own code.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "jpeg_crt.hpp"
#include "jpeglib.hpp"
#include "../../registry.hpp"

namespace joan::jpeg {
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

void bad_state(j_compress_ptr c) { errexit1(c, JERR_BAD_STATE, c->global_state); }

// =============================================================================================
// jerror.c (D3DX8's copy)
// =============================================================================================
constexpr int JMSG_LENGTH_MAX = 200;

// 0x00765e9b format_message
void format_message(j_compress_ptr c, char* buffer) {
    jpeg_error_mgr* err = c->err;
    const int msg_code = err->msg_code;
    const char* msgtext = nullptr;
    if (msg_code > 0 && msg_code <= err->last_jpeg_message) {
        msgtext = err->jpeg_message_table[msg_code];
    } else if (err->addon_message_table != nullptr && msg_code >= err->first_addon_message &&
               msg_code <= err->last_addon_message) {
        msgtext = err->addon_message_table[msg_code - err->first_addon_message];
    }
    if (msgtext == nullptr) {
        err->msg_parm.i[0] = msg_code;
        msgtext = err->jpeg_message_table[0];
    }
    bool isstring = false;
    for (const char* msgptr = msgtext; *msgptr != '\0';) {
        const char ch = *msgptr++;
        if (ch == '%') {
            if (*msgptr == 's') isstring = true;
            break;
        }
    }
    if (isstring)
        crt::sprintf_s(buffer, msgtext, err->msg_parm.s);
    else
        crt::sprintf_i8(buffer, msgtext, err->msg_parm.i);
}
// 0x00765e4d output_message: format into a JMSG_LENGTH_MAX buffer, then nothing (D3DX8 build)
void output_message(j_compress_ptr c) {
    char buffer[JMSG_LENGTH_MAX];
    c->err->format_message(c, buffer);
}
// 0x00765e6a emit_message
void emit_message(j_compress_ptr c, int msg_level) {
    jpeg_error_mgr* err = c->err;
    if (msg_level < 0) {
        if (err->num_warnings == 0 || err->trace_level >= 3) err->output_message(c);
        err->num_warnings++;
    } else {
        if (err->trace_level >= msg_level) err->output_message(c);
    }
}
// 0x00765e32 error_exit
void error_exit(j_compress_ptr c) {
    c->err->output_message(c);
    jpeg_destroy(c);
    crt::exit_failure();
}
// 0x00765f32 reset_error_mgr
void reset_error_mgr(j_compress_ptr c) {
    c->err->num_warnings = 0;
    c->err->msg_code = 0;
}

}  // namespace

// 0x00765f43
jpeg_error_mgr* jpeg_std_error(jpeg_error_mgr* err) {
    err->error_exit = &error_exit;
    err->emit_message = &emit_message;
    err->output_message = &output_message;
    err->format_message = &format_message;
    err->reset_error_mgr = &reset_error_mgr;
    err->trace_level = 0;
    err->num_warnings = 0;
    err->msg_code = 0;
    err->jpeg_message_table = crt::message_table();
    err->last_jpeg_message = JMSG_LAST_MESSAGE_CODE;
    err->addon_message_table = nullptr;
    err->first_addon_message = 0;
    err->last_addon_message = 0;
    return err;
}

// =============================================================================================
// jcapimin.c
// =============================================================================================
// 0x007a9a60
void jpeg_CreateCompress(j_compress_ptr c, int version, jsize structsize) {
    c->mem = nullptr;
    if (version != JPEG_LIB_VERSION) errexit2(c, JERR_BAD_LIB_VERSION, JPEG_LIB_VERSION, version);
    if (structsize != SIZEOF<jpeg_compress_struct>)
        errexit2(c, JERR_BAD_STRUCT_SIZE, static_cast<int>(SIZEOF<jpeg_compress_struct>), static_cast<int>(structsize));
    {
        jpeg_error_mgr* err = c->err;
        void* client_data = c->client_data;
        std::memset(static_cast<void*>(c), 0, sizeof(jpeg_compress_struct));
        c->err = err;
        c->client_data = client_data;
    }
    c->is_decompressor = 0;
    jinit_memory_mgr(c);
    c->progress = nullptr;
    c->dest = nullptr;
    c->comp_info = nullptr;
    for (int i = 0; i < NUM_QUANT_TBLS; i++) c->quant_tbl_ptrs[i] = nullptr;
    for (int i = 0; i < NUM_HUFF_TBLS; i++) {
        c->dc_huff_tbl_ptrs[i] = nullptr;
        c->ac_huff_tbl_ptrs[i] = nullptr;
    }
    c->script_space = nullptr;
    c->input_gamma = 1.0;
    c->global_state = CSTATE_START;
}

// 0x007a9b30 (originally `jmp` D3DX8's jpeg_destroy 0x00788919)
void jpeg_destroy_compress(j_compress_ptr c) { jpeg_destroy(c); }
// 0x007a9b40 (originally `jmp` D3DX8's jpeg_abort 0x007888f7)
void jpeg_abort_compress(j_compress_ptr c) { jpeg_abort(c); }

// 0x007a9b50
void jpeg_suppress_tables(j_compress_ptr c, boolean suppress) {
    for (int i = 0; i < NUM_QUANT_TBLS; i++)
        if (c->quant_tbl_ptrs[i] != nullptr) c->quant_tbl_ptrs[i]->sent_table = suppress;
    for (int i = 0; i < NUM_HUFF_TBLS; i++) {
        if (c->dc_huff_tbl_ptrs[i] != nullptr) c->dc_huff_tbl_ptrs[i]->sent_table = suppress;
        if (c->ac_huff_tbl_ptrs[i] != nullptr) c->ac_huff_tbl_ptrs[i]->sent_table = suppress;
    }
}

// 0x007a9ba0
void jpeg_finish_compress(j_compress_ptr c) {
    if (c->global_state == CSTATE_SCANNING || c->global_state == CSTATE_RAW_OK) {
        if (c->next_scanline < c->image_height) errexit(c, JERR_TOO_LITTLE_DATA);
        c->master->finish_pass(c);
    } else if (c->global_state != CSTATE_WRCOEFS) {
        bad_state(c);
    }
    while (c->master->is_last_pass == 0) {
        c->master->prepare_for_pass(c);
        for (JDIMENSION iMCU_row = 0; iMCU_row < c->total_iMCU_rows; iMCU_row++) {
            if (c->progress != nullptr) {
                c->progress->pass_counter = static_cast<jlong>(iMCU_row);
                c->progress->pass_limit = static_cast<jlong>(c->total_iMCU_rows);
                c->progress->progress_monitor(c);
            }
            if (c->coef->compress_data(c, nullptr) == 0) errexit(c, JERR_CANT_SUSPEND);
        }
        c->master->finish_pass(c);
    }
    c->marker->write_file_trailer(c);
    c->dest->term_destination(c);
    jpeg_abort(c);
}

// =============================================================================================
// jcapistd.c
// =============================================================================================
// 0x007a9e00
void jpeg_start_compress(j_compress_ptr c, boolean write_all_tables) {
    if (c->global_state != CSTATE_START) bad_state(c);
    if (write_all_tables != 0) jpeg_suppress_tables(c, 0);
    c->err->reset_error_mgr(c);
    c->dest->init_destination(c);
    jinit_compress_master(c);
    c->master->prepare_for_pass(c);
    c->next_scanline = 0;
    c->global_state = c->raw_data_in != 0 ? CSTATE_RAW_OK : CSTATE_SCANNING;
}

// 0x007a9e80
JDIMENSION jpeg_write_scanlines(j_compress_ptr c, JSAMPARRAY scanlines, JDIMENSION num_lines) {
    if (c->global_state != CSTATE_SCANNING) bad_state(c);
    if (c->next_scanline >= c->image_height) emit_msg(c, JWRN_TOO_MUCH_DATA, -1);
    if (c->progress != nullptr) {
        c->progress->pass_counter = static_cast<jlong>(c->next_scanline);
        c->progress->pass_limit = static_cast<jlong>(c->image_height);
        c->progress->progress_monitor(c);
    }
    if (c->master->call_pass_startup != 0) c->master->pass_startup(c);
    const JDIMENSION rows_left = c->image_height - c->next_scanline;
    if (num_lines > rows_left) num_lines = rows_left;
    JDIMENSION row_ctr = 0;
    c->main->process_data(c, scanlines, &row_ctr, num_lines);
    c->next_scanline += row_ctr;
    return row_ctr;
}

// =============================================================================================
// jcparam.c
// =============================================================================================
namespace {
constexpr unsigned int kStdLuminanceQuant[DCTSIZE2] = {
    16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
    14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};
constexpr unsigned int kStdChrominanceQuant[DCTSIZE2] = {
    17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99,
    99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99};
constexpr u8 kBitsDcLuminance[17] = {0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
constexpr u8 kBitsDcChrominance[17] = {0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
constexpr u8 kValDc[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
constexpr u8 kBitsAcLuminance[17] = {0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
constexpr u8 kValAcLuminance[162] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71,
    0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72,
    0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3,
    0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
constexpr u8 kBitsAcChrominance[17] = {0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
constexpr u8 kValAcChrominance[162] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1,
    0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a,
    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
    0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};

// 0x007aa310 add_huff_table(cinfo, htblptr, bits, val); the table slot is named by (is_ac, index) here
void add_huff_table(j_compress_ptr c, bool is_ac, int index, const u8* bits, const u8* val) {
    JHUFF_TBL* htbl = is_ac ? c->ac_huff_tbl_ptrs[index] : c->dc_huff_tbl_ptrs[index];
    if (htbl == nullptr) {
        htbl = jpeg_alloc_huff_table(c);
        if (is_ac)
            c->ac_huff_tbl_ptrs[index] = htbl;
        else
            c->dc_huff_tbl_ptrs[index] = htbl;
    }
    std::memcpy(htbl->bits, bits, sizeof(htbl->bits));
    int nsymbols = 0;
    for (int len = 1; len <= 16; len++) nsymbols += bits[len];
    if (nsymbols < 1 || nsymbols > 256) errexit(c, JERR_BAD_HUFF_TABLE);
    std::memcpy(htbl->huffval, val, static_cast<std::size_t>(nsymbols));
    htbl->sent_table = 0;
}
// 0x007aa2b0 std_huff_tables
void std_huff_tables(j_compress_ptr c) {
    add_huff_table(c, false, 0, kBitsDcLuminance, kValDc);
    add_huff_table(c, true, 0, kBitsAcLuminance, kValAcLuminance);
    add_huff_table(c, false, 1, kBitsDcChrominance, kValDc);
    add_huff_table(c, true, 1, kBitsAcChrominance, kValAcChrominance);
}

// SET_COMP
void set_comp(j_compress_ptr c, int index, int id, int hsamp, int vsamp, int quant, int dctbl, int actbl) {
    jpeg_component_info* compptr = &c->comp_info[index];
    compptr->component_id = id;
    compptr->h_samp_factor = hsamp;
    compptr->v_samp_factor = vsamp;
    compptr->quant_tbl_no = quant;
    compptr->dc_tbl_no = dctbl;
    compptr->ac_tbl_no = actbl;
}
}  // namespace

// 0x007aa020
void jpeg_add_quant_table(j_compress_ptr c, int which_tbl, const unsigned int* basic_table, int scale_factor,
                          boolean force_baseline) {
    if (c->global_state != CSTATE_START) bad_state(c);
    if (which_tbl < 0 || which_tbl >= NUM_QUANT_TBLS) errexit1(c, JERR_DQT_INDEX, which_tbl);
    if (c->quant_tbl_ptrs[which_tbl] == nullptr) c->quant_tbl_ptrs[which_tbl] = jpeg_alloc_quant_table(c);
    for (int i = 0; i < DCTSIZE2; i++) {
        // (long) basic_table[i] * scale_factor + 50L in the game's 32-bit long (wrapping), then / 100
        jlong temp = static_cast<jlong>(basic_table[i] * static_cast<u32>(scale_factor) + 50u) / 100;
        if (temp <= 0) temp = 1;
        if (temp > 32767) temp = 32767;
        if (force_baseline != 0 && temp > 255) temp = 255;
        c->quant_tbl_ptrs[which_tbl]->quantval[i] = static_cast<UINT16>(temp);
    }
    c->quant_tbl_ptrs[which_tbl]->sent_table = 0;
}

// 0x007aa100
void jpeg_set_linear_quality(j_compress_ptr c, int scale_factor, boolean force_baseline) {
    jpeg_add_quant_table(c, 0, kStdLuminanceQuant, scale_factor, force_baseline);
    jpeg_add_quant_table(c, 1, kStdChrominanceQuant, scale_factor, force_baseline);
}

// 0x007aa140
int jpeg_quality_scaling(int quality) {
    if (quality <= 0) quality = 1;
    if (quality > 100) quality = 100;
    if (quality < 50)
        quality = 5000 / quality;
    else
        quality = 200 - quality * 2;
    return quality;
}

// 0x007aa180
void jpeg_set_quality(j_compress_ptr c, int quality, boolean force_baseline) {
    quality = jpeg_quality_scaling(quality);
    jpeg_set_linear_quality(c, quality, force_baseline);
}

// 0x007aa1b0
void jpeg_set_defaults(j_compress_ptr c) {
    if (c->global_state != CSTATE_START) bad_state(c);
    if (c->comp_info == nullptr)
        c->comp_info = static_cast<jpeg_component_info*>(
            c->mem->alloc_small(c, JPOOL_PERMANENT, MAX_COMPONENTS * SIZEOF<jpeg_component_info>));
    c->data_precision = 8;
    jpeg_set_quality(c, 75, 1);
    std_huff_tables(c);
    for (int i = 0; i < NUM_ARITH_TBLS; i++) {
        c->arith_dc_L[i] = 0;
        c->arith_dc_U[i] = 1;
        c->arith_ac_K[i] = 5;
    }
    c->scan_info = nullptr;
    c->num_scans = 0;
    c->raw_data_in = 0;
    c->arith_code = 0;
    c->optimize_coding = 0;
    if (c->data_precision > 8) c->optimize_coding = 1;
    c->CCIR601_sampling = 0;
    c->smoothing_factor = 0;
    c->dct_method = JDCT_ISLOW;
    c->restart_interval = 0;
    c->restart_in_rows = 0;
    c->JFIF_major_version = 1;
    c->JFIF_minor_version = 1;
    c->density_unit = 0;
    c->X_density = 1;
    c->Y_density = 1;
    jpeg_default_colorspace(c);
}

// 0x007aa3b0
void jpeg_default_colorspace(j_compress_ptr c) {
    switch (c->in_color_space) {
        case JCS_GRAYSCALE: jpeg_set_colorspace(c, JCS_GRAYSCALE); break;
        case JCS_RGB: jpeg_set_colorspace(c, JCS_YCbCr); break;
        case JCS_YCbCr: jpeg_set_colorspace(c, JCS_YCbCr); break;
        case JCS_CMYK: jpeg_set_colorspace(c, JCS_CMYK); break;
        case JCS_YCCK: jpeg_set_colorspace(c, JCS_YCCK); break;
        case JCS_UNKNOWN: jpeg_set_colorspace(c, JCS_UNKNOWN); break;
        default: errexit(c, JERR_BAD_IN_COLORSPACE); break;
    }
}

// 0x007aa430
void jpeg_set_colorspace(j_compress_ptr c, int colorspace) {
    if (c->global_state != CSTATE_START) bad_state(c);
    c->jpeg_color_space = colorspace;
    c->write_JFIF_header = 0;
    c->write_Adobe_marker = 0;
    switch (colorspace) {
        case JCS_GRAYSCALE:
            c->write_JFIF_header = 1;
            c->num_components = 1;
            set_comp(c, 0, 1, 1, 1, 0, 0, 0);
            break;
        case JCS_RGB:
            c->write_Adobe_marker = 1;
            c->num_components = 3;
            set_comp(c, 0, 0x52, 1, 1, 0, 0, 0);
            set_comp(c, 1, 0x47, 1, 1, 0, 0, 0);
            set_comp(c, 2, 0x42, 1, 1, 0, 0, 0);
            break;
        case JCS_YCbCr:
            c->write_JFIF_header = 1;
            c->num_components = 3;
            set_comp(c, 0, 1, 2, 2, 0, 0, 0);
            set_comp(c, 1, 2, 1, 1, 1, 1, 1);
            set_comp(c, 2, 3, 1, 1, 1, 1, 1);
            break;
        case JCS_CMYK:
            c->write_Adobe_marker = 1;
            c->num_components = 4;
            set_comp(c, 0, 0x43, 1, 1, 0, 0, 0);
            set_comp(c, 1, 0x4d, 1, 1, 0, 0, 0);
            set_comp(c, 2, 0x59, 1, 1, 0, 0, 0);
            set_comp(c, 3, 0x4b, 1, 1, 0, 0, 0);
            break;
        case JCS_YCCK:
            c->write_Adobe_marker = 1;
            c->num_components = 4;
            set_comp(c, 0, 1, 2, 2, 0, 0, 0);
            set_comp(c, 1, 2, 1, 1, 1, 1, 1);
            set_comp(c, 2, 3, 1, 1, 1, 1, 1);
            set_comp(c, 3, 4, 2, 2, 0, 0, 0);
            break;
        case JCS_UNKNOWN:
            c->num_components = c->input_components;
            if (c->num_components < 1 || c->num_components > MAX_COMPONENTS)
                errexit2(c, JERR_COMPONENT_COUNT, c->num_components, MAX_COMPONENTS);
            for (int ci = 0; ci < c->num_components; ci++) set_comp(c, ci, ci, 1, 1, 0, 0, 0);
            break;
        default:
            errexit(c, JERR_BAD_J_COLORSPACE);
            break;
    }
}

// =============================================================================================
// jdatadst.c
// =============================================================================================
namespace {
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(push, 1)
#endif
struct StdioDest {  // my_destination_mgr
    jpeg_destination_mgr pub;
    void* outfile;  // the game CRT's FILE*
    JOCTET* buffer;
};
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(pop)
static_assert(sizeof(StdioDest) == 0x1c, "my_destination_mgr");
#endif
constexpr jsize OUTPUT_BUF_SIZE = 4096;

StdioDest* stdio_dest(j_compress_ptr c) { return reinterpret_cast<StdioDest*>(c->dest); }

// 0x007aaa20
void init_destination(j_compress_ptr c) {
    StdioDest* dest = stdio_dest(c);
    dest->buffer = static_cast<JOCTET*>(c->mem->alloc_small(c, JPOOL_IMAGE, OUTPUT_BUF_SIZE * SIZEOF<JOCTET>));
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}
// 0x007aaa50
boolean empty_output_buffer(j_compress_ptr c) {
    StdioDest* dest = stdio_dest(c);
    if (crt::fwrite(dest->buffer, 1, OUTPUT_BUF_SIZE, dest->outfile) != OUTPUT_BUF_SIZE) errexit(c, JERR_FILE_WRITE);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    return 1;
}
// 0x007aaaa0
void term_destination(j_compress_ptr c) {
    StdioDest* dest = stdio_dest(c);
    const jsize datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
    if (datacount > 0) {
        if (crt::fwrite(dest->buffer, 1, datacount, dest->outfile) != datacount) errexit(c, JERR_FILE_WRITE);
    }
    crt::fflush(dest->outfile);
    if (crt::ferror(dest->outfile)) errexit(c, JERR_FILE_WRITE);
}
}  // namespace

// 0x007aa9e0
void jpeg_stdio_dest(j_compress_ptr c, void* outfile) {
    if (c->dest == nullptr)
        c->dest = static_cast<jpeg_destination_mgr*>(c->mem->alloc_small(c, JPOOL_PERMANENT, SIZEOF<StdioDest>));
    StdioDest* dest = stdio_dest(c);
    dest->pub.init_destination = &init_destination;
    dest->pub.empty_output_buffer = &empty_output_buffer;
    dest->pub.term_destination = &term_destination;
    dest->outfile = outfile;
}

// =============================================================================================
// Registration (entry patches; the names and modules of the copies this file replaces)
// =============================================================================================
#define L(name, addr, f) JOAN_REPLACE("lib/misc", name, addr, f)
L("jpeg_std_error", 0x00765f43, &jpeg_std_error);
L("jpeg error_exit", 0x00765e32, &error_exit);
L("jpeg output_message", 0x00765e4d, &output_message);
L("jpeg emit_message", 0x00765e6a, &emit_message);
L("jpeg format_message", 0x00765e9b, &format_message);
L("jpeg reset_error_mgr", 0x00765f32, &reset_error_mgr);
L("jpeg_CreateCompress", 0x007a9a60, &jpeg_CreateCompress);
L("jpeg_destroy_compress", 0x007a9b30, &jpeg_destroy_compress);
L("jpeg_abort_compress", 0x007a9b40, &jpeg_abort_compress);
L("jpeg_suppress_tables", 0x007a9b50, &jpeg_suppress_tables);
L("jpeg_finish_compress", 0x007a9ba0, &jpeg_finish_compress);
L("jpeg_start_compress", 0x007a9e00, &jpeg_start_compress);
L("jpeg_write_scanlines", 0x007a9e80, &jpeg_write_scanlines);
L("jpeg_add_quant_table", 0x007aa020, &jpeg_add_quant_table);
L("jpeg_set_linear_quality", 0x007aa100, &jpeg_set_linear_quality);
L("jpeg_quality_scaling", 0x007aa140, &jpeg_quality_scaling);
L("jpeg_default_colorspace", 0x007aa3b0, &jpeg_default_colorspace);
L("jpeg_set_colorspace", 0x007aa430, &jpeg_set_colorspace);
L("jpeg dest init_destination", 0x007aaa20, &init_destination);
L("jpeg dest empty_output_buffer", 0x007aaa50, &empty_output_buffer);
L("jpeg dest term_destination", 0x007aaaa0, &term_destination);
#undef L
#define LIB(name, addr, f) JOAN_REPLACE("lib/b113_06", name, addr, f)
LIB("jpeg::jpeg_set_quality", 0x007aa180, &jpeg_set_quality);
LIB("jpeg::jpeg_set_defaults", 0x007aa1b0, &jpeg_set_defaults);
LIB("jpeg::jpeg_stdio_dest", 0x007aa9e0, &jpeg_stdio_dest);
#undef LIB

}  // namespace joan::jpeg
