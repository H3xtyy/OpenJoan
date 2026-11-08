# src/media

FFmpeg behind the Windows Media interface the game calls, for its `.wmv` movies and `.wma` music. No registrations; the
game's own player code is `src/uti3d/video.cpp`.

| File | What |
|---|---|
| `wm_reader.{hpp,cpp}` | Windows Media's synchronous reader (`IWMSyncReader`, `wmvcore.dll`) implemented on FFmpeg; answered through the import overrides (`src/runtime/loader/import_overrides.hpp`) |
| `media_file.{hpp,cpp}` | a movie or music file decoded with FFmpeg into the sample formats the game asks Windows Media for (video as RGB565 frames, bottom-up; audio as PCM) |

Settings: `[Video] Trace` (`openjoan.ini`). Dependencies: FFmpeg 7.x (`cmake/ffmpeg.cmake`, target `joan_ffmpeg`;
`third_party/ffmpeg-win64` on Windows).

Verification: a manual play test (intro movie, mission briefing music).
