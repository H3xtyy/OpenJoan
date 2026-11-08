#!/usr/bin/env python3
"""Attribute every registered replacement (JOAN_REPLACE* / module macros with an address) to the ORIGINAL
source file of joan.exe it came from (d:\\ww\\battle.cpp -> ww/battle.cpp, d:\\ww\\ilib\\dynarr.cpp ->
ww/ilib/dynarr.cpp, MSVC CRT -> crt/<vc6 source>, ...).

Usage (from the repository root):
    python3 re/attribute.py [--exe ../re/joan.exe] [--asm ../re/joan.asm] [--functions ../re/functions.json]
                            [--thunks ../re/thunks.json] [--hook hook] [--out re/attribution.csv]
                            [--summary re/attribution_summary.md]

Inputs: joan.exe, its objdump disassembly (joan.asm) and functions.json / thunks.json from re/analyze.py
(see re/make_csv.py for how to produce them), plus the src/ sources (registrations).

Method (details and caveats in re/attribution_summary.md):
 1. anchors    - game functions that reference a d:\\ww\\...\\x.cpp __FILE__ string in .rdata (plus the manual
                 'analysis' rows of re/functions.csv).
 2. link order - the __FILE__ strings lie in .rdata in object (link) order; .text follows the same order. A
                 forward-only Viterbi pass segments the game functions (address order) into those objects, scored
                 by anchors and by private .rdata / initialised .data references inside each file's data extent
                 (learned from anchored code; .bss is ignored - communal variables are not in object order).
 3. per function: own __FILE__ / private data that contradict the segment = incremental-link move (kept with
                 its own file); between two in-place evidence functions of one file = range; at a segment edge:
                 class name / call graph (class), else nearer side (guess); more than UNKNOWN_DIST functions from
                 any evidence = unknown (mostly objects without a __FILE__ string).
 4. libraries  - import stubs, D3DX8 (by name), IJG libjpeg and MSVC 6 CRT (function name -> source file).
"""
import argparse
import bisect
import collections
import csv
import json
import os
import pickle
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fn_scan as F  # noqa: E402
from pe import PE  # noqa: E402

GAME_HI = 0x763D00          # end of game code: import jump stubs (0x763d02..) and D3DX8 (0x763fc0..) follow
LATE_LO = 0x7C6240          # incremental-link late area after the CRT (game_b114_14 / lib_b113_14)

# ---------------------------------------------------------------------------------------------- library map
# (lo, hi, library, default file). Boundaries from the lib_* module header comments.
LIB_RANGES = [
    (GAME_HI, 0x763FC0, 'imports', 'imports'),
    (0x763FC0, 0x7A8940, 'd3dx', 'd3dx8/d3dx8.cpp'),
    (0x7A8940, 0x7A9420, 'imports', 'imports'),
    (0x7A9420, 0x7A99A0, 'alut', 'openal/alut.c'),
    (0x7A99A0, 0x7A9A60, 'imports', 'imports'),
    (0x7A9A60, 0x7B1540, 'jpeg', 'jpeg/unknown'),
    (0x7B1540, LATE_LO, 'crt', 'crt/unknown'),
]
DLL_PREFIX = re.compile(r'^(KERNEL32|USER32|GDI32|ADVAPI32|WINMM|ole32|comdlg32|d3d8|OpenAL32|DSOUND|DINPUT8|v3d_87)\b',
                        re.I)
# D3DX8 (statically linked d3dx8.lib): name pattern -> file (grouping by the SDK's source areas)
D3DX_FILES = [
    (r'^D3DX(Matrix|Quaternion|Plane|Vec[234])', 'd3dx8/d3dx8math.cpp'),
    (r'^D3DX(Create|Check).*Texture|^D3DX format|^D3DX texture', 'd3dx8/d3dx8tex.cpp'),
    (r'^D3DXAssembleShader', 'd3dx8/d3dx8shader.cpp'),
    (r'^CFileMapping', 'd3dx8/filemapping.cpp'),
    (r'^ID3DXBuffer', 'd3dx8/d3dxbuffer.cpp'),
    (r'^D3DXCreateMatrixStack', 'd3dx8/matrixstack.cpp'),
    (r'^D3DXGetErrorString', 'd3dx8/d3dx8err.cpp'),
    (r'^D3DXDeclaratorFromFVF', 'd3dx8/d3dx8mesh.cpp'),
]

# IJG libjpeg 6b: function -> source file (compressor side + memory manager + utils)
JPEG_FILES = {
    'jcapimin.c': 'jpeg_CreateCompress jpeg_destroy_compress jpeg_abort_compress jpeg_suppress_tables '
                  'jpeg_finish_compress jpeg_write_marker jpeg_write_m_header jpeg_write_m_byte jpeg_write_tables',
    'jcapistd.c': 'jpeg_start_compress jpeg_write_scanlines jpeg_write_raw_data',
    'jcparam.c': 'jpeg_add_quant_table jpeg_set_linear_quality jpeg_quality_scaling jpeg_set_quality '
                 'jpeg_set_defaults jpeg_default_colorspace jpeg_set_colorspace std_huff_tables add_huff_table '
                 'fill_a_scan fill_scans fill_dc_scans jpeg_simple_progression',
    'jdatadst.c': 'jpeg_stdio_dest init_destination empty_output_buffer term_destination',
    'jmemmgr.c': 'jinit_memory_mgr jmemmgr alloc_small alloc_large alloc_sarray alloc_barray request_virt_sarray '
                 'request_virt_barray realize_virt_arrays access_virt_sarray access_virt_barray free_pool self_destruct',
    'jcmarker.c': 'jinit_marker_writer write_marker_header emit_marker emit_byte emit_2bytes write_marker_byte '
                  'write_file_header emit_jfif_app0 emit_adobe_app14 write_frame_header emit_dqt emit_sof '
                  'write_scan_header emit_dht emit_dac emit_dri emit_sos write_file_trailer write_tables_only',
    'jcinit.c': 'jinit_compress_master',
    'jmemnobs.c': 'jpeg_get_small jpeg_free_small jpeg_get_large jpeg_free_large jpeg_mem_available '
                  'jpeg_open_backing_store jpeg_mem_init jpeg_mem_term',
    'jutils.c': 'jdiv_round_up jround_up jcopy_sample_rows jcopy_block_row jzero_far',
    'jcmainct.c': 'jinit_c_main_controller jcmainct',
    'jccoefct.c': 'jinit_c_coef_controller jccoefct',
    'jchuff.c': 'jpeg_make_c_derived_tbl jpeg_gen_optimal_table jinit_huff_encoder start_pass_huff encode_mcu_huff '
                'encode_one_block huff_emit_bits huff_dump_buffer huff_emit_restart huff_flush_bits finish_pass_huff '
                'encode_mcu_gather htest_one_block finish_pass_gather',
    'jcphuff.c': 'jinit_phuff_encoder start_pass_phuff encode_mcu_DC_first phuff_emit_bits phuff_dump_buffer '
                 'emit_symbol phuff_emit_restart phuff_flush_bits emit_eobrun emit_buffered_bits encode_mcu_AC_first '
                 'encode_mcu_DC_refine encode_mcu_AC_refine finish_pass_phuff finish_pass_gather_phuff',
    'jcdctmgr.c': 'jinit_forward_dct start_pass_fdctmgr forward_DCT forward_DCT_float',
    'jcprepct.c': 'jinit_c_prep_controller jcprepct',
    'jcsample.c': 'jinit_downsampler jcsample',
    'jccolor.c': 'jinit_color_converter jccolor',
    'jcmaster.c': 'jinit_c_master_control jcmaster',
    'jfdctint.c': 'jpeg_fdct_islow', 'jfdctfst.c': 'jpeg_fdct_ifast', 'jfdctflt.c': 'jpeg_fdct_float',
    'jcomapi.c': 'jpeg_abort jpeg_destroy jpeg_alloc_quant_table jpeg_alloc_huff_table',
    'jerror.c': 'error_exit output_message emit_message format_message reset_error_mgr jpeg_std_error',
}

