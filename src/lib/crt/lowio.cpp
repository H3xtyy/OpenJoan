// Original library sources (re/attribution.csv original_file, under lib/): crt/chsize.c, crt/close.c, crt/commit.c,
//   crt/ioinit.c, crt/isatty.c, crt/lseek.c, crt/open.c, crt/osfinfo.c, crt/read.c, crt/setmode.c, crt/write.c.
//
// Batch 114 / list 10: family crt/lowio (G part 1) - the MSVC 6 LIBCMT low-level file handle layer
// (lowio/*.c: ioinit.c, osfinfo.c, open.c, read.c, write.c, lseek.c, close.c, commit.c, isatty.c,
// chsize.c, setmode.c), replaced "at birth" as one unit.
//
// Surface (every lowio function reachable from code outside the family; all cdecl, plain `ret`;
// callers outside the family in brackets):
//   0x007c0e73 void _ioinit(void)                       [WinMainCRTStartup 0x7b7267]
//   0x007bce84 int  _close(int fh)                      [_fclose_lk 0x7b3f47; _sopen]
//   0x007bcee1 int  _close_lk(int fh)                   (_close)
//   0x007c0c35 int  _read(int fh, void* buf, unsigned n) [_fread_lk 0x7b6bfc, _filbuf 0x7c0ba1]
//   0x007c0c9a int  _read_lk(int, void*, unsigned)      (_read, _sopen) - text mode CR/LF + Ctrl-Z
//   0x007c1083 long _lseek(int fh, long pos, int how)   [_ftell_lk x3, _fseek_lk, _flsbuf]
//   0x007c10e8 long _lseek_lk(int, long, int)           (_lseek, _read_lk, _write_lk, _sopen, _chsize_lk)
//   0x007c1e0c int  _write(int fh, const void*, unsigned) [_fwrite_lk 0x7b7ce5, _flush 0x7b7dd6, _flsbuf x2]
//   0x007c1e71 int  _write_lk(int, const void*, unsigned) (_write, _chsize_lk) - LF -> CR LF
//   0x007c1ffc int  _commit(int fh)                     [_fflush_lk 0x7b7d9a]
//   0x007c2ded int  _isatty(int fh)                     [_flsbuf 0x7bb58c, _stbuf 0x7bcf97]
//   0x007c31b7 int  _alloc_osfhnd(void)                 (_sopen)
//   0x007c32da int  _set_osfhnd(int fh, long h)         (_sopen)
//   0x007c3356 int  _free_osfhnd(int fh)                (_close_lk)
//   0x007c33d5 long _get_osfhandle(int fh)              (_close_lk, _lseek_lk, _commit, _chsize_lk)
//   0x007c34be void _lock_fhandle(int fh)               (wrappers above, _alloc_osfhnd)
//   0x007c351d void _unlock_fhandle(int fh)             (wrappers above, _sopen)
//   0x007c3556 int  _sopen(const char*, int oflag, int shflag, int pmode)  [_openfile 0x7bd185]
//   0x007c5509 int  _chsize_lk(int fh, long size)       (_sopen: Ctrl-Z removal of text files)
//   0x007c5b7e int  _setmode_lk(int fh, int mode)       (_chsize_lk)
// Not replaced (no caller anywhere, no pointer to them in the image): _ioterm 0x7c102f,
// _open_osfhandle 0x7c3417, _open 0x7c353f, _chsize 0x7c54b0, _setmode 0x7c5b25 (and _strdup
// 0x7c5afa sits next to them, heap family). functions.json merges several of these bodies
// (0x7c33d5 includes _open_osfhandle, 0x7c351d includes _open, 0x7c5434 is __multtenpow12 +
// _chsize, 0x7c5afa includes _setmode): the boundaries above come from the call targets.
//
// State: kept at the ORIGINAL addresses with the VC6 layout, because still-original (or crt/stdio)
// code reads it directly: _ftell_lk 0x7b6d02 (FTEXT / FCRLF), _flsbuf 0x7bb52b (FAPPEND),
// __initstdio 0x7bcd24 (osfhnd of fd 0..2), _filbuf 0x7c0b59 (FTEXT / FEOFLAG).
//   __pioinfo[64] 0x00a6b5a0 (blocks of 32 ioinfo, 0x24 bytes: +0 osfhnd, +4 osfile, +5 pipech,
//   +8 lockinitflag, +0xc CRITICAL_SECTION), _nhandle 0x00a6b6dc; read only: __app_type 0x00a30cc0,
//   _fmode 0x00a6b57c, _umaskval 0x00a6b1e0.
// Since the state, its layout and every write are identical to the original, swapping the family
// is also safe after handles exist; it is installed with the other hooks before 0x7b71e6 anyway.
//
// Other families are called only through their original addresses (crt/mt, crt/heap, crt/exit,
// pure library): _errno 0x7bbfb5, __doserrno 0x7bbfbe, _dosmaperr 0x7bbf42, _lock 0x7b82aa /
// _unlock 0x7b830b (locks 0x11 _LOCKTAB_LOCK, 0x12 _OSFHND_LOCK), malloc 0x7b6c54, _amsg_exit
// 0x7b72ee, memset 0x7b1690. Win32 calls go through joan.exe's IAT slots (same calls, same order,
// GetLastError read at the same points). Calls inside the family are direct (all entries are ours).
// The call order of _errno / __doserrno / GetLastError is kept everywhere (_getptd may allocate).
// Only difference: the stack probe 0x7b29b0 (_chkstk) of _chsize_lk's 4 KB buffer is our
// compiler's own probe (no observable effect).
//
// Module: crt/lowio.
#include "ioinfo.hpp"
#include "../../callconv.hpp"
#include <cstddef>
#include <cstdint>

