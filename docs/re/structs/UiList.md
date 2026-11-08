# UiList and ComboBox (interfac\list.cpp, interfac\combobox.cpp)

Widgets used by the options screen and in-game panels. They share the first bytes of [Widget2D](Widget2D.md) (+3/+4 flags, rect +9..+0xF).

## List (init 0x6F80C0, free 0x6F8470, add 0x6F8670)

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x03 / 0x04 | 1 each | u8 | `enabled2`, `visible` | init args | read |
| 0x05 | 4 | i32 | `ready` | 1 after init, 0 after free | read |
| 0x09..0x0F | 4x2 | i16 | rect | | read |
| 0x11 | 4 | Item* | `items` | stride **0x195**: `char text[0x191]`, `i32 user` at +0x191 | verified |
| 0x15 | 4 | i32 | `count` | | verified |
| 0x19 | 4 | i32 | `capacity` | grows by 5 | verified |
| 0x1D / 0x1F / 0x21 / 0x23 | 2 each | i16 | scroll-button anchor rect | | read |
| 0x25 | 2 | i16 | `style` | init arg | read |
| 0x37 | 4 | BmpRes* | `bmpres` | | read |
| 0x44 / 0x48 / 0x4C / 0x50 | 4 each | | `hover?`, `top?`, `user`, `selected` (-1 none) | | inferred |
| 0x54, 0x67, 0x7A, 0x8D | 9 each | char[9] | bitmap names (up/down/over/disabled) for the scroll buttons | | read |
| 0x5D + i*0x13 | | | per-state colour + blend ids | like Button2D state | read |
| 0xA0 | 4 | Font2D* | `font` | | read |
| 0xA4 | 4 | u32 | text colour | | read |
| scroll block | | | `+0x74` rows (char), `+0x75` first, `+0x79` total, `+0x7D` page, `+0x81` top (0x62CD40/0x62CED0, `this` = embedded scroll helper) | | inferred |

## ComboBox (free 0x6B3130, add 0x6B3930, remove 0x6B3B90, select 0x6B44E0)

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x70 | 4 | i32 | `ready` | | read |
| 0x74 | 4 | i32 | `type` | 2 = built-in string array, else uses `list` | verified |
| 0x80 | 4 | i32 | `selected` | 1-based, 0 = none | verified |
| 0x8C / 0x90 / 0x94 / 0x98 | 4 each | Button2D* | caption / arrows / drop button (owned, deleted in free) | | read |
| 0x9C | 4 | char* | `strings` | `(count+1) * 0x191` bytes | verified |
| 0xA0 | 4 | i32 | `count` (type 2) | | verified |
| 0xA5 | 4 | List* | `list` (type != 2) | | read |
