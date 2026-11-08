# SeTable (ilib\se.cpp) and SeEntry — joan.exe

Index of sound effects built from two ResLX banks ([res.md](../../formats/res.md)); builder 0x490D30, destructor 0x59E0C0.

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x04 | 4 | SeEntry* | `entries` | `total * 0x28` bytes | 0x490D30 | V |
| 0x08 | 4 | u8* | `flags` | `total` bytes (all 0 at build; per-entry runtime flag) | 0x490D30 | V |
| 0x50 | 4 | i32 | `initialised` | cleared by 0x59E0C0 | 0x59E0C0 | V |
| 0x54 | 4 | i32 | `owns_arrays` | free `entries`/`flags` when set | 0x59E0C0 | V |
| 0x58 | 4 | i32 | `n_wave` | copy of `*(this+0x189)` (payload count of bank 1) | 0x490D30 | V |
| 0x5C | 4 | i32 | `n_extra` | `*(*(this+0x19D)+0x121)` = ResLX count of bank 2 | 0x490D30 | V |
| 0x60 | 4 | i32 | `total` | `n_wave + n_extra` | 0x490D30 | V |
| 0x189 | 4 | i32 | (bank-1 count) | | 0x490D30 | R |
| 0x19D | 4 | ResLX* | (bank-2) | | 0x490D30 | R |

SeEntry (0x28 bytes): `+0 void* wave` (RIFF payload pointer from `ResLX::get_payload(i+1)`, NULL for the extra entries), `+4 i16 id` (1-based within its
bank), rest unknown (voice state/priority, probably; TODO(re)).
