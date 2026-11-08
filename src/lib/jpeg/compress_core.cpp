// Original library sources (re/attribution.csv original_file, under lib/): jpeg/jccoefct.c, jpeg/jccolor.c,
//   jpeg/jcinit.c, jpeg/jcmainct.c, jpeg/jcmaster.c, jpeg/jcprepct.c, jpeg/jcsample.c, jpeg/jmemmgr.c,
//   jpeg/jmemnobs.c.
//
// IJG libjpeg 6b compressor internals, part 1 (the game's copy: /Zp1, 1-byte boolean, MAX_COMPONENTS 10), typed
// C++ over the structures of jpeglib.hpp (packed exactly like the original on x86, natural elsewhere). Module
// "lib/b115_05". This file = memory manager (jmemmgr + jmemnobs), jcinit, jcmaster, jcmainct, jccoefct, jcprepct,
// jcsample, jccolor; compress_entropy.cpp = jcmarker, jutils, jchuff, jcphuff, jcdctmgr, jfdct*, jcomapi;
// compress_api.cpp = jcapimin, jcapistd, jcparam, jdatadst, jerror. Calls between the modules are direct C++ calls
// or go through the cinfo method tables (real function pointers), exactly where the original calls.
//
// Replaced (address, IJG name; every function is also registered at its original address):
//  jmemmgr.c  0x007aab10 jinit_memory_mgr / 0x007aac50 alloc_small / 0x007aad80 out_of_memory / 0x007aada0
//             alloc_large / 0x007aae50 alloc_sarray / 0x007aaef0 alloc_barray / 0x007aafa0 request_virt_sarray /
//             0x007ab010 request_virt_barray / 0x007ab080 realize_virt_arrays / 0x007ab210 access_virt_sarray /
//             0x007ab360 do_sarray_io / 0x007ab400 access_virt_barray / 0x007ab550 do_barray_io / 0x007ab5f0
//             free_pool / 0x007ab6f0 self_destruct
//  jmemnobs.c 0x007ac0c0 jpeg_get_small / 0x007ac0e0 jpeg_get_large (malloc 0x007b6c54), 0x007ac0d0
//             jpeg_free_small / 0x007ac0f0 jpeg_free_large (free 0x007b4e1d), 0x007ac100 jpeg_mem_available
//             (returns max_bytes_needed), 0x007ac110 jpeg_open_backing_store (ERREXIT JERR_NO_BACKING_STORE),
//             0x007ac130 jpeg_mem_init (0) / 0x007ac140 jpeg_mem_term (nothing)
//  jcinit.c   0x007ac000 jinit_compress_master
//  jcmainct.c 0x007ac220 jinit_c_main_controller / 0x007ac2b0 start_pass_main / 0x007ac300 process_data_simple_main
//  jccoefct.c 0x007ac3b0 jinit_c_coef_controller / 0x007ac480 start_pass_coef / 0x007ac530 start_iMCU_row /
//             0x007ac580 compress_data / 0x007ac7d0 compress_first_pass / 0x007ac9e0 compress_output
//  jcprepct.c 0x007aea30 jinit_c_prep_controller / 0x007aeae0 start_pass_prep / 0x007aeb30 pre_process_data /
//             0x007aecb0 expand_bottom_edge / 0x007aecf0 pre_process_context / 0x007aeec0 create_context_buffer
//  jcsample.c 0x007af000 jinit_downsampler / 0x007af180 start_pass_downsample / 0x007af190 sep_downsample /
//             0x007af220 int_downsample / 0x007af340 expand_right_edge / 0x007af3a0 fullsize_downsample /
//             0x007af3f0 h2v1_downsample / 0x007af4a0 h2v2_downsample / 0x007af570 h2v2_smooth_downsample /
//             0x007af860 fullsize_smooth_downsample
//  jccolor.c  0x007af9f0 jinit_color_converter / 0x007afc30 rgb_ycc_start / 0x007afd10 rgb_ycc_convert / 0x007afe40
//             rgb_gray_convert / 0x007afef0 cmyk_ycck_convert / 0x007b0060 grayscale_convert / 0x007b00d0
//             null_convert / 0x007b0160 null_method
//  jcmaster.c 0x007b0170 jinit_c_master_control / 0x007b0230 initial_setup / 0x007b0400 validate_script /
//             0x007b07a0 prepare_for_pass / 0x007b0940 select_scan_parameters / 0x007b0a40 per_scan_setup /
//             0x007b0c10 pass_startup / 0x007b0c40 finish_pass_master
//
// State: cinfo, the pools and the pool / virtual-array structures are the original's on x86 (jpeglib.hpp layouts,
// asserted below); the same malloc / free calls (sizes, order) are made, so heap state and the (uninitialised)
// contents of pool memory are identical (re/p3/jpeg compares the heap of the original and of this code byte for
// byte). Object sizes are sizeof of the typed structures (on x86 the original numbers); pointer arrays are sized with
// sizeof(pointer). Data: the slop tables (0x009e6d94 {1600, 16000}, 0x009e6d9c {0, 5000}) and the strings "JPEGMEM"
// (0x00a30928) / "%ld%c" (0x00a30920) are our own constants (getenv / sscanf only read the text).
//
// Deviations: none on any reachable path. Paths that only exist when error_exit returns (it never does: the game's
// error_exit ends the game) behave as the C source / the original code: execution continues after the ERREXIT. One
// exception: validate_script with num_components > 10 (only reachable after a returned JERR_COMPONENT_COUNT)
// overruns the original's 0xa2c-byte frame; ours clamps the -1 fill / component_sent clear to the 10-entry arrays.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "jpeg_crt.hpp"
#include "jpeglib.hpp"
#include "../../registry.hpp"

namespace joan::jpeg {
namespace {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

constexpr jsize kMaxAllocChunk = 1000000000u;
constexpr jsize kFirstPoolSlop[JPOOL_NUMPOOLS] = {1600, 16000};  // 0x009e6d94
constexpr jsize kExtraPoolSlop[JPOOL_NUMPOOLS] = {0, 5000};      // 0x009e6d9c
constexpr jsize kAlignType = 8;                                  // SIZEOF(ALIGN_TYPE) (double)

// ---------------------------------------------------------------------------------------------
// Private structures (jmemmgr.c / jmemsys.h / jc*.c of IJG 6b; packed on x86 like the original)
// ---------------------------------------------------------------------------------------------
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(push, 1)
#endif
struct PoolHdr {  // small_pool_hdr / large_pool_hdr (the union with the 8-byte ALIGN_TYPE adds nothing)
    PoolHdr* next;
    jsize bytes_used;
    jsize bytes_left;
};
struct BackingStore {  // backing_store_info (jmemsys.h); never opened (jmemnobs)
    void (*read_backing_store)(j_compress_ptr cinfo, BackingStore* info, void* buffer_address, jlong file_offset,
                               jlong byte_count);
    void (*write_backing_store)(j_compress_ptr cinfo, BackingStore* info, void* buffer_address, jlong file_offset,
                                jlong byte_count);
    void (*close_backing_store)(j_compress_ptr cinfo, BackingStore* info);
    void* temp_file;
    char temp_name[64];
};
}  // namespace

// jvirt_sarray_control (Array = JSAMPARRAY) / jvirt_barray_control (Array = JBLOCKARRAY): one layout
template <class Array>
struct jvirt_array_control {
    Array mem_buffer;
    JDIMENSION rows_in_array;
    JDIMENSION per_row;  // samplesperrow / blocksperrow
    JDIMENSION maxaccess;
    JDIMENSION rows_in_mem;
    JDIMENSION rowsperchunk;
    JDIMENSION cur_start_row;
    JDIMENSION first_undef_row;
    boolean pre_zero;
    boolean dirty;
    boolean b_s_open;
    jvirt_array_control* next;
    BackingStore b_s_info;
};

namespace {
struct MemMgr {  // my_memory_mgr
    jpeg_memory_mgr pub;
    PoolHdr* small_list[JPOOL_NUMPOOLS];
    PoolHdr* large_list[JPOOL_NUMPOOLS];
    jvirt_sarray_ptr virt_sarray_list;
    jvirt_barray_ptr virt_barray_list;
    jlong total_space_allocated;
    JDIMENSION last_rowsperchunk;
};
struct Master {  // my_comp_master
    jpeg_comp_master pub;
    int pass_type;
    int pass_number;
    int total_passes;
    int scan_number;
};
struct MainC {  // my_main_controller
    jpeg_c_main_controller pub;
    JDIMENSION cur_iMCU_row;
    JDIMENSION rowgroup_ctr;
    boolean suspended;
    int pass_mode;
    JSAMPARRAY buffer[MAX_COMPONENTS];
};
struct Coef {  // my_coef_controller
    jpeg_c_coef_controller pub;
    JDIMENSION iMCU_row_num;
    JDIMENSION mcu_ctr;
    int MCU_vert_offset;
    int MCU_rows_per_iMCU_row;
    JBLOCKROW MCU_buffer[C_MAX_BLOCKS_IN_MCU];
    jvirt_barray_ptr whole_image[MAX_COMPONENTS];
};
struct Prep {  // my_prep_controller
    jpeg_c_prep_controller pub;
    JSAMPARRAY color_buf[MAX_COMPONENTS];
    JDIMENSION rows_to_go;
    int next_buf_row;
    int this_row_group;
    int next_buf_stop;
};
using Downsample1 = void (*)(j_compress_ptr cinfo, jpeg_component_info* compptr, JSAMPARRAY input_data,
                             JSAMPARRAY output_data);
struct Downsample {  // my_downsampler
    jpeg_downsampler pub;
    Downsample1 methods[MAX_COMPONENTS];
};
struct CConvert {  // my_color_converter
    jpeg_color_converter pub;
    i32* rgb_ycc_tab;
};
#if JOAN_JPEG_X86_LAYOUT
#pragma pack(pop)
#endif

using VArrayS = jvirt_array_control<JSAMPARRAY>;
using VArrayB = jvirt_array_control<JBLOCKARRAY>;

#if JOAN_JPEG_X86_LAYOUT  // the original layouts
static_assert(sizeof(PoolHdr) == 12, "pool header");
static_assert(sizeof(VArrayS) == 0x77 && sizeof(VArrayB) == 0x77 && offsetof(VArrayS, next) == 0x23,
              "jvirt_*array_control");
static_assert(sizeof(MemMgr) == 0x54 && offsetof(MemMgr, small_list) == 0x34, "my_memory_mgr");
static_assert(sizeof(Master) == 0x1e && offsetof(Master, pass_type) == 0xe, "my_comp_master");
static_assert(sizeof(MainC) == 0x3d && offsetof(MainC, pass_mode) == 0x11, "my_main_controller");
static_assert(sizeof(Coef) == 0x68 && offsetof(Coef, whole_image) == 0x40, "my_coef_controller");
static_assert(sizeof(Prep) == 0x40 && offsetof(Prep, rows_to_go) == 0x30, "my_prep_controller");
static_assert(sizeof(Downsample) == 0x31 && offsetof(Downsample, methods) == 9, "my_downsampler");
static_assert(sizeof(CConvert) == 0xc, "my_color_converter");
#endif

// Private struct of a public method table (the public part is the first member, as in IJG).
template <class T, class P>
T* priv(P* pub) {
    return reinterpret_cast<T*>(pub);
}
MemMgr* mem_of(j_compress_ptr c) { return priv<MemMgr>(c->mem); }

JOAN_COUNTER(c_mem_init, "jinit_memory_mgr");
JOAN_COUNTER(c_compress_master, "jinit_compress_master");

// =============================================================================================
// jmemnobs.c
// =============================================================================================
void* jpeg_get_small(j_compress_ptr, jsize size) { return crt::malloc(size); }
void jpeg_free_small(j_compress_ptr, void* object, jsize) { crt::free(object); }
void* jpeg_get_large(j_compress_ptr, jsize size) { return crt::malloc(size); }
void jpeg_free_large(j_compress_ptr, void* object, jsize) { crt::free(object); }
jlong jpeg_mem_available(j_compress_ptr, jlong, jlong max_bytes_needed, jlong) { return max_bytes_needed; }
void jpeg_open_backing_store(j_compress_ptr c, BackingStore*, jlong) { errexit(c, JERR_NO_BACKING_STORE); }
jlong jpeg_mem_init(j_compress_ptr) { return 0; }
void jpeg_mem_term(j_compress_ptr) {}

// =============================================================================================
// jmemmgr.c
// =============================================================================================
constexpr jsize kHdrSize = SIZEOF<PoolHdr>;

// 0x007aad80
void out_of_memory(j_compress_ptr c, int which) { errexit1(c, JERR_OUT_OF_MEMORY, which); }

// 0x007aac50
void* alloc_small(j_compress_ptr c, int pool_id, jsize sizeofobject) {
    MemMgr* mem = mem_of(c);
    if (sizeofobject > kMaxAllocChunk - kHdrSize) out_of_memory(c, 1);
    const jsize odd_bytes = sizeofobject % kAlignType;
    if (odd_bytes > 0) sizeofobject += kAlignType - odd_bytes;
    if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS) errexit1(c, JERR_BAD_POOL_ID, pool_id);
    PoolHdr* prev = nullptr;
    PoolHdr* hdr = mem->small_list[pool_id];
    while (hdr != nullptr) {
        if (hdr->bytes_left >= sizeofobject) break;
        prev = hdr;
        hdr = hdr->next;
    }
    if (hdr == nullptr) {
        const jsize min_request = sizeofobject + kHdrSize;
        jsize slop = prev == nullptr ? kFirstPoolSlop[pool_id] : kExtraPoolSlop[pool_id];
        if (slop > kMaxAllocChunk - min_request) slop = kMaxAllocChunk - min_request;
        for (;;) {
            hdr = static_cast<PoolHdr*>(jpeg_get_small(c, min_request + slop));
            if (hdr != nullptr) break;
            slop /= 2;
            if (slop < 50) out_of_memory(c, 2);
        }
        mem->total_space_allocated += static_cast<jlong>(min_request + slop);
        hdr->next = nullptr;
        hdr->bytes_used = 0;
        hdr->bytes_left = sizeofobject + slop;
        if (prev == nullptr)
            mem->small_list[pool_id] = hdr;
        else
            prev->next = hdr;
    }
    u8* data = reinterpret_cast<u8*>(hdr + 1) + hdr->bytes_used;
    hdr->bytes_used += sizeofobject;
    hdr->bytes_left -= sizeofobject;
    return data;
}

// 0x007aada0
void* alloc_large(j_compress_ptr c, int pool_id, jsize sizeofobject) {
    MemMgr* mem = mem_of(c);
    if (sizeofobject > kMaxAllocChunk - kHdrSize) out_of_memory(c, 3);
    const jsize odd_bytes = sizeofobject % kAlignType;
    if (odd_bytes > 0) sizeofobject += kAlignType - odd_bytes;
    if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS) errexit1(c, JERR_BAD_POOL_ID, pool_id);
    auto* hdr = static_cast<PoolHdr*>(jpeg_get_large(c, sizeofobject + kHdrSize));
    if (hdr == nullptr) out_of_memory(c, 4);
    mem->total_space_allocated += static_cast<jlong>(sizeofobject + kHdrSize);
    hdr->next = mem->large_list[pool_id];
    hdr->bytes_used = sizeofobject;
    hdr->bytes_left = 0;
    mem->large_list[pool_id] = hdr;
    return hdr + 1;
}