# MSVC 6 CRT (LIBCMT): function -> VC6 source file (crt/src, crt/src/intel)
CRT_FILES = {
    'atox.c': 'atol atoi', 'intel/memset.asm': 'memset', 'intel/strcmp.asm': 'strcmp', 'intel/strstr.asm': 'strstr',
    'strupr.c': '_strupr _strlwr', 'intel/exsup.asm': '_global_unwind2 __unwind_handler _local_unwind2 _NLG_Notify',
    'delete.cpp': 'delete', 'new.cpp': 'new', 'fpinit.c': '_fpmath _cfltcvt_init',
    'intel/strncpy.asm': 'strncpy', 'intel/strcspn.asm': 'strcspn', 'intel/strlen.asm': 'strlen',
    'tran/i386/fabs.asm': 'fabs', 'intel/strcat.asm': 'strcpy strcat', 'atof.c': 'atof',
    'tran/i386/87ctriga.asm': '_CIatan atan atan2 _CIatan2 _CIacos acos _CIasin asin',
    'intel/strrchr.asm': 'strrchr', 'tran/i386/87csqrt.asm': '_CIsqrt sqrt',
    'tran/i386/87ctrig.asm': '_CIsin sin _CIcos cos _CItan tan', 'intel/memcmp.asm': 'memcmp',
    'tran/i386/ftol.asm': '_ftol', 'tran/i386/87cpow.asm': '_CIpow pow', 'tran/i386/87cfmod.asm': 'fmod _CIfmod',
    'sprintf.c': 'sprintf _snprintf', 'abs.c': 'abs', 'intel/chkstk.asm': '_chkstk', 'intel/strncmp.asm': 'strncmp',
    'intel/memcpy.asm': 'memcpy', 'qsort.c': 'qsort', 'intel/ulldiv.asm': '_aulldiv',
    'ehvecctr.cpp': 'eh_vector_constructor_iterator __ArrayUnwind', 'intel/llmul.asm': '_allmul',
    'ehvecdtr.cpp': 'eh_vector_destructor_iterator', 'strtok.c': 'strtok', 'tran/i386/ceil.c': 'ceil',
    'tran/i386/floor.c': 'floor', 'unlink.c': 'remove _unlink', 'time.c': 'time', 'mkdir.c': '_mkdir',
    'intel/strchr.asm': 'strchr', 'onexit.c': '_onexit atexit __onexitinit', 'bsearch.c': 'bsearch',
    'lfind.c': '_lfind', 'findfile.c': '_findfirst _findnext _findclose __timet_from_ft',
    'makepath.c': '_makepath', 'splitpat.c': '_splitpath',
    'ctype.c': 'isalpha isdigit isspace isalnum', 'intel/strncat.asm': 'strncat', 'rand.c': 'srand rand',
    'sscanf.c': 'sscanf', 'fclose.c': 'fclose _fclose_lk', 'fprintf.c': 'fprintf', 'fopen.c': '_fsopen fopen',
    'chdir.c': '_chdir', 'getcwd.c': '_getcwd _getdcwd_lk _validdrive', 'rename.c': 'rename',
    'tran/i386/modf.asm': 'modf', 'thread.c': '_beginthread _threadstart _endthread',
    'toupper.c': 'toupper _toupper_lk', 'localtim.c': 'localtime',
    'crt0dat.c': '_cinit exit _exit _cexit _c_exit doexit _initterm',
    'handler.cpp': '_set_new_handler _callnewh', 'vsprintf.c': 'vsprintf _vsnprintf', 'tolower.c': 'tolower _tolower_lk',
    'free.c': 'free', 'mbctype.c': '_setmbcp __initmbctable', 'setlocal.c': 'setlocale',
    'mbsnextc.c': '_mbsnextc', 'mbsinc.c': '_mbsinc', 'mbslen.c': '_mbslen', 'intel/wcslen.c': 'wcslen',
    'wcslen.c': 'wcslen', 'mbscpy.c': '_mbscpy', 'mbscat.c': '_mbscat', 'wcscat.c': 'wcscat wcscpy',
    'mbsncpy.c': '_mbsncpy', 'wcsncpy.c': 'wcsncpy', 'mbsnbcpy.c': '_mbsnbcpy', 'mbsncat.c': '_mbsncat',
    'wcsncat.c': 'wcsncat', 'mbsnbcat.c': '_mbsnbcat', 'mbschr.c': '_mbschr', 'wcschr.c': 'wcschr',
    'mbsrchr.c': '_mbsrchr', 'wcsrchr.c': 'wcsrchr', 'mbsstr.c': '_mbsstr', 'wcsstr.c': 'wcsstr',
    'mbscmp.c': '_mbscmp', 'wcscmp.c': 'wcscmp', 'mbsncmp.c': '_mbsncmp', 'wcsncmp.c': 'wcsncmp',
    'strcoll.c': 'strcoll', 'mbscoll.c': '_mbscoll', 'wcscoll.c': 'wcscoll', 'strncoll.c': '_strncoll',
    'mbsncoll.c': '_mbsncoll', 'wcsncoll.c': '_wcsncoll', 'stricoll.c': '_stricoll', 'mbsicoll.c': '_mbsicoll',
    'wcsicoll.c': '_wcsicoll', 'strnicol.c': '_strnicoll', 'mbsnicol.c': '_mbsnicoll', 'wcsnicol.c': '_wcsnicoll',
    'mbslwr.c': '_mbslwr', 'mbsupr.c': '_mbsupr', 'clock.c': 'clock __inittime', 'fread.c': 'fread _fread_lk',
    'malloc.c': 'malloc _nh_malloc _heap_alloc', 'ftell.c': 'ftell _ftell_lk', 'fseek.c': 'fseek _fseek_lk',
    'fgetc.c': 'fgetc getc', 'intel/lldiv.asm': '_alldiv', 'tran/i386/87clog.asm': '_CIlog log',
    'tran/i386/87cexp.asm': 'exp _CIexp', 'purevirt.c': '_purecall', 'typinfo.cpp': '~type_info scalar_deleting_destructor',
    'throw.cpp': '_CxxThrowException', 'crt0.c': 'WinMainCRTStartup _amsg_exit fast_error_exit',
    'tran/ieee87.c': '_copysign _chgsign _scalb _logb _nextafter _finite _isnan _fpclass',
    'tran/i386/ieee87.c': '_statusfp _clearfp _control87 _controlfp _fpreset', 'calloc.c': 'calloc',
    'fwrite.c': 'fwrite _fwrite_lk', 'fflush.c': 'fflush _fflush_lk _flush _flushall flsall',
    'getenv.c': 'getenv _getenv_lk', 'isctype.c': '_isctype', 'a_map.c': '__crtLCMapStringA __crtLCMapStringW',
    'mlock.c': '_mtinitlocks _mtdeletelocks _lock _unlock',
    'tidtable.c': '_mtinit _mtterm _initptd _getptd _freeptd', 'hooks.cpp': 'terminate',
    'fp8.c': '_setdefaultprecision', 'intel/testfdiv.c': '_ms_p5_test_fdiv _ms_p5_mp_test_fdiv',
    'cvt.c': '_forcdecpt _cropzeros _positive _fassign _cfltcvt',
    'tran/i386/fpexcept.c': '_handle_qnan1 _except1 _ctrlfp', 'tran/sptype.c': '_sptype',
    'fltin.c': '_fltin', 'tran/i386/87except.c': '_math_nan_result', 'tran/i386/87fload.asm': '_fload',
    'tran/i386/87disp.asm': '_fast_exit _check_range_exit _math_exit',
    'intel/adj_fdiv.asm': 'fdivp_sti_st _adj_fdivrp_sti_st _fprem_core _adj_fprem_common',
    '_flsbuf.c': '_flsbuf', 'output.c': '_output', 'intel/exsup3.asm': '_except_handler3 _seh_longjmp_unwind',
    'tran/i386/frnd.c': '_frnd', 'dosmap.c': '_dosmaperr _errno __doserrno', 'dtoxtime.c': '__loctotime_t',
    'realloc.c': 'realloc', 'msize.c': '_msize', 'mbsdec.c': '_mbsdec',
    'input.c': '_input _hextodec _inc _un_inc _whiteout', '_file.c': '__initstdio __endstdio _lock_file _lock_file2 '
    '_unlock_file _unlock_file2', 'close.c': '_close _close_lk', '_freebuf.c': '_freebuf', '_sftbuf.c': '_stbuf _ftbuf',
    '_open.c': '_openfile', 'stream.c': '_getstream', 'mbtoupr.c': '_mbctoupper',
    'winxfltr.c': '_XcptFilter xcptlookup __pxcptinfoptrs', 'tzset.c': '__tzset _tzset_lk _isindst _isindst_lk cvtdate',
    'gmtime.c': 'gmtime', 'heapinit.c': '_heap_init', 'a_str.c': '__crtGetStringTypeA __crtGetStringTypeW',
    'mbsbtype.c': '_mbsbtype', 'ismbslead.c': '_ismbslead', 'a_cmp.c': '__crtCompareStringA __crtCompareStringW',
    'mbsnbcnt.c': '_mbsnbcnt', 'intel/_stricmp.asm': '_stricmp', 'wcsicmp.c': '_wcsicmp',
    'intel/_strnicm.asm': '_strnicmp', 'wcsnicmp.c': '_wcsnicmp', '_filbuf.c': '_filbuf', 'read.c': '_read _read_lk',
    'ioinit.c': '_ioinit', 'lseek.c': '_lseek _lseek_lk',
    'unhandld.cpp': '__CxxUnhandledExceptionFilter __CxxSetUnhandledExceptionFilter __CxxRestoreUnhandledExceptionFilter',
    'wincmdln.c': '_wincmdln', 'stdenvp.c': '_setenvp', 'stdargv.c': '_setargv parse_cmdline',
    'a_env.c': '__crtGetEnvironmentStringsA', 'crt0msg.c': '_FF_MSGBANNER _NMSG_WRITE',
    'winsig.c': 'signal ctrlevent_capture raise siglookup', 'write.c': '_write _write_lk', 'commit.c': '_commit',
    'mbsnbico.c': '_mbsnbicoll', 'wtombenv.c': '__wtomb_environ', 'validate.c': '_ValidateExecute',
    'abort.c': 'abort', 'intrncvt.c': '_ld12tod _ld12tof _ld12told _atodbl _atoldbl _atoflt',
    '_fptostr.c': '_fptostr', 'cfout.c': '_fltout2', 'crt0fp.c': '_fptrap', 'strgtold.c': '__strgtold12',
    'mantold.c': '$I10_INPUT', '_getbuf.c': '_getbuf', 'isatty.c': '_isatty', 'wctomb.c': 'wctomb _wctomb_lk',
    'mbtowc.c': 'mbtowc _mbtowc_lk', 'ungetc.c': 'ungetc _ungetc_lk', 'closeall.c': '_fcloseall',
    'osfinfo.c': '_alloc_osfhnd _set_osfhnd _free_osfhnd _get_osfhandle _lock_fhandle _unlock_fhandle',
    'open.c': '_sopen', 'getqloc.c': '__getlocaleinfo', 'xtoa.c': '_itoa _ltoa _ultoa', 'mbbtype.c': '_mbbtype',
    'towlower.c': '_towlower_lk', 'ismbbyte.c': '_ismbblead', 'crtmbox.c': '__crtMessageBoxA',
    'setenv.c': '__crtsetenv findenv copy_environ', 'chsize.c': '_chsize_lk', 'a_loc.c': '__crtGetLocaleInfoW '
    '__crtGetLocaleInfoA', 'iswctype.c': 'iswctype', 'strdup.c': '_strdup', 'setmode.c': '_setmode_lk',
    'wcslwr.c': '_wcslwr', 'wcsupr.c': '_wcsupr', 'fcvt.c': '_fcvt _fpcvt',
}


