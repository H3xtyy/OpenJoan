// Generated (re/facades/gen2.py): typed members for the Dialogue functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0063b830 Dialogue::draw
void draw(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) { ::joan::call::thiscall<void>(::joan::fn::Dialogue_draw, this, a0, a1, a2, a3); }
/// 0x0063c000 Dialogue::text_y
std::uint32_t text_y() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Dialogue_text_y, this); }
/// 0x0063c170 Dialogue::sdtor_63c170
void* sdtor_63c170(std::uint32_t a0);
/// 0x0063da60 Dialogue::layout_text
std::uint32_t layout_text() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Dialogue_layout_text, this); }
/// 0x0063e0d0 Dialogue::push_copy
std::uint32_t push_copy(void* a0);
/// 0x0063e240 Dialogue::line_count
std::uint32_t line_count();
/// 0x0063e2a0 Dialogue::right
std::int32_t right();
/// 0x0063e2e0 Dialogue::bottom
std::int32_t bottom();
/// 0x0063bf90 Obj::dtor_body_63bf90
std::uint32_t destruct();
/// 0x0063c0a0 Obj::close_63c0a0
void close() { ::joan::call::thiscall<void>(::joan::fn::Obj_close_63c0a0, this); }
/// 0x0063c8a0 DialogueList::draw
void draw_63c8a0(std::uint8_t a0);
/// 0x0063cc80 DialogueList::blink_dot
void blink_dot();
/// 0x0063d120 Speaker::say_63d120
std::uint32_t delete_line(std::uint32_t a0);
/// 0x0063d770 Obj::field_353
std::uint32_t choice_of(std::uint32_t a0);
/// 0x0063e320 Obj::byte0
std::uint8_t is_open();
/// 0x0063d7b0 Obj::all_ready
bool all_ready();
/// 0x0063d860 Dialogue::hit_choice
bool hit_choice(std::uint32_t a0);
