> Structure is identified from `docs/re/`; the grammar is not specified here.

# `dropitem.res` - drop tables

`item/dropitem.res` (the file the loader opens, `src/game/ditemres.cpp`; `Resource/dropitem.res` is a different file) is a ResLX bank ([res.md](res.md), 9 records: `DEFAULT`, `ARROW`, ...). Each payload is a column-based text list that
starts with `VERSION 1.0`, then `Code NAME`, then weighted item entries, and ends at the token `END`.

- Loader: `ditemres_load` 0x00463AA0 and `ditemres_parse_list` 0x00463CB0 in `src/game/ditemres.cpp` (original `ditemres.cpp`). Each value token is
  split with `strtok` into a weight and optional fields; items are resolved by code through the item table. `ditem_roll` 0x00464030 picks an
  entry by weight, last entry first.
- Functions and in-memory layout: [../re/functions/items_facilities.md](../re/functions/items_facilities.md) section 1.2 and
  [../re/structs/Inventory.md](../re/structs/Inventory.md) section 6.
- TODO(docs): the exact token grammar of an entry (the optional fields after the weight).
