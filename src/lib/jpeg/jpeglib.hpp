// The game's IJG libjpeg 6b compressor (joan.exe 0x007a9a60..0x007b1540 + D3DX8's jcomapi / jerror), typed.
//
// joan.exe links two libjpeg copies: D3DX8's (decoder, default packing: jcomapi 0x007888f7.., jerror 0x00765e32..)
// and the game's own compressor, built with /Zp1 and a 1-byte `boolean`, MAX_COMPONENTS 10. Its only user is the
// screenshot writer (elib/jpeg.cpp, Screenshot::write_jpeg 0x005a9040, the only caller of jpeg_CreateCompress; no
// save game or thumbnail code uses it). The compressor's objects are allocated from the game heap (cinfo 0x15b /
// err 0x84 bytes by the Jpeg object at 0x00a69b40) and the jmemmgr pools (malloc), never from the data image; the
// only data-image object involved is the Jpeg object (pointers to cinfo / err, typed by the generated jo::Jpeg).
//
// Layouts: on x86 (UINTPTR_MAX == 0xffffffff) the game's structures are packed exactly like the original (/Zp1, the
// offsets are asserted), so the bytes written to cinfo, the pools and the heap are those of joan.exe. On other
// targets they are natural C++ structures with pointer members (sizes from sizeof, never from x86 numbers). The three
// D3DX8 structures (JQUANT_TBL 0x82, JHUFF_TBL 0x112, jpeg_error_mgr 0x84 on x86) use default packing on every
// target, as in D3DX8. Integer widths are the game's on every target: JDIMENSION / `long` / the size arithmetic of
// the memory manager are 32-bit (jsize), so the arithmetic (and its wraparound) is the original's.
//
// The quirks of the original are kept and named where they appear (jpeg_abort / jpeg_destroy: D3DX's aligned
// jcomapi applied to the packed struct, see compress_entropy.cpp).
#pragma once

#include <cstddef>
#include <cstdint>

#if UINTPTR_MAX == 0xffffffffu
#define JOAN_JPEG_X86_LAYOUT 1  // the original packed layout (x86 only)
#else
#define JOAN_JPEG_X86_LAYOUT 0
#endif

