// Original library sources (re/attribution.csv original_file, under lib/): crt/_filbuf.c, crt/_file.c,
//   crt/_flsbuf.c, crt/_freebuf.c, crt/_getbuf.c, crt/_open.c, crt/_sftbuf.c, crt/closeall.c, crt/fclose.c,
//   crt/fflush.c, crt/fgetc.c, crt/fopen.c, crt/fprintf.c, crt/fread.c, crt/fseek.c, crt/ftell.c, crt/fwrite.c,
//   crt/stream.c, crt/ungetc.c.
//
// Batch 114 / list 11: family crt/stdio (G part 2) - the MSVC 6 LIBCMT buffered stream layer (FILE*).
//
// Design (CRT_SYSTEM.md 5.1 / NOTE114 rules): a 1:1 C++ transcription of the VC6 stdio functions with
// ALL state left at its original address and in its original layout, so the family can be installed
// at birth (before WinMainCRTStartup 0x7b71e6) or at any later time, in whole or per function
// (openjoan.ini can disable single names): every stream, buffer and lock is created and read exactly
// as the original does.
//   _iob[20]   0x00a314a0..0x00a31720 (VC6 FILE, 0x20 bytes: +0 _ptr, +4 _cnt, +8 _base, +0xc _flag,
//              +0x10 _file, +0x14 _charbuf, +0x18 _bufsiz, +0x1c _tmpfname); stdout 0x00a314c0,
//              stderr 0x00a314e0; streams >= 20 are malloc'd _FILEX (FILE + CRITICAL_SECTION at +0x20,
//              0x38 bytes). x64: FILE has its natural 48-byte layout and _iob is a native array (iob() below).
//   __piob     0x00a6b710 (FILE*[_nstream]),  _nstream 0x00a6ca60,  _cflush 0x00a6b2d0,
//   _stdbuf[2] 0x00a6b2d4 / 0x00a6b2d8,       _commode 0x00a6b55c (read only, never written),
//   _exitflag  0x00a6b218 (byte, written by doexit; read by __endstdio).
// Census (joan.asm): __piob / _nstream / _cflush / _stdbuf / _commode / the _iob addresses are read or
// written ONLY by the functions below plus _lock_file / _unlock_file 0x7bcde0 / 0x7bce32 (lock layer,
// family crt/mt). Outside the CRT the FILE layout is read by libjpeg's term_destination 0x7aaaa0
// (FILE+0xc & 0x20 = ferror) and the string FILEs built by sprintf 0x7b2952, _snprintf 0x7b77e3,
// _vsnprintf 0x7b7b37, vsprintf / sscanf (C++, lib/crt/string_format.cpp), _output / _input: all kept compatible
// because the layout and every flag bit are the VC6 ones.
//
// Surface replaced (all cdecl, plain ret; no thunks: generic library entry patch; callers listed):
//   0x007b3fe0 FILE* fopen(name, mode)        = _fsopen(name, mode, _SH_DENYNO 0x40)
//                                               game 0x5303cf 0x5a9071 0x5a9429 0x618ac5 0x638b09, ALUT 0x7a94dd, C++
//   0x007b3faf FILE* _fsopen(name, mode, sh)  fopen
//   0x007bd046 FILE* _openfile(name, mode, sh, FILE*)   _fsopen
//   0x007bd1b6 FILE* _getstream(void)          _fsopen (returns the stream LOCKED)
//   0x007b3ef6 int fclose(FILE*)               game x8, ALUT 0x7a9730, _fcloseall, C++
//   0x007b3f27 int _fclose_lk(FILE*)           fclose
//   0x007b3f73 int fprintf(FILE*, fmt, ...)    game 0x53043c 0x638c6d 0x638c8b 0x638c9f, C++
//   0x007b6b3d size_t fread(buf, size, n, FILE*)  game x4, ALUT x6, C++
//   0x007b6b6c size_t _fread_lk(...)            fread
//   0x007b7c14 size_t fwrite(buf, size, n, FILE*) libjpeg 0x7aaa68 / 0x7aaabf
//   0x007b7c43 size_t _fwrite_lk(...)           fwrite
//   0x007b6ce0 long ftell(FILE*)                game x3, C++
//   0x007b6d02 long _ftell_lk(FILE*)            ftell, _fseek_lk
//   0x007b6e63 int fseek(FILE*, long, int)      game x5, ALUT x5, C++
//   0x007b6e8f int _fseek_lk(...)               fseek
//   0x007b6f1c int fgetc(FILE*)                 getc
//   0x007b6f4d int getc(FILE*) (-> fgetc)       game 0x5a9e27 0x5a9e3a 0x5aa13d 0x5aa14c 0x5aa163 0x5aa17a, C++
//   0x007c309f int ungetc(int, FILE*)           (no caller in the image; replaced for completeness)
//   0x007c30c8 int _ungetc_lk(int, FILE*)       ungetc, _un_inc 0x7bcce9 (scanf engine, crt/scanf)
//   0x007b7d4d int fflush(FILE*)                libjpeg 0x7aaae0
//   0x007b7d7c int _fflush_lk(FILE*)            fflush, flsall
//   0x007b7daa int _flush(FILE*)                _fclose_lk, _fseek_lk, _fwrite_lk, _fflush_lk, _ftbuf
//   0x007b7e06 int _flushall(void)              __endstdio
//   0x007b7e0f int flsall(int)                  fflush(NULL), _flushall
//   0x007c3136 int _fcloseall(void)             __endstdio (tail jmp)
//   0x007c0b59 int _filbuf(FILE*)               _fread_lk, fgetc, _inc 0x7bcccf (crt/scanf)
//   0x007bb52b int _flsbuf(int, FILE*)          _fwrite_lk, sprintf 0x7b2998, _vsnprintf 0x7b4ccf,
//                                               _snprintf 0x7b7828, 0x7b7b7b, write_char 0x7bbda1 (crt/printf), C++ vsprintf
//   0x007c2da9 void _getbuf(FILE*)              _flsbuf, _filbuf, _ungetc_lk
//   0x007bcf64 void _freebuf(FILE*)             _fclose_lk
//   0x007bcf8f int _stbuf(FILE*)                fprintf
//   0x007bd01c void _ftbuf(int, FILE*)          fprintf
//   0x007bcd24 __initstdio (XI initialiser, slot in 0x9f4c8c..0x9f4fa8)
//   0x007bcdcc __endstdio  (XP pre-terminator: _flushall, then _fcloseall if _exitflag)
// Not present in joan.exe (not linked): fputc, fputs, putc, puts, printf, feof / ferror functions
// (the jpeg code uses the ferror macro), setvbuf, _fileno, tmpfile, _rmtmp, _setmaxstdio.
//
// Stays original / other families (called ONLY through their original addresses, so whichever
// implementation is installed there runs):
//   lock layer (crt/mt): _lock 0x7b82aa / _unlock 0x7b830b (lock 2 = _IOB_SCAN_LOCK),
//     _lock_file 0x7bcde0 / _unlock_file 0x7bce32 / _lock_file2 0x7bce0f / _unlock_file2 0x7bce61
//     (_iob streams use _locktable[28 + i], _FILEX streams their own CS at +0x20 - the latter are
//     created / entered / deleted HERE in _getstream / _fcloseall exactly as the original);
//   heap (crt/heap): malloc 0x7b6c54, calloc 0x7b7b87, free 0x7b4e1d;
//   lowio (crt/lowio): _read 0x7c0c35, _write 0x7c1e0c, _lseek 0x7c1083, _close 0x7bce84,
//     _sopen 0x7c3556, _isatty 0x7c2ded, _commit 0x7c1ffc;
//   lowio STATE read directly (as the original does, no API exists for it): __pioinfo 0xa6b5a0
//     (blocks of 32 ioinfo x 0x24, osfhnd +0, osfile +4) in _ftell_lk (FTEXT 0x80, FCRLF 0x04),
//     _flsbuf (FAPPEND 0x20), _filbuf (FTEXT|FEOFLAG 0x82 -> _IOCTRLZ flag 0x2000) and __initstdio
//     (osfhnd of fd 0..2), plus __badioinfo 0xa31ff0 for fd -1. crt/lowio must keep that table at
//     its address with the VC6 layout while this module (or the original stdio) is in use;
//   errno (crt/mt): _errno 0x7bbfb5 (EINVAL 0x16 from _ftell_lk / _fseek_lk);
//   exit (crt/exit): _amsg_exit 0x7b72ee (__initstdio failure, _RT_STDIOINIT 0x1a);
//   printf (crt/printf): _output 0x7bb643 (fprintf); memcpy 0x7b2a20 (crt pure);
//   Win32 critical sections through the game's IAT slots (0xa6d908 Initialize, 0xa6d8ec Enter,
//     0xa6d8e4 Delete), like the original.
// Within the family every call also goes through the ORIGINAL address (e.g. fopen -> 0x7b3faf), so
// the call graph, and any per-name disable in openjoan.ini, behave like the original.
//
// Exactness notes: every flag test keeps its original width (byte / word tests of _flag are the
// same bit tests on the dword), signed / unsigned comparisons as in the asm (counts vs _cnt
// unsigned, _cnt sign tests signed, pointer loops unsigned), integer arithmetic on u32 (size * n
// wraps like imul; byte-count divisions unsigned). Return values of the "void" helpers are not
// read by any caller (checked: _getbuf / _freebuf / _ftbuf results unused); __initstdio returns
// the last osfhnd it read and __endstdio the _flushall / _fcloseall result, as the original eax.
// Module: crt/stdio.
#include "ioinfo.hpp"
#include "../../callconv.hpp"
#include <cstdarg>
#include <cstdint>