#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using joan::hook::global;

using joan::call::cdecl_call;
template <typename R, typename... A>
R import_call(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

// An OS handle as the HANDLE the APIs take: ioinfo keeps it as a 32-bit long (VC6), sign-extended like LongToHandle
// (INVALID_HANDLE_VALUE stays -1; kernel handles have 32 significant bits).
inline void* os(std::int32_t h) { return reinterpret_cast<void*>(static_cast<std::intptr_t>(h)); }
inline std::int32_t os_long(void* h) { return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(h)); }  // HandleToLong

JOAN_COUNTER(c_ioinit, "crt::_ioinit");
JOAN_COUNTER(c_read_lk, "crt::_read_lk");
JOAN_COUNTER(c_write_lk, "crt::_write_lk");
JOAN_COUNTER(c_alloc_osfhnd, "crt::_alloc_osfhnd");
JOAN_COUNTER(c_sopen, "crt::_sopen");
JOAN_COUNTER(c_chsize_lk, "crt::_chsize_lk");

// ---------------------------------------------------------------- original addresses
constexpr img::Addr kPioinfo = d::g_pioinfo;  // ioinfo* __pioinfo[64]
constexpr img::Addr kNhandle = d::g_nhandle;  // int _nhandle
constexpr img::Addr kAppType = d::g_appType;  // int __app_type (2 = _GUI_APP)
constexpr img::Addr kFmode = d::g_fmode;    // int _fmode
constexpr img::Addr kUmask = d::g_umask;    // int _umaskval

constexpr fn::Code kErrno = fn::crt_errno;      // int* _errno(void)
constexpr fn::Code kDoserrno = fn::crt_doserrno;   // unsigned long* __doserrno(void)
constexpr fn::Code kDosmaperr = fn::crt_dosmaperr;  // void _dosmaperr(unsigned long)
constexpr fn::Code kLock = fn::crt_lock;       // void _lock(int)
constexpr fn::Code kUnlock = fn::crt_unlock;     // void _unlock(int)
constexpr fn::Code kMalloc = fn::malloc;     // void* malloc(size_t)
constexpr fn::Code kAmsgExit = fn::crt_amsg_exit;   // void _amsg_exit(int)
constexpr fn::Code kMemset = fn::crt_memset;     // void* memset(void*, int, size_t)

// KERNEL32 IAT slots of joan.exe
constexpr img::Addr kInitializeCriticalSection = d::iat_KERNEL32_InitializeCriticalSection;
constexpr img::Addr kEnterCriticalSection = d::iat_KERNEL32_EnterCriticalSection;
constexpr img::Addr kLeaveCriticalSection = d::iat_KERNEL32_LeaveCriticalSection;
constexpr img::Addr kWriteFile = d::iat_KERNEL32_WriteFile;
constexpr img::Addr kCloseHandle = d::iat_KERNEL32_CloseHandle;
constexpr img::Addr kCreateFileA = d::iat_KERNEL32_CreateFileA;
constexpr img::Addr kSetFilePointer = d::iat_KERNEL32_SetFilePointer;
constexpr img::Addr kReadFile = d::iat_KERNEL32_ReadFile;
constexpr img::Addr kGetStartupInfoA = d::iat_KERNEL32_GetStartupInfoA;
constexpr img::Addr kSetEndOfFile = d::iat_KERNEL32_SetEndOfFile;
constexpr img::Addr kGetLastError = d::iat_KERNEL32_GetLastError;
constexpr img::Addr kSetStdHandle = d::iat_KERNEL32_SetStdHandle;
constexpr img::Addr kFlushFileBuffers = d::iat_KERNEL32_FlushFileBuffers;
constexpr img::Addr kGetFileType = d::iat_KERNEL32_GetFileType;
constexpr img::Addr kGetStdHandle = d::iat_KERNEL32_GetStdHandle;
constexpr img::Addr kSetHandleCount = d::iat_KERNEL32_SetHandleCount;

// osfile flags (VC6 internal.h)
constexpr u8 FOPEN = 0x01, FEOFLAG = 0x02, FCRLF = 0x04, FPIPE = 0x08, FNOINHERIT = 0x10, FAPPEND = 0x20,
             FDEV = 0x40, FTEXT = 0x80;

