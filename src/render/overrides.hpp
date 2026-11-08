#pragma once
// Per-draw corrections requested by game code (the [Fixes] options). The D3D8 adapter applies
// them on top of the game's render states. Kept free of GL / STL headers so game modules can use it.

/// Native render features of OpenJoan (shadows, ambient occlusion, water, grass batching, pixel lighting, texture building) and the per-draw [Fixes] overrides.
namespace joan::render {

/// Switches game code sets for the draws that follow ([Fixes] options); the adapter reads them per draw.
struct DrawOverrides {
    bool depth_test = false;  ///< depth test even when the game has ZENABLE off (no depth write)
};
/// The process-wide DrawOverrides.
DrawOverrides& overrides();

}  // namespace joan::render