// alloc_sarray / alloc_barray share the body; Row = JSAMPROW (rows of JSAMPLE) or JBLOCKROW (rows of JBLOCK)
template <class Row>
Row* alloc_array(j_compress_ptr c, int pool_id, JDIMENSION per_row, JDIMENSION numrows) {
    MemMgr* mem = mem_of(c);
    const jsize rowbytes = per_row * SIZEOF<std::remove_pointer_t<Row>>;
    const jlong ltemp = static_cast<jlong>((kMaxAllocChunk - kHdrSize) / rowbytes);
    if (ltemp <= 0) errexit(c, JERR_WIDTH_OVERFLOW);
    JDIMENSION rowsperchunk = ltemp < static_cast<jlong>(numrows) ? static_cast<JDIMENSION>(ltemp) : numrows;
    mem->last_rowsperchunk = rowsperchunk;
    auto* result = static_cast<Row*>(alloc_small(c, pool_id, numrows * SIZEOF<Row>));
    JDIMENSION currow = 0;
    while (currow < numrows) {
        if (rowsperchunk >= numrows - currow) rowsperchunk = numrows - currow;
        auto workspace = static_cast<Row>(alloc_large(c, pool_id, rowsperchunk * per_row * SIZEOF<std::remove_pointer_t<Row>>));
        for (JDIMENSION i = rowsperchunk; i > 0; i--) {
            result[currow++] = workspace;
            workspace += per_row;
        }
    }
    return result;
}
// 0x007aae50
JSAMPARRAY alloc_sarray(j_compress_ptr c, int pool_id, JDIMENSION samplesperrow, JDIMENSION numrows) {
    return alloc_array<JSAMPROW>(c, pool_id, samplesperrow, numrows);
}
// 0x007aaef0
JBLOCKARRAY alloc_barray(j_compress_ptr c, int pool_id, JDIMENSION blocksperrow, JDIMENSION numrows) {
    return alloc_array<JBLOCKROW>(c, pool_id, blocksperrow, numrows);
}

template <class Array>
jvirt_array_control<Array>* request_virt(j_compress_ptr c, int pool_id, boolean pre_zero, JDIMENSION per_row,
                                         JDIMENSION numrows, JDIMENSION maxaccess,
                                         jvirt_array_control<Array>*& list) {
    if (pool_id != JPOOL_IMAGE) errexit1(c, JERR_BAD_POOL_ID, pool_id);
    auto* r = static_cast<jvirt_array_control<Array>*>(alloc_small(c, pool_id, SIZEOF<jvirt_array_control<Array>>));
    r->mem_buffer = nullptr;
    r->rows_in_array = numrows;
    r->per_row = per_row;
    r->maxaccess = maxaccess;
    r->pre_zero = pre_zero;
    r->b_s_open = 0;
    r->next = list;
    list = r;
    return r;
}
// 0x007aafa0
jvirt_sarray_ptr request_virt_sarray(j_compress_ptr c, int pool_id, boolean pre_zero, JDIMENSION samplesperrow,
                                     JDIMENSION numrows, JDIMENSION maxaccess) {
    return request_virt<JSAMPARRAY>(c, pool_id, pre_zero, samplesperrow, numrows, maxaccess,
                                    mem_of(c)->virt_sarray_list);
}
// 0x007ab010
jvirt_barray_ptr request_virt_barray(j_compress_ptr c, int pool_id, boolean pre_zero, JDIMENSION blocksperrow,
                                     JDIMENSION numrows, JDIMENSION maxaccess) {
    return request_virt<JBLOCKARRAY>(c, pool_id, pre_zero, blocksperrow, numrows, maxaccess,
                                     mem_of(c)->virt_barray_list);
}

// space of one row of a virtual array (JSAMPLE / JBLOCK units)
template <class Array>
constexpr jsize kUnit = SIZEOF<std::remove_pointer_t<std::remove_pointer_t<Array>>>;

// 0x007ab080
void realize_virt_arrays(j_compress_ptr c) {
    MemMgr* mem = mem_of(c);
    jlong space_per_minheight = 0;
    jlong maximum_space = 0;
    for (VArrayS* p = mem->virt_sarray_list; p != nullptr; p = p->next)
        if (p->mem_buffer == nullptr) {
            space_per_minheight += static_cast<jlong>(p->maxaccess * p->per_row * kUnit<JSAMPARRAY>);
            maximum_space += static_cast<jlong>(p->rows_in_array * p->per_row * kUnit<JSAMPARRAY>);
        }
    for (VArrayB* p = mem->virt_barray_list; p != nullptr; p = p->next)
        if (p->mem_buffer == nullptr) {
            space_per_minheight += static_cast<jlong>(p->maxaccess * p->per_row * kUnit<JBLOCKARRAY>);
            maximum_space += static_cast<jlong>(p->rows_in_array * p->per_row * kUnit<JBLOCKARRAY>);
        }
    if (space_per_minheight <= 0) return;
    const jlong avail_mem = jpeg_mem_available(c, space_per_minheight, maximum_space, mem->total_space_allocated);
    jlong max_minheights;
    if (avail_mem >= maximum_space) {
        max_minheights = 1000000000;
    } else {
        max_minheights = avail_mem / space_per_minheight;
        if (max_minheights <= 0) max_minheights = 1;
    }
    auto realize = [&](auto* list, auto alloc) {
        for (auto* p = list; p != nullptr; p = p->next) {
            if (p->mem_buffer != nullptr) continue;
            const jlong minheights = static_cast<jlong>((p->rows_in_array - 1) / p->maxaccess + 1);
            if (minheights <= max_minheights) {
                p->rows_in_mem = p->rows_in_array;
            } else {
                p->rows_in_mem = static_cast<JDIMENSION>(max_minheights) * p->maxaccess;
                using Array = decltype(p->mem_buffer);
                jpeg_open_backing_store(c, &p->b_s_info,
                                        static_cast<jlong>(p->rows_in_array * p->per_row * kUnit<Array>));
                p->b_s_open = 1;
            }
            p->mem_buffer = alloc(c, JPOOL_IMAGE, p->per_row, p->rows_in_mem);
            p->rowsperchunk = mem->last_rowsperchunk;
            p->cur_start_row = 0;
            p->first_undef_row = 0;
            p->dirty = 0;
        }
    };
    realize(mem->virt_sarray_list, &alloc_sarray);
    realize(mem->virt_barray_list, &alloc_barray);
}

template <class Array>
void do_array_io(j_compress_ptr c, jvirt_array_control<Array>* p, boolean writing) {
    const jlong bytesperrow = static_cast<jlong>(p->per_row * kUnit<Array>);
    jlong file_offset = static_cast<jlong>(p->cur_start_row) * bytesperrow;
    for (jlong i = 0; i < static_cast<jlong>(p->rows_in_mem); i += static_cast<jlong>(p->rowsperchunk)) {
        jlong rows = static_cast<jlong>(p->rows_in_mem) - i;
        if (static_cast<jlong>(p->rowsperchunk) < rows) rows = static_cast<jlong>(p->rowsperchunk);
        const jlong thisrow = static_cast<jlong>(p->cur_start_row) + i;
        if (static_cast<jlong>(p->first_undef_row) - thisrow <= rows) rows = static_cast<jlong>(p->first_undef_row) - thisrow;
        if (static_cast<jlong>(p->rows_in_array) - thisrow <= rows) rows = static_cast<jlong>(p->rows_in_array) - thisrow;
        if (rows <= 0) break;
        const jlong byte_count = rows * bytesperrow;
        if (writing != 0)
            p->b_s_info.write_backing_store(c, &p->b_s_info, static_cast<void*>(p->mem_buffer[i]), file_offset,
                                            byte_count);
        else
            p->b_s_info.read_backing_store(c, &p->b_s_info, static_cast<void*>(p->mem_buffer[i]), file_offset,
                                           byte_count);
        file_offset += byte_count;
    }
}
// 0x007ab360
void do_sarray_io(j_compress_ptr c, jvirt_sarray_ptr p, boolean writing) { do_array_io(c, p, writing); }
// 0x007ab550
void do_barray_io(j_compress_ptr c, jvirt_barray_ptr p, boolean writing) { do_array_io(c, p, writing); }

