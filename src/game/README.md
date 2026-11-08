# src/game

The game proper: the code of joan.exe's root `d:\ww\*.cpp` files and everything not yet attributed to a file.

- `<name>.cpp`: the functions of the original `d:\ww\<name>.cpp` (attribution: `re/attribution.csv`, from the
  `__FILE__` strings of joan.exe and the link order; `re/attribute.py`).
- `unknown_<topic>.cpp`: functions whose original source file is not known. They were the batch files of
  `game/_unplaced/` (split from batch modules by `re/split_modules.py`, named by topic); Phase 3 renamed
  them (2026-11-07) after the evidence-based moves of curation waves 1 and 2. Not every attributed function has moved
  yet: 180 functions here have only a `class` attribution, 4 are certain or enclosed RE2 matches, 86 are `probable` RE2
  matches, and 50 functions elsewhere sit outside their attributed file . Each file's header names the address stretch it came from (the
  known files around it in link order). Functions move to their original file when evidence names it: a
  `__FILE__`-carrying neighbour, an RE2 engine match (`re/re2_engine_matches.csv`), a curated class name. Engine code
  among them (en3d, ilib, uti3d ... guesses) goes to its library directory, not to `game/`.
- Hand-written OpenJoan additions (`[Fixes]` switches and their helpers: `animal_pen`, `unit_stuck`,
  `mission_count`, `pad_feedback`, `sound_slots`, `stream_cache_api` ...) live here beside the code they change.

Every function keeps its registration (name, module key, address); `python3 re/check_registrations.py --dump`
lists where each one is.