def name_map(table, prefix):
    m = {}
    for f, names in table.items():
        for n in names.split():
            m.setdefault(n, prefix + f)
    return m


JPEG_BY_NAME = name_map(JPEG_FILES, 'jpeg/')
CRT_BY_NAME = name_map(CRT_FILES, 'crt/')
CRT_BY_NAME['wcslen'] = 'crt/wcslen.c'



# ---------------------------------------------------------------------------------------------- binary scan
def load_binary(args):
    key = (os.path.abspath(args.asm), os.path.getmtime(args.asm), os.path.getmtime(args.functions))
    cache = os.path.join(tempfile.gettempdir(), 'joan_attribute_cache.pkl')
    if os.path.exists(cache):
        try:
            c = pickle.load(open(cache, 'rb'))
            if c['key'] == key:
                return c['data']
        except Exception:
            pass
    pe = PE(args.exe)
    funcs = json.load(open(args.functions))
    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(args.thunks)).items()}
    starts = sorted(int(f['addr'], 16) for f in funcs)
    rd = pe.section('.rdata')
    da = pe.section('.data')
    RLO, RHI = pe.base + rd[1], pe.base + rd[1] + rd[2]
    DLO, DHI = pe.base + da[1], pe.base + da[1] + da[2]
    TLO = 0x401000
    THI = RLO
    line_re = re.compile(r'^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$')
    hexnum = re.compile(r'0x([0-9a-f]+)')
    rrefs = collections.defaultdict(set)   # func -> .rdata addresses
    drefs = collections.defaultdict(set)   # func -> .data addresses
    calls = collections.defaultdict(set)
    vals = collections.defaultdict(set)    # func -> code addresses used as values
    si = 0
    cur = None
    nxt = starts[0]
    for ln in open(args.asm):
        m = line_re.match(ln)
        if not m:
            continue
        a = int(m.group(1), 16)
        if a < 0x414000:
            continue
        while a >= nxt:
            cur = nxt
            si += 1
            nxt = starts[si] if si < len(starts) else 1 << 40
        if cur is None:
            continue
        mn, ops = m.group(2), m.group(3)
        for h in hexnum.findall(ops):
            v = int(h, 16)
            if RLO <= v < RHI:
                rrefs[cur].add(v)
            elif DLO <= v < DHI:
                drefs[cur].add(v)
            elif TLO <= v < THI:
                t = thunks.get(v, v)
                if mn == 'call':
                    calls[cur].add(t)
                elif mn in ('push', 'mov', 'lea', 'jmp'):
                    if mn == 'jmp' and not (t < cur or t >= nxt):
                        continue
                    (calls if mn == 'jmp' else vals)[cur].add(t)
    # __FILE__ strings in .rdata
    data = pe.data
    off = pe.va2off(RLO)
    blob = data[off:off + rd[2]]
    files = {}
    for mm in re.finditer(rb'[a-zA-Z]:\\ww\\[\w\\.]+?\.(?:cpp|h|c)\x00', blob, re.I):
        name = mm.group(0)[:-1].decode('latin1').lower().replace('\\', '/')
        name = 'ww/' + name.split('/ww/', 1)[1]
        files[RLO + mm.start()] = name
    d = dict(DRAW=da[4], starts=starts, rrefs=dict(rrefs), drefs=dict(drefs), calls=dict(calls), vals=dict(vals),
             files=files, RLO=RLO, RHI=RHI, DLO=DLO, DHI=DHI, thunks=thunks)
    try:
        pickle.dump(dict(key=key, data=d), open(cache, 'wb'))
    except Exception:
        pass
    return d