template <class Array>
Array access_virt(j_compress_ptr c, jvirt_array_control<Array>* p, JDIMENSION start_row, JDIMENSION num_rows,
                  boolean writable) {
    JDIMENSION end_row = start_row + num_rows;
    if (end_row > p->rows_in_array || num_rows > p->maxaccess || p->mem_buffer == nullptr)
        errexit(c, JERR_BAD_VIRTUAL_ACCESS);
    if (start_row < p->cur_start_row || end_row > p->cur_start_row + p->rows_in_mem) {
        if (p->b_s_open == 0) errexit(c, JERR_VIRTUAL_BUG);
        if (p->dirty != 0) {
            do_array_io(c, p, 1);
            p->dirty = 0;
        }
        if (start_row > p->cur_start_row) {
            p->cur_start_row = start_row;
        } else {
            jlong ltemp = static_cast<jlong>(end_row - p->rows_in_mem);
            if (ltemp < 0) ltemp = 0;
            p->cur_start_row = static_cast<JDIMENSION>(ltemp);
        }
        do_array_io(c, p, 0);
    }
    if (p->first_undef_row < end_row) {
        JDIMENSION undef_row;
        if (p->first_undef_row < start_row) {
            if (writable != 0) errexit(c, JERR_BAD_VIRTUAL_ACCESS);
            undef_row = start_row;
        } else {
            undef_row = p->first_undef_row;
        }
        if (writable != 0) p->first_undef_row = end_row;
        if (p->pre_zero != 0) {
            const jsize bytesperrow = p->per_row * kUnit<Array>;
            undef_row -= p->cur_start_row;
            end_row -= p->cur_start_row;
            while (undef_row < end_row) {
                jzero_far(p->mem_buffer[undef_row], bytesperrow);
                undef_row++;
            }
        } else {
            if (writable == 0) errexit(c, JERR_BAD_VIRTUAL_ACCESS);
        }
    }
    if (writable != 0) p->dirty = 1;
    return p->mem_buffer + (start_row - p->cur_start_row);
}
// 0x007ab210
JSAMPARRAY access_virt_sarray(j_compress_ptr c, jvirt_sarray_ptr p, JDIMENSION start_row, JDIMENSION num_rows,
                              boolean writable) {
    return access_virt(c, p, start_row, num_rows, writable);
}
// 0x007ab400
JBLOCKARRAY access_virt_barray(j_compress_ptr c, jvirt_barray_ptr p, JDIMENSION start_row, JDIMENSION num_rows,
                               boolean writable) {
    return access_virt(c, p, start_row, num_rows, writable);
}

// 0x007ab5f0
void free_pool(j_compress_ptr c, int pool_id) {
    MemMgr* mem = mem_of(c);
    if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS) errexit1(c, JERR_BAD_POOL_ID, pool_id);
    if (pool_id == JPOOL_IMAGE) {
        for (VArrayS* p = mem->virt_sarray_list; p != nullptr; p = p->next)
            if (p->b_s_open != 0) {
                p->b_s_open = 0;
                p->b_s_info.close_backing_store(c, &p->b_s_info);
            }
        mem->virt_sarray_list = nullptr;
        for (VArrayB* p = mem->virt_barray_list; p != nullptr; p = p->next)
            if (p->b_s_open != 0) {
                p->b_s_open = 0;
                p->b_s_info.close_backing_store(c, &p->b_s_info);
            }
        mem->virt_barray_list = nullptr;
    }
    PoolHdr* hdr = mem->large_list[pool_id];
    mem->large_list[pool_id] = nullptr;
    while (hdr != nullptr) {
        PoolHdr* next = hdr->next;
        const jsize space_freed = hdr->bytes_left + hdr->bytes_used + kHdrSize;
        jpeg_free_large(c, hdr, space_freed);
        mem->total_space_allocated -= static_cast<jlong>(space_freed);
        hdr = next;
    }
    hdr = mem->small_list[pool_id];
    mem->small_list[pool_id] = nullptr;
    while (hdr != nullptr) {
        PoolHdr* next = hdr->next;
        const jsize space_freed = hdr->bytes_left + hdr->bytes_used + kHdrSize;
        jpeg_free_small(c, hdr, space_freed);
        mem->total_space_allocated -= static_cast<jlong>(space_freed);
        hdr = next;
    }
}

// 0x007ab6f0
void self_destruct(j_compress_ptr c) {
    for (int pool = JPOOL_NUMPOOLS - 1; pool >= JPOOL_PERMANENT; pool--) free_pool(c, pool);
    jpeg_free_small(c, c->mem, SIZEOF<MemMgr>);
    c->mem = nullptr;
    jpeg_mem_term(c);
}

}  // namespace

// 0x007aab10
void jinit_memory_mgr(j_compress_ptr c) {
    c_mem_init.hit();
    c->mem = nullptr;
    jlong max_to_use = jpeg_mem_init(c);
    auto* mem = static_cast<MemMgr*>(jpeg_get_small(c, SIZEOF<MemMgr>));
    if (mem == nullptr) {
        jpeg_mem_term(c);
        errexit1(c, JERR_OUT_OF_MEMORY, 0);
    }
    mem->pub.alloc_small = &alloc_small;
    mem->pub.alloc_large = &alloc_large;
    mem->pub.alloc_sarray = &alloc_sarray;
    mem->pub.alloc_barray = &alloc_barray;
    mem->pub.request_virt_sarray = &request_virt_sarray;
    mem->pub.request_virt_barray = &request_virt_barray;
    mem->pub.realize_virt_arrays = &realize_virt_arrays;
    mem->pub.access_virt_sarray = &access_virt_sarray;
    mem->pub.access_virt_barray = &access_virt_barray;
    mem->pub.free_pool = &free_pool;
    mem->pub.self_destruct = &self_destruct;
    mem->pub.max_alloc_chunk = static_cast<jlong>(kMaxAllocChunk);
    mem->pub.max_memory_to_use = max_to_use;
    for (int pool = JPOOL_NUMPOOLS - 1; pool >= JPOOL_PERMANENT; pool--) {
        mem->small_list[pool] = nullptr;
        mem->large_list[pool] = nullptr;
    }
    mem->virt_sarray_list = nullptr;
    mem->virt_barray_list = nullptr;
    mem->total_space_allocated = static_cast<jlong>(SIZEOF<MemMgr>);
    c->mem = &mem->pub;
    static const char kEnvName[] = "JPEGMEM";  // 0x00a30928
    static const char kFormat[] = "%ld%c";     // 0x00a30920
    const char* memenv = crt::getenv(kEnvName);
    if (memenv != nullptr) {
        char ch = 'x';
        if (crt::sscanf_long_char(memenv, kFormat, &max_to_use, &ch) > 0) {
            if (ch == 'm' || ch == 'M') max_to_use *= 1000;
            mem->pub.max_memory_to_use = max_to_use * 1000;
        }
    }
}

namespace {

// =============================================================================================
// jcmaster.c
// =============================================================================================
constexpr int kMainPass = 0, kHuffOptPass = 1, kOutputPass = 2;

// 0x007b0230
void initial_setup(j_compress_ptr c) {
    if (c->image_height == 0 || c->image_width == 0 || c->num_components <= 0 || c->input_components <= 0)
        errexit(c, JERR_EMPTY_IMAGE);
    if (static_cast<jlong>(c->image_height) > 65500 || static_cast<jlong>(c->image_width) > 65500)
        errexit1(c, JERR_IMAGE_TOO_BIG, 65500);
    if (c->data_precision != 8) errexit1(c, JERR_BAD_PRECISION, c->data_precision);
    if (c->num_components > MAX_COMPONENTS) errexit2(c, JERR_COMPONENT_COUNT, c->num_components, MAX_COMPONENTS);
    c->max_h_samp_factor = 1;
    c->max_v_samp_factor = 1;
    jpeg_component_info* comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++) {
        if (comp->h_samp_factor <= 0 || comp->h_samp_factor > MAX_SAMP_FACTOR || comp->v_samp_factor <= 0 ||
            comp->v_samp_factor > MAX_SAMP_FACTOR)
            errexit(c, JERR_BAD_SAMPLING);
        if (c->max_h_samp_factor <= comp->h_samp_factor) c->max_h_samp_factor = comp->h_samp_factor;
        if (c->max_v_samp_factor <= comp->v_samp_factor) c->max_v_samp_factor = comp->v_samp_factor;
    }
    comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++) {
        comp->component_index = ci;
        comp->DCT_scaled_size = DCTSIZE;
        comp->width_in_blocks = static_cast<JDIMENSION>(jdiv_round_up(
            static_cast<jlong>(c->image_width * static_cast<JDIMENSION>(comp->h_samp_factor)),
            static_cast<jlong>(c->max_h_samp_factor * DCTSIZE)));
        comp->height_in_blocks = static_cast<JDIMENSION>(jdiv_round_up(
            static_cast<jlong>(c->image_height * static_cast<JDIMENSION>(comp->v_samp_factor)),
            static_cast<jlong>(c->max_v_samp_factor * DCTSIZE)));
        comp->downsampled_width = static_cast<JDIMENSION>(
            jdiv_round_up(static_cast<jlong>(c->image_width * static_cast<JDIMENSION>(comp->h_samp_factor)),
                          static_cast<jlong>(c->max_h_samp_factor)));
        comp->downsampled_height = static_cast<JDIMENSION>(
            jdiv_round_up(static_cast<jlong>(c->image_height * static_cast<JDIMENSION>(comp->v_samp_factor)),
                          static_cast<jlong>(c->max_v_samp_factor)));
        comp->component_needed = 1;
    }
    c->total_iMCU_rows = static_cast<JDIMENSION>(
        jdiv_round_up(static_cast<jlong>(c->image_height), static_cast<jlong>(c->max_v_samp_factor * DCTSIZE)));
}

