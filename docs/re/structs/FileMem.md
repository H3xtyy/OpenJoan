# FileMem (ilib\filemem.cpp, sizeof >= 0x11C) — joan.exe

Memory image of a file or a range of a file, or a forwarding wrapper. NOT derived from `File` (own layout).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x000 | 0x105 | char[] | `path` | copied from the source | 0x588000 (`strcpy(this, path)`) |
| 0x105 | 1 | u8 | `packed` | copy of File.packed (+0x10D) | 0x588280 |
| 0x106 | 1 | u8 | `report_errors` | copy of File.report_errors | 0x588280, 0x5892F0 |
| 0x107 | 1 | u8 | `owns_buffer` | free `base` on release | 0x5886D0 |
| 0x108 | 4 | u8* | `cursor` | 0 = no memory image (forward to parent) | 0x5889F0 |
| 0x10C | 4 | u8* | `base` | start of the image | 0x5886D0 |
| 0x110 | 4 | u32 | `size` | image length | 0x588830 |
| 0x114 | 4 | File* | `parent_file` | when set the parent is repositioned on release | 0x588280, 0x5886D0 |
| 0x118 | 4 | FileMem* | `parent_mem` | same for a FileMem parent | 0x588430 |
