# KeyMap (`remapkey.cpp`) — logical actions, default bindings, key-name table — joan.exe

Source file `d:\ww\remapkey.cpp` (assert/alloc strings at `0x4CAE70`, `0x4CB310`). Functions: [../functions/control.md](../functions/control.md) 1.8; users: the five input handlers
(`key_pressed 0x4CB820`, `key_down 0x4CB6D0`), the option pages (`0x7435E0`, `0x7478A0`, `0x745720`), the HUD/menu captions (`0x4CB5B0`), builtin `get_key_string` (`0x65E0A0`).
The tables come from `Resource/Std.set` (dBase sections `DEFAULTK` and `KEYDESC`, schemas U and I in [../../formats/gameset.md](../../formats/gameset.md)); the strings are **Polish** in the retail
install and must go through the localisation layer. Everything below marked **(data)** was read from the shipped `Std.set` (`DEFAULTK` @0xDC88C, 55 x 47 bytes; `KEYDESC` @0x9CD9, 115 x 38 bytes).

## 1. Objects

### KeyMap (0x14 bytes, `memset` by `0x4CAE10`)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `desc_count` | number of KEYDESC records (115) | 0x4CAE70, 0x4CB9B0 |
| 0x04 | 4 | KeyDesc* | `descs` | `desc_count * 0x28` bytes | 0x4CAE70 |
| 0x08 | 4 | Binding* | `live` | the player's current bindings, `bind_count * 0x30` bytes (index = action - 1) | 0x4CB580 |
| 0x0C | 4 | Binding* | `defaults` | the pristine copy loaded from `DEFAULTK` | 0x4CAE70, 0x4CB410 |
| 0x10 | 4 | i32 | `bind_count` | number of actions (55 = 0x37) | 0x4CAE70 |

The singleton's address is not visible in the export (all callers pass it in `ecx`): `TODO(re)`. Reset: `0x4CC220` — `live := defaults` when the config flag `GameConfig+0xB4` (`DAT_00A5806C`) is 0, else `live :=`
the saved copy kept in `GameConfig+0xB5` (**0xA50 bytes = 55 x 0x30, exactly this table, verbatim including the name text**). So CONFIG.DAT persists the key bindings as the raw `live` array.

### Binding (0x30 bytes) — one logical action

| offset | size | type | name | meaning | source column |
|---|---|---|---|---|---|
| 0x00 | 0x20 | char[32] | `name` | action description (Polish, cp1250) | `KEY_ACTION` C32 |
| 0x20 | 2 | — | pad | | |
| 0x22 | 1 | u8 | `joypad_button` | joypad button, 1-based (hex digit: `A` = 10, `B` = 11); `0` or `0xFF` = none | `J_JOYPAD` C1 (hex) |
| 0x23 | 1 | u8 | `mouse_button` | mouse button 1 = left, 2 = right, 3 = wheel click; `0` / `0xFF` = none | `FPS_MOUSE` C1 (decimal) |
| 0x24 | 4 | i32 | `key_k` | DirectInput scancode for scheme 0 (also used by scheme 2) ; blank field -> `0xFFFF` | `K_KEY` C4 (hex) |
| 0x28 | 4 | i32 | `key_j` | alternative keyboard code used by scheme 1 when no joypad button is bound | `J_KEY` C4 (hex) |
| 0x2C | 4 | i32 | `key_fps` | keyboard code for scheme 3; used before the mouse button | `FPS_KEY` C4 (hex) |

Parsing (`0x4CAE70`): each field is `strncpy`'d to a 4-byte buffer and parsed with a `sscanf`-style call (`FUN_007B3EC2`, format `DAT_009D5780`, hex — verified by the data: KEYDESC event `010` is `Q` = scancode 0x10); an
empty field stores `0xFFFF` (keys) / `0xFF` (joypad, mouse). `J_KEY = "0"` stores 0, which the test code also treats as "none".