// VC6 ioinfo (internal.h, _MT), 0x24 bytes. The blocks are malloc'd here (kBlockBytes) and read by crt/stdio
// (lib/crt/stdio.cpp, the same struct): x64 widens the CRITICAL_SECTION to 0x28 bytes at +0x10 (0x38 bytes) -
// InitializeCriticalSection writes all of it.
using joan::crt::IoInfo;  // lib/crt/ioinfo.hpp (one definition for lowio.cpp and stdio.cpp)
constexpr u32 kBlockBytes = 32 * sizeof(IoInfo);  // 0x480

struct StartupInfoA {  // STARTUPINFOA, 0x44 bytes (x64: 0x68)
    u32 cb;
    char* reserved;
    char* desktop;
    char* title;
    u32 x, y, xsize, ysize, xchars, ychars, fill, flags;
    u16 show_window;     // +0x30
    u16 cb_reserved2;    // +0x32
    u8* lp_reserved2;    // +0x34 (x64: +0x48)
    void* std_in;
    void* std_out;
    void* std_err;
};
static_assert(sizeof(StartupInfoA) == (sizeof(void*) == 4 ? 0x44 : 0x68), "STARTUPINFOA size");
static_assert(offsetof(StartupInfoA, lp_reserved2) == (sizeof(void*) == 4 ? 0x34 : 0x48), "STARTUPINFOA.lpReserved2");

struct SecurityAttributes {
    u32 length;
    void* descriptor;
    std::int32_t inherit;
};

IoInfo*& block(int i) { return global<IoInfo*>(kPioinfo + 4u * static_cast<u32>(i)); }
// _pioinfo(fh): __pioinfo[fh >> 5] + (fh & 31)  (sar, like the original)
IoInfo* pio(int fh) { return block(fh >> 5) + (fh & 0x1f); }
int& nhandle() { return global<int>(kNhandle); }
bool in_range(int fh) { return static_cast<u32>(fh) < static_cast<u32>(nhandle()); }

int* errno_p() { return cdecl_call<int*>(kErrno); }
u32* doserrno_p() { return cdecl_call<u32*>(kDoserrno); }
void dosmaperr(u32 e) { cdecl_call<void>(kDosmaperr, e); }
u32 last_error() { return import_call<u32>(kGetLastError); }

void init_block(IoInfo* b) {
    for (IoInfo* p = b; p < b + 32; ++p) {
        p->osfile = 0;
        p->osfhnd = -1;
        p->lockinitflag = 0;
        p->pipech = 10;
    }
}

// error tail shared by the locking wrappers: errno = EBADF, _doserrno = 0, -1
int ebadf_doserr0() {
    *errno_p() = 9;
    *doserrno_p() = 0;
    return -1;
}

// ---------------------------------------------------------------- osfinfo.c
// 0x007c34be
void JOAN_CDECL crt_lock_fhandle(int fh) {
    IoInfo* p = pio(fh);
    if (p->lockinitflag == 0) {
        cdecl_call<void>(kLock, 0x11);
        if (p->lockinitflag == 0) {
            import_call<void>(kInitializeCriticalSection, static_cast<void*>(p->lock));
            ++p->lockinitflag;
        }
        cdecl_call<void>(kUnlock, 0x11);
    }
    import_call<void>(kEnterCriticalSection, static_cast<void*>(pio(fh)->lock));
}

// 0x007c351d
void JOAN_CDECL crt_unlock_fhandle(int fh) { import_call<void>(kLeaveCriticalSection, static_cast<void*>(pio(fh)->lock)); }

// 0x007c31b7: returns the new handle locked, or -1
int JOAN_CDECL crt_alloc_osfhnd() {
    c_alloc_osfhnd.hit();
    int fh = -1;
    cdecl_call<void>(kLock, 0x12);
    int first = 0;  // i * 32
    for (int i = 0; i < 64; ++i, first += 32) {
        IoInfo* b = block(i);
        if (b == nullptr) {
            IoInfo* nb = cdecl_call<IoInfo*>(kMalloc, kBlockBytes);
            if (nb != nullptr) {
                nhandle() += 32;
                block(i) = nb;
                init_block(nb);
                fh = i * 32;
                crt_lock_fhandle(fh);
            }
            break;
        }
        for (IoInfo* p = b; p < block(i) + 32; ++p) {
            if (p->osfile & FOPEN) continue;
            if (p->lockinitflag == 0) {
                cdecl_call<void>(kLock, 0x11);
                if (p->lockinitflag == 0) {
                    import_call<void>(kInitializeCriticalSection, static_cast<void*>(p->lock));
                    ++p->lockinitflag;
                }
                cdecl_call<void>(kUnlock, 0x11);
            }
            import_call<void>(kEnterCriticalSection, static_cast<void*>(p->lock));
            if (p->osfile & FOPEN) {  // taken meanwhile
                import_call<void>(kLeaveCriticalSection, static_cast<void*>(p->lock));
                continue;
            }
            p->osfhnd = -1;
            fh = static_cast<int>(p - block(i)) + first;
            break;
        }
        if (fh != -1) break;
    }
    cdecl_call<void>(kUnlock, 0x12);
    return fh;
}