# ---------------------------------------------------------------------------------------------- registrations
def registrations(hook_dir):
    regs = []
    for rel, path in F.hook_files(hook_dir):
        src = F.strip(open(path, errors='replace').read(), strings=False)
        # module keys given through a macro (`#define M "game/b105_09"` ... `JOAN_REPLACE(M, "name", ...)`)
        for d in re.finditer(r'^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)[ \t]+("[^"\n]*")[ \t]*$', src, re.M):
            src = re.sub(r'(\b[A-Za-z_]\w*\s*\(\s*)' + d.group(1) + r'(\s*,)', lambda mm: mm.group(1) + d.group(2) + mm.group(2), src)
        for m in F.REG_RE.finditer(src):
            macro = m.group(1)
            if macro.startswith('JOAN_REPLACE_IMPORT') or macro == 'JOAN_IMAGE_SECTION':
                continue
            a = int(m.group(4), 16)
            if not (0x401000 <= a < 0x9CF000):
                continue
            regs.append(dict(addr=a, name=m.group(3), file='src/' + rel, macro=macro, module=m.group(2) or ''))
    return regs


def class_of(name):
    n = name.split(' ')[0]
    if '::' in n:
        c = n.split('::')[0]
        if c not in ('crt', 'jpeg', 'd', 'fn'):
            return c
    return None


# ---------------------------------------------------------------------------------------------- segmentation
W_ANCHOR = 10.0   # function references the file's __FILE__ string
W_DATA = 1.0      # per private .rdata / .data reference inside a file's data extent
W_DATA_CAP = 4.0
P_SWITCH = 3.0    # cost of starting a new object in code order
PRIVATE_MAX_USERS = 4
UNKNOWN_DIST = 12   # edge functions farther than this from any in-place evidence -> unknown
PRIVATE_MAX_SPAN = 0x20000


def mono_viterbi(scores, nstates, penalty):
    """forward-only segmentation: state index never decreases along the sequence. scores: list of
       {state: score}. Ties keep the current state (a new object starts at its first evidence)."""
    NEG = -1e18
    prev = [0.0] * nstates          # the sequence may start in any state
    back = []
    for sc in scores:
        cur = [0.0] * nstates
        bp = [0] * nstates
        best_v, best_i = NEG, 0     # running max of prev[0..i-1]
        for i in range(nstates):
            stay = prev[i]
            sw = best_v - penalty
            if sw > stay:
                cur[i], bp[i] = sw, best_i
            else:
                cur[i], bp[i] = stay, i
            if prev[i] > best_v:
                best_v, best_i = prev[i], i
        for s, v in sc.items():
            cur[s] += v
        back.append(bp)
        prev = cur
    i = max(range(nstates), key=lambda k: (prev[k], -k))
    out = [0] * len(scores)
    for t in range(len(scores) - 1, -1, -1):
        out[t] = i
        i = back[t][i]
    return out


