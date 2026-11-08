// Original library sources (re/attribution.csv original_file, under lib/): crt/fcvt.c (notes only, nothing
//   registered), crt/wcslwr.c, crt/wcsupr.c.
//
// Batch 113/13: library-range roots 0x007c5ef4, 0x007c6031, 0x007c616e, 0x007c6240
// (+ context 0x007c61de). NOTHING is replaced in this file - every root is either stateful
// CRT code or game code. Details: scratchpad w113_13/ids.md.
//
// Identification (MSVC 6 static CRT, Win32 build):
//  0x007c5ef4  wchar_t* __cdecl _wcslwr(wchar_t* s)            317 B
//      __lc_handle[LC_CTYPE] (0x00a6b2b8) == 0: in-place 'A'..'Z' -> +0x20 on 16-bit units.
//      Otherwise: InterlockedIncrement(__setlc_active 0x00a6ca8c), if __unguarded_readlc_active
//      (0x00a6ca7c) != 0 -> _lock(0x13) (0x7b82aa) / _unlock (0x7b830b); then
//      __crtLCMapStringW (0x7c585f, LCMAP_LOWERCASE 0x100, cp 0) twice (size, then into a
//      malloc (0x7b6c54) buffer), wcscpy (0x7b5a9a) back into s, free (0x7b4e1d).
//      Caller: 0x0058e472 (String::lower in game_b103_10.cpp calls it via cdecl_call).
//  0x007c6031  wchar_t* __cdecl _wcsupr(wchar_t* s)            317 B  - same, 'a'..'z' -> -0x20,
//      LCMAP_UPPERCASE 0x200. Caller: 0x0058e5b2.
//      => STATEFUL family=locale: the game calls setlocale(LC_ALL, <locale>) (0x7b5269 from
//      CodePage::setup 0x0058b2d0), so the LCMapStringW + heap path is live; replace together
//      with setlocale/__lc_handle/_lock family (and heap for the temp buffer).
//  0x007c616e  char* __cdecl _fcvt(double v, int ndigit, int* decpt, int* sign)   112 B
//      _fltout2 (0x7c26ae: __dtold 0x7c270a + $I10_OUTPUT 0x7c4f81 + strcpy 0x7b2020) into a
//      local STRFLT, then _fpcvt (0x7c61de) with ndigit + strflt.decpt.
//      Callers: 0x0058fe45, 0x005a29ce (game_stream2.cpp, misc.cpp crt_fcvt).
//  0x007c61a1  _ecvt (not a root, same shape, no callers from game code found).
//  0x007c61de  static char* _fpcvt(STRFLT* pflt, int digits, int* decpt, int* sign)  98 B (context)
//      _getptd() (0x7b8c21) -> ptd->_cvtbuf (+0x44); malloc(0x15d = CVTBUFSIZE) on first use;
//      _fptostr (0x7c2637) with min(digits, 0x15b); *sign = (pflt->sign == '-'); *decpt.
//      => _fcvt/_ecvt are STATEFUL family=errno/ptd (per-thread _cvtbuf) + heap: the returned
//      pointer is the per-thread buffer, so the result's identity/lifetime is CRT state.
//  0x007c6240  GAME CODE (not a library): Unit::die_7c6240(int killer_id), thiscall, ret 4,
//      6256 B, thunk 0x00408571, recursive (calls its own thunk 3x). Linked after the CRT like
//      0x007c7ab0 (batch 113/14). Not pure -> skipped; belongs in a game-code batch.
//
// No registrations in this file.

namespace {}  // namespace
