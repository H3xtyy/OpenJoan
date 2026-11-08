# VideoCapture (`uti3d/videocap.cpp`) — developer screen recorder (NOT part of gameplay)

Reference build joan.exe. Functions: [video.md](../functions/video.md). `this` of 0x686350 / 0x686F50. Global instance (address not
exported). Fields inferred from the open routine and the close routine; the per-frame writer is not in the export.

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | int32 | open | 1 after successful open | verified 0x686350, 0x686F50 |
| 0x04 | 4 | uint32 | bytes_per_pixel | `bpp(d3dformat(fmt))` from 0x5FFE60(0x600080(fmt)) | read |
| 0x08 | 4 | int32 | width | arg 4 (the back-buffer width `DAT_00a4885c`) | read |
| 0x0C | 4 | int32 | height | arg 5 (`DAT_00a48860`) | read |
| 0x10 | 4 | uint8* | frame_ptr | pointer to the pixel buffer of the current INSSBuffer (`INSSBuffer::GetBuffer`, slot 6); zeroed on close | read |
| 0x14 | 4 | int32 | video_input | writer input index of the video input (-1 if none) | read |
| 0x18 | 4 | HMODULE | wmvcore | `LoadLibraryA("wmvcore.dll")`; FreeLibrary on close | verified |
| 0x1C | 4 | IWMWriter* | writer | `WMCreateWriter(NULL, &writer)` | verified |
| 0x20 | 4 | IWMWriterAdvanced* | writer_adv | QI of the writer with IID at 0x9D12D0 (IWMWriterAdvanced) | inferred (IID bytes not exported) |
| 0x24 | 4 | INSSBuffer* | sample | `IWMWriter::AllocateSample(bpp*w*h)` | read |

Args of 0x686350: `(const char* path, int unused30 /*0x1e, not read*/, int fmt, int w, int h)`. The constant `640x480.prx` is a WM profile file
loaded from the current directory (not shipped; dev-only).