void set_std_handle(int fh, std::int32_t value) {
    if (global<int>(kAppType) != 1) return;  // _CONSOLE_APP only
    switch (fh) {
    case 0: import_call<std::int32_t>(kSetStdHandle, static_cast<u32>(-10), os(value)); break;
    case 1: import_call<std::int32_t>(kSetStdHandle, static_cast<u32>(-11), os(value)); break;
    case 2: import_call<std::int32_t>(kSetStdHandle, static_cast<u32>(-12), os(value)); break;
    default: break;
    }
}

// 0x007c32da
int JOAN_CDECL crt_set_osfhnd(int fh, std::int32_t value) {
    if (in_range(fh) && pio(fh)->osfhnd == -1) {
        set_std_handle(fh, value);
        pio(fh)->osfhnd = value;
        return 0;
    }
    return ebadf_doserr0();
}

// 0x007c3356
int JOAN_CDECL crt_free_osfhnd(int fh) {
    if (in_range(fh)) {
        IoInfo* p = pio(fh);
        if ((p->osfile & FOPEN) && p->osfhnd != -1) {
            set_std_handle(fh, 0);
            pio(fh)->osfhnd = -1;
            return 0;
        }
    }
    return ebadf_doserr0();
}

// 0x007c33d5
std::int32_t JOAN_CDECL crt_get_osfhandle(int fh) {
    if (in_range(fh)) {
        IoInfo* p = pio(fh);
        if (p->osfile & FOPEN) return p->osfhnd;
    }
    return ebadf_doserr0();
}

// ---------------------------------------------------------------- ioinit.c
// 0x007c0e73
void JOAN_CDECL crt_ioinit() {
    c_ioinit.hit();
    IoInfo* b = cdecl_call<IoInfo*>(kMalloc, kBlockBytes);
    if (b == nullptr) cdecl_call<void>(kAmsgExit, 0x1b);  // _RT_LOWIOINIT (does not return)
    block(0) = b;
    nhandle() = 32;
    init_block(b);

    StartupInfoA si;
    import_call<void>(kGetStartupInfoA, &si);
    if (si.cb_reserved2 != 0 && si.lp_reserved2 != nullptr) {
        int cfi_len = *reinterpret_cast<std::int32_t*>(si.lp_reserved2);
        u8* posfile = si.lp_reserved2 + 4;
        std::int32_t* posfhnd = reinterpret_cast<std::int32_t*>(posfile + cfi_len);
        if (cfi_len >= 0x800) cfi_len = 0x800;
        if (nhandle() < cfi_len) {
            int i = 1;
            do {
                IoInfo* nb = cdecl_call<IoInfo*>(kMalloc, kBlockBytes);
                if (nb == nullptr) {
                    cfi_len = nhandle();
                    break;
                }
                nhandle() += 32;
                block(i) = nb;
                init_block(nb);
                ++i;
            } while (nhandle() < cfi_len);
        }
        for (int fh = 0; fh < cfi_len; ++fh, ++posfile, ++posfhnd) {
            const std::int32_t h = *posfhnd;
            if (h != -1 && (*posfile & FOPEN) &&
                ((*posfile & FPIPE) || import_call<u32>(kGetFileType, os(h)) != 0)) {
                IoInfo* p = pio(fh);
                p->osfhnd = *posfhnd;
                p->osfile = *posfile;
            }
        }
    }

    for (int fh = 0; fh < 3; ++fh) {
        IoInfo* p = block(0) + fh;
        if (p->osfhnd == -1) {
            p->osfile = FOPEN | FTEXT;
            const u32 which = fh == 0 ? static_cast<u32>(-10) : fh == 1 ? static_cast<u32>(-11) : static_cast<u32>(-12);
            void* const h = import_call<void*>(kGetStdHandle, which);
            u32 type = 0;
            if (h != os(-1) && (type = import_call<u32>(kGetFileType, h)) != 0) {
                p->osfhnd = os_long(h);
                type &= 0xff;
                if (type == 2)  // FILE_TYPE_CHAR
                    p->osfile |= FDEV;
                else if (type == 3)  // FILE_TYPE_PIPE
                    p->osfile |= FPIPE;
            } else {
                p->osfile |= FDEV;
            }
        } else {
            p->osfile |= FTEXT;
        }
    }
    import_call<u32>(kSetHandleCount, static_cast<u32>(nhandle()));
}