// 0x007b0400
void validate_script(j_compress_ptr c) {
    boolean component_sent[MAX_COMPONENTS];
    int last_bitpos[MAX_COMPONENTS][DCTSIZE2];
    if (c->num_scans <= 0) errexit1(c, JERR_BAD_SCAN_SCRIPT, 0);
    const jpeg_scan_info* scan = c->scan_info;
    const int nc = c->num_components < MAX_COMPONENTS ? c->num_components : MAX_COMPONENTS;  // see header
    if (scan->Ss != 0 || scan->Se != DCTSIZE2 - 1) {
        c->progressive_mode = 1;
        for (int ci = 0; ci < nc; ci++)
            for (int k = 0; k < DCTSIZE2; k++) last_bitpos[ci][k] = -1;
    } else {
        c->progressive_mode = 0;
        for (int ci = 0; ci < nc; ci++) component_sent[ci] = 0;
    }
    for (int scanno = 1; scanno <= c->num_scans; scan++, scanno++) {
        const int ncomps = scan->comps_in_scan;
        if (ncomps <= 0 || ncomps > MAX_COMPS_IN_SCAN) errexit2(c, JERR_COMPONENT_COUNT, ncomps, MAX_COMPS_IN_SCAN);
        for (int ci = 0; ci < ncomps; ci++) {
            const int thisi = scan->component_index[ci];
            if (thisi < 0 || thisi >= c->num_components) errexit1(c, JERR_BAD_SCAN_SCRIPT, scanno);
            if (ci > 0 && thisi <= scan->component_index[ci - 1]) errexit1(c, JERR_BAD_SCAN_SCRIPT, scanno);
        }
        const int Ss = scan->Ss, Se = scan->Se, Ah = scan->Ah, Al = scan->Al;
        if (c->progressive_mode != 0) {
            if (Ss < 0 || Ss >= DCTSIZE2 || Se < Ss || Se >= DCTSIZE2 || Ah < 0 || Ah > 10 || Al < 0 || Al > 10)
                errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
            if (Ss == 0) {
                if (Se != 0) errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
            } else {
                if (ncomps != 1) errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
            }
            for (int ci = 0; ci < ncomps; ci++) {
                int* lb = last_bitpos[scan->component_index[ci]];
                if (Ss != 0 && lb[0] < 0) errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
                for (int k = Ss; k <= Se; k++) {
                    if (lb[k] < 0) {
                        if (Ah != 0) errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
                    } else {
                        if (Ah != lb[k] || Al != Ah - 1) errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
                    }
                    lb[k] = Al;
                }
            }
        } else {
            if (Ss != 0 || Se != DCTSIZE2 - 1 || Ah != 0 || Al != 0) errexit1(c, JERR_BAD_PROG_SCRIPT, scanno);
            for (int ci = 0; ci < ncomps; ci++) {
                const int thisi = scan->component_index[ci];
                if (component_sent[thisi] != 0) errexit1(c, JERR_BAD_SCAN_SCRIPT, scanno);
                component_sent[thisi] = 1;
            }
        }
    }
    if (c->progressive_mode != 0) {
        for (int ci = 0; ci < c->num_components; ci++)
            if (last_bitpos[ci][0] < 0) errexit(c, JERR_MISSING_DATA);
    } else {
        for (int ci = 0; ci < c->num_components; ci++)
            if (component_sent[ci] == 0) errexit(c, JERR_MISSING_DATA);
    }
}

// 0x007b0940
void select_scan_parameters(j_compress_ptr c) {
    if (c->scan_info != nullptr) {
        const jpeg_scan_info* scan = c->scan_info + priv<Master>(c->master)->scan_number;
        c->comps_in_scan = scan->comps_in_scan;
        for (int ci = 0; ci < scan->comps_in_scan; ci++)
            c->cur_comp_info[ci] = &c->comp_info[scan->component_index[ci]];
        c->Ss = scan->Ss;
        c->Se = scan->Se;
        c->Ah = scan->Ah;
        c->Al = scan->Al;
    } else {
        if (c->num_components > MAX_COMPS_IN_SCAN)
            errexit2(c, JERR_COMPONENT_COUNT, c->num_components, MAX_COMPS_IN_SCAN);
        c->comps_in_scan = c->num_components;
        for (int ci = 0; ci < c->num_components; ci++) c->cur_comp_info[ci] = &c->comp_info[ci];
        c->Ss = 0;
        c->Ah = 0;
        c->Al = 0;
        c->Se = DCTSIZE2 - 1;
    }
}

// 0x007b0a40
void per_scan_setup(j_compress_ptr c) {
    if (c->comps_in_scan == 1) {
        jpeg_component_info* comp = c->cur_comp_info[0];
        c->MCUs_per_row = comp->width_in_blocks;
        c->MCU_rows_in_scan = comp->height_in_blocks;
        comp->MCU_width = 1;
        comp->MCU_height = 1;
        comp->MCU_blocks = 1;
        comp->MCU_sample_width = DCTSIZE;
        comp->last_col_width = 1;
        int tmp = static_cast<int>(comp->height_in_blocks % static_cast<JDIMENSION>(comp->v_samp_factor));
        if (tmp == 0) tmp = comp->v_samp_factor;
        comp->last_row_height = tmp;
        c->blocks_in_MCU = 1;
        c->MCU_membership[0] = 0;
    } else {
        if (c->comps_in_scan <= 0 || c->comps_in_scan > MAX_COMPS_IN_SCAN)
            errexit2(c, JERR_COMPONENT_COUNT, c->comps_in_scan, MAX_COMPS_IN_SCAN);
        c->MCUs_per_row = static_cast<JDIMENSION>(
            jdiv_round_up(static_cast<jlong>(c->image_width), static_cast<jlong>(c->max_h_samp_factor * DCTSIZE)));
        c->MCU_rows_in_scan = static_cast<JDIMENSION>(
            jdiv_round_up(static_cast<jlong>(c->image_height), static_cast<jlong>(c->max_v_samp_factor * DCTSIZE)));
        c->blocks_in_MCU = 0;
        for (int ci = 0; ci < c->comps_in_scan; ci++) {
            jpeg_component_info* comp = c->cur_comp_info[ci];
            comp->MCU_width = comp->h_samp_factor;
            comp->MCU_height = comp->v_samp_factor;
            comp->MCU_blocks = comp->MCU_width * comp->MCU_height;
            comp->MCU_sample_width = comp->MCU_width * DCTSIZE;
            int tmp = static_cast<int>(comp->width_in_blocks % static_cast<JDIMENSION>(comp->MCU_width));
            if (tmp == 0) tmp = comp->MCU_width;
            comp->last_col_width = tmp;
            tmp = static_cast<int>(comp->height_in_blocks % static_cast<JDIMENSION>(comp->MCU_height));
            if (tmp == 0) tmp = comp->MCU_height;
            comp->last_row_height = tmp;
            int mcublks = comp->MCU_blocks;
            if (c->blocks_in_MCU + mcublks > C_MAX_BLOCKS_IN_MCU) errexit(c, JERR_BAD_MCU_SIZE);
            while (mcublks-- > 0) c->MCU_membership[c->blocks_in_MCU++] = ci;
        }
    }
    if (c->restart_in_rows > 0) {
        const jlong nominal = static_cast<jlong>(c->restart_in_rows) * static_cast<jlong>(c->MCUs_per_row);
        c->restart_interval = static_cast<unsigned int>(nominal < 65535 ? nominal : 65535);
    }
}

// 0x007b07a0
void prepare_for_pass(j_compress_ptr c) {
    Master* m = priv<Master>(c->master);
    switch (m->pass_type) {
    case kMainPass:
        select_scan_parameters(c);
        per_scan_setup(c);
        if (c->raw_data_in == 0) {
            c->cconvert->start_pass(c);
            c->downsample->start_pass(c);
            c->prep->start_pass(c, JBUF_PASS_THRU);
        }
        c->fdct->start_pass(c);
        c->entropy->start_pass(c, c->optimize_coding);
        c->coef->start_pass(c, m->total_passes > 1 ? JBUF_SAVE_AND_PASS : JBUF_PASS_THRU);
        c->main->start_pass(c, JBUF_PASS_THRU);
        m->pub.call_pass_startup = c->optimize_coding != 0 ? 0 : 1;
        break;
    case kHuffOptPass:
        select_scan_parameters(c);
        per_scan_setup(c);
        if (c->Ss != 0 || c->Ah == 0 || c->arith_code != 0) {
            c->entropy->start_pass(c, 1);
            c->coef->start_pass(c, JBUF_CRANK_DEST);
            m->pub.call_pass_startup = 0;
            break;
        }
        m->pass_type = kOutputPass;
        m->pass_number++;
        [[fallthrough]];
    case kOutputPass:
        if (c->optimize_coding == 0) {
            select_scan_parameters(c);
            per_scan_setup(c);
        }
        c->entropy->start_pass(c, 0);
        c->coef->start_pass(c, JBUF_CRANK_DEST);
        if (m->scan_number == 0) c->marker->write_frame_header(c);
        c->marker->write_scan_header(c);
        m->pub.call_pass_startup = 0;
        break;
    default:
        errexit(c, JERR_NOT_COMPILED);
        break;
    }
    m->pub.is_last_pass = m->pass_number == m->total_passes - 1 ? 1 : 0;
    if (c->progress != nullptr) {
        c->progress->completed_passes = m->pass_number;
        c->progress->total_passes = m->total_passes;
    }
}

// 0x007b0c10
void pass_startup(j_compress_ptr c) {
    c->master->call_pass_startup = 0;
    c->marker->write_frame_header(c);
    c->marker->write_scan_header(c);
}

// 0x007b0c40
void finish_pass_master(j_compress_ptr c) {
    Master* m = priv<Master>(c->master);
    c->entropy->finish_pass(c);
    switch (m->pass_type) {
    case kMainPass:
        m->pass_type = kOutputPass;
        if (c->optimize_coding == 0) m->scan_number++;
        break;
    case kHuffOptPass:
        m->pass_type = kOutputPass;
        break;
    case kOutputPass:
        if (c->optimize_coding != 0) m->pass_type = kHuffOptPass;
        m->scan_number++;
        break;
    default:
        break;
    }
    m->pass_number++;
}

// 0x007b0170
void jinit_c_master_control(j_compress_ptr c, boolean transcode_only) {
    auto* m = static_cast<Master*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<Master>));
    c->master = &m->pub;
    m->pub.prepare_for_pass = &prepare_for_pass;
    m->pub.pass_startup = &pass_startup;
    m->pub.finish_pass = &finish_pass_master;
    m->pub.is_last_pass = 0;
    initial_setup(c);
    if (c->scan_info != nullptr) {
        validate_script(c);
    } else {
        c->progressive_mode = 0;
        c->num_scans = 1;
    }
    if (c->progressive_mode != 0) c->optimize_coding = 1;
    if (transcode_only != 0)
        m->pass_type = c->optimize_coding != 0 ? kHuffOptPass : kOutputPass;
    else
        m->pass_type = kMainPass;
    m->scan_number = 0;
    m->pass_number = 0;
    m->total_passes = c->optimize_coding != 0 ? c->num_scans * 2 : c->num_scans;
}

// =============================================================================================
// jccolor.c
// =============================================================================================
constexpr int R_Y_OFF = 0, G_Y_OFF = 256, B_Y_OFF = 512, R_CB_OFF = 768, G_CB_OFF = 1024, B_CB_OFF = 1280,
              R_CR_OFF = 1280, G_CR_OFF = 1536, B_CR_OFF = 1792, TABLE_SIZE = 2048;

// 0x007afc30
void rgb_ycc_start(j_compress_ptr c) {
    CConvert* cc = priv<CConvert>(c->cconvert);
    auto* tab = static_cast<i32*>(c->mem->alloc_small(c, JPOOL_IMAGE, TABLE_SIZE * SIZEOF<i32>));
    cc->rgb_ycc_tab = tab;
    for (i32 i = 0; i <= 255; i++) {
        tab[i + R_Y_OFF] = 19595 * i;
        tab[i + G_Y_OFF] = 38470 * i;
        tab[i + B_Y_OFF] = 7471 * i + 32768;
        tab[i + R_CB_OFF] = -11059 * i;
        tab[i + G_CB_OFF] = -21709 * i;
        tab[i + B_CB_OFF] = 32768 * i + (128 << 16) + 32767;
        tab[i + G_CR_OFF] = -27439 * i;
        tab[i + B_CR_OFF] = -5329 * i;
    }
}