### KeyDesc (0x28 bytes) — display names for a (device, code) pair

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | i32 | `code` | `EVENT` (hex): DirectInput scancode (device 0), joypad button 1..12 (device 1), mouse button 1..3 (device 2); `0xFFFF` / `0xFF` = "Not Available" |
| 0x04 | 0x20 | char[32] | `name` | `STRING` — returned by `get_key_string` |
| 0x24 | 3 | — | pad | |
| 0x27 | 1 | u8 | `device` | `DEVICE` char as a number (`atoi`): **0 keyboard, 1 joypad, 2 mouse** |

## 2. Lookup rules

| function | rule |
|---|---|
| `get_key_string(scheme, action)` `0x4CB5B0` | scheme 0 -> KeyDesc(0, `key_k`); scheme 1 -> `joypad_button` valid (not 0/0xFF) ? KeyDesc(1, joypad_button) : KeyDesc(0, `key_j`); scheme 3 -> `key_fps` valid (not 0/0xFFFF) ? KeyDesc(0, key_fps) : KeyDesc(2, mouse_button); other schemes (2, 4) -> NULL. `KeyDesc` = `keydesc_find(device, code)` (linear search, NULL if missing) |
| `key_pressed(scheme, action)` `0x4CB820` | trigger: scheme 0: `scancode_pressed(key_k)`; scheme 1: joypad button (when a joystick is present, `*DAT_00A51806`) else `key_j`; scheme 3: `key_fps` else mouse button edge (`0x594700(button-1)`) |
| `key_down(scheme, action)` `0x4CB6D0` | hold test with the same selection (`scancode_down`, `0x597ED0(button-1)`, `DAT_00A51DA4[button]`) |
| scheme used by each control mode | mode 0 -> scheme 0, mode 1 -> 1, **mode 2 -> 0** (`0x438B30` queries `(0, ...)`), mode 3 -> 3, RTS (4) -> the mode it was entered from (`ControlMgr+0x14`); the hotkeys/menu screens use the current mode's scheme (`DAT_00A5B9F8`, base mode while in RTS) |
| option page device mapping | the options menu's control list (selected value `v` at `menu+0xC0 -> +0x80`): `v = 1` -> scheme 0, `v = 2` -> scheme 3, `v = 3` -> scheme 1. Mode 2 is not offered. The configured scheme is `GameConfig+0x6B` (`DAT_00A58023`, default **3**); a configured 1 falls back to 0 when no joystick is present (`0x42DE70`) |
| editable rows | an option page lists the actions of a range whose bound name is not "Not Available" and not "Escape" (so headers and the ESC binding are hidden). Page ranges (action ids): 1..10 movement, 11..21 combat, 22..32 command, 35..41 menus (0x23..0x29), 42..46 indicators, 47..51 camera, 53..55 extra (`0x7478A0`) |
| reset | `keymap_reset_scheme(scheme)` (`0x4CB410`) restores only the columns of that scheme (0 -> `key_k`; 1 -> `key_j` + `joypad_button`; 3 -> `key_fps` + `mouse_button`) |

## 3. The 55 logical actions and default bindings **(data)**

Index = 1-based record number = the `action` argument of every `key_pressed/key_down/get_key_string` call. Columns: scheme 0 key (`K`), scheme 1 (`J` key / joypad button), scheme 3 (`FPS` key / mouse button).
Scancodes are DirectInput (DIK_) hex; names in parentheses are the English keys. `—` = blank. "Script name" = the argument string of builtin `get_key_string` (`0x65E0A0`, case-insensitive compare).
English glosses of the Polish names are mine. Rows marked *header* carry no keys and are section titles in the options UI.

