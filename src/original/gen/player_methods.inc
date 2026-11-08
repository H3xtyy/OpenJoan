// Generated (re/facades/gen2.py): typed members for the Player functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0042b6e0 Player::switch_unit
std::uint32_t switch_unit(std::int32_t a0);
/// 0x0042e1a0 Player::on_key_42e1a0
std::uint32_t on_key_42e1a0();
/// 0x0042f9c0 Player::set_unit
std::uint32_t set_unit(std::int32_t a0);
/// 0x00432300 Player::select_hero
void select_hero(std::int32_t a0);
/// 0x004343e0 Player::handle_input_4343e0
std::uint16_t handle_input_4343e0();
/// 0x00436190 Player::set_follow
std::uintptr_t set_follow(std::int32_t a0);
/// 0x004384e0 Player::update_camera
std::uint32_t update_camera();
/// 0x00440b30 Player::handle_input
std::int16_t handle_input();
/// 0x004720d0 Player::flag_bit
std::uint32_t flag_bit_4720d0(std::int32_t a0);
/// 0x00472150 Player::set_flag_bit
void set_flag_bit(std::int32_t a0, std::uint32_t a1);
/// 0x00472220 Player::flag_bit
std::uint32_t flag_bit_472220(std::int32_t a0);
/// 0x00472370 Player::flag_bit
std::uint8_t flag_bit_472370(std::int32_t a0);
/// 0x004a8f20 Player::score
float score(std::int32_t a0);
/// 0x004b42e0 Player::relation
void* relation(std::int32_t a0);
/// 0x004b4610 Player::set_relation
void* set_relation(std::int32_t a0, char a1);
/// 0x004b4790 Player::give
std::uint32_t give(std::uint32_t a0, std::int32_t a1);
/// 0x004b4ce0 Player::add_stat
void add_stat(std::int32_t a0, float a1, std::int32_t a2);
/// 0x004b5420 Player::add_mood
void add_mood(float a0);
/// 0x004b8710 Player::score_curve
double score_curve(std::uint32_t a0);
/// 0x0050a9e0 Player::set_unit_state
void set_unit_state(std::int16_t a0, std::int8_t a1);
/// 0x00687180 Player::~Player
void dtor();
/// 0x006b12a0 Player::start_move
void start_move(void* a0, std::int32_t a1, std::uint32_t a2);
/// 0x004203c0 get_dword_0
std::uint32_t unit_id();
/// 0x0042a6c0 UnitRef::reset_42a6c0
void reset_unit();
/// 0x0042a770 UnitCmd::reset_42a770
void reset_command();
/// 0x0042a810 Mission::cmd_42a810
std::uint32_t command(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x0042a8a0 ArcherIcon::update
void archer_icon_update();
/// 0x0042c7c0 UnitRef::set_word
void set_target(std::int32_t a0);
/// 0x0042c830 Follow::update_camera
void follow_camera();
/// 0x0042cb20 Aim::update
void aim_update();
/// 0x0042d180 Obj::call_5ef400
void scene_flag_off();
/// 0x0042d1d0 UnitCmd::reset_42d1d0
void reset_squad_command();
/// 0x0042d270 Obj::act_42d270
std::uint32_t squad_act(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5);
/// 0x0042d2e0 empty_0042d2e0
void nop_42d2e0();
/// 0x0042d560 Obj::zero_42d560
std::uint32_t check_unit(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x0042d5c0 Obj::act_42d5c0
std::uint32_t squad_order(float a0, std::int32_t a1, std::int32_t a2);
/// 0x0042d810 Obj::reset_42d810
void* construct();
/// 0x0042d900 Obj::free_18
void release();
/// 0x0042f700 Mission::tick_42f700
void tick();
/// 0x0042fdd0 Obj::unit_value_42fdd0
float unit_progress();
/// 0x004300e0 Obj::byte_81
std::uint8_t byte_81();
/// 0x00430440 Obj::nop_430440
void enter_mode3();
/// 0x00430460 Obj430460::nop
void leave_mode3();
/// 0x00430480 PlayerCtl::update_430480
std::int16_t handle_input_mode3();
/// 0x00431520 Obj::dtor_431520
std::uint32_t tick_mode3();
/// 0x00431550 empty_00431550
void draw_mode3();
/// 0x00431570 direction_angle
std::int32_t direction_angle(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x00431680 anim_code
std::int32_t anim_code(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x00431790 PlayerControl::mouse_look
void mouse_look(std::int32_t a0) { ::joan::call::thiscall<void>(::joan::fn::PlayerControl_mouse_look, this, a0); }
/// 0x004322d0 Obj::get_e1
float look_sensitivity() { return ::joan::call::thiscall<float>(::joan::fn::Obj_get_e1, this); }
/// 0x00432d10 Obj432d10::nop
void leave_mode1();
/// 0x004338c0 Obj::dtor_4338c0
std::uint32_t tick_mode1();
/// 0x004338f0 Obj4338f0::update
void draw_mode1();
/// 0x00433f40 Pair::units_close
std::int16_t target_close();
/// 0x00434080 Face::update
void face_target();
/// 0x004343c0 Obj4343c0::nop
void leave_mode0();
/// 0x00434f20 empty_00434f20
void draw_mode0();
/// 0x00434f40 Obj::dtor_434f40
std::uint32_t tick_mode0();
/// 0x004350f0 UnitView::order_icon
std::int32_t order_icon();
/// 0x00436380 Obj::stub_zero
std::int16_t stub_zero(std::uint32_t a0);
/// 0x004367c0 UnitCtl::order_update_4367c0
std::uint32_t order_update(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x00437ed0 Mission::unit_order_437ed0
std::uint32_t unit_order(std::int32_t a0, std::int32_t a1);
/// 0x00438ad0 Obj::reset_db
void enter_mode2();
/// 0x00439090 Obj::update_439090
std::uint32_t tick_mode2();
/// 0x004390f0 empty_004390f0
void draw_mode2();
/// 0x00439110 Picker::pick_439110
void* pick_object(std::uint32_t a0, std::uint32_t a1, void* a2);
/// 0x0043a3b0 Obj::pick_ray
std::int16_t pick_ray(std::int32_t a0, std::int32_t a1, float a2, float a3, float a4, void* a5);
/// 0x0043a650 Obj::try_set_43a650
std::uint32_t goto_object(std::uint32_t a0);
/// 0x0043a720 Obj::zero16_43a720
std::uint32_t stub_zero_43a720(std::int32_t a0);
/// 0x0043a740 Obj::try_1e_43a740
std::int16_t try_kind_1e(std::uint32_t a0);
/// 0x0043a9e0 Obj::goto_target_43a9e0
std::int16_t goto_target(std::int32_t a0, std::int32_t a1);
/// 0x0043abd0 Mover::goto_object
std::uint32_t goto_object_43abd0(std::int32_t a0);
/// 0x0043ac60 Obj43ac60::reset
void* reset_goto();
/// 0x0043c210 Pair::spawn_and_reset
std::int32_t link_nearest_enemy();
/// 0x0043c480 Obj::store_heading
void store_heading();
/// 0x0043c500 UnitCmd::reset_43c500
void reset_mount();
/// 0x0043c5c0 Obj::dispatch_43c5c0
std::int16_t dispatch_keys(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6);
/// 0x0043c7b0 empty_0043c7b0
void draw_mounted();
/// 0x0043c7d0 Pair::try_attack
std::int16_t try_mount(std::int32_t a0);
/// 0x0043c920 Mount::try_dismount
std::int16_t try_dismount(std::int32_t a0);
/// 0x0043dab0 UnitCtl::steer_update
void steer_update(std::uint32_t a0, std::int32_t a1, float a2, std::int32_t a3);
/// 0x0043ef30 Pair::heading_changed
std::int16_t heading_changed();
/// 0x0043f940 Obj::reset_43f940
std::uint32_t init_rts();
/// 0x0043fc90 Obj::rts_init_43fc90
std::uint32_t enter_rts();
/// 0x00447740 Obj::turn_447740
void rts_turn(std::uint32_t a0);
/// 0x00448340 Pick::object_448340
void* rts_pick_object(std::uint32_t a0, std::uint32_t a1, void* a2);
/// 0x004489c0 Viewer::cull_visible_4489c0
std::uint8_t cull_visible(std::int32_t a0);
/// 0x0044a430 PickCursor::use_44a430
std::uint8_t rts_pick_use();
/// 0x0044f750 Leader::assign_troops_44f750
std::uint8_t assign_troops(std::int32_t a0, std::int32_t a1);
/// 0x00452320 map_quad
std::uint8_t map_quad(void* a0, void* a1);
/// 0x00452830 Pick::ray_query_452830
std::uint8_t ray_query(std::uint32_t a0, std::uint32_t a1, float a2, float a3, float a4, void* a5);
/// 0x00453590 Groups::contains
std::uint8_t group_contains(std::int32_t a0);
/// 0x00485990 X::reset_state
void reset_state();
/// 0x004e4430 X::flag_1365_1366
std::int32_t has_flag_1365();
/// 0x00503fb0 Obj::set_9d
void* set_9d();
/// 0x00503fe0 Obj::set_a8
void* set_a8(std::uint32_t a0);
/// 0x0051c4b0 set_byte_c4
void* set_byte_c4(std::uint32_t a0);
/// 0x006d37c0 Obj::byte_ac
std::uint8_t flag_ac();
/// 0x006f5ed0 Obj6f::get_1238
std::int32_t unit_1238();
/// 0x00438110 Unit::order_pair_438110
bool order_direction(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x00430170 Obj::true_430170
bool mode3_switch_done();
/// 0x00430150 Obj::true_430150
bool mode2_switch_done();
/// 0x00430130 Obj::true_430130
bool mode1_switch_done();
/// 0x00430110 Obj::true_430110
bool true_430110();
/// 0x00452720 Obj::spawn_effect_200
bool spawn_effect_200(std::int32_t a0);
/// 0x00509d40 Obj::byte_9d
std::int16_t byte_9d();
/// 0x0044ac60 Cursor::pick_44ac60
bool rts_pick(void* a0, void* a1, void* a2);
/// 0x00453930 Cam::corners_on_map_453930
template <class T0>
std::uint8_t corners_on_map(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::Cam_corners_on_map_453930, this, a0); }
/// 0x0043a580 Order::move_to_43a580
template <class T0>
std::int16_t move_to(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::int16_t>(::joan::fn::Order_move_to_43a580, this, a0); }
/// 0x0044cfb0 RtsGroup::move_formation_44cfb0
template <class T0>
std::uint8_t move_formation(T0 a0, std::int32_t a1) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::RtsGroup_move_formation_44cfb0, this, a0, a1); }
/// 0x00452610 Zones::find_containing
template <class T0>
std::int32_t find_place_at(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Zones_find_containing, this, a0); }
/// 0x00453da0 Unit::name_tag
const char* name_tag(std::uint32_t a0);
/// 0x0042fe30 heading_to_degrees
float heading_to_degrees(void* a0, float a1);
