// The game's sound slots (the sound list at 0xa5cf98, Panels::slots): every effect, voice and ambient sound plays in
// one of them. The original has 12 (Panels::slots[12], a DirectSound-era limit); a sound finds no free slot when all
// 12 are busy and is not played, and an ambient sound trigger then releases the farthest slot (0x00493340), cutting
// the sound in it short. Rain's ambient loops keep several busy, so in rain sounds went missing or stopped early.
//
// [Fixes] SoundSlots=1 (always on in the release build) gives the game kFixed slots: slots 13.. live in an array of
// ours next to the game's 12 (SoundList::entry 0x004cf690 hands them out by index as before) and every loop over the
// slots runs to count(). OpenAL Soft has 256 sources, so the extra slots cost nothing audible.
#pragma once

#include <cstdint>

namespace joan::sound_slots {

inline constexpr int kOriginal = 12;  // Panels::slots
inline constexpr int kFixed = 32;     // with [Fixes] SoundSlots

/// The slots the game uses: kOriginal, or kFixed with [Fixes] SoundSlots.
int count();
/// Slot `i` (1-based) of the sound list `list`: the game's own for 1..12, ours above.
void* entry(void* list, std::int32_t i);

}  // namespace joan::sound_slots
