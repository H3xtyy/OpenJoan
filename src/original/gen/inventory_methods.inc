// Generated (re/facades/gen2.py): typed members for the Inventory functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00457ba0 Inventory::add
std::int32_t add_457ba0(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8);
/// 0x00458260 Inventory::find
std::int32_t find(std::int32_t a0);
/// 0x00458390 Inventory::count
std::int32_t count(std::int32_t a0);
/// 0x00495f40 Inventory::add
std::int32_t add_495f40(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7);
/// 0x006d6680 Inventory::load_sprite
void load_sprite();
/// 0x006d6740 Inventory::draw_items_6d6740
void draw_items_6d6740(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::Inventory_draw_items_6d6740, this, a0); }
/// 0x006d8ac0 Inventory::update_6d8ac0
std::uint8_t update_6d8ac0(std::uint32_t a0);
/// 0x006ddf10 Inventory::draw_item_6ddf10
std::uint32_t draw_item_6ddf10(std::uint32_t a0, std::uint32_t a1, const char* a2) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Inventory_draw_item_6ddf10, this, a0, a1, a2); }
/// 0x006de4d0 Inventory::item_sound
std::uint32_t item_sound(std::uint32_t a0);
/// 0x006f6280 Inventory::draw
void draw() { ::joan::call::thiscall<void>(::joan::fn::Inventory_draw, this); }
/// 0x00430340 Obj::get_6ae
std::uint8_t is_open();
/// 0x00457220 Obj::~Obj_457220
void destruct();
/// 0x006d6650 X::reset_6d6650
void release();
/// 0x006d66f0 Res1::free_6d66f0
void free_sprite();
/// 0x006de1d0 X::not_byte8
std::int32_t is_closed();
/// 0x006dea60 Obj::sword_item_6dea60
std::uint16_t sword_item(std::uint32_t a0, std::uint32_t a1, void* a2);
/// 0x004577a0 Inventory::add_4577a0
std::uintptr_t add_stack(std::int32_t a0, std::int32_t a1, void* a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6) { return ::joan::call::thiscall<std::uintptr_t>(::joan::fn::Inventory_add_4577a0, this, a0, a1, a2, a3, a4, a5, a6); }
/// 0x006de590 Map::move_to_town_6de590
bool move_to_town(std::int32_t a0, std::uint32_t a1);
