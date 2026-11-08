# `CONFIG.DAT` - the options image

`CONFIG.DAT` in the game folder is a raw memory dump of the `GameConfig` object: no header, version or checksum. `config_load` 0x0045AAF0
requires a file size of exactly 0xB05 bytes (2,821) and reads it over the object (`GameConfig::read` 0x0045ADC0); a file of another size is
ignored and defaults are used (the TSC clock is then calibrated, 0x0045C8C0). Written by the game's options screens and by `VSetting.exe`.

- Field layout: [../re/structs/GameConfig.md](../re/structs/GameConfig.md). Load flow: [../re/functions/gameloop.md](../re/functions/gameloop.md).
- The profile store `Save/Player.dat` is a different file: [sav.md](sav.md).
- OpenJoan reads and writes this file through the ported code (`Config::load` 0x0045aaf0, `Config::save_file` 0x0045a9f0; `src/game/unknown_config.cpp`). The modern options live separately in `src/openjoan.ini`.