#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../runtime/patch.hpp"
#include "crt.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using joan::hook::global;

JOAN_COUNTER(c_fread_lk, "crt::_fread_lk");
JOAN_COUNTER(c_fwrite_lk, "crt::_fwrite_lk");
JOAN_COUNTER(c_ftell_lk, "crt::_ftell_lk");
JOAN_COUNTER(c_fseek_lk, "crt::_fseek_lk");
JOAN_COUNTER(c_filbuf, "crt::_filbuf");
JOAN_COUNTER(c_flsbuf, "crt::_flsbuf");
JOAN_COUNTER(c_openfile, "crt::_openfile");
JOAN_COUNTER(c_getstream, "crt::_getstream");
JOAN_COUNTER(c_flsall, "crt::flsall");
JOAN_COUNTER(c_fcloseall, "crt::_fcloseall");
JOAN_COUNTER(c_initstdio, "crt::__initstdio");
JOAN_COUNTER(c_fprintf, "crt::fprintf");

using joan::call::cdecl_call;
template <typename... A>
void win_call(img::Addr iat_slot, A... args) {
    reinterpret_cast<void(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(iat_slot))(args...);
}

// VC6 struct _iobuf (stdio.h), 32 bytes (x64: the same members, natural layout, 48 bytes).
struct Iobuf {
    char* ptr;       // +0x00
    int cnt;         // +0x04
    char* base;      // +0x08
    u32 flag;        // +0x0c
    int file;        // +0x10
    int charbuf;     // +0x14
    int bufsiz;      // +0x18
    char* tmpfname;  // +0x1c
};
static_assert(sizeof(void*) != 4 || sizeof(Iobuf) == 0x20, "Iobuf size");
// _FILEX (streams >= 20, malloc'd): the FILE, then its CRITICAL_SECTION (0x18 bytes; x64 0x28): 0x38 bytes on x86.
constexpr u32 kFilexCs = sizeof(Iobuf), kFilexSize = kFilexCs + (sizeof(void*) == 4 ? 0x18u : 0x28u);
static_assert(sizeof(void*) != 4 || kFilexSize == 0x38u, "_FILEX size");

// _flag bits
constexpr u32 kIoRead = 0x0001, kIoWrt = 0x0002, kIoNbf = 0x0004, kIoMyBuf = 0x0008, kIoEof = 0x0010,
              kIoErr = 0x0020, kIoStrg = 0x0040, kIoRw = 0x0080, kIoYourBuf = 0x0100, kIoSetVBuf = 0x0400,
              kIoFlrtn = 0x1000, kIoCtrlZ = 0x2000, kIoCommit = 0x4000;

// ---- state (original addresses)
constexpr img::Addr kIob = d::g_a314a0, kStdout = d::g_stdout, kStderr = d::g_stderr;
constexpr img::Addr kPiob = d::g_piob, kNstream = d::g_nstream, kCflush = d::g_cflush, kStdbuf = d::g_stdbuf;
constexpr img::Addr kCommode = d::g_commode, kExitflag = d::g_exitflag;
constexpr img::Addr kPioinfo = d::g_pioinfo, kBadioinfo = d::g_badioinfo;