// ---------------------------------------------------------------- lseek.c
// 0x007c10e8
std::int32_t JOAN_CDECL crt_lseek_lk(int fh, std::int32_t pos, int mthd) {
    const std::int32_t h = crt_get_osfhandle(fh);
    if (h == -1) {
        *errno_p() = 9;
        return -1;
    }
    const u32 newpos = import_call<u32>(kSetFilePointer, os(h), pos, static_cast<void*>(nullptr), mthd);
    const u32 err = newpos == 0xffffffffu ? last_error() : 0u;
    if (err != 0) {
        dosmaperr(err);
        return -1;
    }
    pio(fh)->osfile &= static_cast<u8>(~FEOFLAG);
    return static_cast<std::int32_t>(newpos);
}

// 0x007c1083
std::int32_t JOAN_CDECL crt_lseek(int fh, std::int32_t pos, int mthd) {
    if (in_range(fh) && (pio(fh)->osfile & FOPEN)) {
        crt_lock_fhandle(fh);
        const std::int32_t r = crt_lseek_lk(fh, pos, mthd);
        crt_unlock_fhandle(fh);
        return r;
    }
    return ebadf_doserr0();
}

// ---------------------------------------------------------------- read.c
// 0x007c0c9a
int JOAN_CDECL crt_read_lk(int fh, char* buf, u32 cnt) {
    c_read_lk.hit();
    int bytes_read = 0;
    char* p = buf;
    if (cnt == 0) return 0;
    IoInfo* e = pio(fh);
    const u8 flags = e->osfile;
    if (flags & FEOFLAG) return 0;
    if ((flags & (FPIPE | FDEV)) && e->pipech != 10) {
        --cnt;
        *buf = static_cast<char>(e->pipech);
        p = buf + 1;
        bytes_read = 1;
        pio(fh)->pipech = 10;
    }
    u32 os_read = 0;
    if (!import_call<std::int32_t>(kReadFile, os(pio(fh)->osfhnd), static_cast<void*>(p), cnt, &os_read,
                                   static_cast<void*>(nullptr))) {
        const u32 err = last_error();
        if (err == 5) {  // ERROR_ACCESS_DENIED
            *errno_p() = 9;
            *doserrno_p() = 5;
            return -1;
        }
        if (err == 0x6d) return 0;  // ERROR_BROKEN_PIPE
        dosmaperr(err);
        return -1;
    }
    bytes_read += static_cast<int>(os_read);

    u8* osfile = &pio(fh)->osfile;
    u8 f = *osfile;
    if (!(f & FTEXT)) return bytes_read;
    if (os_read != 0 && *buf == '\n')
        f |= FCRLF;
    else
        f &= static_cast<u8>(~FCRLF);
    *osfile = f;

    char* src = buf;
    char* q = buf;
    char* const end = buf + bytes_read;
    while (src < end) {
        const char ch = *src;
        if (ch == 0x1a) {  // Ctrl-Z: end of file (not for devices)
            u8* of = &pio(fh)->osfile;
            if (!(*of & FDEV)) *of = static_cast<u8>(*of | FEOFLAG);
            break;
        }
        if (ch != '\r') {
            *q++ = ch;
            ++src;
            continue;
        }
        if (src < end - 1) {
            if (src[1] == '\n') {
                src += 2;
                *q++ = '\n';
            } else {
                *q++ = '\r';
                ++src;
            }
            continue;
        }
        // CR is the last byte read: peek at the next one
        ++src;
        char peekchr = 0;
        bool cr = false;
        if (!import_call<std::int32_t>(kReadFile, os(pio(fh)->osfhnd), static_cast<void*>(&peekchr), 1u,
                                       &os_read, static_cast<void*>(nullptr)) &&
            last_error() != 0) {
            cr = true;
        } else if (os_read == 0) {
            cr = true;
        } else if (pio(fh)->osfile & (FPIPE | FDEV)) {
            if (peekchr == '\n') {
                *q++ = '\n';
            } else {
                *q++ = '\r';
                pio(fh)->pipech = static_cast<u8>(peekchr);
            }
        } else if (q == buf && peekchr == '\n') {
            *q++ = '\n';
        } else {
            crt_lseek_lk(fh, -1, 1);  // FILE_CURRENT
            if (peekchr != '\n') cr = true;
        }
        if (cr) *q++ = '\r';
    }
    return static_cast<int>(q - buf);
}

// 0x007c0c35
int JOAN_CDECL crt_read(int fh, char* buf, u32 cnt) {
    if (in_range(fh) && (pio(fh)->osfile & FOPEN)) {
        crt_lock_fhandle(fh);
        const int r = crt_read_lk(fh, buf, cnt);
        crt_unlock_fhandle(fh);
        return r;
    }
    return ebadf_doserr0();
}