// 0x007afd10
void rgb_ycc_convert(j_compress_ptr c, JSAMPARRAY input_buf, JSAMPIMAGE output_buf, JDIMENSION output_row,
                     int num_rows) {
    const i32* tab = priv<CConvert>(c->cconvert)->rgb_ycc_tab;
    const JDIMENSION num_cols = c->image_width;
    while (--num_rows >= 0) {
        const JSAMPLE* in = *input_buf++;
        JSAMPROW out0 = output_buf[0][output_row];
        JSAMPROW out1 = output_buf[1][output_row];
        JSAMPROW out2 = output_buf[2][output_row];
        output_row++;
        for (JDIMENSION col = 0; col < num_cols; col++) {
            const int r = in[0], g = in[1], b = in[2];
            in += 3;
            out0[col] = static_cast<JSAMPLE>((tab[r + R_Y_OFF] + tab[g + G_Y_OFF] + tab[b + B_Y_OFF]) >> 16);
            out1[col] = static_cast<JSAMPLE>((tab[r + R_CB_OFF] + tab[g + G_CB_OFF] + tab[b + B_CB_OFF]) >> 16);
            out2[col] = static_cast<JSAMPLE>((tab[r + R_CR_OFF] + tab[g + G_CR_OFF] + tab[b + B_CR_OFF]) >> 16);
        }
    }
}

// 0x007afe40
void rgb_gray_convert(j_compress_ptr c, JSAMPARRAY input_buf, JSAMPIMAGE output_buf, JDIMENSION output_row,
                      int num_rows) {
    const i32* tab = priv<CConvert>(c->cconvert)->rgb_ycc_tab;
    const JDIMENSION num_cols = c->image_width;
    while (--num_rows >= 0) {
        const JSAMPLE* in = *input_buf++;
        JSAMPROW out = output_buf[0][output_row];
        output_row++;
        for (JDIMENSION col = 0; col < num_cols; col++) {
            const int r = in[0], g = in[1], b = in[2];
            in += 3;
            out[col] = static_cast<JSAMPLE>((tab[r + R_Y_OFF] + tab[g + G_Y_OFF] + tab[b + B_Y_OFF]) >> 16);
        }
    }
}

// 0x007afef0
void cmyk_ycck_convert(j_compress_ptr c, JSAMPARRAY input_buf, JSAMPIMAGE output_buf, JDIMENSION output_row,
                       int num_rows) {
    const i32* tab = priv<CConvert>(c->cconvert)->rgb_ycc_tab;
    const JDIMENSION num_cols = c->image_width;
    while (--num_rows >= 0) {
        const JSAMPLE* in = *input_buf++;
        JSAMPROW out0 = output_buf[0][output_row];
        JSAMPROW out1 = output_buf[1][output_row];
        JSAMPROW out2 = output_buf[2][output_row];
        JSAMPROW out3 = output_buf[3][output_row];
        output_row++;
        for (JDIMENSION col = 0; col < num_cols; col++) {
            const int r = 255 - in[0], g = 255 - in[1], b = 255 - in[2];
            out3[col] = in[3];
            in += 4;
            out0[col] = static_cast<JSAMPLE>((tab[r + R_Y_OFF] + tab[g + G_Y_OFF] + tab[b + B_Y_OFF]) >> 16);
            out1[col] = static_cast<JSAMPLE>((tab[r + R_CB_OFF] + tab[g + G_CB_OFF] + tab[b + B_CB_OFF]) >> 16);
            out2[col] = static_cast<JSAMPLE>((tab[r + R_CR_OFF] + tab[g + G_CR_OFF] + tab[b + B_CR_OFF]) >> 16);
        }
    }
}

// 0x007b0060
void grayscale_convert(j_compress_ptr c, JSAMPARRAY input_buf, JSAMPIMAGE output_buf, JDIMENSION output_row,
                       int num_rows) {
    const JDIMENSION num_cols = c->image_width;
    const int instride = c->input_components;
    while (--num_rows >= 0) {
        const JSAMPLE* in = *input_buf++;
        JSAMPROW out = output_buf[0][output_row];
        output_row++;
        for (JDIMENSION col = 0; col < num_cols; col++) {
            out[col] = in[0];
            in += instride;
        }
    }
}

// 0x007b00d0
void null_convert(j_compress_ptr c, JSAMPARRAY input_buf, JSAMPIMAGE output_buf, JDIMENSION output_row,
                  int num_rows) {
    const int nc = c->num_components;
    const JDIMENSION num_cols = c->image_width;
    while (--num_rows >= 0) {
        for (int ci = 0; ci < nc; ci++) {
            const JSAMPLE* in = *input_buf;
            JSAMPROW out = output_buf[ci][output_row];
            for (JDIMENSION col = 0; col < num_cols; col++) {
                out[col] = in[ci];
                in += nc;
            }
        }
        input_buf++;
        output_row++;
    }
}

// 0x007b0160
void null_method(j_compress_ptr) {}

// 0x007af9f0
void jinit_color_converter(j_compress_ptr c) {
    auto* cc = static_cast<CConvert*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<CConvert>));
    c->cconvert = &cc->pub;
    cc->pub.start_pass = &null_method;
    switch (c->in_color_space) {
    case JCS_GRAYSCALE:
        if (c->input_components != 1) errexit(c, JERR_BAD_IN_COLORSPACE);
        break;
    case JCS_RGB:
    case JCS_YCbCr:
        if (c->input_components != 3) errexit(c, JERR_BAD_IN_COLORSPACE);
        break;
    case JCS_CMYK:
    case JCS_YCCK:
        if (c->input_components != 4) errexit(c, JERR_BAD_IN_COLORSPACE);
        break;
    default:
        if (c->input_components < 1) errexit(c, JERR_BAD_IN_COLORSPACE);
        break;
    }
    switch (c->jpeg_color_space) {
    case JCS_GRAYSCALE:
        if (c->num_components != 1) errexit(c, JERR_BAD_J_COLORSPACE);
        if (c->in_color_space == JCS_GRAYSCALE) {
            cc->pub.color_convert = &grayscale_convert;
        } else if (c->in_color_space == JCS_RGB) {
            cc->pub.start_pass = &rgb_ycc_start;
            cc->pub.color_convert = &rgb_gray_convert;
        } else if (c->in_color_space == JCS_YCbCr) {
            cc->pub.color_convert = &grayscale_convert;
        } else {
            errexit(c, JERR_CONVERSION_NOTIMPL);
        }
        break;
    case JCS_RGB:
        if (c->num_components != 3) errexit(c, JERR_BAD_J_COLORSPACE);
        if (c->in_color_space == JCS_RGB)
            cc->pub.color_convert = &null_convert;
        else
            errexit(c, JERR_CONVERSION_NOTIMPL);
        break;
    case JCS_YCbCr:
        if (c->num_components != 3) errexit(c, JERR_BAD_J_COLORSPACE);
        if (c->in_color_space == JCS_RGB) {
            cc->pub.start_pass = &rgb_ycc_start;
            cc->pub.color_convert = &rgb_ycc_convert;
        } else if (c->in_color_space == JCS_YCbCr) {
            cc->pub.color_convert = &null_convert;
        } else {
            errexit(c, JERR_CONVERSION_NOTIMPL);
        }
        break;
    case JCS_CMYK:
        if (c->num_components != 4) errexit(c, JERR_BAD_J_COLORSPACE);
        if (c->in_color_space == JCS_CMYK)
            cc->pub.color_convert = &null_convert;
        else
            errexit(c, JERR_CONVERSION_NOTIMPL);
        break;
    case JCS_YCCK:
        if (c->num_components != 4) errexit(c, JERR_BAD_J_COLORSPACE);
        if (c->in_color_space == JCS_CMYK) {
            cc->pub.start_pass = &rgb_ycc_start;
            cc->pub.color_convert = &cmyk_ycck_convert;
        } else if (c->in_color_space == JCS_YCCK) {
            cc->pub.color_convert = &null_convert;
        } else {
            errexit(c, JERR_CONVERSION_NOTIMPL);
        }
        break;
    default:
        if (c->jpeg_color_space != c->in_color_space || c->num_components != c->input_components)
            errexit(c, JERR_CONVERSION_NOTIMPL);
        cc->pub.color_convert = &null_convert;
        break;
    }
}

// =============================================================================================
// jcsample.c
// =============================================================================================
// 0x007af340
void expand_right_edge(JSAMPARRAY image_data, int num_rows, JDIMENSION input_cols, JDIMENSION output_cols) {
    const int numcols = static_cast<int>(output_cols - input_cols);
    if (numcols > 0) {
        for (int row = 0; row < num_rows; row++) {
            JSAMPROW p = image_data[row] + input_cols;
            const JSAMPLE pixval = p[-1];
            for (int count = numcols; count > 0; count--) *p++ = pixval;
        }
    }
}

// 0x007af180
void start_pass_downsample(j_compress_ptr) {}

// 0x007af190
void sep_downsample(j_compress_ptr c, JSAMPIMAGE input_buf, JDIMENSION in_row_index, JSAMPIMAGE output_buf,
                    JDIMENSION out_row_group_index) {
    Downsample* ds = priv<Downsample>(c->downsample);
    jpeg_component_info* comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++) {
        JSAMPARRAY in = input_buf[ci] + in_row_index;
        JSAMPARRAY out = output_buf[ci] + out_row_group_index * static_cast<JDIMENSION>(comp->v_samp_factor);
        ds->methods[ci](c, comp, in, out);
    }
}

// 0x007af220
void int_downsample(j_compress_ptr c, jpeg_component_info* comp, JSAMPARRAY input_data, JSAMPARRAY output_data) {
    const JDIMENSION output_cols = comp->width_in_blocks * DCTSIZE;
    const int h_expand = c->max_h_samp_factor / comp->h_samp_factor;
    const int v_expand = c->max_v_samp_factor / comp->v_samp_factor;
    const int numpix = h_expand * v_expand;
    const int numpix2 = numpix / 2;
    expand_right_edge(input_data, c->max_v_samp_factor, c->image_width,
                      output_cols * static_cast<JDIMENSION>(h_expand));
    int inrow = 0;
    for (int outrow = 0; outrow < comp->v_samp_factor; outrow++) {
        JSAMPROW out = output_data[outrow];
        JDIMENSION outcol_h = 0;
        for (JDIMENSION outcol = 0; outcol < output_cols;
             outcol++, outcol_h += static_cast<JDIMENSION>(h_expand)) {
            i32 outvalue = 0;
            for (int v = 0; v < v_expand; v++) {
                const JSAMPLE* in = input_data[inrow + v] + outcol_h;
                for (int h = 0; h < h_expand; h++) outvalue += *in++;
            }
            *out++ = static_cast<JSAMPLE>((outvalue + numpix2) / numpix);
        }
        inrow += v_expand;
    }
}

// 0x007af3a0
void fullsize_downsample(j_compress_ptr c, jpeg_component_info* comp, JSAMPARRAY input_data, JSAMPARRAY output_data) {
    jcopy_sample_rows(input_data, 0, output_data, 0, c->max_v_samp_factor, c->image_width);
    expand_right_edge(output_data, c->max_v_samp_factor, c->image_width, comp->width_in_blocks * DCTSIZE);
}

// 0x007af3f0
void h2v1_downsample(j_compress_ptr c, jpeg_component_info* comp, JSAMPARRAY input_data, JSAMPARRAY output_data) {
    const JDIMENSION output_cols = comp->width_in_blocks * DCTSIZE;
    expand_right_edge(input_data, c->max_v_samp_factor, c->image_width, output_cols * 2);
    for (int outrow = 0; outrow < comp->v_samp_factor; outrow++) {
        JSAMPROW out = output_data[outrow];
        const JSAMPLE* in = input_data[outrow];
        int bias = 0;
        for (JDIMENSION outcol = 0; outcol < output_cols; outcol++) {
            *out++ = static_cast<JSAMPLE>((in[0] + in[1] + bias) >> 1);
            bias ^= 1;
            in += 2;
        }
    }
}