def analyse(B, regs, extra_anchor=None, extra_label=None):
    """returns (starts, late, label, res, anchor, hdr, prefs); res[start] = (file, kind, detail) with kind
       anchor (own __FILE__), data (own private .rdata/.data in the file's zone), pos (between two evidence
       functions of the same file), none (segment edge, decided by position only)"""
    starts = [s for s in B['starts'] if 0x414000 <= s < GAME_HI]
    late = [s for s in B['starts'] if LATE_LO <= s < 0x7D5000]
    allf = starts + late
    fstr = B['files']
    fpos = sorted((a, f) for a, f in fstr.items() if f.endswith('.cpp'))
    paddr = [a for a, _ in fpos]
    order = [f for _, f in fpos]
    K = len(order)
    kidx = {f: i for i, f in enumerate(order)}

    anchor, hdr = {}, {}
    for s in allf:
        fs = sorted({fstr[r] for r in B['rrefs'].get(s, ()) if r in fstr})
        cp = [f for f in fs if f in kidx]
        if cp:
            anchor[s] = cp[0]
        h = [f for f in fs if f not in kidx]
        if h:
            hdr[s] = h[0]
    analyse.extra_anchored = set()
    analyse.extra_label = extra_label or {}
    for s, f in (extra_anchor or {}).items():
        # manual attributions (functions.csv analysis rows, re/re2_engine_matches.csv): an anchor for files that have a
        # __FILE__ string, below the function's own __FILE__; the others below every strong evidence (see classify)
        if s not in anchor and f in kidx:
            anchor[s] = f
            analyse.extra_anchored.add(s)
    extra = {s: f for s, f in (extra_anchor or {}).items() if f not in kidx}

    # private references per address space: r = .rdata, d = initialised .data, b = .bss part of .data
    DINIT = B['DLO'] + B.get('DRAW', 0x40000)
    def space(r):
        if B['RLO'] <= r < B['RHI']:
            return 'r'
        return 'd' if r < DINIT else 'b'
    users = collections.defaultdict(list)
    for s in allf:
        for r in list(B['rrefs'].get(s, ())) + list(B['drefs'].get(s, ())):
            if r not in fstr:
                users[r].append(s)
    private = {r for r, u in users.items() if len(u) <= PRIVATE_MAX_USERS and max(u) - min(u) <= PRIVATE_MAX_SPAN}
    prefs = {}
    for s in allf:
        p = collections.defaultdict(list)
        for r in list(B['rrefs'].get(s, ())) + list(B['drefs'].get(s, ())):
            # .bss (uninitialised globals) is left out: MSVC allocates communal variables at the end of
            # .data in no object order
            if r in private and space(r) != 'b':
                p[space(r)].append(r)
        prefs[s] = p

    ext = {sp: {} for sp in 'rdb'}     # space -> file index -> [lo, hi]
    def reset_ext():
        for sp in 'rdb':
            ext[sp] = {}
        for k in range(K):
            ext['r'][k] = [paddr[k], paddr[k]]

    def zone(sp, p):
        """file index whose known extent (learned from anchored code) holds a private reference, or None"""
        e = ext[sp]
        if sp == 'r':
            k = bisect.bisect_right(paddr, p) - 1
            for q in (k, k + 1):
                if 0 <= q < K and e[q][0] <= p <= e[q][1]:
                    return q
            return None
        inside = [k for k, (a, b) in e.items() if a <= p <= b]
        return inside[0] if len(inside) == 1 else None

    def fvotes(s):
        v = collections.Counter()
        for sp, rs in prefs[s].items():
            for p in rs:
                z = zone(sp, p)
                if z is not None:
                    v[z] += 1
        return v

    def soft(s):
        """.rdata references outside any extent: weak votes for the file whose __FILE__ string precedes them
           (an object's strings mostly follow its __FILE__ string) and the next one"""
        v = collections.Counter()
        for p in prefs[s].get('r', ()):
            if zone('r', p) is None:
                k = bisect.bisect_right(paddr, p) - 1
                if k >= 0:
                    v[k] += 0.5
                if k + 1 < K:
                    v[k + 1] += 0.25
        return v

    reset_ext()
    label, kind = {}, {}
    for it in range(4):
        scores = []
        for s in starts:
            sc = collections.Counter()
            if s in anchor:
                sc[kidx[anchor[s]]] += W_ANCHOR
            if it:
                for k, n in fvotes(s).items():
                    sc[k] += min(W_DATA * n, W_DATA_CAP)
                for k, n in soft(s).items():
                    sc[k] += min(W_DATA * n, W_DATA_CAP / 2)
            scores.append(dict(sc))
        lab = mono_viterbi(scores, K, P_SWITCH)
        label = dict(zip(starts, lab))
        kind = {}
        for s in starts:
            k = label[s]
            if s in anchor and kidx[anchor[s]] == k:
                kind[s] = 'anchor'
            elif it and fvotes(s).get(k, 0) > 0 and fvotes(s).most_common(1)[0][0] == k:
                kind[s] = 'data'
        ev_idx = [i for i, s in enumerate(starts) if s in kind]
        for a, b in zip(ev_idx, ev_idx[1:]):
            if label[starts[a]] == label[starts[b]]:
                for i in range(a + 1, b):
                    kind.setdefault(starts[i], 'pos')
        # extents only from anchors and functions between two anchors of the same file (no drift through
        # data-labelled functions)
        an_idx = [i for i, s in enumerate(starts) if kind.get(s) == 'anchor']
        apos = {starts[i] for i in an_idx}
        for a, b in zip(an_idx, an_idx[1:]):
            if label[starts[a]] == label[starts[b]]:
                apos.update(starts[a + 1:b])
        reset_ext()
        for s in apos:
            k = label[s]
            for sp, rs in prefs[s].items():
                if sp == 'r':
                    lim_lo = paddr[k - 1] if k > 0 else 0
                    lim_hi = paddr[k + 1] if k + 1 < K else 1 << 32
                    rs = [p for p in rs if lim_lo < p < lim_hi]
                for p in rs:
                    e = ext[sp].setdefault(k, [p, p])
                    e[0] = min(e[0], p)
                    e[1] = max(e[1], p)
        # .data extents must not interleave: drop the ones overlapping a neighbour in object order
        for sp in 'db':
            ks = sorted(ext[sp])
            bad = set()
            for a, b in zip(ks, ks[1:]):
                if ext[sp][a][1] >= ext[sp][b][0]:
                    bad.add(a if ext[sp][a][1] - ext[sp][a][0] > ext[sp][b][1] - ext[sp][b][0] else b)
            for k in bad:
                del ext[sp][k]

    res = {}
    for s in allf:
        st = label.get(s)
        v = fvotes(s)
        if s in anchor and (st is None or st != kidx[anchor[s]]):
            res[s] = (anchor[s], 'anchor', '__FILE__ %s; code outside its object range (incremental-link move)'
                      % anchor[s])
            continue
        if s in anchor:
            res[s] = (anchor[s], 'anchor', (extra_label or {}).get(s) if s in analyse.extra_anchored
                      else '__FILE__ ' + anchor[s])
            continue
        if v:
            q, n = v.most_common(1)[0]
            if (st is None or st != q) and n >= 2 and n > sum(v.values()) - n:
                res[s] = (order[q], 'data', '%d private data ref(s) in %s zone; code outside its object range'
                          % (n, order[q]))
                continue
            if st is None:
                res[s] = (order[q], 'data', '%d private data ref(s) in %s zone' % (n, order[q]))
                continue
        if st is None:
            res[s] = None
            continue
        res[s] = (order[st], kind.get(s, 'none'), '')
    analyse.extra = extra
    analyse.order = order
    analyse.ext = ext
    return starts, late, label, res, anchor, hdr, prefs


def classify(starts, late, res, B, regs, label, order):
    """final (file, confidence, evidence) per function start"""
    sidx = {s: i for i, s in enumerate(starts)}
    out = {}
    n = len(starts)
    # in-place strong evidence (anchor / data agreeing with the code segment): moved functions do not
    # delimit segments
    strong = [res[s] is not None and res[s][1] in ('anchor', 'data') and res[s][0] == order[label[s]]
              for s in starts]
    prv, nxt = [None] * n, [None] * n
    last = None
    for i in range(n):
        prv[i] = last
        if strong[i]:
            last = i
    last = None
    for i in range(n - 1, -1, -1):
        nxt[i] = last
        if strong[i]:
            last = i

    # class name -> file, from strongly attributed functions
    def containing(a):
        i = bisect.bisect_right(starts, a) - 1
        return starts[i] if i >= 0 else None
    cls = collections.defaultdict(set)
    for r in regs:
        c = class_of(r['name'])
        s = containing(r['addr'])
        if c and s is not None:
            cls[s].add(c)
    cdist = collections.defaultdict(collections.Counter)
    for i, s in enumerate(starts):
        if strong[i] and not res[s][0].startswith('unknown'):
            for c in cls.get(s, ()):
                cdist[c][res[s][0]] += 1
    cbest = {}
    for c, cnt in cdist.items():
        f, k = cnt.most_common(1)[0]
        if k >= 3 and k / sum(cnt.values()) >= 0.75:
            cbest[c] = f
    # call graph neighbours with strong attribution
    nbr = collections.defaultdict(set)
    for s in starts:
        for t in B['calls'].get(s, ()):
            ts = t if t in sidx else containing(t)
            if ts is not None and ts != s and ts in sidx:
                nbr[s].add(ts)
                nbr[ts].add(s)

    def fname(f):
        return 'unknown' if f.startswith('unknown') else f

    # functions.csv manual attributions to files without a __FILE__ string (ilib/mem.cpp, ilib/misc.cpp, ...)
    # take part as evidence of their own
    manual = getattr(analyse, 'extra', {})
    for i, s in enumerate(starts):
        if s in manual and not strong[i]:
            res[s] = (manual[s], 'manual', analyse.extra_label.get(s, 'functions.csv analysis: ' + manual[s]))
            strong[i] = True
    prv, nxt = [None] * n, [None] * n
    last = None
    for i in range(n):
        prv[i] = last
        if strong[i]:
            last = i
    last = None
    for i in range(n - 1, -1, -1):
        nxt[i] = last
        if strong[i]:
            last = i

    for i, s in enumerate(starts):
        f, k, ev = res[s]
        if k == 'manual':
            out[s] = (f, 'class', ev)
            continue
        if k in ('anchor', 'data') and not strong[i]:
            out[s] = (f, 'anchor' if k == 'anchor' else 'range', ev)
            continue
        if k == 'anchor':
            out[s] = (f, 'anchor', ev)
            continue
        if k == 'data':
            out[s] = (fname(f), 'range', ev or 'private data refs in %s zone' % f)
            continue
        p, q = prv[i], nxt[i]
        pf = res[starts[p]][0] if p is not None else None
        qf = res[starts[q]][0] if q is not None else None
        if k == 'pos' and pf == f == qf:
            out[s] = (fname(f), 'range', 'between %s evidence 0x%08x..0x%08x' % (f, starts[p], starts[q]))
            continue
        # edge of a segment: class name, then call graph, then the nearer side
        cands = {x for x in (pf, qf) if x}
        cv = collections.Counter()
        for c in cls.get(s, ()):
            if c in cbest:
                cv[cbest[c]] += 2
        for t in nbr.get(s, ()):
            j = sidx[t]
            if strong[j]:
                cv[res[t][0]] += 1
        best = [x for x, _ in cv.most_common() if x in cands]
        if best:
            out[s] = (fname(best[0]), 'class', 'segment edge %s|%s; class/call graph -> %s' % (pf, qf, best[0]))
            continue
        if pf and qf and pf != qf:
            g = pf if (i - p) <= (q - i) else qf
        else:
            g = f
        span = (q if q is not None else n) - (p if p is not None else -1) - 1
        dist = min(i - p if p is not None else n, q - i if q is not None else n)
        if dist > UNKNOWN_DIST:
            out[s] = ('unknown', 'guess', 'no evidence within %d functions; %d-function stretch between %s and %s '
                      '(possibly an object without __FILE__ string)' % (UNKNOWN_DIST, span, pf, qf))
            continue
        out[s] = (fname(g), 'guess', 'no own evidence; %d-function stretch between %s and %s' % (span, pf, qf))
    for s in late:
        r = res.get(s)
        out[s] = (r[0], 'anchor' if r[1] == 'anchor' else 'range', r[2]) if r else \
            ('unknown', 'guess', 'late incremental-link area, no evidence')
    return out