// ---------------------------------------------------------------- write.c
// 0x007c1e71
int JOAN_CDECL crt_write_lk(int fh, const char* buf, u32 cnt) {
    c_write_lk.hit();
    int lfcount = 0;
    int charcount = 0;
    u32 dosretval = 0;
    u32 written = 0;
    if (cnt == 0) return 0;
    if (pio(fh)->osfile & FAPPEND) crt_lseek_lk(fh, 0, 2);  // FILE_END
    if (pio(fh)->osfile & FTEXT) {
        const char* p = buf;
        // (cnt > 0 here, so the outer loop runs at least once)
        do {
            char lfbuf[1025];  // BUF_SIZE 1025, filled up to index 1024 at most
            char* q = lfbuf;
            while (static_cast<u32>(p - buf) < cnt) {
                const char ch = *p++;
                if (ch == '\n') {
                    ++lfcount;
                    *q++ = '\r';
                }
                *q++ = ch;
                if (static_cast<int>(q - lfbuf) >= 0x400) break;
            }
            const int towrite = static_cast<int>(q - lfbuf);
            if (!import_call<std::int32_t>(kWriteFile, os(pio(fh)->osfhnd), static_cast<const void*>(lfbuf),
                                           static_cast<u32>(towrite), &written, static_cast<void*>(nullptr))) {
                dosretval = last_error();
                break;
            }
            charcount += static_cast<int>(written);
            if (static_cast<int>(written) < towrite) break;
        } while (static_cast<u32>(p - buf) < cnt);
    } else {
        if (import_call<std::int32_t>(kWriteFile, os(pio(fh)->osfhnd), static_cast<const void*>(buf), cnt,
                                      &written, static_cast<void*>(nullptr))) {
            dosretval = 0;
            charcount = static_cast<int>(written);
        } else {
            dosretval = last_error();
        }
    }

    if (charcount != 0) return charcount - lfcount;
    if (dosretval != 0) {
        if (dosretval == 5) {  // ERROR_ACCESS_DENIED -> EBADF
            *errno_p() = 9;
            *doserrno_p() = 5;
        } else {
            dosmaperr(dosretval);
        }
        return -1;
    }
    if ((pio(fh)->osfile & FDEV) && *buf == 0x1a) return 0;
    *errno_p() = 0x1c;  // ENOSPC
    *doserrno_p() = 0;
    return -1;
}

// 0x007c1e0c
int JOAN_CDECL crt_write(int fh, const char* buf, u32 cnt) {
    if (in_range(fh) && (pio(fh)->osfile & FOPEN)) {
        crt_lock_fhandle(fh);
        const int r = crt_write_lk(fh, buf, cnt);
        crt_unlock_fhandle(fh);
        return r;
    }
    return ebadf_doserr0();
}

// ---------------------------------------------------------------- close.c
// 0x007bcee1
int JOAN_CDECL crt_close_lk(int fh) {
    u32 dosretval = 0;
    if (crt_get_osfhandle(fh) != -1) {
        bool do_close = true;
        if (fh == 1 || fh == 2) {
            const std::int32_t h2 = crt_get_osfhandle(2);
            const std::int32_t h1 = crt_get_osfhandle(1);
            if (h1 == h2) do_close = false;  // stdout and stderr share one OS handle
        }
        if (do_close &&
            !import_call<std::int32_t>(kCloseHandle, os(crt_get_osfhandle(fh))))
            dosretval = last_error();
    }
    crt_free_osfhnd(fh);
    pio(fh)->osfile = 0;
    if (dosretval != 0) {
        dosmaperr(dosretval);
        return -1;
    }
    return 0;
}

// 0x007bce84
int JOAN_CDECL crt_close(int fh) {
    if (in_range(fh) && (pio(fh)->osfile & FOPEN)) {
        crt_lock_fhandle(fh);
        const int r = crt_close_lk(fh);
        crt_unlock_fhandle(fh);
        return r;
    }
    return ebadf_doserr0();
}

// ---------------------------------------------------------------- commit.c
// 0x007c1ffc
int JOAN_CDECL crt_commit(int fh) {
    if (!in_range(fh) || !(pio(fh)->osfile & FOPEN)) {
        *errno_p() = 9;
        return -1;
    }
    crt_lock_fhandle(fh);
    int retval;
    if (pio(fh)->osfile & FOPEN) {
        const u32 err = import_call<std::int32_t>(kFlushFileBuffers, os(crt_get_osfhandle(fh)))
                            ? 0u
                            : last_error();
        retval = static_cast<int>(err);
        if (err != 0) {
            *doserrno_p() = err;
            *errno_p() = 9;
            retval = -1;
        }
    } else {
        *errno_p() = 9;
        retval = -1;
    }
    crt_unlock_fhandle(fh);
    return retval;
}

// ---------------------------------------------------------------- isatty.c
// 0x007c2ded
int JOAN_CDECL crt_isatty(int fh) {
    if (!in_range(fh)) return 0;
    return pio(fh)->osfile & FDEV;
}