// 0x007af4a0
void h2v2_downsample(j_compress_ptr c, jpeg_component_info* comp, JSAMPARRAY input_data, JSAMPARRAY output_data) {
    const JDIMENSION output_cols = comp->width_in_blocks * DCTSIZE;
    expand_right_edge(input_data, c->max_v_samp_factor, c->image_width, output_cols * 2);
    int inrow = 0;
    for (int outrow = 0; outrow < comp->v_samp_factor; outrow++) {
        JSAMPROW out = output_data[outrow];
        const JSAMPLE* in0 = input_data[inrow];
        const JSAMPLE* in1 = input_data[inrow + 1];
        int bias = 1;
        for (JDIMENSION outcol = 0; outcol < output_cols; outcol++) {
            *out++ = static_cast<JSAMPLE>((in0[0] + in0[1] + in1[0] + in1[1] + bias) >> 2);
            bias ^= 3;
            in0 += 2;
            in1 += 2;
        }
        inrow += 2;
    }
}

// 0x007af570
void h2v2_smooth_downsample(j_compress_ptr c, jpeg_component_info* comp, JSAMPARRAY input_data,
                            JSAMPARRAY output_data) {
    const JDIMENSION output_cols = comp->width_in_blocks * DCTSIZE;
    expand_right_edge(input_data - 1, c->max_v_samp_factor + 2, c->image_width, output_cols * 2);
    const i32 memberscale = 16384 - c->smoothing_factor * 80;
    const i32 neighscale = c->smoothing_factor * 16;
    int inrow = 0;
    for (int outrow = 0; outrow < comp->v_samp_factor; outrow++) {
        JSAMPROW out = output_data[outrow];
        const JSAMPLE* in0 = input_data[inrow];
        const JSAMPLE* in1 = input_data[inrow + 1];
        const JSAMPLE* above = input_data[inrow - 1];
        const JSAMPLE* below = input_data[inrow + 2];
        i32 membersum = in0[0] + in0[1] + in1[0] + in1[1];
        i32 neighsum = above[0] + above[1] + below[0] + below[1] + in0[0] + in0[2] + in1[0] + in1[2];
        neighsum += neighsum;
        neighsum += above[0] + above[2] + below[0] + below[2];
        membersum = membersum * memberscale + neighsum * neighscale;
        *out++ = static_cast<JSAMPLE>((membersum + 32768) >> 16);
        in0 += 2;
        in1 += 2;
        above += 2;
        below += 2;
        for (JDIMENSION colctr = output_cols - 2; colctr > 0; colctr--) {
            membersum = in0[0] + in0[1] + in1[0] + in1[1];
            neighsum = above[0] + above[1] + below[0] + below[1] + in0[-1] + in0[2] + in1[-1] + in1[2];
            neighsum += neighsum;
            neighsum += above[-1] + above[2] + below[-1] + below[2];
            membersum = membersum * memberscale + neighsum * neighscale;
            *out++ = static_cast<JSAMPLE>((membersum + 32768) >> 16);
            in0 += 2;
            in1 += 2;
            above += 2;
            below += 2;
        }
        membersum = in0[0] + in0[1] + in1[0] + in1[1];
        neighsum = above[0] + above[1] + below[0] + below[1] + in0[-1] + in0[1] + in1[-1] + in1[1];
        neighsum += neighsum;
        neighsum += above[-1] + above[1] + below[-1] + below[1];
        membersum = membersum * memberscale + neighsum * neighscale;
        *out = static_cast<JSAMPLE>((membersum + 32768) >> 16);
        inrow += 2;
    }
}

// 0x007af860
void fullsize_smooth_downsample(j_compress_ptr c, jpeg_component_info* comp, JSAMPARRAY input_data,
                                JSAMPARRAY output_data) {
    const JDIMENSION output_cols = comp->width_in_blocks * DCTSIZE;
    expand_right_edge(input_data - 1, c->max_v_samp_factor + 2, c->image_width, output_cols);
    const i32 memberscale = 65536 - c->smoothing_factor * 512;
    const i32 neighscale = c->smoothing_factor * 64;
    for (int outrow = 0; outrow < comp->v_samp_factor; outrow++) {
        JSAMPROW out = output_data[outrow];
        const JSAMPLE* in = input_data[outrow];
        const JSAMPLE* above = input_data[outrow - 1];
        const JSAMPLE* below = input_data[outrow + 1];
        i32 colsum = *above++ + *below++ + in[0];
        i32 membersum = *in++;
        i32 nextcolsum = above[0] + below[0] + in[0];
        i32 neighsum = colsum + (colsum - membersum) + nextcolsum;
        membersum = membersum * memberscale + neighsum * neighscale;
        *out++ = static_cast<JSAMPLE>((membersum + 32768) >> 16);
        i32 lastcolsum = colsum;
        colsum = nextcolsum;
        for (JDIMENSION colctr = output_cols - 2; colctr > 0; colctr--) {
            membersum = *in++;
            above++;
            below++;
            nextcolsum = above[0] + below[0] + in[0];
            neighsum = lastcolsum + (colsum - membersum) + nextcolsum;
            membersum = membersum * memberscale + neighsum * neighscale;
            *out++ = static_cast<JSAMPLE>((membersum + 32768) >> 16);
            lastcolsum = colsum;
            colsum = nextcolsum;
        }
        membersum = in[0];
        neighsum = lastcolsum + (colsum - membersum) + colsum;
        membersum = membersum * memberscale + neighsum * neighscale;
        *out = static_cast<JSAMPLE>((membersum + 32768) >> 16);
    }
}

// 0x007af000
void jinit_downsampler(j_compress_ptr c) {
    bool smoothok = true;
    auto* ds = static_cast<Downsample*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<Downsample>));
    c->downsample = &ds->pub;
    ds->pub.start_pass = &start_pass_downsample;
    ds->pub.downsample = &sep_downsample;
    ds->pub.need_context_rows = 0;
    if (c->CCIR601_sampling != 0) errexit(c, JERR_CCIR601_NOTIMPL);
    jpeg_component_info* comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++) {
        Downsample1 m;
        if (comp->h_samp_factor == c->max_h_samp_factor && comp->v_samp_factor == c->max_v_samp_factor) {
            if (c->smoothing_factor != 0) {
                m = &fullsize_smooth_downsample;
                ds->pub.need_context_rows = 1;
            } else {
                m = &fullsize_downsample;
            }
        } else if (comp->h_samp_factor * 2 == c->max_h_samp_factor && comp->v_samp_factor == c->max_v_samp_factor) {
            smoothok = false;
            m = &h2v1_downsample;
        } else if (comp->h_samp_factor * 2 == c->max_h_samp_factor &&
                   comp->v_samp_factor * 2 == c->max_v_samp_factor) {
            if (c->smoothing_factor != 0) {
                m = &h2v2_smooth_downsample;
                ds->pub.need_context_rows = 1;
            } else {
                m = &h2v2_downsample;
            }
        } else if (c->max_h_samp_factor % comp->h_samp_factor == 0 && c->max_v_samp_factor % comp->v_samp_factor == 0) {
            smoothok = false;
            m = &int_downsample;
        } else {
            errexit(c, JERR_FRACT_SAMPLE_NOTIMPL);
            continue;
        }
        ds->methods[ci] = m;
    }
    if (c->smoothing_factor != 0 && !smoothok) emit_msg(c, JTRC_SMOOTH_NOTIMPL, 0);
}

// =============================================================================================
// jcprepct.c
// =============================================================================================
// 0x007aecb0
void expand_bottom_edge(JSAMPARRAY image_data, JDIMENSION num_cols, int input_rows, int output_rows) {
    for (int row = input_rows; row < output_rows; row++)
        jcopy_sample_rows(image_data, input_rows - 1, image_data, row, 1, num_cols);
}

// 0x007aeae0
void start_pass_prep(j_compress_ptr c, int pass_mode) {
    Prep* prep = priv<Prep>(c->prep);
    if (pass_mode != JBUF_PASS_THRU) errexit(c, JERR_BAD_BUFFER_MODE);
    prep->rows_to_go = c->image_height;
    prep->next_buf_row = 0;
    prep->this_row_group = 0;
    prep->next_buf_stop = 2 * c->max_v_samp_factor;
}

// 0x007aeb30
void pre_process_data(j_compress_ptr c, JSAMPARRAY input_buf, JDIMENSION* in_row_ctr, JDIMENSION in_rows_avail,
                      JSAMPIMAGE output_buf, JDIMENSION* out_row_group_ctr, JDIMENSION out_row_groups_avail) {
    Prep* prep = priv<Prep>(c->prep);
    while (*in_row_ctr < in_rows_avail && *out_row_group_ctr < out_row_groups_avail) {
        const JDIMENSION inrows = in_rows_avail - *in_row_ctr;
        JDIMENSION numrows = static_cast<JDIMENSION>(c->max_v_samp_factor - prep->next_buf_row);
        if (numrows >= inrows) numrows = inrows;
        c->cconvert->color_convert(c, input_buf + *in_row_ctr, prep->color_buf,
                                   static_cast<JDIMENSION>(prep->next_buf_row), static_cast<int>(numrows));
        *in_row_ctr += numrows;
        prep->next_buf_row += static_cast<int>(numrows);
        prep->rows_to_go -= numrows;
        if (prep->rows_to_go == 0 && prep->next_buf_row < c->max_v_samp_factor) {
            for (int ci = 0; ci < c->num_components; ci++)
                expand_bottom_edge(prep->color_buf[ci], c->image_width, prep->next_buf_row, c->max_v_samp_factor);
            prep->next_buf_row = c->max_v_samp_factor;
        }
        if (prep->next_buf_row == c->max_v_samp_factor) {
            c->downsample->downsample(c, prep->color_buf, 0, output_buf, *out_row_group_ctr);
            prep->next_buf_row = 0;
            (*out_row_group_ctr)++;
        }
        if (prep->rows_to_go == 0 && *out_row_group_ctr < out_row_groups_avail) {
            jpeg_component_info* comp = c->comp_info;
            for (int ci = 0; ci < c->num_components; ci++, comp++)
                expand_bottom_edge(output_buf[ci], comp->width_in_blocks * DCTSIZE,
                                   static_cast<int>(*out_row_group_ctr * static_cast<JDIMENSION>(comp->v_samp_factor)),
                                   static_cast<int>(out_row_groups_avail * static_cast<JDIMENSION>(comp->v_samp_factor)));
            *out_row_group_ctr = out_row_groups_avail;
            break;
        }
    }
}