// ---- other families / own family, original addresses
constexpr fn::Code kLock = fn::crt_lock, kUnlock = fn::crt_unlock;
constexpr fn::Code kLockFile = fn::crt_lock_file, kUnlockFile = fn::crt_unlock_file, kLockFile2 = fn::crt_lock_file2,
                         kUnlockFile2 = fn::crt_unlock_file2;
constexpr fn::Code kMalloc = fn::malloc, kCalloc = fn::calloc, kFree = fn::free, kMemcpy = fn::crt_memcpy_7b2a20;
constexpr fn::Code kRead = fn::crt_read, kWrite = fn::crt_write, kLseek = fn::crt_lseek, kClose = fn::crt_close,
                         kSopen = fn::crt_sopen, kIsatty = fn::crt_isatty, kCommit = fn::crt_commit;
constexpr fn::Code kErrno = fn::crt_errno, kAmsgExit = fn::crt_amsg_exit, kOutput = fn::crt_output;
constexpr img::Addr kInitCs = d::iat_KERNEL32_InitializeCriticalSection, kEnterCs = d::iat_KERNEL32_EnterCriticalSection, kDeleteCs = d::iat_KERNEL32_DeleteCriticalSection;  // IAT slots

constexpr fn::Code aFsopen = fn::crt_fsopen, aOpenfile = fn::crt_openfile, aGetstream = fn::crt_getstream,
                         aFclose = fn::crt_fclose, aFcloseLk = fn::crt_fclose_lk, aFreadLk = fn::crt_fread_lk,
                         aFwriteLk = fn::crt_fwrite_lk, aFtellLk = fn::crt_ftell_lk, aFseekLk = fn::crt_fseek_lk,
                         aFgetc = fn::crt_fgetc, aUngetcLk = fn::crt_ungetc_lk, aFflushLk = fn::crt_fflush_lk,
                         aFlush = fn::crt_flush, aFlushall = fn::crt_flushall, aFlsall = fn::crt_flsall,
                         aFcloseall = fn::crt_fcloseall, aFilbuf = fn::crt_filbuf, aFlsbuf = fn::crt_flsbuf,
                         aGetbuf = fn::crt_getbuf, aFreebuf = fn::crt_freebuf, aStbuf = fn::crt_stbuf, aFtbuf = fn::crt_ftbuf;

inline int pdiff(const char* a, const char* b) { return static_cast<int>(a - b); }  // buffer offsets (< 2 GB)
inline Iobuf** piob() { return global<Iobuf**>(kPiob); }
#if !JOAN_IMAGE_X64
// _iob[i]: in the data image at its original address (0x20-byte entries), shared with the original code.
inline Iobuf* iob(u32 i) { return &img::global<Iobuf>(kIob + i * 0x20u); }
inline bool is_iob(const Iobuf* fp, img::Addr a) { return reinterpret_cast<std::uintptr_t>(fp) == img::addr(a); }
#else
// x64: the image keeps the 32-bit bytes of _iob (nothing else in the data refers to them: no data relocation points
// into 0xa314a0..0xa31720; only this file and _lock_file / _unlock_file in lib/crt/mt.cpp use the addresses), so the
// streams are a native array of FILEs with the image's initial values (stdin's buffer pointers: the relocated words
// 0xa314a0 / 0xa314a8, -> _bufin; the integers of every entry; no other pointer word of the array is relocated: null).
Iobuf* iob_table() {
    static Iobuf* const t = [] {
        static Iobuf a[20];
        for (u32 i = 0; i < 20; ++i) {
            const img::Addr e = kIob + i * 0x20u;
            Iobuf& f = a[i];
            f.ptr = i == 0 ? global<char*>(e) : nullptr;
            f.cnt = global<int>(e + 0x4);
            f.base = i == 0 ? global<char*>(e + 0x8) : nullptr;
            f.flag = global<u32>(e + 0xc);
            f.file = global<int>(e + 0x10);
            f.charbuf = global<int>(e + 0x14);
            f.bufsiz = global<int>(e + 0x18);
            f.tmpfname = nullptr;
        }
        return a;
    }();
    return t;
}
inline Iobuf* iob(u32 i) { return iob_table() + i; }
inline bool is_iob(const Iobuf* fp, img::Addr a) { return fp == iob(static_cast<u32>((a - kIob) / 0x20)); }
#endif
inline int nstream() { return global<int>(kNstream); }

// VC6 ioinfo as crt/lowio allocates it (lib/crt/lowio.cpp IoInfo, the same struct): 0x24 bytes, osfhnd +0, osfile
// +4; x64 widens the CRITICAL_SECTION at its end (0x38 bytes), so the blocks are indexed by this type.
using joan::crt::IoInfo;  // lib/crt/ioinfo.hpp (one definition for lowio.cpp and stdio.cpp)
// &__pioinfo[fd >> 5][fd & 31] (sar: arithmetic shift, as the original; no range check)
inline const IoInfo* ioinfo(int fd) {
    const IoInfo* const block = global<const IoInfo*>(kPioinfo + static_cast<u32>((fd >> 5) * 4));
    return block + (fd & 0x1f);
}
inline u8 osfile(int fd) { return ioinfo(fd)->osfile; }
// _pioinfo(fd) with the fd == -1 -> __badioinfo check (_flsbuf / _filbuf); __badioinfo is image data in the x86
// layout, of which only osfile (+4) is read
inline u8 osfile_checked(int fd) {
    const IoInfo* const info = fd != -1 ? ioinfo(fd) : static_cast<const IoInfo*>(img::obj(kBadioinfo));
    return info->osfile;
}

inline void lock_file(Iobuf* fp) { cdecl_call<void>(kLockFile, fp); }
inline void unlock_file(Iobuf* fp) { cdecl_call<void>(kUnlockFile, fp); }
inline int flush(Iobuf* fp) { return cdecl_call<int>(aFlush, fp); }
inline void* errno_ptr() { return cdecl_call<void*>(kErrno); }

// ------------------------------------------------------------------ 0x007b3fe0 fopen
static Iobuf* JOAN_CDECL crt_fopen(const char* name, const char* mode) {
    return cdecl_call<Iobuf*>(aFsopen, name, mode, 0x40);  // _SH_DENYNO
}

