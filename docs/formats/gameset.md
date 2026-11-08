# GameSet container (`Resource/Resource`, `Resource/Std.set`, `Resource/Interfac.set`, `Resource/Gameplay.res`): `u16 count` + `{char name[9]; u32 offset}` x (count+1)

Reader: `ilib\resx.cpp` `ResX::open` **0x59C550** (joan.exe), section fetch 0x59CBB0, name search 0x59CA60; set list manager
`ilib\gameset.cpp` 0x58AE90 ([functions/resdb.md](../re/functions/resdb.md), [structs/ResX.md](../re/structs/ResX.md), [structs/GameSet.md](../re/structs/GameSet.md)).
Wider-name sibling for `.res` banks: [res.md](res.md).

## Layout

```
0x00   u16   count                         number of sections N
0x02   entry[N+1], 13 bytes each:
         char name[9]    NUL padded, max 8 characters (e.g. "CRITICAL" fills 8 + NUL)
         u32  offset     ABSOLUTE file offset of the section payload
       entry[N] = sentinel: name empty, offset = end of file
2+13*(N+1)   first payload (== entry[0].offset)
```
Section `i` = `[entry[i].offset, entry[i+1].offset)`. Name lookup is exact and case-sensitive (`strcmp` over a qsorted copy; 0x59CA60). Section order in the
file is free (Interfac.set lists `HEADER` 4th, Resource lists it 1st); lookups are by name.

## Verified on the install

| file | size | count | first section / offset | check |
|---|---|---|---|---|
| `Resource/Resource` | 94,235 | 0x14 = 20 | `HEADER` @0x113 (275 = 2+21*13) | then ANIMATE 0x1BA, SDATE 0x408, DEFAULT 0x548, AMAZON1 0x2CD4, MODEL 0x4746, RACE 0x49D2... |
| `Resource/Std.set` | 1,048,899 | 0x8C = 140 | `HEADER` @0x072B (= 2+141*13) | ANIMATE 0x07D2, SDATE 0x1555, MODEL 0x1695, RACE 0x1921, NATION 0x19D4, ORLEAN 0x1B49, JOAN 0x44CC, KEYDESC 0x9CD9... |
| `Resource/Interfac.set` | 136,205 | 0x4F = 79 | `MAINGAME` @0x0412 (= 2+80*13) | CRITI_F 0x1D0C, CRITI_K 0x1F98, CRITICAL 0x2224, HEADER 0x2576, INVENT 0x261D... |
| `Resource/Gameplay.res` | 8,808 | 6 | `ECON` @93 (= 2+7*13) | see [gameplay.md](gameplay.md) |

Sizes of the HEADER section of `Resource/Resource`: 0x1BA-0x113 = 167 = 32 + 2*32 + 1 + (1+8+60) + 1: a dBase III table with **one record and two
fields (8 and 60 characters)**.

## What the sections contain

1. **dBase III tables** ([dbf.md](dbf.md)) — in `Resource`, `Std.set`, `Interfac.set` every section starts with `03 yy mm dd` and is a complete `.dbf` image;
   the reader opens them with `Db::open_memory(ptr)`. Joan reads fields by fixed offsets (the table below is derived from the readers):