// 0x007aecf0
void pre_process_context(j_compress_ptr c, JSAMPARRAY input_buf, JDIMENSION* in_row_ctr, JDIMENSION in_rows_avail,
                         JSAMPIMAGE output_buf, JDIMENSION* out_row_group_ctr, JDIMENSION out_row_groups_avail) {
    Prep* prep = priv<Prep>(c->prep);
    const int buf_height = c->max_v_samp_factor * 3;
    while (*out_row_group_ctr < out_row_groups_avail) {
        if (*in_row_ctr < in_rows_avail) {
            const JDIMENSION inrows = in_rows_avail - *in_row_ctr;
            JDIMENSION numrows = static_cast<JDIMENSION>(prep->next_buf_stop - prep->next_buf_row);
            if (numrows >= inrows) numrows = inrows;
            c->cconvert->color_convert(c, input_buf + *in_row_ctr, prep->color_buf,
                                       static_cast<JDIMENSION>(prep->next_buf_row), static_cast<int>(numrows));
            if (prep->rows_to_go == c->image_height) {
                for (int ci = 0; ci < c->num_components; ci++)
                    for (int row = 1; row <= c->max_v_samp_factor; row++)
                        jcopy_sample_rows(prep->color_buf[ci], 0, prep->color_buf[ci], -row, 1, c->image_width);
            }
            *in_row_ctr += numrows;
            prep->next_buf_row += static_cast<int>(numrows);
            prep->rows_to_go -= numrows;
        } else {
            if (prep->rows_to_go != 0) break;
            if (prep->next_buf_row < prep->next_buf_stop) {
                for (int ci = 0; ci < c->num_components; ci++)
                    expand_bottom_edge(prep->color_buf[ci], c->image_width, prep->next_buf_row, prep->next_buf_stop);
                prep->next_buf_row = prep->next_buf_stop;
            }
        }
        if (prep->next_buf_row == prep->next_buf_stop) {
            c->downsample->downsample(c, prep->color_buf, static_cast<JDIMENSION>(prep->this_row_group), output_buf,
                                      *out_row_group_ctr);
            (*out_row_group_ctr)++;
            prep->this_row_group += c->max_v_samp_factor;
            if (prep->this_row_group >= buf_height) prep->this_row_group = 0;
            if (prep->next_buf_row >= buf_height) prep->next_buf_row = 0;
            prep->next_buf_stop = prep->next_buf_row + c->max_v_samp_factor;
        }
    }
}

// 0x007aeec0
void create_context_buffer(j_compress_ptr c) {
    Prep* prep = priv<Prep>(c->prep);
    const int rgroup_height = c->max_v_samp_factor;
    auto* fake_buffer = static_cast<JSAMPARRAY>(c->mem->alloc_small(
        c, JPOOL_IMAGE, static_cast<jsize>(c->num_components * 5 * rgroup_height) * SIZEOF<JSAMPROW>));
    jpeg_component_info* comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++) {
        const int width = static_cast<int>(comp->width_in_blocks * DCTSIZE *
                                           static_cast<JDIMENSION>(c->max_h_samp_factor)) /
                          comp->h_samp_factor;
        JSAMPARRAY true_buffer = c->mem->alloc_sarray(c, JPOOL_IMAGE, static_cast<JDIMENSION>(width),
                                                      static_cast<JDIMENSION>(3 * rgroup_height));
        std::memcpy(fake_buffer + rgroup_height, true_buffer,
                    static_cast<jsize>(3 * rgroup_height) * SIZEOF<JSAMPROW>);
        for (int i = 0; i < rgroup_height; i++) {
            fake_buffer[i] = true_buffer[2 * rgroup_height + i];
            fake_buffer[4 * rgroup_height + i] = true_buffer[i];
        }
        prep->color_buf[ci] = fake_buffer + rgroup_height;
        fake_buffer += 5 * rgroup_height;
    }
}

// 0x007aea30
void jinit_c_prep_controller(j_compress_ptr c, boolean need_full_buffer) {
    if (need_full_buffer != 0) errexit(c, JERR_BAD_BUFFER_MODE);
    auto* prep = static_cast<Prep*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<Prep>));
    c->prep = &prep->pub;
    prep->pub.start_pass = &start_pass_prep;
    if (c->downsample->need_context_rows != 0) {
        prep->pub.pre_process_data = &pre_process_context;
        create_context_buffer(c);
    } else {
        prep->pub.pre_process_data = &pre_process_data;
        jpeg_component_info* comp = c->comp_info;
        for (int ci = 0; ci < c->num_components; ci++, comp++) {
            const int width = static_cast<int>(comp->width_in_blocks * DCTSIZE *
                                               static_cast<JDIMENSION>(c->max_h_samp_factor)) /
                              comp->h_samp_factor;
            prep->color_buf[ci] = c->mem->alloc_sarray(c, JPOOL_IMAGE, static_cast<JDIMENSION>(width),
                                                       static_cast<JDIMENSION>(c->max_v_samp_factor));
        }
    }
}

// =============================================================================================
// jcmainct.c
// =============================================================================================
// 0x007ac300
void process_data_simple_main(j_compress_ptr c, JSAMPARRAY input_buf, JDIMENSION* in_row_ctr,
                              JDIMENSION in_rows_avail) {
    MainC* m = priv<MainC>(c->main);
    constexpr JDIMENSION kRowGroups = DCTSIZE;  // row groups per iMCU row
    while (m->cur_iMCU_row < c->total_iMCU_rows) {
        if (m->rowgroup_ctr < kRowGroups)
            c->prep->pre_process_data(c, input_buf, in_row_ctr, in_rows_avail, m->buffer, &m->rowgroup_ctr,
                                      kRowGroups);
        if (m->rowgroup_ctr != kRowGroups) return;
        if (c->coef->compress_data(c, m->buffer) == 0) {
            if (m->suspended == 0) {
                (*in_row_ctr)--;
                m->suspended = 1;
            }
            return;
        }
        if (m->suspended != 0) {
            (*in_row_ctr)++;
            m->suspended = 0;
        }
        m->rowgroup_ctr = 0;
        m->cur_iMCU_row++;
    }
}

// 0x007ac2b0
void start_pass_main(j_compress_ptr c, int pass_mode) {
    MainC* m = priv<MainC>(c->main);
    if (c->raw_data_in != 0) return;
    m->cur_iMCU_row = 0;
    m->rowgroup_ctr = 0;
    m->suspended = 0;
    m->pass_mode = pass_mode;
    if (pass_mode == JBUF_PASS_THRU)
        m->pub.process_data = &process_data_simple_main;
    else
        errexit(c, JERR_BAD_BUFFER_MODE);
}

// 0x007ac220
void jinit_c_main_controller(j_compress_ptr c, boolean need_full_buffer) {
    auto* m = static_cast<MainC*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<MainC>));
    c->main = &m->pub;
    m->pub.start_pass = &start_pass_main;
    if (c->raw_data_in != 0) return;
    if (need_full_buffer != 0) {
        errexit(c, JERR_BAD_BUFFER_MODE);
        return;
    }
    jpeg_component_info* comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++)
        m->buffer[ci] = c->mem->alloc_sarray(c, JPOOL_IMAGE, comp->width_in_blocks * DCTSIZE,
                                             static_cast<JDIMENSION>(comp->v_samp_factor * DCTSIZE));
}

// =============================================================================================
// jccoefct.c
// =============================================================================================
// 0x007ac530
void start_iMCU_row(j_compress_ptr c) {
    Coef* coef = priv<Coef>(c->coef);
    if (c->comps_in_scan > 1) {
        coef->MCU_rows_per_iMCU_row = 1;
    } else {
        if (coef->iMCU_row_num < c->total_iMCU_rows - 1)
            coef->MCU_rows_per_iMCU_row = c->cur_comp_info[0]->v_samp_factor;
        else
            coef->MCU_rows_per_iMCU_row = c->cur_comp_info[0]->last_row_height;
    }
    coef->mcu_ctr = 0;
    coef->MCU_vert_offset = 0;
}

// 0x007ac580
boolean compress_data(j_compress_ptr c, JSAMPIMAGE input_buf) {
    Coef* coef = priv<Coef>(c->coef);
    const JDIMENSION last_MCU_col = c->MCUs_per_row - 1;
    const JDIMENSION last_iMCU_row = c->total_iMCU_rows - 1;
    for (int yoffset = coef->MCU_vert_offset; yoffset < coef->MCU_rows_per_iMCU_row; yoffset++) {
        for (JDIMENSION col = coef->mcu_ctr; col <= last_MCU_col; col++) {
            int blkn = 0;
            for (int ci = 0; ci < c->comps_in_scan; ci++) {
                jpeg_component_info* comp = c->cur_comp_info[ci];
                const int blockcnt = col < last_MCU_col ? comp->MCU_width : comp->last_col_width;
                const JDIMENSION xpos = col * static_cast<JDIMENSION>(comp->MCU_sample_width);
                JDIMENSION ypos = static_cast<JDIMENSION>(yoffset) * DCTSIZE;
                for (int yindex = 0; yindex < comp->MCU_height; yindex++) {
                    if (coef->iMCU_row_num < last_iMCU_row || yoffset + yindex < comp->last_row_height) {
                        c->fdct->forward_DCT(c, comp, input_buf[comp->component_index], coef->MCU_buffer[blkn], ypos,
                                             xpos, static_cast<JDIMENSION>(blockcnt));
                        if (blockcnt < comp->MCU_width) {
                            jzero_far(coef->MCU_buffer[blkn + blockcnt],
                                      static_cast<jsize>(comp->MCU_width - blockcnt) * SIZEOF<JBLOCK>);
                            for (int bi = blockcnt; bi < comp->MCU_width; bi++)
                                coef->MCU_buffer[blkn + bi][0][0] = coef->MCU_buffer[blkn + bi - 1][0][0];
                        }
                    } else {
                        jzero_far(coef->MCU_buffer[blkn],
                                  static_cast<jsize>(comp->MCU_width) * SIZEOF<JBLOCK>);
                        for (int bi = 0; bi < comp->MCU_width; bi++)
                            coef->MCU_buffer[blkn + bi][0][0] = coef->MCU_buffer[blkn - 1][0][0];
                    }
                    blkn += comp->MCU_width;
                    ypos += DCTSIZE;
                }
            }
            if (c->entropy->encode_mcu(c, coef->MCU_buffer) == 0) {
                coef->MCU_vert_offset = yoffset;
                coef->mcu_ctr = col;
                return 0;
            }
        }
        coef->mcu_ctr = 0;
    }
    coef->iMCU_row_num++;
    start_iMCU_row(c);
    return 1;
}

// 0x007ac9e0
boolean compress_output(j_compress_ptr c, JSAMPIMAGE) {
    Coef* coef = priv<Coef>(c->coef);
    JBLOCKARRAY buffer[MAX_COMPS_IN_SCAN];
    for (int ci = 0; ci < c->comps_in_scan; ci++) {
        jpeg_component_info* comp = c->cur_comp_info[ci];
        buffer[ci] = c->mem->access_virt_barray(c, coef->whole_image[comp->component_index],
                                                coef->iMCU_row_num * static_cast<JDIMENSION>(comp->v_samp_factor),
                                                static_cast<JDIMENSION>(comp->v_samp_factor), 0);
    }
    for (int yoffset = coef->MCU_vert_offset; yoffset < coef->MCU_rows_per_iMCU_row; yoffset++) {
        for (JDIMENSION col = coef->mcu_ctr; col < c->MCUs_per_row; col++) {
            int blkn = 0;
            for (int ci = 0; ci < c->comps_in_scan; ci++) {
                jpeg_component_info* comp = c->cur_comp_info[ci];
                const JDIMENSION start_col = col * static_cast<JDIMENSION>(comp->MCU_width);
                for (int yindex = 0; yindex < comp->MCU_height; yindex++) {
                    JBLOCKROW bp = buffer[ci][yindex + yoffset] + start_col;
                    for (int xindex = 0; xindex < comp->MCU_width; xindex++) coef->MCU_buffer[blkn++] = bp++;
                }
            }
            if (c->entropy->encode_mcu(c, coef->MCU_buffer) == 0) {
                coef->MCU_vert_offset = yoffset;
                coef->mcu_ctr = col;
                return 0;
            }
        }
        coef->mcu_ctr = 0;
    }
    coef->iMCU_row_num++;
    start_iMCU_row(c);
    return 1;
}

