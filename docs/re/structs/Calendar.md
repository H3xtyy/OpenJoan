# Calendar — in-game date/time-of-day clock — joan.exe

Single global object at **`0xA47BA8`** (size **0x50**; constructor `0x495290` does `memset(this, 0, 0x50)`). The base is deduced from the script-visible globals:
`get_game_date` returns `DAT_00A47BB0` (= `+0x08`), `get_game_hour/minute` use `DAT_00A47BB4` (= `+0x0C`), `get_game_day/month/year` return `DAT_00A47BB8/BBC/BC0` (= `+0x10/+0x14/+0x18`),
and `set_game_start_date` writes `DAT_00A47BC4/BC8` (= `+0x1C/+0x20`). Created at start-up by `Calendar(2002, 1, 1)` (`game_bringup_and_run` 0x479720) and **re-created by every `set_game_start_date`**.
Functions: [functions/battle.md](../functions/battle.md) §6. Consumers of `+0x0C`: lightsys day schedule ([LightSys](LightSys.md), `DAT_00A47BB4` "game minute").

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `epoch_serial` | day serial number of the start date (`calc_serial(year, month, day)` 0x57F990) | 0x495290 | verified |
| 0x04 | 4 | i32 | `epoch_year` | copy of the start year (`+0x18` at construction) | 0x495290 | verified |
| 0x08 | 4 | i32 | `serial` | current day serial (starts = `epoch_serial`); `+1` per elapsed day. `get_game_date()` returns it; `calculate_date(y,m,d)` (builtin 0x6596F0) is the same `calc_serial` | 0x495290, 0x4953D0, 0x659720 | verified |
| 0x0C | 4 | f32 | `minute_of_day` | game minutes since midnight, `0.0 .. <1440.0`. `get_game_hour()` = `/ DAT_009CF9CC` (60.0, inferred) as float; `get_game_minute()` = `(int)fmod(x, 60.0)` | 0x476BB0, 0x659760, 0x659790 | verified |
| 0x10 | 4 | i32 | `day` | day of month, 1-based, **clamped at 30** (see §2) | 0x4953D0, 0x6597D0 | verified |
| 0x14 | 4 | i32 | `month` | 1..12 | 0x4953D0, 0x6597F0 | verified |
| 0x18 | 4 | i32 | `year` | calendar year (2002 default; S1L1 sets 1429) | 0x4953D0, 0x659810 | verified |
| 0x1C | 4 | f32 | `day_start_minute` | value `minute_of_day` is reset to at midnight; 0.0 at construction; 5th argument of `set_game_start_date`. Also used as "now" origin by the weather roll (`rand(N) + DAT_00A47BC4`, lights.md) | 0x476BB0, 0x659550, 0x45B160 | verified |
| 0x20 | 4 | f32 | `day_length` | 1440.0 (`0x44B40000`) at construction; 6th argument of `set_game_start_date`. Read by lightsys (0x4956A0 `clock_elapsed_since`: adds it when the difference is negative). **Not** used by the midnight test in 0x476BB0, which compares with the literal 1440.0 | 0x495290, 0x659550, lights.md | verified |
| 0x24 | 4 | i32 | `cnt_day30` | `(+1) % 30` every day | 0x4953D0 | read (consumer unknown) |
| 0x28 | 4 | i32 | `cnt_month30` | `(+1) % 30` every month change | 0x4953D0 | read |
| 0x2C | 4 | i32 | `cnt_year30` | `(+1) % 30` every year change | 0x4953D0 | read |
| 0x30 | 4 | i32 | `cnt_year10` | `(+1) % 10` every year change | 0x4953D0 | read |
| 0x34 | 4 | i32 | `cnt_month6` | `(+1) % 6` every month change | 0x4953D0 | read |
| 0x38 | 4 | i32 | `cnt_month12` | `(+1) % 12` every month change | 0x4953D0 | read |
| 0x3C | 4 | u32 | `created_at` | `time(NULL)` at construction (0x7B33BF) | 0x495290 | verified |
| 0x40 | 4 | i32 | zero | cleared at construction | 0x495290 | verified |
| 0x44 | 4 | i32 | `weekday` | `serial % 7` (set on every day roll; **not** set at construction) | 0x4953D0 | verified |
| 0x48 | 4 | i32 | `day_of_year` | `day_of_year(year, month, day)` 0x57FEF0 (1..366, −1 if invalid) | 0x495290, 0x4953D0 | verified |
| 0x4C | 4 | i32 | `year_changes` | `++` at every year change | 0x4953D0 | read |

## 1. Date arithmetic helpers

| function | address | behaviour |
|---|---|---|
| `day_of_year(y, m, d)` | 0x57FEF0 | leap year = `(y%4==0 && y%100!=0) || y%400==0`. Month length = `cum[m+1]-cum[m]` (+1 for February in leap years) from the cumulative-days table `DAT_00A15198` (`cum[1]=0`, `cum[2]=31`, ...). Returns **−1** when `y<0`, `m∉[1,12]`, `d<1` or `d>len`; else `cum[m] + d + (leap && m>=3 ? 1 : 0)` |
| `calc_serial(y, m, d)` | 0x57F990 | `doy = day_of_year(y,m,d)`; `doy<1 → −1`, else `0x57FFF0(y) + doy + 0x1A4451` (0x1A4451 = 1 721 425; with `0x57FFF0(y)` = days before year `y` this is the Julian Day Number, proleptic Gregorian; `0x57FFF0` not exported) |
| `month_of(serial)` / `year_of(serial)` | 0x495810 / 0x4957E0 | thin wrappers over `0x57FA80(serial, 'M' = 0x4D)` / `0x57FA80(serial, 'Y' = 0x59)` (inverse of `calc_serial`, not exported) |

