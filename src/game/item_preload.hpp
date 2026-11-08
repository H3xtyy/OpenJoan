// [Game] ItemPreload: the models of every item that can drop (item classes POTION - food, potions, arrows -, GOLD,
// ENHANCER - gems -, CROSS) are built while the first mission loads and kept for the session.
//
// The game builds an item's model (ModelRes::acquire through Item::acquire_anim: model, animations, textures) the
// first time one lies in the world and frees it when the last one is gone, so each drop of an item not in the
// world built it again - a few ms, a small hitch when a box breaks. Here every such item is acquired once more
// (Item::acquire_anim(0), the game's own call for a dropped item) and never released: the game's own acquires and
// releases only move the reference count above that one. The item and model tables are built once at startup
// (Game::load_resources) and live for the whole session, so the reference stays valid across missions.
#pragma once

namespace joan::item_preload {

/// The first mission load (World::load_pln): acquires the drop items' models, once per session.
void once();

}  // namespace joan::item_preload