// 0x007ac7d0
boolean compress_first_pass(j_compress_ptr c, JSAMPIMAGE input_buf) {
    Coef* coef = priv<Coef>(c->coef);
    const JDIMENSION last_iMCU_row = c->total_iMCU_rows - 1;
    jpeg_component_info* comp = c->comp_info;
    for (int ci = 0; ci < c->num_components; ci++, comp++) {
        JBLOCKARRAY buffer = c->mem->access_virt_barray(
            c, coef->whole_image[ci], coef->iMCU_row_num * static_cast<JDIMENSION>(comp->v_samp_factor),
            static_cast<JDIMENSION>(comp->v_samp_factor), 1);
        int block_rows;
        if (coef->iMCU_row_num < last_iMCU_row) {
            block_rows = comp->v_samp_factor;
        } else {
            block_rows = static_cast<int>(comp->height_in_blocks % static_cast<JDIMENSION>(comp->v_samp_factor));
            if (block_rows == 0) block_rows = comp->v_samp_factor;
        }
        JDIMENSION blocks_across = comp->width_in_blocks;
        const int h_samp_factor = comp->h_samp_factor;
        int ndummy = static_cast<int>(blocks_across % static_cast<JDIMENSION>(h_samp_factor));
        if (ndummy > 0) ndummy = h_samp_factor - ndummy;
        for (int block_row = 0; block_row < block_rows; block_row++) {
            JBLOCKROW thisblockrow = buffer[block_row];
            c->fdct->forward_DCT(c, comp, input_buf[ci], thisblockrow, static_cast<JDIMENSION>(block_row * DCTSIZE),
                                 0, blocks_across);
            if (ndummy > 0) {
                thisblockrow += blocks_across;
                jzero_far(thisblockrow, static_cast<jsize>(ndummy) * SIZEOF<JBLOCK>);
                const JCOEF lastDC = thisblockrow[-1][0];
                for (int bi = 0; bi < ndummy; bi++) thisblockrow[bi][0] = lastDC;
            }
        }
        if (coef->iMCU_row_num == last_iMCU_row) {
            blocks_across += static_cast<JDIMENSION>(ndummy);
            const JDIMENSION MCUs_across = blocks_across / static_cast<JDIMENSION>(h_samp_factor);
            for (int block_row = block_rows; block_row < comp->v_samp_factor; block_row++) {
                JBLOCKROW thisblockrow = buffer[block_row];
                JBLOCKROW lastblockrow = buffer[block_row - 1];
                jzero_far(thisblockrow, blocks_across * SIZEOF<JBLOCK>);
                for (JDIMENSION MCUindex = 0; MCUindex < MCUs_across; MCUindex++) {
                    const JCOEF lastDC = lastblockrow[h_samp_factor - 1][0];
                    for (int bi = 0; bi < h_samp_factor; bi++) thisblockrow[bi][0] = lastDC;
                    thisblockrow += h_samp_factor;
                    lastblockrow += h_samp_factor;
                }
            }
        }
    }
    return compress_output(c, input_buf);
}

// 0x007ac480
void start_pass_coef(j_compress_ptr c, int pass_mode) {
    Coef* coef = priv<Coef>(c->coef);
    coef->iMCU_row_num = 0;
    start_iMCU_row(c);
    switch (pass_mode) {
    case JBUF_PASS_THRU:
        if (coef->whole_image[0] != nullptr) errexit(c, JERR_BAD_BUFFER_MODE);
        coef->pub.compress_data = &compress_data;
        break;
    case JBUF_SAVE_AND_PASS:
        if (coef->whole_image[0] == nullptr) errexit(c, JERR_BAD_BUFFER_MODE);
        coef->pub.compress_data = &compress_first_pass;
        break;
    case JBUF_CRANK_DEST:
        if (coef->whole_image[0] == nullptr) errexit(c, JERR_BAD_BUFFER_MODE);
        coef->pub.compress_data = &compress_output;
        break;
    default:
        errexit(c, JERR_BAD_BUFFER_MODE);
        break;
    }
}

// 0x007ac3b0
void jinit_c_coef_controller(j_compress_ptr c, boolean need_full_buffer) {
    auto* coef = static_cast<Coef*>(c->mem->alloc_small(c, JPOOL_IMAGE, SIZEOF<Coef>));
    c->coef = &coef->pub;
    coef->pub.start_pass = &start_pass_coef;
    if (need_full_buffer != 0) {
        jpeg_component_info* comp = c->comp_info;
        for (int ci = 0; ci < c->num_components; ci++, comp++) {
            const jlong rows = jround_up(static_cast<jlong>(comp->height_in_blocks), comp->v_samp_factor);
            const jlong blocks = jround_up(static_cast<jlong>(comp->width_in_blocks), comp->h_samp_factor);
            coef->whole_image[ci] = c->mem->request_virt_barray(c, JPOOL_IMAGE, 0, static_cast<JDIMENSION>(blocks),
                                                                static_cast<JDIMENSION>(rows),
                                                                static_cast<JDIMENSION>(comp->v_samp_factor));
        }
    } else {
        auto buffer = static_cast<JBLOCKROW>(
            c->mem->alloc_large(c, JPOOL_IMAGE, C_MAX_BLOCKS_IN_MCU * SIZEOF<JBLOCK>));
        for (int i = 0; i < C_MAX_BLOCKS_IN_MCU; i++) coef->MCU_buffer[i] = buffer + i;
        coef->whole_image[0] = nullptr;
    }
}

}  // namespace

// =============================================================================================
// jcinit.c
// =============================================================================================
// 0x007ac000
void jinit_compress_master(j_compress_ptr c) {
    c_compress_master.hit();
    jinit_c_master_control(c, 0);
    if (c->raw_data_in == 0) {
        jinit_color_converter(c);
        jinit_downsampler(c);
        jinit_c_prep_controller(c, 0);
    }
    jinit_forward_dct(c);
    if (c->arith_code != 0) {
        errexit(c, JERR_ARITH_NOTIMPL);
    } else {
        if (c->progressive_mode != 0)
            jinit_phuff_encoder(c);
        else
            jinit_huff_encoder(c);
    }
    jinit_c_coef_controller(c, (c->num_scans > 1 || c->optimize_coding != 0) ? 1 : 0);
    jinit_c_main_controller(c, 0);
    jinit_marker_writer(c);
    c->mem->realize_virt_arrays(c);
    c->marker->write_file_header(c);
}

// =============================================================================================
// Registration (no thunks: entry patches)
// =============================================================================================
#define L(name, addr, f) JOAN_REPLACE("lib/b115_05", name, addr, f)
L("jinit_memory_mgr", 0x007aab10, &jinit_memory_mgr);
L("jmemmgr alloc_small", 0x007aac50, &alloc_small);
L("jmemmgr out_of_memory", 0x007aad80, &out_of_memory);
L("jmemmgr alloc_large", 0x007aada0, &alloc_large);
L("jmemmgr alloc_sarray", 0x007aae50, &alloc_sarray);
L("jmemmgr alloc_barray", 0x007aaef0, &alloc_barray);
L("jmemmgr request_virt_sarray", 0x007aafa0, &request_virt_sarray);
L("jmemmgr request_virt_barray", 0x007ab010, &request_virt_barray);
L("jmemmgr realize_virt_arrays", 0x007ab080, &realize_virt_arrays);
L("jmemmgr access_virt_sarray", 0x007ab210, &access_virt_sarray);
L("jmemmgr do_sarray_io", 0x007ab360, &do_sarray_io);
L("jmemmgr access_virt_barray", 0x007ab400, &access_virt_barray);
L("jmemmgr do_barray_io", 0x007ab550, &do_barray_io);
L("jmemmgr free_pool", 0x007ab5f0, &free_pool);
L("jmemmgr self_destruct", 0x007ab6f0, &self_destruct);
L("jinit_compress_master", 0x007ac000, &jinit_compress_master);
L("jpeg_get_small", 0x007ac0c0, &jpeg_get_small);
L("jpeg_free_small", 0x007ac0d0, &jpeg_free_small);
L("jpeg_get_large", 0x007ac0e0, &jpeg_get_large);
L("jpeg_free_large", 0x007ac0f0, &jpeg_free_large);
L("jpeg_mem_available", 0x007ac100, &jpeg_mem_available);
L("jpeg_open_backing_store", 0x007ac110, &jpeg_open_backing_store);
L("jpeg_mem_init", 0x007ac130, &jpeg_mem_init);
L("jpeg_mem_term", 0x007ac140, &jpeg_mem_term);
L("jinit_c_main_controller", 0x007ac220, &jinit_c_main_controller);
L("jcmainct start_pass_main", 0x007ac2b0, &start_pass_main);
L("jcmainct process_data_simple_main", 0x007ac300, &process_data_simple_main);
L("jinit_c_coef_controller", 0x007ac3b0, &jinit_c_coef_controller);
L("jccoefct start_pass_coef", 0x007ac480, &start_pass_coef);
L("jccoefct start_iMCU_row", 0x007ac530, &start_iMCU_row);
L("jccoefct compress_data", 0x007ac580, &compress_data);
L("jccoefct compress_first_pass", 0x007ac7d0, &compress_first_pass);
L("jccoefct compress_output", 0x007ac9e0, &compress_output);
L("jinit_c_prep_controller", 0x007aea30, &jinit_c_prep_controller);
L("jcprepct start_pass_prep", 0x007aeae0, &start_pass_prep);
L("jcprepct pre_process_data", 0x007aeb30, &pre_process_data);
L("jcprepct expand_bottom_edge", 0x007aecb0, &expand_bottom_edge);
L("jcprepct pre_process_context", 0x007aecf0, &pre_process_context);
L("jcprepct create_context_buffer", 0x007aeec0, &create_context_buffer);
L("jinit_downsampler", 0x007af000, &jinit_downsampler);
L("jcsample start_pass_downsample", 0x007af180, &start_pass_downsample);
L("jcsample sep_downsample", 0x007af190, &sep_downsample);
L("jcsample int_downsample", 0x007af220, &int_downsample);
L("jcsample expand_right_edge", 0x007af340, &expand_right_edge);
L("jcsample fullsize_downsample", 0x007af3a0, &fullsize_downsample);
L("jcsample h2v1_downsample", 0x007af3f0, &h2v1_downsample);
L("jcsample h2v2_downsample", 0x007af4a0, &h2v2_downsample);
L("jcsample h2v2_smooth_downsample", 0x007af570, &h2v2_smooth_downsample);
L("jcsample fullsize_smooth_downsample", 0x007af860, &fullsize_smooth_downsample);
L("jinit_color_converter", 0x007af9f0, &jinit_color_converter);
L("jccolor rgb_ycc_start", 0x007afc30, &rgb_ycc_start);
L("jccolor rgb_ycc_convert", 0x007afd10, &rgb_ycc_convert);
L("jccolor rgb_gray_convert", 0x007afe40, &rgb_gray_convert);
L("jccolor cmyk_ycck_convert", 0x007afef0, &cmyk_ycck_convert);
L("jccolor grayscale_convert", 0x007b0060, &grayscale_convert);
L("jccolor null_convert", 0x007b00d0, &null_convert);
L("jccolor null_method", 0x007b0160, &null_method);
L("jinit_c_master_control", 0x007b0170, &jinit_c_master_control);
L("jcmaster initial_setup", 0x007b0230, &initial_setup);
L("jcmaster validate_script", 0x007b0400, &validate_script);
L("jcmaster prepare_for_pass", 0x007b07a0, &prepare_for_pass);
L("jcmaster select_scan_parameters", 0x007b0940, &select_scan_parameters);
L("jcmaster per_scan_setup", 0x007b0a40, &per_scan_setup);
L("jcmaster pass_startup", 0x007b0c10, &pass_startup);
L("jcmaster finish_pass_master", 0x007b0c40, &finish_pass_master);
#undef L

}  // namespace joan::jpeg
