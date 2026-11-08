# PEmiResEntry (`pemires.cpp`, 0x28 bytes) and table header

Table header `this` of 0x4C1B40 (`pemires_scan_scripts`): `+0 i32 count`, `+4 PEmiResEntry* entries` (allocated `count*0x28`).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 0x22 | char[34] | file_name | strcpy of `_finddata_t.name` (no bound check; names over 33 chars would overflow into index) | 0x4C1B40 read |
| 0x22 | 2 | u16 | index | 1-based position in the enumeration order of `_findnext` | 0x4C1B40 read |
| 0x24 | 4 | PSystemInstance* | template | lazily parsed template: 0x4C1D80(index) allocates `new 0x78` (ctor 0x6758F0 + init 0x6759A0), reads `SCRIPT\PSCRIPT\<file_name>` (max 0x10000 bytes on the stack, larger on heap), runs parser 0x677390, never freed; non-null = already loaded | verified (0x4C1D80, 0x4C2060) |

Status: read (single function). Directory order is the CRT `_findfirst` order (filesystem order on NTFS = alphabetical); the port must sort or use stable order if indices are saved anywhere. TODO(re): how 1-based index maps to a scenario/effect id.
