// Generated (re/facades/gen3.py): typed members for the Date functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0057f990 julian(y,
std::int32_t julian(std::int32_t a0, std::int32_t a1, std::int32_t a2) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::julian_y_m_d, this, a0, a1, a2); }
/// 0x0057fa80 get_date
std::int32_t get_date(std::int32_t a0, char a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::get_date, this, a0, a1); }
/// 0x0057fc30 date_str
const char* date_str(std::int32_t a0, std::int32_t a1, std::int32_t a2) { return ::joan::call::thiscall<const char*>(::joan::fn::date_str, this, a0, a1, a2); }
/// 0x00580510 time_str(minutes)
const char* time_str(const char* a0, float a1, char a2, char a3) { return ::joan::call::thiscall<const char*>(::joan::fn::time_str_minutes, this, a0, a1, a2, a3); }
/// 0x005803c0 file_time_to_minutes
double file_time_to_minutes(void* a0) { return ::joan::call::thiscall<double>(::joan::fn::file_time_to_minutes, this, a0); }
/// 0x00580350 file_time_to_julian
std::int32_t file_time_to_julian(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::file_time_to_julian, this, a0); }
/// 0x0057fef0 day_year
std::int32_t day_year(std::int32_t a0, std::int32_t a1, std::int32_t a2) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::day_year, this, a0, a1, a2); }
/// 0x0057fc00 day_of_week
std::int32_t day_of_week(std::int32_t a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::day_of_week, this, a0); }