## 2. Per-tick advance (`calendar_update` 0x476BB0, called from `game_update` 0x4E5FC0 inside the world-update chain)

```
if (game_speed (DAT_00A500CC) != 0.0):
    minute_of_day += ((K / 60.0) / 1000.0) * (float) dt_ms        // K = *(float*)0x9D3D74, dt_ms = DAT_00A500BE
    if (minute_of_day >= 1440.0):
        minute_of_day = day_start_minute
        day_roll()
```
* `dt_ms` is `DAT_00A500BE` = the **speed-scaled game millisecond delta** of the frame (`Game+0x36 = (int)(dt × speed)`, TIMING.md), so the clock follows the game speed; the explicit `speed != 0` test only avoids work.
* `K` = **24.0** (float32 at 0x9D3D74, read from the executable); `0x9CF9CC` = 60.0; `0x9CF778` = 1000.0. So `minute_of_day += 0.0004 * dt_game_ms`, i.e. 0.4 game minutes per game second: one day = 3600 game seconds (60 min at speed 1.0). Final.
* No catch-up loop: at most one day roll per tick even if `dt` is huge.

### 2.1 `day_roll` (0x4953D0), in this order
1. `day = min(day + 1, 30)`; `serial += 1`; `weekday = serial % 7`; `cnt_day30 = (cnt_day30 + 1) % 30`.
2. If `month_of(serial) != month`: `day = 1; month = month_of(serial); cnt_month30 = (+1)%30; cnt_month6 = (+1)%6; cnt_month12 = (+1)%12`.
3. If `year_of(serial) != year`: **`month = 1`**; `year = year_of(serial); year_changes++; cnt_year30 = (+1)%30; cnt_year10 = (+1)%10`.
4. `day_of_year = day_of_year(year, month, day)`.
5. Hooks: `0x4B68D0` (for every nation 1..count that exists: `Nation::daily` 0x4B4920; then 0x4B6A40, 0x4B6D40 — nation.cpp, see the world/nation page), `0x45B160` (weather roll, below), `0x4E42E0 → 0x486C40` (daily hook, owner unknown).
6. `if (day == 1)`: `0x4B6960` (every nation: `Nation::monthly` 0x4B49F0); and `if (month == 1)` also `0x4B69D0` (`Nation::yearly` 0x4B4A10).
Quirk: because `day` is clamped at 30 instead of following the month length, `get_game_day()` reads 30 on the 31st of a month; the month still changes at the right calendar day (it is derived from `serial`). In the year-change branch `month` is forced to 1 (it is already the correct month; harmless).

### 2.2 Weather roll `0x45B160` (called once per day roll; `this` = lightsys object)
Two independent rolls with the project RNG `rand(n)` ([functions/battle.md](../functions/battle.md) §11): `if ((float)rand(100) < rain_probability (this+0x58C))` then `this+0x594 = (float) rand(ftol(N)) + day_start_minute (+0x1C)` (start minute of the rain) and `this+0x598 = 0`; the same for snow with `+0x59C / +0x5A4 / +0x5A8`. `N` (the `__ftol` argument) is not visible in the export; lights.md documents durations 400..579 game minutes.

## 3. Script surface (all handlers verified)

| builtin (id) | handler | effect |
|---|---|---|
| `set_game_start_date(year, month, day, minute, lo, hi)` (205) | 0x659550 | `Calendar(year, month, day)` (0x495290) — full re-initialisation; then `+0x1C = (float) lo`, `+0x20 = (float) hi`; then `minute_of_day = clamp((double)minute, lo, hi)` (0x590B00) and `0x4A5630` (lightsys: re-evaluate the day schedule). S1L1 calls `(1429, 4, 29, 720, 0, 1440)` = 29 April 1429, 12:00, day = 0..1440. The 4-argument form used once in the scripts reads two missing arguments as 0 (TODO(re)) |
| `get_game_date` (210) | 0x659720 | `serial` |
| `get_game_hour` (211) | 0x659740→0x659760 | `(float) minute_of_day / 60.0` |
| `get_game_minute` (212) | 0x659790 | `(int) fmod(minute_of_day, 60.0)` |
| `get_game_day/month/year` (213/214/215) | 0x6597D0 / 0x6597F0 / 0x659810 | `day` (1..30), `month`, `year` |
| `calculate_date(y, m, d)` (209) | 0x6596F0 | `calc_serial(y, m, d)` (returns −1 for invalid dates) |

## 4. Open questions
* `TODO(re)`: consumers of the six cyclic counters `+0x24..+0x38` (moon/season-like cycles?) and of `+0x44/+0x48/+0x4C`.
* `TODO(re) 0x4E42E0/0x486C40`: what the third daily hook does.