| id | Polish name (data) | English | K | J key / pad | FPS key / mouse | script name | used by |
|---|---|---|---|---|---|---|---|
| 1 | Movement Keys | *header* | — | — | — | | |
| 2 | Centrowanie kamery na postaci | center camera (joystick variant) | — | 0 / pad **A** (10) | — | SHIFT_CAMERA | mode 1: dodge modifier, `0x432D30` |
| 3 | Centrowanie kamery na postaci | center camera / walk modifier | 0x2A (Shift) | — | — | SHIFT | mode 0: camera follow, walk instead of run, strafe, dodge modifier |
| 4 | Do przodu | forward | 0xC8 (Up) | — | 0x11 (W) | FORWARD | all |
| 5 | Do tylu | backward | 0xD0 (Down) | — | 0x1F (S) | BACKWARD | all |
| 6 | W lewo | strafe left | — | — | 0x1E (A) | SHIFT_LEFT | mode 3 |
| 7 | W prawo | strafe right | — | — | 0x20 (D) | SHIFT_RIGHT | mode 3 |
| 8 | Zwrot w lewo | turn left | 0xCB (Left) | — | — | TURN_LEFT | mode 0 |
| 9 | Zwrot w prawo | turn right | 0xCD (Right) | — | — | TURN_RIGHT | mode 0 |
| 10 | Marsz | walk (march) | — | — | 0x2A (Shift) | | mode 3 walk modifier |
| 11 | Sterowanie | *header* (Controls) | — | — | — | | |
| 12 | Obrona | defend / block | 0x12 (E) | 0 / pad 2 | 0x10 (Q) | DEFEND, DOWN_ATTACK | block, down attack |
| 13 | Lekki atak | light attack (A) | 0x10 (Q) | 0 / pad 4 | — / mouse 1 | A_ATTACK | combos; fire in aim mode |
| 14 | Silny atak | strong attack (B) | 0x11 (W) | 0 / pad 3 | — / mouse 2 | B_ATTACK | combos |
| 15 | Atak energii | energy attack | 0x1F (S) | — / — | 0x2E (C) | ENERGY_ATTACK | `0x437910` (joystick: A+B chord) |
| 16 | Skok | jump | 0x39 (Space) | 0 / pad 1 | 0x39 (Space) | | jump (hold time) |
| 17 | Interakcja | interact | 0x13 (R) | 0 / pad 8 | 0x13 (R) | INTERACT (+ one more string) | talk, free prisoner, hire, recruit, mount, siege, barracks, workshop |
| 18 | Strzelanie z luku/kuszy | shoot bow/crossbow | 0x2C (Z) | 0 / pad 9 | 0x2C (Z) | ARROW_SHOOTING | aim mode toggle (hold in mode 1) |
| 19 | Przywrocenie zdrowia | restore health | 0x3D (F3) | 0 / pad 5 | 0x3D (F3) | USE_HEAL_ITEM | `0x43B490` |
| 20 | Przywrocenie energii | restore energy | 0x3E (F4) | 0 / pad 6 | 0x3E (F4) | | `0x43B990` |
| 21 | Chain | chain attack | — | — | — | | mode 3 only (`0x438820`), unbound by default |
| 22 | Dowodzenie | *header* (Command) | — | — | — | | |
| 23 | Zmiana Akcja/RTS | switch Action / RTS | 0x3C (F2) | 0x3C / 0 | 0x3C (F2) | RTS_MODE | RTS toggle |
| 24 | Zmiana postaci | switch character | 0x29 (`) | 0x29 / 0 | 0x29 | SWITCH_CHARACTER | `0x432300` |
| 25 | Joanna d'Arc | Joan | 0x02 (1) | 0x02 / 0 | 0x02 | SWITCH_JOAN | Shift+key slot 1 |
| 26 | Jean de Metz | | 0x03 (2) | 0x03 | 0x03 | SWITCH_JEAN | slot 2 |
| 27 | La Hire | | 0x04 (3) | 0x04 | 0x04 | SWITCH_LA_HIRE | slot 3 |
| 28 | Ksiaze Alencon | Duke of Alencon | 0x05 (4) | 0x05 | 0x05 | | slot 4 |
| 29 | Bastard z Orleanu | Bastard of Orleans | 0x06 (5) | 0x06 | 0x06 | | slot 5 |
| 30 | Sir Arthur de Richemont | | 0x07 (6) | 0x07 | 0x07 | | slot 7 (the handler order is 0x19,0x1A,0x1B,0x1C,0x1D,**0x1F,0x1E**,0x20) |
| 31 | Krol Karol VII | King Charles VII | 0x08 (7) | 0x08 | 0x08 | | slot 6 |
| 32 | Guillaume de Flavy | | — | — | — | | slot 8, unbound |
| 33 | Menu gry | *header* (Game menu) | — | — | — | | |
| 34 | Glowne menu gry | main game menu | 0x01 (Esc) | 0x01 | 0x01 | | hotkey 0x22 |
| 35 | Opis bohatera | hero description | 0x3B (F1) | 0 / pad 7 | 0x3B (F1) | CHARACTER_INTERFACE | 0x23 |
| 36 | Okno atakow laczonych | combo window | — | — | — | COMBO_INTERFACE | 0x24 |
| 37 | Zapis stanu gry | save game | 0x41 (F7) | 0x41 | 0x41 | | 0x25 |
| 38 | Wczytanie stanu gry | load game | 0x42 (F8) | 0x42 | 0x42 | | 0x26 |
| 39 | Menu Opcji | options | 0x43 (F9) | 0x43 | 0x43 | | 0x27 |
| 40 | O misji | about the mission | 0x58 (F12) | 0x58 | 0x58 | GAME_OBJECTIVE_KEY | 0x28 |
| 41 | Dziennik gry | message log | 0x32 (M) | 0x32 | 0x32 | | 0x29 |
| 42 | Wskazniki ekranowe | *header* (HUD indicators) | — | — | — | | |
| 43 | Ostrzezenia | warnings | 0x1D (Ctrl) | 0x1D | 0x1D | | HUD warnings |
| 44 | Wskaznik zdrowia | health indicator | 0x57 (F11) | 0x57 | 0x57 | | HUD |
| 45 | Przyblizenie minimapy | minimap zoom | 0x0F (Tab) | 0x0F | 0x0F | | HUD |
| 46 | Cele misji | mission goals | 0x18 (O) | 0x18 | 0x18 | | `0x42E1A0` action 0x2E |
| 47 | Sterowanie kamera | *header* (Camera) | — | — | — | | |
| 48 | Przyblizenie | zoom in | 0x4E (Num +) | 0x4E | 0x4E | ZOOM_IN | |
| 49 | Oddalenie | zoom out | 0x4A (Num -) | 0x4A | 0x4A | ZOOM_OUT | |
| 50 | Obrot widoku w prawo | rotate view right | 0x33 (,) | 0x33 | 0x33 | | RTS only |
| 51 | Obrot widoku w lewo | rotate view left | 0x34 (.) | 0x34 | 0x34 | | RTS only |
| 52 | Dodatkowe | *header* (Extra) | — | — | — | | |
| 53 | Szybki zapis | quick save | 0x3F (F5) | 0x3F | 0x3F | | `0x42E1A0` |
| 54 | Szybkie wczytanie | quick load | 0x40 (F6) | 0x40 | 0x40 | | `0x42E1A0` |
| 55 | Zrzut ekranu | screenshot | 0x44 (F10) | 0x44 | 0x44 | | |

Notes on the table:
* Joystick column: a `J key` of `0` = none; joypad button numbers are 1-based, `A` = 10. The stick itself (axes 0/1, fallback 4/5) is hard-wired to movement. The hat/analog options are not in `DEFAULTK`.
* The "Joystick" scheme uses `key_j` as the keyboard fallback: for rows 23..46 and 53..55 the joystick scheme and the FPS scheme have the same keys as the keyboard scheme.
* The raw record layout is `[flag ' '][name 32][K 4][J 4][pad 1][FPS 4][mouse 1]`, 47 bytes; the printed keys above are decoded from those fields. Rows 2 and 3 are the only ones where the same text appears twice.
* Rows whose Polish names contain diacritics are given here without them for ASCII safety; the file stores cp1250.

## 4. KEYDESC contents **(data)**

* Device 0 (keyboard, 98 records): Escape 01; digits 02..0B (`1`..`9`, `0`); `-` 0C; `=` 0D; Backspace 0E; Tab 0F; `Q W E R T Y U I O P` 10..19; `[` 1A; `]` 1B; Enter 1C; Control 1D; `A S D F G H J K L` 1E..26; `;` 27; `'` 28; `` ` `` 29; Shift 2A;
  `\` 2B; `Z X C V B N M` 2C..32; `,` 33; `.` 34; `/` 35; Shift 36 (right); `NUMPAD*` 37; Alt 38; Spacja 39; F1..F10 3B..44; `NUMPAD7/8/9` 47..49; `NUMPAD-` 4A; `NUMPAD4/5/6` 4B..4D; `NUMPAD+` 4E; `NUMPAD1/2/3` 4F..51; `NUMPAD0` 52; `NUMPAD.` 53;
  F11 57; F12 58; Enter 9C (keypad); Control 9D (right); `NUMPAD/` B5; Alt B8 (right); Pause C5; Home C7; Up C8; PageUp C9; Left CB; Right CD; End CF; Down D0; PageDown D1; Insert D2; Delete D3; `0xFFFF` "Not Available".
  (CapsLock 3A, NumLock 45, ScrollLock 46 and F13+ have no record.)
* Device 1 (joypad): `JOYPAD1` .. `JOYPAD12`. In the dBase text the record starts with the DEVICE digit `1` followed by the hex event: `11` = JOYPAD1 (event 1), ... `19` = JOYPAD9, `1A` = JOYPAD10, `1B` = JOYPAD11, `1C` = JOYPAD12; plus `1FF` = device 1, event `FF`, "Not Available".
* Device 2 (mouse): `21` "Lewy Przycisk Myszy" (left), `22` "Prawy Przycisk Myszy" (right), `23` "Kolko myszy" (wheel click) and one "Not Available" record (the 115th, not printed by the probe).

Both Ctrl/Shift/Alt codes are reachable but `scancode_pressed` merges left+right (modifier bits), so a binding to Shift triggers on either shift key.

## 5. Script builtin `get_key_string` (`0x65E0A0`, [script_builtins.md](../functions/script_builtins.md))

The argument string is compared (case-insensitively) against: `A_ATTACK` 0xD, `B_ATTACK` 0xE, `DEFEND` 0xC, `ARROW_SHOOTING` 0x12, `INTERACT` 0x11 (a second string at `0x9DBA30` also maps to 0x11), `CHARACTER_INTERFACE` 0x23, `DOWN_ATTACK` 0xC,
`USE_HEAL_ITEM` 0x13, `COMBO_INTERFACE` 0x24, `SWITCH_CHARACTER` 0x18, `FORWARD` 4, `BACKWARD` 5, `TURN_LEFT` 8, `TURN_RIGHT` 9, `SHIFT_LEFT` 6, `SHIFT_RIGHT` 7, `SHIFT` 3, `SHIFT_CAMERA` 2, `ZOOM_IN` 0x30, `ZOOM_OUT` 0x31,
`ENERGY_ATTACK` 0xF, `SWITCH_JOAN` 0x19, `SWITCH_JEAN` 0x1A, `SWITCH_LA_HIRE` 0x1B, `GAME_OBJECTIVE_KEY` 0x28, `RTS_MODE` 0x17 (26 names). The result string is `get_key_string(DAT_00A5B9F8, id)` (the *current* control mode's scheme) pushed with `0x5A1440`;
an unknown name pushes nothing.

## 6. Open questions

* TODO(re): address of the KeyMap singleton (the `ecx` of `0x4CB820`); `DAT_009D5780` format string (expected `%x`); how a rebind is captured (the option page code `0x7435E0..0x744CE0` is only partly exported: it writes into `live[]` and calls `keymap_reset_scheme`).
* TODO(re): the second `INTERACT` string at `0x9DBA30`.