// ------------------------------------------------------------------ 0x007b3faf _fsopen
static Iobuf* JOAN_CDECL crt_fsopen(const char* name, const char* mode, int shflag) {
    Iobuf* const stream = cdecl_call<Iobuf*>(aGetstream);  // returned locked
    if (stream == nullptr) return nullptr;
    Iobuf* const r = cdecl_call<Iobuf*>(aOpenfile, name, mode, shflag, stream);
    unlock_file(stream);
    return r;
}

// ------------------------------------------------------------------ 0x007bd046 _openfile
static Iobuf* JOAN_CDECL crt_openfile(const char* name, const char* mode, int shflag, Iobuf* stream) {
    c_openfile.hit();
    u32 streamflag = global<u32>(kCommode);
    int commodeset = 0, scanset = 0;
    u32 modeflag;
    const char* m = mode;
    switch (*m) {
    case 'a': modeflag = 0x109; streamflag |= kIoWrt; break;  // _O_WRONLY | _O_CREAT | _O_APPEND
    case 'r': modeflag = 0; streamflag |= kIoRead; break;     // _O_RDONLY
    case 'w': modeflag = 0x301; streamflag |= kIoWrt; break;  // _O_WRONLY | _O_CREAT | _O_TRUNC
    default: return nullptr;
    }
    int valid = 1;
    while (*++m != '\0' && valid != 0) {
        switch (static_cast<signed char>(*m)) {
        case '+':
            if (modeflag & 0x2) valid = 0;  // _O_RDWR
            else {
                modeflag = (modeflag & ~1u) | 0x2;
                streamflag = (streamflag & ~3u) | kIoRw;
            }
            break;
        case 'b':
            if (modeflag & 0xc000) valid = 0;  // _O_TEXT | _O_BINARY
            else modeflag |= 0x8000;
            break;
        case 't':
            if (modeflag & 0xc000) valid = 0;
            else modeflag |= 0x4000;
            break;
        case 'c':
            if (commodeset != 0) valid = 0;
            else { commodeset = 1; streamflag |= kIoCommit; }
            break;
        case 'n':
            if (commodeset != 0) valid = 0;
            else { commodeset = 1; streamflag &= ~kIoCommit; }
            break;
        case 'S':
            if (scanset != 0) valid = 0;
            else { scanset = 1; modeflag |= 0x20; }  // _O_SEQUENTIAL
            break;
        case 'R':
            if (scanset != 0) valid = 0;
            else { scanset = 1; modeflag |= 0x10; }  // _O_RANDOM
            break;
        case 'T':
            if (modeflag & 0x1000) valid = 0;  // _O_SHORT_LIVED
            else modeflag |= 0x1000;
            break;
        case 'D':
            if (modeflag & 0x40) valid = 0;  // _O_TEMPORARY
            else modeflag |= 0x40;
            break;
        default: valid = 0; break;
        }
    }
    const int fh = cdecl_call<int>(kSopen, name, static_cast<int>(modeflag), shflag, 0x1a4);
    if (fh < 0) return nullptr;
    ++global<int>(kCflush);
    stream->flag = streamflag;
    stream->cnt = 0;
    stream->ptr = nullptr;
    stream->base = nullptr;
    stream->tmpfname = nullptr;
    stream->file = fh;
    return stream;
}

// ------------------------------------------------------------------ 0x007bd1b6 _getstream
static Iobuf* JOAN_CDECL crt_getstream() {
    c_getstream.hit();
    Iobuf* retval = nullptr;
    cdecl_call<void>(kLock, 2);  // _IOB_SCAN_LOCK
    for (int i = 0; i < nstream(); ++i) {
        Iobuf* s = piob()[i];
        if (s == nullptr) {
            piob()[i] = cdecl_call<Iobuf*>(kMalloc, kFilexSize);  // sizeof(_FILEX)
            if (piob()[i] != nullptr) {
                win_call(kInitCs, reinterpret_cast<char*>(piob()[i]) + kFilexCs);
                win_call(kEnterCs, reinterpret_cast<char*>(piob()[i]) + kFilexCs);
                retval = piob()[i];
            }
            break;
        }
        if ((s->flag & 0x83) == 0) {
            cdecl_call<void>(kLockFile2, i, s);
            s = piob()[i];
            if ((s->flag & 0x83) == 0) {
                retval = piob()[i];
                break;
            }
            cdecl_call<void>(kUnlockFile2, i, s);
        }
    }
    if (retval != nullptr) {
        retval->file = -1;
        retval->cnt = 0;
        retval->flag = 0;
        retval->base = nullptr;
        retval->ptr = nullptr;
        retval->tmpfname = nullptr;
    }
    cdecl_call<void>(kUnlock, 2);
    return retval;
}

// ------------------------------------------------------------------ 0x007b3ef6 fclose
static int JOAN_CDECL crt_fclose(Iobuf* fp) {
    int r = -1;
    if (fp->flag & kIoStrg) {
        fp->flag = 0;
    } else {
        lock_file(fp);
        r = cdecl_call<int>(aFcloseLk, fp);
        unlock_file(fp);
    }
    return r;
}

// ------------------------------------------------------------------ 0x007b3f27 _fclose_lk
static int JOAN_CDECL crt_fclose_lk(Iobuf* fp) {
    int r = -1;
    if (fp->flag & 0x83) {
        r = flush(fp);
        cdecl_call<void>(aFreebuf, fp);
        if (cdecl_call<int>(kClose, fp->file) < 0) {
            r = -1;
        } else if (fp->tmpfname != nullptr) {
            cdecl_call<void>(kFree, fp->tmpfname);
            fp->tmpfname = nullptr;
        }
    }
    fp->flag = 0;
    return r;
}

