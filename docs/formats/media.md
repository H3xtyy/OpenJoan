# `.wma` music and `.wmv` movies

| files | where | count | container |
|---|---|---|---|
| `.wma` | `music/` (`JOA_*` tracks, `env_*` ambiences) | 40 | Windows Media (ASF); the ASF header GUID starts every file |
| `.wmv` | `movie/` (`intro.wmv`, `enlight.wmv`) | 2 | Windows Media (ASF), same signature |

The files are Microsoft's formats and are not specified here. The original decoded them with the Windows Media synchronous reader; OpenJoan
implements that interface on FFmpeg: [../../src/media/README.md](../../src/media/README.md) (`wm_reader.{hpp,cpp}`, `media_file.{hpp,cpp}`).
A user never has to convert the files.
