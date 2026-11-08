# How en3d draws a frame

Measured with `[Graphics] DrawProfile=1` (src/gl/draw_profile.cpp) on 2026-10-06:
68,671 frames (menus, missions with fights, cutscenes, town/shop), 25.9 M draws,
8.25 G triangles - about 376 draws and 120 k triangles per frame. Every draw is
attributed to the chain of return addresses in joan.exe (it keeps frame pointers).

## Call tree

```
Game::draw 0x4d48f0
 └ 0x44c800 (world video memory / frame) ──┐
                                           Camera::render 0x614760  ('Lightning')
                                            └ Scene::render 0x5ebac0 (scene.cpp, 7952 B)
                                               ├ SceneS::draw_lists 0x5f07f0 (scene_s.cpp)  ── most objects
                                               │   ├ Actor::draw 0x5aba20 (actor.cpp) ─┐
                                               │   ├ 0x5b1cb0 / 0x5b1180 (units, props) ─┤
                                               │   └ terrain, effects ─────────────────┤
                                               ├ shadows 0x5ee1d0 -> 0x5f1cc0 (scene_s.cpp)   │
                                               ├ sky (vs1) / effects directly                 │
                                               └ 0x459cd0 (2 big meshes per frame)            │
                                                                                              ▼
                                                     Plane::draw 0x5d0570 (plane.cpp, 9520 B)
                                                      ├ DrawIndexedPrimitive directly (0x5d1c8b)
                                                      └ VgaBuf::draw_indexed 0x600a90 -> DrawIndexedPrimitive (0x600adb)
                                                     0x5d2b80 (plane.cpp, 2608 B): second mesh draw path
                                                     VgaBuf::draw 0x600a00 -> DrawPrimitive (0x600a3f): 2D quads
```

**Plane::draw (0x5d0570) issues almost every 3D draw** - static meshes, skinned
characters (fixed-function vertex blending and the vs2/vs3 shaders), terrain, sky,
effects. Replacing it is the core of stage 2.

## Where the triangles go (whole session)

| Share | Chain (innermost first) | Formats | What |
|---|---|---|---|
| 44% | Plane::draw <- Actor::draw 0x5aba20 <- SceneS 0x5f09fd <- Scene 0x5ecc0e | 0x152, 0x156 | characters (skinned FFP + static parts) |
| 26% | Plane::draw <- Actor::draw <- SceneS 0x5f0bab <- Scene 0x5ed09c | 0x142, 0x156, 0x152 | characters, second list (alpha / effects pass?) |
| 9.7% | Plane::draw <- SceneS 0x5f08a7 <- Scene 0x5ecc0e | 0x452 | terrain (4 texture stages) |
| 5.1% + 2.3% + 0.7% | Plane::draw <- 0x5b1cb0 <- (0x5b1180 / SceneS 0x5f0a31) | 0x152 | units / props |
| 3.4% + 0.7% | Plane::draw <- 0x5b1cb0 (0x5b1f65) <- SceneS 0x5f0a31 / 0x5b1180 | vs 5, 7 | skinned characters through vertex shaders vs2/vs3 |
| 2 x 2.3% | Plane::draw <- 0x459cd0 <- Scene 0x5ebce2 | 0x152 | two large meshes, once per frame each |
| 2 x 1.1% | VgaBuf 0x600a90 <- 0x5f1cc0 <- 0x5ee1d0 <- Scene 0x5ece16 | 0x152 | shadow volumes (two stencil passes, same counts) |
| 0.6% | Plane::draw <- Scene 0x5ebc10 | vs 3 | sky / atmosphere (vs1) |
| 0.6% | 0x5d2b80 <- 0x5b1cb0 | 0x152 | second plane.cpp path |
| small | VgaBuf 0x600a90 <- Plane::draw <- SceneS 0x5f08a7 / Scene 0x5ed244, 0x5ebdd1, 0x486d10 | 0x142 | sprites / particles / effects |
| 1 / frame | VgaBuf::draw 0x600a00 <- 0x5ee1d0 <- Scene 0x5ece16 | 0x144 | full-screen shadow darkening quad |
| menus | Plane::draw <- Actor::draw <- Scene 0x5ece9e <- Camera::render <- 0x720bc0 / 0x4d61b5.. | 0x152, vs b | 3D portraits / maps in menus |

## Device calls inside Plane::draw

Direct vtable calls: SetTransform x30 (+0x94), SetRenderState (+0xc8), SetTexture
(+0xf4), SetTextureStageState (+0xfc), SetMaterial (+0xa8), SetVertexShaderConstant
(+0x13c), SetStreamSource (+0x14c), SetIndices (+0x154), DrawIndexedPrimitive
(+0x11c). Most render/texture-stage state goes through en3d's state wrappers around
0x5fe000-0x603000 (0x6002b0, 0x600490, 0x600510, 0x60ef80, ...) - the natural layer
to sit under a native renderer. The device wrapper object is the global 0xa52420
(0x5d2b40 = per-draw statistics).