def lib_attr(regs):
    """library rows: (file, confidence, evidence) per registration address"""
    out = {}
    named = []
    for r in regs:
        a = r['addr']
        if a < GAME_HI or a >= LATE_LO:
            continue
        lib = next((x for x in LIB_RANGES if x[0] <= a < x[1]), None)
        words = r['name'].replace('crt::', '').replace('jpeg::', '').replace('type_info::', '').split()
        if DLL_PREFIX.match(r['name']) or (lib and lib[2] == 'imports'):
            out[a] = ('lib/imports/import_stubs.cpp', 'range', 'import jump stub')
            continue
        f = None
        if lib and lib[2] == 'crt':
            f = next((CRT_BY_NAME[w] for w in words if w in CRT_BY_NAME), None)
        elif lib and lib[2] in ('jpeg', 'd3dx'):
            f = next((JPEG_BY_NAME[w] for w in words if w in JPEG_BY_NAME), None)
            if f and lib[2] == 'd3dx':
                f = 'd3dx8/' + f      # D3DX8 links its own IJG libjpeg (decompressor side)
            if not f and lib[2] == 'd3dx':
                f = next((g for pat, g in D3DX_FILES if re.search(pat, r['name'])), None)
        if f:
            out[a] = ('lib/' + f, 'anchor', 'library function name')
            named.append((a, 'lib/' + f))
        else:
            out[a] = None
    # unnamed library functions: between two named ones of the same file -> range, else library default
    named.sort()
    na = [a for a, _ in named]
    for r in regs:
        a = r['addr']
        if a in out and out[a] is None:
            lib = next((x for x in LIB_RANGES if x[0] <= a < x[1]), None)
            i = bisect.bisect_left(na, a)
            if 0 < i < len(na) and named[i - 1][1] == named[i][1]:
                out[a] = (named[i][1], 'range', 'between %s functions' % named[i][1])
            else:
                out[a] = ('lib/' + lib[3], 'range' if lib[2] == 'd3dx' else 'guess', '%s library range' % lib[2])
    return out



def main():
    ap = argparse.ArgumentParser()
    root = os.path.dirname(HERE)
    bin_dir = os.path.join(os.path.dirname(root), 're')
    ap.add_argument('--exe', default=os.path.join(bin_dir, 'joan.exe'))
    ap.add_argument('--asm', default=os.path.join(bin_dir, 'joan.asm'))
    ap.add_argument('--functions', default=os.path.join(bin_dir, 'functions.json'))
    ap.add_argument('--thunks', default=os.path.join(bin_dir, 'thunks.json'))
    ap.add_argument('--hook', default=os.path.join(root, 'src'))
    ap.add_argument('--csv', default=os.path.join(HERE, 'functions.csv'))
    ap.add_argument('--out', default=os.path.join(HERE, 'attribution.csv'))
    ap.add_argument('--summary', default=os.path.join(HERE, 'attribution_summary.md'))
    ap.add_argument('--re2', default=os.path.join(HERE, 're2_engine_matches.csv'),
                    help='engine functions matched to the Restaurant Empire 2 symbols (certain and probable-enclosed rows are used)')
    args = ap.parse_args()

    B = load_binary(args)
    regs = registrations(args.hook)
    # functions.csv 'analysis' rows (manual attributions) count as anchors
    extra = {}
    if os.path.exists(args.csv):
        for row in csv.DictReader(open(args.csv)):
            if row['module_evidence'] == 'analysis' and row['module']:
                extra[int(row['address'], 16)] = 'ww/' + row['module']
    labels = {a: 'functions.csv analysis: ' + f for a, f in extra.items()}
    # RE2 engine matches (Phase 3): the 2008 engine's file under joan's ww/ tree; an analysis row wins
    if os.path.exists(args.re2):
        for row in csv.DictReader(l for l in open(args.re2) if not l.startswith('#')):
            a = int(row['address'], 16)
            if row['confidence'] not in ('certain', 'probable-enclosed') or a in extra:
                continue
            extra[a] = 'ww/' + row['re2_file'].lower()
            labels[a] = 'RE2 engine symbols%s: %s (%s)' % ('' if row['confidence'] == 'certain' else ' (probable, enclosed)',
                                                          row['re2_name'], row['re2_file'])
    starts, late, label, res0, anchor, hdr, prefs = analyse(B, regs, extra, labels)
    res = classify(starts, late, res0, B, regs, label, analyse.order)
    allf = sorted(starts + late)
    lib = lib_attr(regs)

    rows = []
    for r in sorted(regs, key=lambda r: (r['addr'], r['file'])):
        a = r['addr']
        if a in lib:
            f, conf, evd = lib[a]
        else:
            i = bisect.bisect_right(allf, a) - 1
            s = allf[i]
            f, conf, evd = res[s]
            if s != a:
                evd += ' (inside fn 0x%08x)' % s
            if a >= LATE_LO:
                evd += '; late incremental-link area'
        evd = evd.replace('between None and', 'between start and').replace(' and None', ' and end') \
            .replace('None|', 'start|').replace('|None', '|end')
        rows.append(dict(address='0x%08x' % a, registered_name=r['name'], current_file=r['file'],
                         original_file=f, confidence=conf, evidence=evd))
    with open(args.out, 'w', newline='') as o:
        w = csv.DictWriter(o, lineterminator='\n', fieldnames=['address', 'registered_name', 'current_file', 'original_file',
                                          'confidence', 'evidence'])
        w.writeheader()
        w.writerows(rows)
    write_summary(args.summary, rows, allf, res)
    c = collections.Counter('unknown' if r['original_file'] == 'unknown' else r['confidence'] for r in rows)
    print('%d registrations -> %s' % (len(rows), args.out), dict(c))


def target_path(f):
    """original file -> proposed src/ path"""
    if f == 'unknown':
        return None
    if f.startswith('ww/'):
        rest = f[3:]
        top = rest.split('/')[0] if '/' in rest else None
        return 'src/' + (rest if top else 'game/' + rest)
    return 'src/' + f