// ---------------------------------------------------------------- setmode.c / chsize.c
// 0x007c5b7e
int JOAN_CDECL crt_setmode_lk(int fh, int mode) {
    u8* osfile = &pio(fh)->osfile;
    const u8 old = *osfile;
    const bool was_text = (old & FTEXT) != 0;
    if (mode == 0x8000)  // _O_BINARY
        *osfile = static_cast<u8>(old & 0x7f);
    else if (mode == 0x4000)  // _O_TEXT
        *osfile = static_cast<u8>(old | 0x80);
    else {
        *errno_p() = 0x16;  // EINVAL
        return -1;
    }
    return was_text ? 0x4000 : 0x8000;
}

// 0x007c5509
int JOAN_CDECL crt_chsize_lk(int fh, std::int32_t size) {
    c_chsize_lk.hit();
    int retval = 0;
    const std::int32_t place = crt_lseek_lk(fh, 0, 1);  // SEEK_CUR
    if (place == -1) return -1;
    const std::int32_t filend = crt_lseek_lk(fh, 0, 2);  // SEEK_END
    if (filend == -1) return -1;
    std::int32_t extend = size - filend;
    if (extend > 0) {
        char blanks[0x1000];
        cdecl_call<void*>(kMemset, static_cast<void*>(blanks), 0, 0x1000u);
        const int oldmode = crt_setmode_lk(fh, 0x8000);
        do {
            const std::int32_t num = extend >= 0x1000 ? 0x1000 : extend;
            const int cnt = crt_write_lk(fh, blanks, static_cast<u32>(num));
            if (cnt == -1) {
                if (*doserrno_p() == 5) *errno_p() = 0xd;  // EACCES
                retval = -1;
                break;
            }
            extend -= cnt;
        } while (extend > 0);
        crt_setmode_lk(fh, oldmode);
    } else if (extend < 0) {
        crt_lseek_lk(fh, size, 0);  // SEEK_SET
        retval = import_call<std::int32_t>(kSetEndOfFile, os(crt_get_osfhandle(fh))) ? 0 : -1;
        if (retval == -1) {
            *errno_p() = 0xd;  // EACCES
            const u32 err = last_error();
            *doserrno_p() = err;
        }
    }
    crt_lseek_lk(fh, place, 0);
    return retval;
}

// ---------------------------------------------------------------- open.c
// 0x007c3556  _sopen(path, oflag, shflag, pmode) (pmode read only with _O_CREAT)
int JOAN_CDECL crt_sopen(const char* path, int oflag, int shflag, int pmode) {
    c_sopen.hit();
    SecurityAttributes sa;
    sa.length = 0xc;
    sa.descriptor = nullptr;
    u8 fileflags;
    if (oflag & 0x80) {  // _O_NOINHERIT
        sa.inherit = 0;
        fileflags = FNOINHERIT;
    } else {
        fileflags = 0;
        sa.inherit = 1;
    }
    if (!(oflag & 0x8000) && ((oflag & 0x4000) || global<int>(kFmode) != 0x8000)) fileflags |= FTEXT;

    u32 fileaccess;
    switch (oflag & 3) {
    case 0: fileaccess = 0x80000000u; break;  // GENERIC_READ
    case 1: fileaccess = 0x40000000u; break;  // GENERIC_WRITE
    case 2: fileaccess = 0xc0000000u; break;
    default: goto einval;
    }
    u32 fileshare;
    switch (shflag) {
    case 0x10: fileshare = 0; break;  // _SH_DENYRW
    case 0x20: fileshare = 1; break;  // _SH_DENYWR
    case 0x30: fileshare = 2; break;  // _SH_DENYRD
    case 0x40: fileshare = 3; break;  // _SH_DENYNO
    default: goto einval;
    }
    u32 filecreate;
    switch (oflag & 0x700) {  // _O_CREAT 0x100 | _O_TRUNC 0x200 | _O_EXCL 0x400
    case 0:
    case 0x400: filecreate = 3; break;  // OPEN_EXISTING
    case 0x100: filecreate = 4; break;  // OPEN_ALWAYS
    case 0x500:
    case 0x700: filecreate = 1; break;  // CREATE_NEW
    case 0x300: filecreate = 2; break;  // CREATE_ALWAYS
    case 0x200:
    case 0x600: filecreate = 5; break;  // TRUNCATE_EXISTING
    default: goto einval;
    }
    {
        u32 fileattrib = 0x80;  // FILE_ATTRIBUTE_NORMAL
        if (oflag & 0x100) {
            if (!((pmode & ~global<int>(kUmask)) & 0x80)) fileattrib = 1;  // !_S_IWRITE -> READONLY
        }
        if (oflag & 0x40) {  // _O_TEMPORARY
            fileattrib |= 0x4000000u;  // FILE_FLAG_DELETE_ON_CLOSE
            fileaccess |= 0x10000u;    // DELETE
        }
        if (oflag & 0x1000) fileattrib |= 0x100u;  // _O_SHORT_LIVED -> FILE_ATTRIBUTE_TEMPORARY
        if (oflag & 0x20)
            fileattrib |= 0x8000000u;  // _O_SEQUENTIAL
        else if (oflag & 0x10)
            fileattrib |= 0x10000000u;  // _O_RANDOM

        const int fh = crt_alloc_osfhnd();  // returned locked
        if (fh == -1) {
            *errno_p() = 0x18;  // EMFILE
            *doserrno_p() = 0;
            return -1;
        }
        int ret;
        void* const osfh = import_call<void*>(kCreateFileA, path, fileaccess, fileshare, &sa, filecreate, fileattrib,
                                            static_cast<void*>(nullptr));
        u32 isdev = 0;
        if (osfh == os(-1)) {
            dosmaperr(last_error());
            ret = -1;
        } else if ((isdev = import_call<u32>(kGetFileType, osfh)) == 0) {
            import_call<std::int32_t>(kCloseHandle, osfh);
            dosmaperr(last_error());
            ret = -1;
        } else {
            if (isdev == 2)
                fileflags |= FDEV;
            else if (isdev == 3)
                fileflags |= FPIPE;
            crt_set_osfhnd(fh, os_long(osfh));
            fileflags |= FOPEN;
            const u8 devpipe = fileflags & (FDEV | FPIPE);
            pio(fh)->osfile = fileflags;
            ret = fh;
            if (devpipe == 0 && (fileflags & FTEXT) && (oflag & 2)) {  // _O_RDWR: drop a trailing Ctrl-Z
                const std::int32_t filepos = crt_lseek_lk(fh, -1, 2);
                if (filepos == -1) {
                    if (*doserrno_p() != 0x83) {  // ERROR_NEGATIVE_SEEK: file is empty, fine
                        crt_close(fh);
                        ret = -1;
                    }
                } else {
                    char ch = 0;
                    const int nread = crt_read_lk(fh, &ch, 1);
                    if (nread == 0 && ch == 26 && crt_chsize_lk(fh, filepos) == -1) {
                        crt_close(fh);
                        ret = -1;
                    } else if (crt_lseek_lk(fh, 0, 0) == -1) {
                        crt_close(fh);
                        ret = -1;
                    }
                }
            }
            if (ret != -1 && devpipe == 0 && (oflag & 8)) pio(fh)->osfile |= FAPPEND;  // _O_APPEND
        }
        crt_unlock_fhandle(fh);
        return ret;
    }
einval:
    *errno_p() = 0x16;  // EINVAL
    *doserrno_p() = 0;
    return -1;
}

}  // namespace