| section (in `Resource`/`*.set`) | reader | row layout (offset in row w/o delete flag : width) |
|---|---|---|
| `HEADER` | 0x58AE90 | name C8 @0, title C60 @8 (the set's file base name and a description; used to list the installed sets) |
| `ANIMATE` | 0x4172A0 | `CODE` C32 @0, `DBFNAME` C8 @0x20 (name of another section of the same file holding that animation group, e.g. `GENERAL1`, `AMAZON1`, `INFANT1`, `ARCHER1`, `JOAN`...) |
| animation-group sections (`GENERAL1`, `AMAZON1`...) | animgrp.cpp 0x414A30 (other family) | fields `ANIM_CODE POSITION MOVEMENT COMBO ITEM_USING SPECIAL DAMAGE BLEND_TIME START_TIME END_TIME UPPERFILE UPPERNAME LOWERFILE LOWERNAME SCRIPT SOUND SOUND_TIME` (names seen in the `Resource` dump; widths TODO(re)) |
| `SDATE` | 0x4CE0F0 | `DATENAME` C32 @0, `YEAR` N4 @0x20, `MONTH` N2 @0x24, `DAY` N2 @0x26, `WEEKDAY` N1 @0x28 |
| `RACE` | 0x4CA480 | `CODE` C8 @0, `DES` C20 @8, `STD` C20 @0x1C |
| `MODEL`, `ITEM`, `ITEMCLAS`, `ITEMTYPE`, `NATION`, `KEYDESC`, `MORPH`... | modelres / itemres / nationres (other families) | TODO(re) |
| `Interfac.set` sections (`MAINGAME`, `CRITICAL`, `INVENT`, `MAINMENU`, `S1L1W`...) | `BmpRes::load` 0x620BC0 (fin family, [Bitmap.md](../re/structs/Bitmap.md)) | `BITMAP` C8 @0, `TEXTURE` C32 @8 (name of the texture, e.g. `maingame`), `SRC_X1 SRC_Y1 SRC_X2 SRC_Y2` N4 @0x28/0x2C/0x30/0x34, `ROTATION` N1 @0x38, `WIDTH` N4 @0x39, `HEIGHT` N4 @0x3D (offsets from the reader; the field names are the strings found in the section header) |

2. **Plain text** — `Gameplay.res` sections `ECON THROWI UNIT CHARMOVE WEATHER GAME` ([gameplay.md](gameplay.md)).

The `fn` special case: asking for a missing table named anything but `RELEASE` is a fatal `Error Loading %s` (0x58B180).

## Set discovery (0x58AE90)

The game lists `RESOURCE\*.SET` (sorted by file name), opens each as a container and reads `HEADER` record 1 into a 0x46-byte entry (`name[9]`, `title[0x3D]`).
Selecting a set opens `RESOURCE\` + name + extension. `Resource/Resource` (no extension) is not matched by `*.SET`; it is the base resource file opened
by name by other code (TODO(re): opener of the base `Resource`, not identified).

## Implementation notes
- Parse `count`, `(count+1)` entries; verify the first offset equals `2+13*(count+1)` and the sentinel equals the file size (true for all four files).
- Provide `find(name)` (exact) and a view over `[off[i], off[i+1])`; do not copy.
- Section payload kinds are decided by the consumer; sniff `03` + plausible date to detect dBase sections.

## Census and dBase schemas

All four files follow the index layout (`first offset == 2 + 13*(count+1)`, offsets contiguous, sentinel == file size, section names unique, longest name 8).
All 239 table sections of `Resource` (20), `Std.set` (140), `Interfac.set` (79) are valid dBase III images. Findings that differ from a textbook dBase file
(see [dbf.md](dbf.md)): the 0x1A end marker is **absent** in 109 Std.set and 10 Interfac.set sections (image size = `header_len + n*record_len`), and tables
rewritten by the editor store **0** in the descriptor "offset" field (the game computes offsets itself). Types are only `C` and `N`.
Section order is not alphabetical; `HEADER` is first in `Resource`/`Std.set` (record: set code `STD`, description "Standard Game Set") and fourth in `Interfac.set`.
Schemas below: widths are field lengths (`.d` = decimals); letters are local labels.

### `Resource/Resource` — 20 sections, 9 distinct schemas

**Schema A** (record_len 69): `CODE` C8 `DES` C60

- sections (records): HEADER(1)

**Schema B** (record_len 41): `CODE` C32 `DBFNAME` C8

- sections (records): ANIMATE(12)

**Schema C** (record_len 42): `DATENAME` C32 `YEAR` N4 `MONTH` N2 `DAY` N2 `WEEKDAY` N1

- sections (records): SDATE(3)

**Schema D** (record_len 258): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `ITEM_USING` C8 `SPECIAL` C8 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `LOWERFILE` C32 `LOWERNAME` C32 `SCRIPT` C32 `SOUND` C8 `SOUND_TIME` N4

- sections (records): DEFAULT(37), AMAZON1(24), GENERAL1(37), INFANT1(25), AMAZON2(35), ARCHER1(25), JOAN(39), FINFANT(27), CANNON(13), EARCHER(24), HORSE(12), EINFANT(27)

**Schema E** (record_len 98): `MODEL_CODE` C32 `FILENAME` C32 `MODELTYPE` C1 `ANIMGROUP` C32

- sections (records): MODEL(5)

**Schema F** (record_len 49): `RACE_CODE` C8 `NAME` C20 `ADJECTIVE` C20

- sections (records): RACE(1)

**Schema G** (record_len 41): `CODE` C8 `NAME` C32

- sections (records): ITEMCLAS(7)

**Schema H** (record_len 81): `NATIONCODE` C8 `NAME` C64 `RACE` C8

- sections (records): NATION(3)

**Schema I** (record_len 41): `CLASS` C8 `TYPE` C32

- sections (records): ITEMTYPE(10)


### `Resource/Std.set` — 140 sections, 24 distinct schemas

**Schema A** (record_len 69): `CODE` C8 `DES` C60

- sections (records): HEADER(1)

**Schema B** (record_len 41): `CODE` C32 `DBFNAME` C8

- sections (records): ANIMATE(82), UPGRADE(43)

- 2 of these have no 0x1A end marker

**Schema C** (record_len 42): `DATENAME` C32 `YEAR` N4 `MONTH` N2 `DAY` N2 `WEEKDAY` N1

- sections (records): SDATE(3)

**Schema D** (record_len 98): `MODEL_CODE` C32 `FILENAME` C32 `MODELTYPE` C1 `ANIMGROUP` C32

- sections (records): MODEL(5)

**Schema E** (record_len 49): `RACE_CODE` C8 `NAME` C20 `ADJECTIVE` C20

- sections (records): RACE(1)

**Schema F** (record_len 81): `NATIONCODE` C8 `NAME` C64 `RACE` C8

- sections (records): NATION(3)

**Schema G** (record_len 242): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND1` C8 `SOUNDTIME1` N19 `SOUND2` C8 `SOUNDTIME2` N19 `SHOW_CRIT` C1

- 32 sections, 1207 records total: ORLEAN(41), CANNON(12), FLINFCAP(47), ELINFANT(90), ELINFCAP(49), FLINFANT(71), TREB(13), CITI4A(25), CITI4B(25), CITI1(28), CITI2(26), CITI3(25), RICHEMON(41), BOMBARD(12), SWILLIAM(35), BATTER_B(2), CANNON_B(2), FAUC_B(2), TREB_B(2), TALBOT(41), SCALES(41), MATTHEW(38), EMACE(90), EMACEC(49), FMACE(71), FMACEC(47), PHILIP(41), BEDFORD(41), WANDOM(41), JHORSE(20), MLIGHT(90), MLIGHT_C(49)

- 32 of these have no 0x1A end marker

**Schema H** (record_len 212): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND` C8 `SOUND_TIME` N4 `SOUND2` C8 `SOUNDTIME2` N4 `SHOW_CRIT` C1

- sections (records): JOAN(103), LAHIRE(101), ALEXANDE(41), WILLIAMP(41), LUXEM(41)

- 5 of these have no 0x1A end marker

**Schema I** (record_len 38): `DEVICE` C1 `EVENT` C4 `STRING` C32

- sections (records): KEYDESC(115)

**Schema J** (record_len 227): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND` C8 `SOUND_TIME` N4 `SOUND2` C8 `SOUNDTIME2` N19 `SHOW_CRIT` C1

- 23 sections, 1086 records total: EARCHER(43), FARCHER(41), FHINFCAP(47), EHINFCAP(46), EHINFANT(43), FHINFANT(43), JEANMETZ(102), BHINFANT(43), BHINFCAP(46), BARCHER(43), FPOLECAP(44), FPOLE(43), EPOLE(43), EPOLECAP(43), B_INF(75), B_INFCAP(49), FLAVY(36), CHARLIE7(46), FASTOLF(40), PERRINET(41), MPOLE(43), MPOLECAP(43), MARCHER(43)

- 23 of these have no 0x1A end marker

**Schema K** (record_len 212): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND1` C8 `SOUNDTIME1` N4 `SOUND2` C8 `SOUNDTIME2` N4 `SHOW_CRIT` C1

- sections (records): HORSE(20), PIG(25), CROSSBOW(12), HOSHARE(20), JOHNPOLE(40), WEAPON(25), BOMB_B(2), HCROSS_B(2), POTION(25), AUTOB2_B(2), AUTOB_B(2)

- 1 of these have no 0x1A end marker

**Schema L** (record_len 129): `MODEL_NAME` C32 `MORPH_NAME` C32 `MORPH_FILE` C32 `MORPH_ANIM` C32

- sections (records): MORPH(2)

- 1 of these have no 0x1A end marker

**Schema M** (record_len 35): `LEVEL` N2 `CODE` C32

- 42 sections, 4642 records total: JOANUP(22), LAHIREUP(77), DEFUP(1), F_ARCUP(30), F_INFUP(40), E_ARCUP(101), E_INFUP(113), M_POLUP(107), F_HINUP(40), E_HINUPC(145), ALEXUP(138), E_ARCUPC(136), E_HINUP(112), JEANUP(22), E_INFUPC(146), B_HINUPC(145), B_HINUP(113), M_LINUPC(144), E_POLEUP(110), E_POLUPC(137), E_MACEUP(104), E_MACUPC(139), F_CROSUP(97), F_CROUPC(137), B_LINUPC(146), B_ARCUP(100), B_ARCUPC(136), B_LINUP(111), ALENUP(77), M_POLUPC(137), M_LINUP(109), ORLEANUP(147), JOHNUP(139), WILLIAUP(141), BEDUP(127), TALBOTUP(132), WANDOMUP(127), ARTHURUP(145), CHARLEUP(140), FLAVYUP(138), M_ARCUPC(136), M_ARCUP(98)

- 31 of these have no 0x1A end marker

**Schema N** (record_len 16): `LEVEL` N2 `EXPERIENCE` N9 `ATT_POINT` N2 `SKI_POINT` N2

- sections (records): LEVEL(30)

**Schema O** (record_len 156): `SOUND_CODE` C48 `WAVE_FILE` C48 `WF_RANDMAX` N2 `GAIN` N6.3 `MIN_PITCH` N5.2 `MAX_PITCH` N5.2 `MIN_DIST` N8.2 `MAX_DIST` N8.2 `ROLLOFF` N7.2 `SOUNDTIME` N5 `LOOP_FLAG` C1 `SPEED_INDE` C1 `DELAY_BEG` N5 `DELAY_END` N5 `MAIN_CHAR` C1

- sections (records): SOUND(505)

- 1 of these have no 0x1A end marker

**Schema P** (record_len 227): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND1` C8 `SOUNDTIME1` N4 `SOUND2` C8 `SOUNDTIME2` N19 `SHOW_CRIT` C1

- sections (records): FAUC(12), FCROSSB(33), FCROSSBC(33), BATTER(8), AUTOBOW(16), H_CART(21), ECROSSB(33), ECROSSBC(33)

- 8 of these have no 0x1A end marker

**Schema Q** (record_len 212): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` N3 `BLEND_TIME` N4 `START_TIME` N4 `END_TIME` N4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND` C8 `SOUND_TIME` N4 `SOUND2` C8 `SOUNDTIME2` N4 `SHOW_CRIT` C1

- sections (records): ALENCON(101)

- 1 of these have no 0x1A end marker

**Schema R** (record_len 95): `CODE` C16 `FILENAME` C64 `TYPE` C1 `STARTMIN` N4 `ENDMIN` N4 `STARTSCE` N2 `ENDSCE` N2 `E_TYPE` C1

- sections (records): MUSIC(42)

- 1 of these have no 0x1A end marker

**Schema S** (record_len 212): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND1` C8 `SOUNDTIME1` C4 `SOUND2` C8 `SOUNDTIME2` N4 `SHOW_CRIT` C1

- sections (records): SIEGETW(3)

**Schema T** (record_len 200): `ANIM_CODE` C32 `POSITION` C8 `MOVEMENT` C8 `COMBO` C6 `COMBOAVAIL` C1 `CHAINAVAIL` C1 `ITEM_USING` C8 `SPECIAL` C8 `ENERGY_PT` N3 `DAMAGE` C3 `BLEND_TIME` N4 `START_TIME` C4 `END_TIME` C4 `UPPERFILE` C32 `UPPERNAME` C32 `SCRIPT` C32 `SOUND` C8 `SOUND_TIME` N4 `SHOW_CRIT` C1

- sections (records): AUTOBOW2(16)

- 1 of these have no 0x1A end marker

**Schema U** (record_len 47): `KEY_ACTION` C32 `K_KEY` C4 `J_KEY` C4 `J_JOYPAD` C1 `FPS_KEY` C4 `FPS_MOUSE` C1

- sections (records): DEFAULTK(55)

**Schema V** (record_len 107): `TYPE` C16 `CODE` C32 `DESC` C32 `PARA1` C6 `PARA12` C6 `PARA3` C6 `COST` N2 `REQUIRE` C6

- sections (records): UPITEM(72)

**Schema W** (record_len 41): `CODE` C8 `NAME` C32

- sections (records): ITEMCLAS(20)

- 1 of these have no 0x1A end marker

**Schema X** (record_len 41): `CLASS` C8 `TYPE` C32

- sections (records): ITEMTYPE(39)

- 1 of these have no 0x1A end marker


### `Resource/Interfac.set` — 79 sections, 2 distinct schemas

**Schema A** (record_len 66): `BITMAP` C8 `TEXTURE` C32 `SRC_X1` N4 `SRC_Y1` N4 `SRC_X2` N4 `SRC_Y2` N4 `ROTATION` N1 `WIDTH` N4 `HEIGHT` N4

- 78 sections, 1665 records total: MAINGAME(92), CRITI_F(5), CRITI_K(5), CRITICAL(8), INVENT(2), LFLARE(4), MAINMENU(7), INTRO(6), VICTORY3(1), POPUPWIN(15), MENU_JOA(4), BUY(12), COMMELE(59), OPTMENUB(3), OPTMENUF(1), OPTMENU(23), IGAMEMNU(2), MAPICON(48), S2L1W(13), VICTORYE(10), MENU1024(13), MENU800(1), MILIBAL(6), SL_BG2(2), LD800CP(20), LD1024CP(20), LD800BG(1), LD1024BG(1), MENU_B(16), IUNITATT(23), IATTCOMB(17), NAME_BAR(3), MESSAGE(96), OBJMENU(128), SAVEMENU(6), GAMEOVER(4), RTSICON(179), S1L1W(8), S5L1W(53), MGEXP(97), BARR_BG(2), BARRACK(53), S3L1W(13), KILL(6), S4L1W(23), GMSG(2), S6L1W(93), LETTER(3), VICTORY2(1), VICTORY(1), HURT(1), SL_COM(4), M1(3), VICT(3), S7L1W(43), M2(3), SL_BG1(1), M3(3), M4(3), QSAVE(6), MINI_MP(9), S8L1W(128), M8(3), M5(7), M6(3), M7(3), COMMELE2(31), HELPBOX(9), MSGLOG(3), INGAME_B(11), TUI(57), QUIT(2), ARMORY(10), GPOPUP(13), DEBRIEF(40), STRATEGY(23), ENDING(16), CREDITS(16)

- 10 of these have no 0x1A end marker

**Schema B** (record_len 69): `CODE` C8 `DES` C60

- sections (records): HEADER(1)

`Gameplay.res` (6 text sections `ECON THROWI UNIT CHARMOVE WEATHER GAME`) is parsed as a container here and as text by [gameplay.md](gameplay.md).

## Name lookup and section counts

- Name lookup: the original uses a case-sensitive binary search over a separately sorted copy of the index (0x59CA60, CRT `strcmp`); the
  port keeps it (`src/ilib/res_idx_impl.hpp`, comparators 0x59D160 and 0x59C410 are plain `strcmp`). A case-insensitive lookup would be a superset of this behaviour. [res.md](res.md) is the long-name sibling (the same lookup).
- Table counts for the shipped files are in the sections table above; `Interfac.set` has 81 sections: its header count, which the reader uses, is 0x51.
