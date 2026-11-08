> Structure is identified from `docs/re/`; the grammar is not specified here.

# `texteff.res` - texture-effect scripts

`Resource/texteff.res` is a ResLX bank ([res.md](res.md), 51 records, names such as `BIGRED`). Each payload is a column-based text script that
starts with `Version : 1.00` and defines a material or UV animation (scrolling, bump, cube-map; the `NumMat...` family of keys) for a model.

- Loader: `TextEffects::load` 0x004DEB70 in `src/game/t_scrres.cpp` (original `t_scrres.cpp`). It opens `RESOURCE\texteff.res`; `t_scrres.cpp`
  does *not* load scenario `.sce` scripts despite the name. The lexer is shared with `animscr.res` ([animscr.md](animscr.md)).
- Full grammar: [../re/structs/ScenarioScriptRes.md](../re/structs/ScenarioScriptRes.md) section 2 (verified from 0x4DED90) and
  [../re/functions/script_res.md](../re/functions/script_res.md).
- TODO(docs): a per-key table of the texture-effect grammar.