def write_summary(path, rows, allf, res):
    C = collections.Counter
    game = [r for r in rows if int(r['address'], 16) < GAME_HI or int(r['address'], 16) >= LATE_LO]
    lib = [r for r in rows if r not in game]

    def conf(r):
        return 'unknown' if r['original_file'] == 'unknown' else r['confidence']
    cats = ['anchor', 'range', 'class', 'guess', 'unknown']
    out = []
    w = out.append
    w('# Attribution of the reimplemented functions to the original source files\n')
    w('Generated by `re/attribute.py` (rerun: `python3 re/attribute.py` from the repository root; inputs: '
      '`../re/joan.exe`, `../re/joan.asm`, `../re/functions.json`, `../re/thunks.json`, `src/**` registrations, '
      '`re/functions.csv`). Per-function result: `re/attribution.csv`.\n')
    w('## Method\n')
    w('* **anchor** - the function references a `d:\\ww\\...\\x.cpp` `__FILE__` string (119 distinct in .rdata, '
      '118 .cpp + `stackt.h`), or a manual attribution in `re/functions.csv`, or (libraries) its name identifies '
      'the VC6 CRT / IJG libjpeg / D3DX8 source.')
    w('* **link order** - the `__FILE__` strings sit in .rdata in project order (ww/ alphabetical, then ilib/, elib/, '
      'en3d/, en2d/, uti3d/, interfac/). That order is the object order of .text too, so the code is segmented '
      'with a forward-only Viterbi pass over the functions in address order (anchors weigh 10, private .rdata / '
      'initialised .data references 1 each, a new object costs 3).')
    w('* **range** - the function lies between two in-place evidence functions (anchor, or private .rdata/.data '
      'references inside the file\'s data extent learned from anchored code) of the same file, or it has such '
      'private data references itself.')
    w('* **class** - at a segment edge: the class name of the registration (classes whose evidence-backed functions '
      'are >= 75 % in one file) or call-graph neighbours pick one of the two neighbouring files; also the manual '
      '`functions.csv` attributions.')
    w('* **guess** - at a segment edge with no own evidence: the nearer of the two neighbouring files (<= %d '
      'functions from evidence).' % UNKNOWN_DIST)
    w('* **unknown** - more than %d functions away from any in-place evidence: most of these stretches are objects '
      'without a `__FILE__` string (see the list below); they are not forced into a neighbour.' % UNKNOWN_DIST)
    w('* Functions whose own `__FILE__` / private data contradict their code position (incremental-link moves: the '
      'exe was linked with /INCREMENTAL, functions that grew were moved into holes) keep their own file; the '
      'evidence says "code outside its object range".\n')
    w('## Coverage\n')
    w('| | registrations | anchor | range | class | guess | unknown |')
    w('|---|---:|---:|---:|---:|---:|---:|')
    for name, rs in (('game code', game), ('libraries', lib), ('total', rows)):
        c = C(conf(r) for r in rs)
        w('| %s | %d | %s |' % (name, len(rs), ' | '.join(str(c.get(k, 0)) for k in cats)))
    w('')
    moved = [r for r in rows if 'outside its object' in r['evidence']]
    w('Relocated (incremental-link) functions identified by own evidence: %d.\n' % len(moved))
    w(FINDINGS)

    # ---- per original file
    by = collections.defaultdict(list)
    for r in rows:
        by[r['original_file']].append(r)
    w('## Original files\n')
    w('Range = lowest..highest registered address attributed to the file in place (relocated functions excluded). '
      'Modules = current hook files holding its functions (count).\n')
    w('| original file | range | fns | anchor/range/class/guess | current modules |')
    w('|---|---|---:|---|---|')

    def key(f):
        return (f == 'unknown', f.startswith('lib/'), f)
    for f in sorted(by, key=key):
        if f == 'unknown':
            continue
        rs = by[f]
        inp = [int(r['address'], 16) for r in rs if 'outside its object' not in r['evidence']] or \
              [int(r['address'], 16) for r in rs]
        c = C(r['confidence'] for r in rs)
        mods = C(r['current_file'].replace('src/modules/', '').replace('src/', '') for r in rs)
        ml = ', '.join('%s (%d)' % m for m in mods.most_common(4))
        if len(mods) > 4:
            ml += ', +%d more' % (len(mods) - 4)
        w('| %s | 0x%06x..0x%06x | %d | %d/%d/%d/%d | %s |' % (f, min(inp), max(inp), len(rs), c['anchor'], c['range'],
                                                              c['class'], c['guess'], ml))
    w('')
    # ---- unknown stretches
    w('## Unattributed stretches (candidate objects without a `__FILE__` string)\n')
    w('Consecutive unknown registrations; "between" = the link-order neighbours (the missing object\'s name sorts '
      'between them). Top classes help to name them.\n')
    w('| range | fns | between | top classes |')
    w('|---|---:|---|---|')
    unk = [r for r in rows if r['original_file'] == 'unknown']
    groups = []
    for r in unk:
        a = int(r['address'], 16)
        if groups and groups[-1]['last_i'] + 1 == rows.index(r):
            g = groups[-1]
        else:
            g = dict(lo=a, rs=[], last_i=None)
            groups.append(g)
        g['rs'].append(r)
        g['hi'] = a
        g['last_i'] = rows.index(r)
    for g in groups:
        if len(g['rs']) < 3:
            continue
        m = re.search(r'between (\S+) and (\S+)', g['rs'][0]['evidence'])
        btw = '%s .. %s' % (m.group(1), m.group(2).rstrip(' (')) if m else '?'
        btw = btw.replace('None .. ', 'start .. ').replace(' .. None', ' .. end of game code')
        cl = C(class_of(r['registered_name']) or '-' for r in g['rs'])
        w('| 0x%06x..0x%06x | %d | %s | %s |' % (g['lo'], g['hi'], len(g['rs']), btw,
                                                ', '.join('%s %d' % x for x in cl.most_common(5))))
    small = sum(len(g['rs']) for g in groups if len(g['rs']) < 3)
    w('\n(+ %d unknown registrations in stretches shorter than 3.)\n' % small)

    # ---- layout
    w('## Proposed layout\n')
    tp = C()
    for f, rs in by.items():
        t = target_path(f)
        if t:
            tp[t.rsplit('/', 1)[0]] += 1
    w('Mirror the original tree under `src/` (one .cpp per original source file, same base name):\n')
    w('| directory | original | files | registrations |')
    w('|---|---|---:|---:|')
    regs_by_dir = C()
    for f, rs in by.items():
        t = target_path(f)
        if t:
            regs_by_dir[t.rsplit('/', 1)[0]] += len(rs)
    orig = {'src/game': 'd:\\ww\\*.cpp', 'src/ilib': 'd:\\ww\\ilib', 'src/elib': 'd:\\ww\\elib',
            'src/en3d': 'd:\\ww\\en3d', 'src/en2d': 'd:\\ww\\en2d', 'src/uti3d': 'd:\\ww\\uti3d',
            'src/interfac': 'd:\\ww\\interfac', 'src/lib/crt': 'MSVC 6 LIBCMT (VC98\\CRT\\SRC)',
            'src/lib/crt/intel': 'LIBCMT asm (CRT\\SRC\\INTEL)', 'src/lib/crt/tran': 'LIBCMT math',
            'src/lib/crt/tran/i386': 'LIBCMT x87 math', 'src/lib/jpeg': 'IJG libjpeg 6b (compressor, game copy)',
            'src/lib/d3dx8': 'DirectX 8.1 SDK d3dx8.lib', 'src/lib/d3dx8/jpeg': 'libjpeg inside d3dx8.lib',
            'src/lib/openal': 'OpenAL 1.0 SDK alut.lib', 'src/lib/imports': 'import jump stubs (`jmp [IAT]`)'}
    for d in sorted(tp):
        w('| `%s/` | %s | %d | %d |' % (d, orig.get(d, ''), tp[d], regs_by_dir[d]))
    w('| `src/game/_unplaced/` | unknown stretches, one file per stretch named after its link-order neighbours '
      '(`after_camres.cpp`, ...) so the order is kept until a name is found | %d | %d |'
      % (len([g for g in groups if len(g['rs']) >= 3]), len(unk)))
    w('')
    w('For the CRT, one file per VC6 source (%d files for %d functions) is finer than useful: keep the existing '
      'family split (`crt/heap`, `crt/mt`, `crt/locale`, `crt/ctype`, `crt/mbs`, `crt/printf`, `crt/exit`, '
      '`crt/lowio`, ... = `src/lib/crt/<family>.cpp`) and list the VC6 source of each function in a comment '
      '(the csv has it). Same for libjpeg (one file per IJG source is natural there: 21 files).\n'
      % (sum(v for k, v in tp.items() if k.startswith('src/lib/crt')),
         sum(v for k, v in regs_by_dir.items() if k.startswith('src/lib/crt'))))
    w('Module names (`JOAN_REPLACE*("game/b98_03", ...)`) are what `[Hooks] Disable=` and the logs use; the move '
      'can keep them (the registry key is the address) or rename them to the new path (`game/battle`; the '
      '`crt/<family>` names stay) in a separate, explicit commit.\n')
    mods = collections.defaultdict(C)
    for r in rows:
        mods[r['current_file']][r['original_file']] += 1
    single = [m for m, c in mods.items() if len(c) == 1 and 'unknown' not in c]
    spread = sorted(len(c) for c in mods.values())
    w('## Moving the code safely\n')
    w('Today %d hook files register functions; %d map to a single known original file (rename whole); the median '
      'file spreads over %d original files (incl. unknown), the worst over %d.\n'
      % (len(mods), len(single), spread[len(spread) // 2], spread[-1]))
    w(MOVE_TEXT)
    open(path, 'w').write('\n'.join(out) + '\n')


FINDINGS = '''### Findings

* joan.exe is an **/INCREMENTAL** link: thunk table at 0x401000, functions padded and moved into holes when they
  grew. Out-of-order `__FILE__` users (e.g. `WorldS::add_entry` 0x42b270 with `world_s.cpp` in the middle of
  the camres..cpuusage area; `ilib/sound3d.cpp`, `se.cpp`, `mousecr.cpp` users at 0x48d120..0x4941b0 among the
  root `i*` files; `unitlvl.cpp` at 0x4a95a0) are moved functions; three game functions even sit after the CRT
  (0x7c6240, 0x7c7ab0, ...). Position-only attributions (`range`, `guess`) can therefore be wrong for single
  moved functions that have no own evidence.
* Link order = .rdata `__FILE__` order: root `ww/` alphabetical, then `ilib/`, `elib/`, `en3d/`, `en2d/`,
  `uti3d/`, `interfac/` (project groups, alphabetical inside).
* Only 413 of ~9,600 game functions reference a `__FILE__` string and only ~1,800 have private data
  references, so most objects are known by 2-15 anchors. Many objects have **no `__FILE__` string at all**
  (they never call the allocator / error reporter with `__FILE__`); the largest unattributed stretches are
  real objects: 0x561270..0x577720 (1,109 registrations, `Pixel`/`Blit`/`StaticInit`) between `world_s.cpp`
  and `ilib/audio.cpp`, 0x4d30f0..0x4de7e0 (404, the `Menu`/`OptMenu`/`ObjMenu` handlers) between
  `speech_s.cpp` and `t_scrres.cpp`, 0x649860..0x661ea0 (445, `Script`) between `uti3d/esstate2.cpp` and
  `uti3d/lflare.cpp`, 0x42ced0..0x43ca90 (203, `Unit`/`Order`/`Player`) between `camres.cpp` and
  `cpuusage.cpp`.
* Game code ends at **0x763d00**, not 0x7693c4 (re/make_csv.py `region()`): import jump stubs 0x763d02..,
  D3DX8 texture loaders + its libjpeg error manager from 0x763fc0, D3DX math 0x7669fb... `functions.csv`
  labels 0x763d00..0x7693c4 as `game`.
* `.bss` is not in object order (communal variables), so only `.rdata` and initialised `.data` references
  are used as evidence.
* `src/data_names.hpp` has 107 of the 119 `__FILE__` strings as `s_d_ww_*`; gamefil2, gamefile, unit,
  ilib/dynarr, dynarrb, filemem, filetxt, gameset, resdb, reslx, resx and en3d/texturea are named otherwise.
'''


MOVE_TEXT = '''1. **Freeze the map.** Commit `re/attribution.csv` and drive every move from it. Move `anchor`/`range` rows
   now, `class`/`guess` after a review, keep `unknown` rows in `src/game/_unplaced/after_<file>.cpp` (link-order
   position preserved) until the object is named.
2. **Whole files first.** Modules whose rows all go to one original file: `git mv` + its line in
   `src/CMakeLists.txt` (modules are listed explicitly there). No code change.
3. **Split with a tool, not by hand.** A `re/move_functions.py` that, for one source module, takes each
   registration (`&H::member` or `&free_fn` on the registration line), finds the member definition in the
   module's `struct H` (or the free function), and emits per target file: the module's includes, every
   module-local entity the moved code uses (types/tags in the anonymous namespace such as `struct Obj {}`,
   `static`/`constexpr` helpers, local `using`s), the members in a per-origin struct (`H_b98_03`: several old
   anonymous-namespace `H`s will share one target file, so the name must be unique per file), and the
   registration lines with the module-local macro (`B98(...)`, `B(...)`, `L82(...)`) re-defined in the target
   (each target needs its own `#define` + `#undef`, keeping the original module-name string).
   - Helpers shared by members that go to several targets: move them once into a header next to the targets
     (`src/<dir>/detail/<topic>.hpp`, `inline`), do not duplicate them.
   - **State** (module-local statics, counters, caches, flags, `std::once_flag`, anything tests or the replay
     checker read): exactly one definition; give it one home and expose it (`extern` / accessor). Duplicating
     a stateless helper is fine, duplicating state silently changes behaviour.
   - Static-initialisation order across files is unspecified: anything relying on same-file initialisation
     order (registrations are fine - keyed by address) stays together.
   - `__declspec(naked)` / `__asm` bodies, `x87.hpp` precision guards and `#pragma`s move with their function;
     inline asm referring to module-local symbols needs those symbols too.
4. **Mechanical checks after every step**: build; `python3 re/attribute.py` - the set of (address,
   registered_name, original_file) must be unchanged, only `current_file` moves; regenerate
   `src/fn_names.hpp` (`re/gen_fn_names.py`) and `src/runtime/code_slots.inc` (`re/gen_code_slots.py`) - **no diff**
   allowed (an address registered twice gets the *first name in path order*: moving files can flip it, so
   resolve the duplicate registrations first); `re/check_no_game_bytes.py`; unit tests and a replay
   (docs/RUNTIME.md) identical before/after.
5. **One commit per original file** (all of `ww/battle.cpp` at once), so a regression bisects to one file.
   Start with `lib/` and `ilib/` (few modules, high confidence), the root `ww/` last.
6. **Then by class** inside each original file, using the registered `Class::` prefixes after a review (many
   are placeholders: `Obj`, `X`, `ObjNNNNNN`).
'''


if __name__ == '__main__':
    main()