namespace joan::jpeg {

using boolean = std::uint8_t;  // the game's copy: 1 byte (tested as al)
using JDIMENSION = std::uint32_t;
using JSAMPLE = std::uint8_t;
using JSAMPROW = JSAMPLE*;
using JSAMPARRAY = JSAMPROW*;
using JSAMPIMAGE = JSAMPARRAY*;
using JCOEF = std::int16_t;
using JBLOCK = JCOEF[64];
using JBLOCKROW = JBLOCK*;
using JBLOCKARRAY = JBLOCKROW*;
using JOCTET = std::uint8_t;
using UINT16 = std::uint16_t;
using jlong = std::int32_t;   // the game's `long`
using jsize = std::uint32_t;  // the game's size_t (32-bit arithmetic on every target)
// SIZEOF() of jconfig.h: an object size in the game's size type.
template <class T>
inline constexpr jsize SIZEOF = static_cast<jsize>(sizeof(T));

inline constexpr int JPEG_LIB_VERSION = 62;
inline constexpr int DCTSIZE = 8, DCTSIZE2 = 64;
inline constexpr int NUM_QUANT_TBLS = 4, NUM_HUFF_TBLS = 4, NUM_ARITH_TBLS = 16;
inline constexpr int MAX_COMPS_IN_SCAN = 4, MAX_COMPONENTS = 10, C_MAX_BLOCKS_IN_MCU = 10, MAX_SAMP_FACTOR = 4;
inline constexpr int JPOOL_PERMANENT = 0, JPOOL_IMAGE = 1, JPOOL_NUMPOOLS = 2;
inline constexpr int CSTATE_START = 100, CSTATE_SCANNING = 101, CSTATE_RAW_OK = 102, CSTATE_WRCOEFS = 103;
inline constexpr int DSTATE_START = 200;
// J_COLOR_SPACE
inline constexpr int JCS_UNKNOWN = 0, JCS_GRAYSCALE = 1, JCS_RGB = 2, JCS_YCbCr = 3, JCS_CMYK = 4, JCS_YCCK = 5;
// J_DCT_METHOD
inline constexpr int JDCT_ISLOW = 0, JDCT_IFAST = 1, JDCT_FLOAT = 2;
// J_BUF_MODE
inline constexpr int JBUF_PASS_THRU = 0, JBUF_SAVE_SOURCE = 1, JBUF_CRANK_DEST = 2, JBUF_SAVE_AND_PASS = 3;

struct jpeg_compress_struct;
using j_compress_ptr = jpeg_compress_struct*;

// ------------------------------------------------------------------------------------- D3DX8 structures (natural)
struct JQUANT_TBL {
    UINT16 quantval[DCTSIZE2];
    boolean sent_table;
};
struct JHUFF_TBL {
    std::uint8_t bits[17];
    std::uint8_t huffval[256];
    boolean sent_table;
};
struct jpeg_error_mgr {
    void (*error_exit)(j_compress_ptr cinfo);
    void (*emit_message)(j_compress_ptr cinfo, int msg_level);
    void (*output_message)(j_compress_ptr cinfo);
    void (*format_message)(j_compress_ptr cinfo, char* buffer);
    void (*reset_error_mgr)(j_compress_ptr cinfo);
    int msg_code;
    union {
        int i[8];
        char s[80];
    } msg_parm;
    int trace_level;
    jlong num_warnings;
    const char* const* jpeg_message_table;
    int last_jpeg_message;
    const char* const* addon_message_table;
    int first_addon_message;
    int last_addon_message;
};

// ------------------------------------------------------------------------- the game's structures (packed on x86)
// jvirt_sarray_control / jvirt_barray_control (one layout, defined in compress_core.cpp)
template <class Array>
struct jvirt_array_control;
using jvirt_sarray_ptr = jvirt_array_control<JSAMPARRAY>*;
using jvirt_barray_ptr = jvirt_array_control<JBLOCKARRAY>*;

#if JOAN_JPEG_X86_LAYOUT
#pragma pack(push, 1)
#endif
struct jpeg_component_info {
    int component_id;
    int component_index;
    int h_samp_factor;
    int v_samp_factor;
    int quant_tbl_no;
    int dc_tbl_no;
    int ac_tbl_no;
    JDIMENSION width_in_blocks;
    JDIMENSION height_in_blocks;
    int DCT_scaled_size;
    JDIMENSION downsampled_width;
    JDIMENSION downsampled_height;
    boolean component_needed;
    int MCU_width;
    int MCU_height;
    int MCU_blocks;
    int MCU_sample_width;
    int last_col_width;
    int last_row_height;
    JQUANT_TBL* quant_table;
    void* dct_table;
};
struct jpeg_scan_info {
    int comps_in_scan;
    int component_index[MAX_COMPS_IN_SCAN];
    int Ss, Se;
    int Ah, Al;
};
struct jpeg_progress_mgr {
    void (*progress_monitor)(j_compress_ptr cinfo);
    jlong pass_counter;
    jlong pass_limit;
    int completed_passes;
    int total_passes;
};
struct jpeg_destination_mgr {
    JOCTET* next_output_byte;
    jsize free_in_buffer;
    void (*init_destination)(j_compress_ptr cinfo);
    boolean (*empty_output_buffer)(j_compress_ptr cinfo);
    void (*term_destination)(j_compress_ptr cinfo);
};
struct jpeg_memory_mgr {
    void* (*alloc_small)(j_compress_ptr cinfo, int pool_id, jsize sizeofobject);
    void* (*alloc_large)(j_compress_ptr cinfo, int pool_id, jsize sizeofobject);
    JSAMPARRAY (*alloc_sarray)(j_compress_ptr cinfo, int pool_id, JDIMENSION samplesperrow, JDIMENSION numrows);
    JBLOCKARRAY (*alloc_barray)(j_compress_ptr cinfo, int pool_id, JDIMENSION blocksperrow, JDIMENSION numrows);
    jvirt_sarray_ptr (*request_virt_sarray)(j_compress_ptr cinfo, int pool_id, boolean pre_zero,
                                            JDIMENSION samplesperrow, JDIMENSION numrows, JDIMENSION maxaccess);
    jvirt_barray_ptr (*request_virt_barray)(j_compress_ptr cinfo, int pool_id, boolean pre_zero,
                                            JDIMENSION blocksperrow, JDIMENSION numrows, JDIMENSION maxaccess);
    void (*realize_virt_arrays)(j_compress_ptr cinfo);
    JSAMPARRAY (*access_virt_sarray)(j_compress_ptr cinfo, jvirt_sarray_ptr ptr, JDIMENSION start_row,
                                     JDIMENSION num_rows, boolean writable);
    JBLOCKARRAY (*access_virt_barray)(j_compress_ptr cinfo, jvirt_barray_ptr ptr, JDIMENSION start_row,
                                      JDIMENSION num_rows, boolean writable);
    void (*free_pool)(j_compress_ptr cinfo, int pool_id);
    void (*self_destruct)(j_compress_ptr cinfo);
    jlong max_memory_to_use;
    jlong max_alloc_chunk;
};

// jpegint.h: the public parts of the compressor's modules
struct jpeg_comp_master {
    void (*prepare_for_pass)(j_compress_ptr cinfo);
    void (*pass_startup)(j_compress_ptr cinfo);
    void (*finish_pass)(j_compress_ptr cinfo);
    boolean call_pass_startup;
    boolean is_last_pass;
};
struct jpeg_c_main_controller {
    void (*start_pass)(j_compress_ptr cinfo, int pass_mode);
    void (*process_data)(j_compress_ptr cinfo, JSAMPARRAY input_buf, JDIMENSION* in_row_ctr,
                         JDIMENSION in_rows_avail);
};
struct jpeg_c_prep_controller {
    void (*start_pass)(j_compress_ptr cinfo, int pass_mode);
    void (*pre_process_data)(j_compress_ptr cinfo, JSAMPARRAY input_buf, JDIMENSION* in_row_ctr,
                             JDIMENSION in_rows_avail, JSAMPIMAGE output_buf, JDIMENSION* out_row_group_ctr,
                             JDIMENSION out_row_groups_avail);
};
struct jpeg_c_coef_controller {
    void (*start_pass)(j_compress_ptr cinfo, int pass_mode);
    boolean (*compress_data)(j_compress_ptr cinfo, JSAMPIMAGE input_buf);
};
struct jpeg_color_converter {
    void (*start_pass)(j_compress_ptr cinfo);
    void (*color_convert)(j_compress_ptr cinfo, JSAMPARRAY input_buf, JSAMPIMAGE output_buf, JDIMENSION output_row,
                          int num_rows);
};
struct jpeg_downsampler {
    void (*start_pass)(j_compress_ptr cinfo);
    void (*downsample)(j_compress_ptr cinfo, JSAMPIMAGE input_buf, JDIMENSION in_row_index, JSAMPIMAGE output_buf,
                       JDIMENSION out_row_group_index);
    boolean need_context_rows;
};
struct jpeg_forward_dct {
    void (*start_pass)(j_compress_ptr cinfo);
    void (*forward_DCT)(j_compress_ptr cinfo, jpeg_component_info* compptr, JSAMPARRAY sample_data,
                        JBLOCKROW coef_blocks, JDIMENSION start_row, JDIMENSION start_col, JDIMENSION num_blocks);
};
struct jpeg_entropy_encoder {
    void (*start_pass)(j_compress_ptr cinfo, boolean gather_statistics);
    boolean (*encode_mcu)(j_compress_ptr cinfo, JBLOCKROW* MCU_data);
    void (*finish_pass)(j_compress_ptr cinfo);
};
struct jpeg_marker_writer {
    void (*write_file_header)(j_compress_ptr cinfo);
    void (*write_frame_header)(j_compress_ptr cinfo);
    void (*write_scan_header)(j_compress_ptr cinfo);
    void (*write_file_trailer)(j_compress_ptr cinfo);
    void (*write_tables_only)(j_compress_ptr cinfo);
    void (*write_marker_header)(j_compress_ptr cinfo, int marker, unsigned int datalen);
    void (*write_marker_byte)(j_compress_ptr cinfo, int val);
};

struct jpeg_compress_struct {
    jpeg_error_mgr* err;
    jpeg_memory_mgr* mem;
    jpeg_progress_mgr* progress;
    void* client_data;
    boolean is_decompressor;
    int global_state;
    jpeg_destination_mgr* dest;
    JDIMENSION image_width;
    JDIMENSION image_height;
    int input_components;
    int in_color_space;
    double input_gamma;
    int data_precision;
    int num_components;
    int jpeg_color_space;
    jpeg_component_info* comp_info;
    JQUANT_TBL* quant_tbl_ptrs[NUM_QUANT_TBLS];
    JHUFF_TBL* dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
    JHUFF_TBL* ac_huff_tbl_ptrs[NUM_HUFF_TBLS];
    std::uint8_t arith_dc_L[NUM_ARITH_TBLS];
    std::uint8_t arith_dc_U[NUM_ARITH_TBLS];
    std::uint8_t arith_ac_K[NUM_ARITH_TBLS];
    int num_scans;
    const jpeg_scan_info* scan_info;
    boolean raw_data_in;
    boolean arith_code;
    boolean optimize_coding;
    boolean CCIR601_sampling;
    int smoothing_factor;
    int dct_method;
    unsigned int restart_interval;
    int restart_in_rows;
    boolean write_JFIF_header;
    std::uint8_t JFIF_major_version;
    std::uint8_t JFIF_minor_version;
    std::uint8_t density_unit;
    UINT16 X_density;
    UINT16 Y_density;
    boolean write_Adobe_marker;
    JDIMENSION next_scanline;
    boolean progressive_mode;
    int max_h_samp_factor;
    int max_v_samp_factor;
    JDIMENSION total_iMCU_rows;
    int comps_in_scan;
    jpeg_component_info* cur_comp_info[MAX_COMPS_IN_SCAN];
    JDIMENSION MCUs_per_row;
    JDIMENSION MCU_rows_in_scan;
    int blocks_in_MCU;
    int MCU_membership[C_MAX_BLOCKS_IN_MCU];
    int Ss, Se, Ah, Al;
    jpeg_comp_master* master;
    jpeg_c_main_controller* main;
    jpeg_c_prep_controller* prep;
    jpeg_c_coef_controller* coef;
    jpeg_marker_writer* marker;
    jpeg_color_converter* cconvert;
    jpeg_downsampler* downsample;
    jpeg_forward_dct* fdct;
    jpeg_entropy_encoder* entropy;
    jpeg_scan_info* script_space;
    int script_space_size;
};
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(pop)
#endif

#if JOAN_JPEG_X86_LAYOUT  // the original layouts (joan.exe's code / D3DX8's)
static_assert(sizeof(JQUANT_TBL) == 0x82 && offsetof(JQUANT_TBL, sent_table) == 0x80);
static_assert(sizeof(JHUFF_TBL) == 0x112 && offsetof(JHUFF_TBL, sent_table) == 0x111);
static_assert(sizeof(jpeg_error_mgr) == 0x84 && offsetof(jpeg_error_mgr, msg_parm) == 0x18 &&
              offsetof(jpeg_error_mgr, trace_level) == 0x68 && offsetof(jpeg_error_mgr, jpeg_message_table) == 0x70 &&
              offsetof(jpeg_error_mgr, last_addon_message) == 0x80);
static_assert(sizeof(jpeg_component_info) == 0x51 && offsetof(jpeg_component_info, component_needed) == 0x30 &&
              offsetof(jpeg_component_info, MCU_width) == 0x31 && offsetof(jpeg_component_info, quant_table) == 0x49);
static_assert(sizeof(jpeg_scan_info) == 0x24);
static_assert(sizeof(jpeg_progress_mgr) == 0x14);
static_assert(sizeof(jpeg_destination_mgr) == 0x14);
static_assert(sizeof(jpeg_memory_mgr) == 0x34 && offsetof(jpeg_memory_mgr, free_pool) == 0x24 &&
              offsetof(jpeg_memory_mgr, self_destruct) == 0x28);
static_assert(sizeof(jpeg_comp_master) == 0xe && sizeof(jpeg_downsampler) == 9);
static_assert(sizeof(jpeg_compress_struct) == 0x15b);
static_assert(offsetof(jpeg_compress_struct, is_decompressor) == 0x10 &&
              offsetof(jpeg_compress_struct, global_state) == 0x11 && offsetof(jpeg_compress_struct, dest) == 0x15 &&
              offsetof(jpeg_compress_struct, input_gamma) == 0x29 &&
              offsetof(jpeg_compress_struct, comp_info) == 0x3d &&
              offsetof(jpeg_compress_struct, quant_tbl_ptrs) == 0x41 &&
              offsetof(jpeg_compress_struct, num_scans) == 0xa1 &&
              offsetof(jpeg_compress_struct, smoothing_factor) == 0xad &&
              offsetof(jpeg_compress_struct, write_JFIF_header) == 0xbd &&
              offsetof(jpeg_compress_struct, X_density) == 0xc1 &&
              offsetof(jpeg_compress_struct, next_scanline) == 0xc6 &&
              offsetof(jpeg_compress_struct, progressive_mode) == 0xca &&
              offsetof(jpeg_compress_struct, cur_comp_info) == 0xdb &&
              offsetof(jpeg_compress_struct, MCU_membership) == 0xf7 && offsetof(jpeg_compress_struct, Ss) == 0x11f &&
              offsetof(jpeg_compress_struct, master) == 0x12f && offsetof(jpeg_compress_struct, marker) == 0x13f &&
              offsetof(jpeg_compress_struct, entropy) == 0x14f &&
              offsetof(jpeg_compress_struct, script_space) == 0x153);
#endif

// ------------------------------------------------------------------------------------------------ error codes
// (this build's message table, D3DX8 .rdata 0x009e1c68, last 0x77)
inline constexpr int JERR_ARITH_NOTIMPL = 1, JERR_BAD_BUFFER_MODE = 4, JERR_BAD_DCT_COEF = 6, JERR_BAD_HUFF_TABLE = 8,
                     JERR_BAD_IN_COLORSPACE = 9, JERR_BAD_J_COLORSPACE = 10, JERR_BAD_LENGTH = 11,
                     JERR_BAD_LIB_VERSION = 12, JERR_BAD_MCU_SIZE = 13, JERR_BAD_POOL_ID = 14,
                     JERR_BAD_PRECISION = 15, JERR_BAD_PROG_SCRIPT = 17, JERR_BAD_SAMPLING = 18,
                     JERR_BAD_SCAN_SCRIPT = 19, JERR_BAD_STATE = 20, JERR_BAD_STRUCT_SIZE = 21,
                     JERR_BAD_VIRTUAL_ACCESS = 22, JERR_CANT_SUSPEND = 24, JERR_CCIR601_NOTIMPL = 25,
                     JERR_COMPONENT_COUNT = 26, JERR_CONVERSION_NOTIMPL = 27, JERR_DQT_INDEX = 31,
                     JERR_EMPTY_IMAGE = 32, JERR_FILE_WRITE = 37, JERR_FRACT_SAMPLE_NOTIMPL = 38,
                     JERR_HUFF_CLEN_OVERFLOW = 39, JERR_HUFF_MISSING_CODE = 40, JERR_IMAGE_TOO_BIG = 41,
                     JERR_MISSING_DATA = 45, JERR_NOT_COMPILED = 48, JERR_NO_BACKING_STORE = 49,
                     JERR_NO_HUFF_TABLE = 50, JERR_NO_QUANT_TABLE = 52, JERR_OUT_OF_MEMORY = 54,
                     JERR_TOO_LITTLE_DATA = 67, JERR_VIRTUAL_BUG = 69, JERR_WIDTH_OVERFLOW = 70,
                     JTRC_16BIT_TABLES = 75, JTRC_SMOOTH_NOTIMPL = 99, JWRN_TOO_MUCH_DATA = 123;
inline constexpr int JMSG_LAST_MESSAGE_CODE = 0x77;  // last_jpeg_message of the D3DX8 table

// ---------------------------------------------------------------------------------------- entry points (C++)
// compress_api.cpp: jcapimin / jcapistd / jcparam / jdatadst (the game's copy) and jerror (D3DX8's copy).
jpeg_error_mgr* jpeg_std_error(jpeg_error_mgr* err);
void jpeg_CreateCompress(j_compress_ptr cinfo, int version, jsize structsize);
void jpeg_destroy_compress(j_compress_ptr cinfo);
void jpeg_abort_compress(j_compress_ptr cinfo);
void jpeg_suppress_tables(j_compress_ptr cinfo, boolean suppress);
void jpeg_finish_compress(j_compress_ptr cinfo);
void jpeg_start_compress(j_compress_ptr cinfo, boolean write_all_tables);
JDIMENSION jpeg_write_scanlines(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines);
void jpeg_add_quant_table(j_compress_ptr cinfo, int which_tbl, const unsigned int* basic_table, int scale_factor,
                          boolean force_baseline);
void jpeg_set_linear_quality(j_compress_ptr cinfo, int scale_factor, boolean force_baseline);
int jpeg_quality_scaling(int quality);
void jpeg_set_quality(j_compress_ptr cinfo, int quality, boolean force_baseline);
void jpeg_set_defaults(j_compress_ptr cinfo);
void jpeg_default_colorspace(j_compress_ptr cinfo);
void jpeg_set_colorspace(j_compress_ptr cinfo, int colorspace);
void jpeg_stdio_dest(j_compress_ptr cinfo, void* outfile);  // outfile: the game CRT's FILE

// compress_core.cpp (jmemmgr, jcinit)
void jinit_memory_mgr(j_compress_ptr cinfo);
void jinit_compress_master(j_compress_ptr cinfo);

// compress_entropy.cpp (jcomapi, D3DX8's copy)
void jpeg_abort(j_compress_ptr cinfo);
void jpeg_destroy(j_compress_ptr cinfo);
JQUANT_TBL* jpeg_alloc_quant_table(j_compress_ptr cinfo);
JHUFF_TBL* jpeg_alloc_huff_table(j_compress_ptr cinfo);

// compress_entropy.cpp: the modules jinit_compress_master starts, and jutils
void jinit_marker_writer(j_compress_ptr cinfo);
void jinit_huff_encoder(j_compress_ptr cinfo);
void jinit_phuff_encoder(j_compress_ptr cinfo);
void jinit_forward_dct(j_compress_ptr cinfo);
jlong jdiv_round_up(jlong a, jlong b);
jlong jround_up(jlong a, jlong b);
void jcopy_sample_rows(JSAMPARRAY input_array, int source_row, JSAMPARRAY output_array, int dest_row, int num_rows,
                       JDIMENSION num_cols);
void jzero_far(void* target, jsize bytestozero);

// ERREXIT* / WARNMS / TRACEMS of jerror.h (cinfo->err re-read for every access, like the original)
inline void errexit(j_compress_ptr c, int code) {
    c->err->msg_code = code;
    c->err->error_exit(c);
}
inline void errexit1(j_compress_ptr c, int code, int p1) {
    c->err->msg_code = code;
    c->err->msg_parm.i[0] = p1;
    c->err->error_exit(c);
}
inline void errexit2(j_compress_ptr c, int code, int p1, int p2) {
    c->err->msg_code = code;
    c->err->msg_parm.i[0] = p1;
    c->err->msg_parm.i[1] = p2;
    c->err->error_exit(c);
}
inline void emit_msg(j_compress_ptr c, int code, int level) {
    c->err->msg_code = code;
    c->err->emit_message(c, level);
}

}  // namespace joan::jpeg