// ------------------------------------------------------------------ 0x007b3f73 fprintf
static int JOAN_CDECL crt_fprintf(Iobuf* fp, const char* fmt, ...) {
    c_fprintf.hit();
    lock_file(fp);
    const int buffing = cdecl_call<int>(aStbuf, fp);
    va_list ap;
    va_start(ap, fmt);
    // MSVC x86 va_list = char* to the first stack vararg: the pointer the original passes
    // (lea eax, [esp+0x20] = entry esp + 0xc).
    const int r = cdecl_call<int>(kOutput, fp, fmt, ap);
    va_end(ap);
    cdecl_call<void>(aFtbuf, buffing, fp);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007b6b3d fread
static u32 JOAN_CDECL crt_fread(void* buf, u32 size, u32 n, Iobuf* fp) {
    lock_file(fp);
    const u32 r = cdecl_call<u32>(aFreadLk, buf, size, n, fp);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007b6b6c _fread_lk
static u32 JOAN_CDECL crt_fread_lk(void* buf, u32 size, u32 n, Iobuf* fp) {
    c_fread_lk.hit();
    const u32 total = size * n;
    char* data = static_cast<char*>(buf);
    u32 count = total;
    if (total == 0) return 0;
    u32 bufsize = (fp->flag & 0x10c) ? static_cast<u32>(fp->bufsiz) : 0x1000u;
    while (count != 0) {
        if ((fp->flag & 0x10c) && fp->cnt != 0) {
            const u32 avail = static_cast<u32>(fp->cnt);
            const u32 nbytes = count < avail ? count : avail;
            cdecl_call<void*>(kMemcpy, data, fp->ptr, nbytes);
            count -= nbytes;
            fp->cnt = static_cast<int>(static_cast<u32>(fp->cnt) - nbytes);
            fp->ptr += nbytes;
            data += nbytes;
        } else if (count >= bufsize) {
            const u32 nbytes = bufsize != 0 ? count - count % bufsize : count;
            const int nread = cdecl_call<int>(kRead, fp->file, data, nbytes);
            if (nread == 0) {
                fp->flag |= kIoEof;
                return (total - count) / size;
            }
            if (nread == -1) {
                fp->flag |= kIoErr;
                return (total - count) / size;
            }
            count -= static_cast<u32>(nread);
            data += nread;
        } else {
            const int c = cdecl_call<int>(aFilbuf, fp);
            if (c == -1) return (total - count) / size;
            *data++ = static_cast<char>(c);
            --count;
            bufsize = static_cast<u32>(fp->bufsiz);
        }
    }
    return n;
}

// ------------------------------------------------------------------ 0x007b7c14 fwrite
static u32 JOAN_CDECL crt_fwrite(const void* buf, u32 size, u32 n, Iobuf* fp) {
    lock_file(fp);
    const u32 r = cdecl_call<u32>(aFwriteLk, buf, size, n, fp);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007b7c43 _fwrite_lk
static u32 JOAN_CDECL crt_fwrite_lk(const void* buf, u32 size, u32 n, Iobuf* fp) {
    c_fwrite_lk.hit();
    const u32 total = size * n;
    const char* data = static_cast<const char*>(buf);
    u32 count = total;
    if (total == 0) return 0;
    u32 bufsize = (fp->flag & 0x10c) ? static_cast<u32>(fp->bufsiz) : 0x1000u;
    while (count != 0) {
        const u32 buffered = fp->flag & 0x108;
        if (buffered != 0 && fp->cnt != 0) {
            const u32 avail = static_cast<u32>(fp->cnt);
            const u32 nbytes = count < avail ? count : avail;
            cdecl_call<void*>(kMemcpy, fp->ptr, data, nbytes);
            fp->cnt = static_cast<int>(static_cast<u32>(fp->cnt) - nbytes);
            fp->ptr += nbytes;
            count -= nbytes;
            data += nbytes;
        } else if (count >= bufsize) {
            if (buffered != 0 && flush(fp) != 0) return (total - count) / size;
            const u32 nbytes = bufsize != 0 ? count - count % bufsize : count;
            const int nwritten = cdecl_call<int>(kWrite, fp->file, data, nbytes);
            if (nwritten == -1) {
                fp->flag |= kIoErr;
                return (total - count) / size;
            }
            data += nwritten;
            count -= static_cast<u32>(nwritten);
            if (static_cast<u32>(nwritten) < nbytes) {
                fp->flag |= kIoErr;
                return (total - count) / size;
            }
        } else {
            if (cdecl_call<int>(aFlsbuf, static_cast<int>(static_cast<signed char>(*data)), fp) == -1)
                return (total - count) / size;
            ++data;
            --count;
            bufsize = static_cast<u32>(fp->bufsiz);
            if (static_cast<int>(bufsize) <= 0) bufsize = 1;
        }
    }
    return n;
}

// ------------------------------------------------------------------ 0x007b6ce0 ftell
static int JOAN_CDECL crt_ftell(Iobuf* fp) {
    lock_file(fp);
    const int r = cdecl_call<int>(aFtellLk, fp);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007b6d02 _ftell_lk
static int JOAN_CDECL crt_ftell_lk(Iobuf* fp) {
    c_ftell_lk.hit();
    const int fd = fp->file;
    if (fp->cnt < 0) fp->cnt = 0;
    int filepos = cdecl_call<int>(kLseek, fd, 0, 1);  // SEEK_CUR
    if (filepos < 0) return -1;
    const u32 flag = fp->flag;
    if ((flag & 0x108) == 0) return static_cast<int>(static_cast<u32>(filepos) - static_cast<u32>(fp->cnt));
    char* const p = fp->ptr;
    char* const base = fp->base;
    int offset = pdiff(p, base);
    if (flag & 3) {
        if (osfile(fd) & 0x80) {  // FTEXT
            for (const char* q = base; q < p; ++q)
                if (*q == '\n') ++offset;
        }
    } else if ((flag & kIoRw) == 0) {
        *static_cast<int*>(errno_ptr()) = 0x16;  // EINVAL
        return -1;
    }
    if (filepos == 0) return offset;
    if (fp->flag & kIoRead) {
        if (fp->cnt == 0) {
            offset = 0;
        } else {
            u32 rdcnt = static_cast<u32>(pdiff(p, base)) + static_cast<u32>(fp->cnt);
            if (osfile(fd) & 0x80) {  // FTEXT
                if (cdecl_call<int>(kLseek, fd, 0, 2) == filepos) {
                    const char* const b = fp->base;
                    const char* const end = b + rdcnt;
                    for (const char* q = b; q < end; ++q)
                        if (*q == '\n') ++rdcnt;
                    if (fp->flag & kIoCtrlZ) ++rdcnt;
                } else {
                    cdecl_call<int>(kLseek, fd, filepos, 0);
                    if (rdcnt <= 0x200u && (fp->flag & kIoMyBuf) && (fp->flag & kIoSetVBuf) == 0)
                        rdcnt = 0x200;
                    else
                        rdcnt = static_cast<u32>(fp->bufsiz);
                    if (osfile(fd) & 0x04) ++rdcnt;  // FCRLF
                }
            }
            filepos = static_cast<int>(static_cast<u32>(filepos) - rdcnt);
        }
    }
    return static_cast<int>(static_cast<u32>(offset) + static_cast<u32>(filepos));
}

// ------------------------------------------------------------------ 0x007b6e63 fseek
static int JOAN_CDECL crt_fseek(Iobuf* fp, int offset, int whence) {
    lock_file(fp);
    const int r = cdecl_call<int>(aFseekLk, fp, offset, whence);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007b6e8f _fseek_lk
static int JOAN_CDECL crt_fseek_lk(Iobuf* fp, int offset, int whence) {
    c_fseek_lk.hit();
    u32 flag = fp->flag;
    if ((flag & 0x83) == 0 || (whence != 0 && whence != 1 && whence != 2)) {
        *static_cast<int*>(errno_ptr()) = 0x16;  // EINVAL
        return -1;
    }
    fp->flag = flag & ~kIoEof;
    if (whence == 1) {
        offset = static_cast<int>(static_cast<u32>(offset) + static_cast<u32>(cdecl_call<int>(aFtellLk, fp)));
        whence = 0;
    }
    flush(fp);
    flag = fp->flag;
    if (flag & kIoRw)
        fp->flag = flag & ~3u;
    else if ((flag & kIoRead) && (flag & kIoMyBuf) && (flag & kIoSetVBuf) == 0)
        fp->bufsiz = 0x200;
    return cdecl_call<int>(kLseek, fp->file, offset, whence) == -1 ? -1 : 0;
}

// ------------------------------------------------------------------ 0x007b6f1c fgetc
static int JOAN_CDECL crt_fgetc(Iobuf* fp) {
    lock_file(fp);
    int c;
    fp->cnt = static_cast<int>(static_cast<u32>(fp->cnt) - 1u);
    if (fp->cnt >= 0)
        c = static_cast<u8>(*fp->ptr++);
    else
        c = cdecl_call<int>(aFilbuf, fp);
    unlock_file(fp);
    return c;
}

// ------------------------------------------------------------------ 0x007b6f4d getc
static int JOAN_CDECL crt_getc(Iobuf* fp) { return cdecl_call<int>(aFgetc, fp); }

// ------------------------------------------------------------------ 0x007c309f ungetc
static int JOAN_CDECL crt_ungetc(int ch, Iobuf* fp) {
    lock_file(fp);
    const int r = cdecl_call<int>(aUngetcLk, ch, fp);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007c30c8 _ungetc_lk
static int JOAN_CDECL crt_ungetc_lk(int ch, Iobuf* fp) {
    if (ch == -1) return -1;
    const u32 flag = fp->flag;
    if ((flag & kIoRead) == 0 && ((flag & kIoRw) == 0 || (flag & kIoWrt) != 0)) return -1;
    if (fp->base == nullptr) cdecl_call<void>(aGetbuf, fp);
    if (fp->ptr == fp->base) {
        if (fp->cnt != 0) return -1;
        ++fp->ptr;
    }
    if (fp->flag & kIoStrg) {
        --fp->ptr;
        if (*fp->ptr != static_cast<char>(ch)) {
            ++fp->ptr;
            return -1;
        }
    } else {
        --fp->ptr;
        *fp->ptr = static_cast<char>(ch);
    }
    ++fp->cnt;
    fp->flag = (fp->flag & ~kIoEof) | kIoRead;
    return ch & 0xff;
}

// ------------------------------------------------------------------ 0x007b7d4d fflush
static int JOAN_CDECL crt_fflush(Iobuf* fp) {
    if (fp == nullptr) return cdecl_call<int>(aFlsall, 0);  // FFLUSHNULL
    lock_file(fp);
    const int r = cdecl_call<int>(aFflushLk, fp);
    unlock_file(fp);
    return r;
}

// ------------------------------------------------------------------ 0x007b7d7c _fflush_lk
static int JOAN_CDECL crt_fflush_lk(Iobuf* fp) {
    if (flush(fp) != 0) return -1;
    if (fp->flag & kIoCommit) return cdecl_call<int>(kCommit, fp->file) != 0 ? -1 : 0;
    return 0;
}

// ------------------------------------------------------------------ 0x007b7daa _flush
static int JOAN_CDECL crt_flush(Iobuf* fp) {
    int r = 0;
    const u32 flag = fp->flag;
    if ((flag & 3) == kIoWrt && (flag & 0x108) != 0) {
        const int nchar = pdiff(fp->ptr, fp->base);
        if (nchar > 0) {
            if (cdecl_call<int>(kWrite, fp->file, fp->base, nchar) == nchar) {
                const u32 f = fp->flag;
                if (f & kIoRw) fp->flag = f & ~kIoWrt;
            } else {
                fp->flag |= kIoErr;
                r = -1;
            }
        }
    }
    fp->cnt = 0;
    fp->ptr = fp->base;
    return r;
}

// ------------------------------------------------------------------ 0x007b7e06 _flushall
static int JOAN_CDECL crt_flushall() { return cdecl_call<int>(aFlsall, 1); }

// ------------------------------------------------------------------ 0x007b7e0f flsall
static int JOAN_CDECL crt_flsall(int flushflag) {
    c_flsall.hit();
    int count = 0, err = 0;
    cdecl_call<void>(kLock, 2);
    for (int i = 0; i < nstream(); ++i) {
        Iobuf* s = piob()[i];
        if (s == nullptr || (s->flag & 0x83) == 0) continue;
        cdecl_call<void>(kLockFile2, i, s);
        s = piob()[i];
        const u32 flag = s->flag;
        if (flag & 0x83) {
            if (flushflag == 1) {
                if (cdecl_call<int>(aFflushLk, s) != -1) ++count;
            } else if (flushflag == 0 && (flag & kIoWrt)) {
                if (cdecl_call<int>(aFflushLk, s) == -1) err = -1;
            }
        }
        cdecl_call<void>(kUnlockFile2, i, piob()[i]);
    }
    cdecl_call<void>(kUnlock, 2);
    return flushflag == 1 ? count : err;
}

// ------------------------------------------------------------------ 0x007c3136 _fcloseall
static int JOAN_CDECL crt_fcloseall() {
    c_fcloseall.hit();
    int count = 0;
    cdecl_call<void>(kLock, 2);
    for (int i = 3; i < nstream(); ++i) {
        Iobuf* const s = piob()[i];
        if (s == nullptr) continue;
        if (s->flag & 0x83) {
            if (cdecl_call<int>(aFclose, s) != -1) ++count;
        }
        if (i >= 20) {
            win_call(kDeleteCs, reinterpret_cast<char*>(piob()[i]) + kFilexCs);
            cdecl_call<void>(kFree, piob()[i]);
            piob()[i] = nullptr;
        }
    }
    cdecl_call<void>(kUnlock, 2);
    return count;
}

// ------------------------------------------------------------------ 0x007c0b59 _filbuf
static int JOAN_CDECL crt_filbuf(Iobuf* fp) {
    c_filbuf.hit();
    u32 flag = fp->flag;
    if ((flag & 0x83) == 0 || (flag & kIoStrg)) return -1;
    if (flag & kIoWrt) {
        fp->flag = flag | kIoErr;
        return -1;
    }
    flag |= kIoRead;
    fp->flag = flag;
    if ((flag & 0x10c) == 0)
        cdecl_call<void>(aGetbuf, fp);
    else
        fp->ptr = fp->base;
    const int n = cdecl_call<int>(kRead, fp->file, fp->base, fp->bufsiz);
    fp->cnt = n;
    if (n == 0 || n == -1) {
        fp->flag |= n != 0 ? kIoErr : kIoEof;
        fp->cnt = 0;
        return -1;
    }
    const u32 f = fp->flag;
    if ((f & (kIoWrt | kIoRw)) == 0 && (osfile_checked(fp->file) & 0x82) == 0x82)  // FTEXT | FEOFLAG
        fp->flag = f | kIoCtrlZ;
    if (fp->bufsiz == 0x200 && (fp->flag & kIoMyBuf) && (fp->flag & kIoSetVBuf) == 0) fp->bufsiz = 0x1000;
    fp->cnt = n - 1;
    const int c = static_cast<u8>(*fp->ptr);
    ++fp->ptr;
    return c;
}

// ------------------------------------------------------------------ 0x007bb52b _flsbuf
static int JOAN_CDECL crt_flsbuf(int ch, Iobuf* fp) {
    c_flsbuf.hit();
    u32 flag = fp->flag;
    const int fd = fp->file;
    if ((flag & (kIoWrt | kIoRw)) == 0 || (flag & kIoStrg)) {
        fp->flag = flag | kIoErr;
        return -1;
    }
    if (flag & kIoRead) {
        fp->cnt = 0;
        if ((flag & kIoEof) == 0) {
            fp->flag = flag | kIoErr;
            return -1;
        }
        fp->ptr = fp->base;
        fp->flag = flag & ~kIoRead;
    }
    flag = fp->flag;
    fp->cnt = 0;
    int written = 0;
    flag = (flag & ~kIoEof) | kIoWrt;
    fp->flag = flag;
    if ((flag & 0x10c) == 0) {
        const bool std_tty = (is_iob(fp, kStdout) || is_iob(fp, kStderr)) && cdecl_call<int>(kIsatty, fd) != 0;
        if (!std_tty) cdecl_call<void>(aGetbuf, fp);
    }
    int charcount;
    if (fp->flag & 0x108) {
        char* const base = fp->base;
        charcount = pdiff(fp->ptr, base);
        fp->ptr = base + 1;
        fp->cnt = fp->bufsiz - 1;
        if (charcount > 0) {
            written = cdecl_call<int>(kWrite, fd, base, charcount);
        } else if (osfile_checked(fd) & 0x20) {  // FAPPEND
            cdecl_call<int>(kLseek, fd, 0, 2);
        }
        *fp->base = static_cast<char>(ch);
    } else {
        charcount = 1;
        written = cdecl_call<int>(kWrite, fd, reinterpret_cast<char*>(&ch), 1);  // low byte of the argument
    }
    if (written != charcount) {
        fp->flag |= kIoErr;
        return -1;
    }
    return ch & 0xff;
}

// ------------------------------------------------------------------ 0x007c2da9 _getbuf
static void JOAN_CDECL crt_getbuf(Iobuf* fp) {
    ++global<int>(kCflush);
    fp->base = cdecl_call<char*>(kMalloc, 0x1000u);
    if (fp->base != nullptr) {
        fp->flag |= kIoMyBuf;
        fp->bufsiz = 0x1000;
    } else {
        fp->flag |= kIoNbf;
        fp->base = reinterpret_cast<char*>(&fp->charbuf);
        fp->bufsiz = 2;
    }
    fp->cnt = 0;
    fp->ptr = fp->base;
}

// ------------------------------------------------------------------ 0x007bcf64 _freebuf
static void JOAN_CDECL crt_freebuf(Iobuf* fp) {
    const u32 flag = fp->flag;
    if ((flag & 0x83) && (flag & kIoMyBuf)) {
        cdecl_call<void>(kFree, fp->base);
        fp->flag &= ~(kIoMyBuf | kIoSetVBuf);
        fp->ptr = nullptr;
        fp->base = nullptr;
        fp->cnt = 0;
    }
}

// ------------------------------------------------------------------ 0x007bcf8f _stbuf
static int JOAN_CDECL crt_stbuf(Iobuf* fp) {
    if (cdecl_call<int>(kIsatty, fp->file) == 0) return 0;
    int index;
    if (is_iob(fp, kStdout)) index = 0;
    else if (is_iob(fp, kStderr)) index = 1;
    else return 0;
    ++global<int>(kCflush);
    if (fp->flag & 0x10c) return 0;
    char*& slot = global<char*>(kStdbuf + 4u * static_cast<u32>(index));
    if (slot == nullptr) {
        slot = cdecl_call<char*>(kMalloc, 0x1000u);
        if (slot == nullptr) {
            fp->base = reinterpret_cast<char*>(&fp->charbuf);
            fp->ptr = fp->base;
            fp->bufsiz = 2;
            fp->cnt = 2;
            fp->flag |= kIoWrt | kIoYourBuf | kIoFlrtn;
            return 1;
        }
    }
    fp->bufsiz = 0x1000;
    fp->base = slot;
    fp->ptr = slot;
    fp->cnt = 0x1000;
    fp->flag |= kIoWrt | kIoYourBuf | kIoFlrtn;
    return 1;
}

// ------------------------------------------------------------------ 0x007bd01c _ftbuf
static void JOAN_CDECL crt_ftbuf(int flag, Iobuf* fp) {
    if (flag != 0 && (fp->flag & kIoFlrtn)) {
        flush(fp);
        fp->flag &= ~(kIoYourBuf | kIoFlrtn);
        fp->bufsiz = 0;
        fp->ptr = nullptr;
        fp->base = nullptr;
    }
}

// ------------------------------------------------------------------ 0x007bcd24 __initstdio (XI)
static int JOAN_CDECL crt_initstdio() {
    c_initstdio.hit();
    int n = nstream();
    if (n == 0) {
        n = 0x200;
        global<int>(kNstream) = n;
    } else if (n < 20) {
        n = 20;
        global<int>(kNstream) = n;
    }
    global<Iobuf**>(kPiob) = cdecl_call<Iobuf**>(kCalloc, static_cast<u32>(n), static_cast<u32>(sizeof(Iobuf*)));
    if (piob() == nullptr) {
        global<int>(kNstream) = 20;
        global<Iobuf**>(kPiob) = cdecl_call<Iobuf**>(kCalloc, 20u, static_cast<u32>(sizeof(Iobuf*)));
        if (piob() == nullptr) cdecl_call<void>(kAmsgExit, 0x1a);  // _RT_STDIOINIT
    }
    for (u32 i = 0; i < 20; ++i) piob()[i] = iob(i);
    int h = 0;
    for (int i = 0; i < 3; ++i) {
        h = ioinfo(i)->osfhnd;  // _osfhnd(i)
        if (h == -1 || h == 0) iob(static_cast<u32>(i))->file = -1;
    }
    return h;
}

// ------------------------------------------------------------------ 0x007bcdcc __endstdio (XP)
static int JOAN_CDECL crt_endstdio() {
    const int r = cdecl_call<int>(aFlushall);
    if (global<u8>(kExitflag) != 0) return cdecl_call<int>(aFcloseall);
    return r;
}

#define B(name, address, fn) JOAN_REPLACE("crt/stdio", name, address, fn)
B("crt::fopen", 0x007b3fe0, &crt_fopen);
B("crt::_fsopen", 0x007b3faf, &crt_fsopen);
B("crt::_openfile", 0x007bd046, &crt_openfile);
B("crt::_getstream", 0x007bd1b6, &crt_getstream);
B("crt::fclose", 0x007b3ef6, &crt_fclose);
B("crt::_fclose_lk", 0x007b3f27, &crt_fclose_lk);
B("crt::fprintf", 0x007b3f73, &crt_fprintf);
B("crt::fread", 0x007b6b3d, &crt_fread);
B("crt::_fread_lk", 0x007b6b6c, &crt_fread_lk);
B("crt::fwrite", 0x007b7c14, &crt_fwrite);
B("crt::_fwrite_lk", 0x007b7c43, &crt_fwrite_lk);
B("crt::ftell", 0x007b6ce0, &crt_ftell);
B("crt::_ftell_lk", 0x007b6d02, &crt_ftell_lk);
B("crt::fseek", 0x007b6e63, &crt_fseek);
B("crt::_fseek_lk", 0x007b6e8f, &crt_fseek_lk);
B("crt::fgetc", 0x007b6f1c, &crt_fgetc);
B("crt::getc", 0x007b6f4d, &crt_getc);
B("crt::ungetc", 0x007c309f, &crt_ungetc);
B("crt::_ungetc_lk", 0x007c30c8, &crt_ungetc_lk);
B("crt::fflush", 0x007b7d4d, &crt_fflush);
B("crt::_fflush_lk", 0x007b7d7c, &crt_fflush_lk);
B("crt::_flush", 0x007b7daa, &crt_flush);
B("crt::_flushall", 0x007b7e06, &crt_flushall);
B("crt::flsall", 0x007b7e0f, &crt_flsall);
B("crt::_fcloseall", 0x007c3136, &crt_fcloseall);
B("crt::_filbuf", 0x007c0b59, &crt_filbuf);
B("crt::_flsbuf", 0x007bb52b, &crt_flsbuf);
B("crt::_getbuf", 0x007c2da9, &crt_getbuf);
B("crt::_freebuf", 0x007bcf64, &crt_freebuf);
B("crt::_stbuf", 0x007bcf8f, &crt_stbuf);
B("crt::_ftbuf", 0x007bd01c, &crt_ftbuf);
B("crt::__initstdio", 0x007bcd24, &crt_initstdio);
B("crt::__endstdio", 0x007bcdcc, &crt_endstdio);
#undef B

}  // namespace

#if JOAN_IMAGE_X64
// lib/crt/mt.cpp's _lock_file / _unlock_file: _iob's lock index range test and the _FILEX critical section.
int joan::crt::iob_index(const void* fp) {
    const Iobuf* const t = iob_table();
    const auto* const f = static_cast<const Iobuf*>(fp);
    return f >= t && f < t + 20 ? static_cast<int>(f - t) : -1;
}
void* joan::crt::filex_cs(void* fp) { return static_cast<char*>(fp) + kFilexCs; }
#endif

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::fclose(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_fclose>(a0)); }  // 0x7b3ef6
auto joan::orig::libc::fopen(const char* a0, const char* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_fopen>(a0, a1)); }  // 0x7b3fe0
auto joan::orig::libc::fread(void* a0, std::uint32_t a1, std::uint32_t a2, void* a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_fread>(a0, a1, a2, a3)); }  // 0x7b6b3d
auto joan::orig::libc::ftell(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_ftell>(a0)); }  // 0x7b6ce0
auto joan::orig::libc::fseek(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_fseek>(a0, a1, a2)); }  // 0x7b6e63
auto joan::orig::libc::getc(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_getc>(a0)); }  // 0x7b6f4d
auto joan::orig::libc::fwrite(void* a0, std::uint32_t a1, std::uint32_t a2, void* a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::crt_fwrite>(a0, a1, a2, a3)); }  // 0x7b7c14
auto joan::orig::libc::fflush(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_fflush>(a0)); }  // 0x7b7d4d
auto joan::orig::libc::_flsbuf(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_flsbuf>(a0, a1)); }  // 0x7bb52b
auto joan::orig::libc::_filbuf(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_filbuf>(a0)); }  // 0x7c0b59
auto joan::orig::libc::_ungetc_lk(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_ungetc_lk>(a0, a1)); }  // 0x7c30c8