JOAN_REPLACE("crt/lowio", "crt::_ioinit", 0x007c0e73, &crt_ioinit);
JOAN_REPLACE("crt/lowio", "crt::_close", 0x007bce84, &crt_close);
JOAN_REPLACE("crt/lowio", "crt::_close_lk", 0x007bcee1, &crt_close_lk);
JOAN_REPLACE("crt/lowio", "crt::_read", 0x007c0c35, &crt_read);
JOAN_REPLACE("crt/lowio", "crt::_read_lk", 0x007c0c9a, &crt_read_lk);
JOAN_REPLACE("crt/lowio", "crt::_lseek", 0x007c1083, &crt_lseek);
JOAN_REPLACE("crt/lowio", "crt::_lseek_lk", 0x007c10e8, &crt_lseek_lk);
JOAN_REPLACE("crt/lowio", "crt::_write", 0x007c1e0c, &crt_write);
JOAN_REPLACE("crt/lowio", "crt::_write_lk", 0x007c1e71, &crt_write_lk);
JOAN_REPLACE("crt/lowio", "crt::_commit", 0x007c1ffc, &crt_commit);
JOAN_REPLACE("crt/lowio", "crt::_isatty", 0x007c2ded, &crt_isatty);
JOAN_REPLACE("crt/lowio", "crt::_alloc_osfhnd", 0x007c31b7, &crt_alloc_osfhnd);
JOAN_REPLACE("crt/lowio", "crt::_set_osfhnd", 0x007c32da, &crt_set_osfhnd);
JOAN_REPLACE("crt/lowio", "crt::_free_osfhnd", 0x007c3356, &crt_free_osfhnd);
JOAN_REPLACE("crt/lowio", "crt::_get_osfhandle", 0x007c33d5, &crt_get_osfhandle);
JOAN_REPLACE("crt/lowio", "crt::_lock_fhandle", 0x007c34be, &crt_lock_fhandle);
JOAN_REPLACE("crt/lowio", "crt::_unlock_fhandle", 0x007c351d, &crt_unlock_fhandle);
JOAN_REPLACE("crt/lowio", "crt::_sopen", 0x007c3556, &crt_sopen);
JOAN_REPLACE("crt/lowio", "crt::_chsize_lk", 0x007c5509, &crt_chsize_lk);
JOAN_REPLACE("crt/lowio", "crt::_setmode_lk", 0x007c5b7e, &crt_setmode_lk);

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_ioinit() -> void { joan::bind::free<&::crt_ioinit>(); }  // 0x7c0e73
