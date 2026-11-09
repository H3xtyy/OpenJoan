# Windows x64 and Linux x86-64 (32-bit x86 only as the Linux objcmp reference, tools/objcmp-x86.sh). Produces
# openjoan.exe / openjoan: the reimplementation (openjoan_game) plus joan.exe's data image as our own object. Copy it
# and openjoan.ini to the game folder.
# (The dinput8.dll proxy for the original joan.exe was retired at stage S7b: the original code would use the data at
# its fixed address, ours uses the relocatable joan_image. Last state with it: git history before S7b-main.)
# Static CRT: nothing may depend on a VC++ redistributable being installed.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# OpenAL Soft for the game's OpenAL calls (lib/openal/openal_soft.cpp): target OpenAL::OpenAL, soft_oal.dll.
include(${PROJECT_SOURCE_DIR}/cmake/openal_soft.cmake)
# FFmpeg for the movies and music (media/): target joan_ffmpeg, JOAN_FFMPEG_DLLS.
include(${PROJECT_SOURCE_DIR}/cmake/ffmpeg.cmake)
# SDL3 for the platform layer (platform/: the game window and its messages): target SDL3::SDL3, SDL3.dll.
include(${PROJECT_SOURCE_DIR}/cmake/sdl3.cmake)
# FreeType for the CJK glyphs of the double-byte editions (en2d/cjk_glyphs.cpp, ADR-0004): target freetype.
include(${PROJECT_SOURCE_DIR}/cmake/freetype.cmake)
add_library(openjoan_game OBJECT
    runtime/hitch.cpp
    runtime/host.cpp
    runtime/patch.cpp
    runtime/registry.cpp
    runtime/code_table.cpp
    runtime/save_audit.cpp
    save/io.cpp
    save/io_file.cpp
    ilib/file.cpp
    ilib/database.cpp
    ilib/filetxt.cpp
    ilib/filemem.cpp
    ilib/resx.cpp
    ilib/reslx.cpp
    ilib/dynarr.cpp
    ilib/gameset.cpp
    ilib/misc.cpp
    ilib/misc_fp.cpp
    ilib/misc_more.cpp
    ilib/mem.cpp
    ilib/blob.cpp
    ilib/date.cpp
    lib/d3dx8/texture_from_file.cpp
    lib/d3dx8/math_matrix.cpp
    lib/d3dx8/math_geometry.cpp
    lib/d3dx8/helpers.cpp
    lib/openal/alut.cpp
    lib/openal/openal_soft.cpp
    media/media_file.cpp
    media/wm_reader.cpp
    runtime/import_census.cpp
    platform/files.cpp
    platform/ini.cpp
    platform/memory.cpp
    platform/process.cpp
    platform/sdl_input.cpp
    platform/system.cpp
    platform/text.cpp
    platform/dbcs.cpp
    platform/sdl_time.cpp
    platform/sdl_window.cpp
    lib/crt/string.cpp
    lib/crt/mem_intrinsics.cpp
    lib/crt/search_rounding.cpp
    lib/crt/string_format.cpp
    lib/crt/wide_string.cpp
    lib/crt/wide_string_compare.cpp
    lib/crt/xtoa_misc.cpp
    lib/crt/wcslwr_fcvt_notes.cpp
    lib/crt/startup.cpp
    lib/crt/exit.cpp
    lib/crt/heap.cpp
    lib/crt/mt.cpp
    lib/crt/locale.cpp
    lib/crt/ctype.cpp
    lib/crt/mbstring.cpp
    lib/crt/printf.cpp
    lib/crt/scanf.cpp
    lib/crt/lowio.cpp
    lib/crt/stdio.cpp
    lib/crt/time.cpp
    lib/crt/x87_math.cpp
    lib/d3dx8/texture_api.cpp
    lib/d3dx8/objects.cpp
    lib/jpeg/compress_api.cpp
    lib/jpeg/compress_core.cpp
    lib/jpeg/compress_entropy.cpp
    lib/jpeg/jpeg_crt.cpp
    lib/imports/import_stubs.cpp
    lib/crt/eh_support.cpp
    lib/crt/seh_unwind.cpp
    lib/d3dx8/texture_in_memory.cpp
    runtime/crash.cpp
    runtime/profiler.cpp
    display.cpp
    fixes.cpp
    canvas_hooks.cpp
    debug_menu.cpp
    freecam.cpp
    d3d8/direct3d_create8.cpp
    gl/glapi.cpp
    gl/context.cpp
    gl/shaders.cpp
    gl/resources.cpp
    gl/direct3d.cpp
    gl/adapter.cpp
    gl/device.cpp
    render/renderer.cpp
    render/ambient_occlusion.cpp
    render/shadow_map.cpp
    render/baked_shadows.cpp
    render/shadow_volumes.cpp
    render/grass_batch.cpp
    render/water.cpp
    render/pixel_lighting.cpp
    render/nvlink.cpp
    render/nvlink_com.cpp
    render/nvlink_log.cpp
    render/image.cpp
    render/v3d.cpp
    lib/v3d/v3d87.cpp
    # The loader's import override table (modules register entries, runtime/loader/joan_re.cpp consults it).
    runtime/loader/import_overrides.cpp
    # Record / replay with per-frame state hashes ([Debug] Record / Replay, runtime/replay/replay.hpp): the recorder wraps
    # the import slots of the data image, which hold the platform layer's functions on every platform (Phase 1).
    runtime/replay/replay.cpp
    runtime/replay/imports.cpp
    runtime/replay/com.cpp
    runtime/replay/sys.cpp
    runtime/det_heap.cpp
    runtime/guard_heap.cpp
    runtime/replay/stream.cpp
    runtime/replay/typed_hash.cpp
    # compile-time check of the typed hash table against the data image's layout (no code)
    runtime/replay/hash_types_check.cpp
    # original source files (re/split_modules.py)
    elib/jpeg.cpp
    en2d/bmpres.cpp
    en2d/font2d.cpp
    en2d/cjk_glyphs.cpp
    en2d/image2d.cpp
    en2d/popupwin.cpp
    en2d/txtbox2d.cpp
    en2d/widget2d.cpp
    en2d/icon3d.cpp
    en3d/actor.cpp
    en3d/actordef.cpp
    en3d/actordeg.cpp
    en3d/anim_cache.cpp
    en3d/animdef.cpp
    en3d/bonetree.cpp
    en3d/morph.cpp
    en3d/plane.cpp
    en3d/plane_sh.cpp
    en3d/pshader.cpp
    en3d/reorder.cpp
    en3d/scene.cpp
    en3d/scene_s.cpp
    en3d/skin.cpp
    en3d/sysen.cpp
    en3d/texdir.cpp
    en3d/texturea.cpp
    en3d/vgabuf.cpp
    en3d/vgalock.cpp
    en3d/genamt.cpp
    en3d/geodef.cpp
    en3d/light.cpp
    en3d/portal.cpp
    en3d/rgb.cpp
    en3d/skybox.cpp
    en3d/sys3d.cpp
    en3d/vertex.cpp
    en3d/vga.cpp
    en3d/view3d.cpp
    en3d/vshader2.cpp
    game/unknown_actorlst.cpp
    game/unknown_ai_behav.cpp
    game/unknown_am2check.cpp
    game/unknown_angle8.cpp
    game/unknown_animscr_fx.cpp
    game/unknown_animscr_tex.cpp
    game/unknown_antiport.cpp
    game/unknown_armory.cpp
    game/unknown_battle_pre.cpp
    game/unknown_billboard.cpp
    game/unknown_blit16.cpp
    game/unknown_blitfx.cpp
    game/unknown_boids.cpp
    game/unknown_button2d.cpp
    game/unknown_calendar.cpp
    game/unknown_camcmd.cpp
    game/unknown_camctl.cpp
    game/unknown_camera3d.cpp
    game/unknown_camzone.cpp
    game/unknown_chatbox.cpp
    game/unknown_cheats.cpp
    game/unknown_clip.cpp
    game/unknown_clouds.cpp
    game/unknown_colgrid.cpp
    game/unknown_collide.cpp
    game/unknown_colour.cpp
    game/unknown_config.cpp
    game/unknown_crc8.cpp
    game/unknown_credits.cpp
    game/unknown_crithud.cpp
    game/unknown_ctl2d.cpp
    game/unknown_ctrl.cpp
    game/unknown_d3dfont.cpp
    game/unknown_d3dsys.cpp
    game/unknown_d3dutil.cpp
    game/unknown_descbox.cpp
    game/unknown_diplo_text.cpp
    game/unknown_dir.cpp
    game/unknown_dispdlg.cpp
    game/unknown_dlgtable.cpp
    game/unknown_dlgtext.cpp
    game/unknown_dxmusic.cpp
    game/unknown_errhand.cpp
    game/unknown_escomp.cpp
    game/unknown_escomp_hd.cpp
    game/unknown_esfuncs.cpp
    game/unknown_eslexer.cpp
    game/unknown_f_barr.cpp
    game/unknown_f_flag.cpp
    game/unknown_f_work.cpp
    game/unknown_facility.cpp
    game/unknown_fadetitl.cpp
    game/unknown_flock_x1.cpp
    game/unknown_fog.cpp
    game/unknown_formmove.cpp
    game/unknown_fx_registry.cpp
    game/unknown_fx_spawn.cpp
    game/unknown_gadget.cpp
    game/unknown_game_tick.cpp
    game/unknown_gamedraw.cpp
    game/unknown_gamefile_tail.cpp
    game/unknown_gamemode.cpp
    game/unknown_gameover.cpp
    game/unknown_globals.cpp
    game/unknown_glowmark.cpp
    game/unknown_hud.cpp
    game/unknown_i_attcmb.cpp
    game/unknown_i_endscr.cpp
    game/unknown_i_igframe.cpp
    game/unknown_i_igmenu.cpp
    game/unknown_i_msglog.cpp
    game/unknown_i_obj02.cpp
    game/unknown_i_obj03.cpp
    game/unknown_i_obj0b.cpp
    game/unknown_i_obj0d.cpp
    game/unknown_i_obj0e.cpp
    game/unknown_i_obj12.cpp
    game/unknown_i_qsave.cpp
    game/unknown_i_quit.cpp
    game/unknown_i_roster.cpp
    game/unknown_i_unitatt.cpp
    game/unknown_i_vict.cpp
    game/unknown_inventory.cpp
    game/unknown_itemres_head.cpp
    game/unknown_mainmenu.cpp
    game/unknown_mainwnd.cpp
    game/unknown_mathlib.cpp
    game/unknown_mbtext.cpp
    game/unknown_mdl.cpp
    game/unknown_message.cpp
    game/unknown_mglist.cpp
    game/unknown_lod_queue.cpp
    game/unknown_mouse_cursor.cpp
    game/unknown_unit_effects.cpp
    game/unknown_unit_type_stats.cpp
    game/unknown_game_run.cpp
    game/unknown_unit_glow.cpp
    game/unknown_target_check.cpp
    game/unknown_unit_idle.cpp
    game/unknown_struct7.cpp
    game/unknown_resource_callback.cpp
    game/unknown_cam_angles.cpp
    game/unknown_save_preview.cpp
    game/unknown_game_input.cpp
    game/unknown_cycler.cpp
    game/unknown_mission.cpp
    game/unknown_mlfader.cpp
    game/unknown_mlight.cpp
    game/unknown_mode.cpp
    game/unknown_modelres_pre.cpp
    game/unknown_msgbox.cpp
    game/unknown_music.cpp
    game/unknown_musicstr.cpp
    game/unknown_natres_names.cpp
    game/unknown_news.cpp
    game/unknown_objm_army.cpp
    game/unknown_objm_cannon.cpp
    game/unknown_objm_debrief.cpp
    game/unknown_objm_map.cpp
    game/unknown_objm_reward.cpp
    game/unknown_objm_shop.cpp
    game/unknown_objm_strat.cpp
    game/unknown_objscr_next.cpp
    game/unknown_objscr_start.cpp
    game/unknown_partslot.cpp
    game/unknown_pathmesh.cpp
    game/unknown_pathzone.cpp
    game/unknown_pemitter.cpp
    game/unknown_pgauge.cpp
    game/unknown_pixfmt.cpp
    game/unknown_places.cpp
    game/unknown_playerctl.cpp
    game/unknown_portal.cpp
    game/unknown_projshad.cpp
    game/unknown_psystem.cpp
    game/unknown_ptoken.cpp
    game/unknown_quat.cpp
    game/unknown_rle.cpp
    game/unknown_rts.cpp
    game/unknown_rts2.cpp
    game/unknown_rworld.cpp
    game/unknown_samplew.cpp
    game/unknown_savemenu.cpp
    game/unknown_sceload.cpp
    game/unknown_scrlbar.cpp
    game/unknown_scrlbox.cpp
    game/unknown_scroll2d.cpp
    game/unknown_shot.cpp
    game/unknown_skybox.cpp
    game/unknown_sndplay.cpp
    game/unknown_soundres.cpp
    game/unknown_sprite2d.cpp
    game/unknown_squadcmd.cpp
    game/unknown_strclass.cpp
    game/unknown_strpool.cpp
    game/unknown_surface.cpp
    game/unknown_syswnd.cpp
    game/unknown_t_basic.cpp
    game/unknown_t_interf.cpp
    game/unknown_talk.cpp
    game/unknown_tfanim.cpp
    game/unknown_timer.cpp
    game/unknown_tokenize.cpp
    game/unknown_top4list.cpp
    game/unknown_triggers.cpp
    game/unknown_turn.cpp
    game/unknown_uinput.cpp
    game/unknown_unit_a.cpp
    game/unknown_unit_addon.cpp
    game/unknown_unit_c.cpp
    game/unknown_unit_core.cpp
    game/unknown_unit_set.cpp
    game/unknown_unit_stats.cpp
    game/unknown_unit_xp.cpp
    game/unknown_unitdata.cpp
    game/unknown_unitdraw.cpp
    game/unknown_units_mgr.cpp
    game/unknown_units_sel.cpp
    game/unknown_unittask.cpp
    game/unknown_unitupd.cpp
    game/unknown_ur_orders.cpp
    game/unknown_us_tactics.cpp
    game/unknown_uscrres_pre.cpp
    game/unknown_vgalock_mb.cpp
    game/unknown_vidcap_tail.cpp
    game/unknown_volpan.cpp
    game/unknown_vshader.cpp
    game/unknown_vtrail.cpp
    game/unknown_vtxconv.cpp
    game/unknown_water.cpp
    game/unknown_wavelist.cpp
    game/unknown_weather.cpp
    game/unknown_weffects.cpp
    game/unknown_widglist.cpp
    game/unknown_worldarr.cpp
    game/unknown_zones.cpp
    game/animgrp.cpp
    game/animres.cpp
    game/animscr.cpp
    game/battle.cpp
    game/camman.cpp
    game/camres.cpp
    game/cpuusage.cpp
    game/ditemres.cpp
    game/dyngrass.cpp
    game/end_con.cpp
    game/f_shop.cpp
    game/flocksys.cpp
    game/game.cpp
    game/gamefil2.cpp
    game/gamefila.cpp
    game/gamefile.cpp
    game/gameplay.cpp
    game/gfile3.cpp
    game/grassres.cpp
    game/help.cpp
    game/i_gate.cpp
    game/i_wall.cpp
    game/item_preload.cpp
    game/itemres.cpp
    game/levelres.cpp
    game/lightsys.cpp
    game/modelres.cpp
    game/msglog.cpp
    game/musicres.cpp
    game/nation.cpp
    game/nation2.cpp
    game/natres.cpp
    game/pemires.cpp
    game/plasma.cpp
    game/profile.cpp
    game/raceres.cpp
    game/recrures.cpp
    game/remapkey.cpp
    game/scenario.cpp
    game/sdateres.cpp
    game/sitemres.cpp
    game/speech_s.cpp
    game/t_scrres.cpp
    game/trackres.cpp
    game/unit.cpp
    game/unitlvl.cpp
    game/unitres.cpp
    game/unitures.cpp
    game/upgrares.cpp
    game/uscrres.cpp
    game/world.cpp
    game/world_s.cpp
    ilib/audio.cpp
    ilib/bitmap.cpp
    ilib/bitmapd.cpp
    ilib/bitmapm.cpp
    ilib/bitmapt.cpp
    ilib/bitmapw.cpp
    ilib/db.cpp
    ilib/dynarrb.cpp
    ilib/flc.cpp
    ilib/mousecr.cpp
    ilib/resdb.cpp
    ilib/se.cpp
    ilib/sound3d.cpp
    ilib/sound_archives.cpp
    ilib/txtres.cpp
    ilib/vqueue.cpp
    ilib/angle.cpp
    ilib/error.cpp
    interfac/combobox.cpp
    interfac/i_barr.cpp
    interfac/i_buy.cpp
    interfac/i_letter.cpp
    interfac/i_sms.cpp
    interfac/i_work.cpp
    interfac/list.cpp
    interfac/minires.cpp
    interfac/objmenu.cpp
    interfac/objmenub.cpp
    interfac/objmenuw.cpp
    interfac/optmenui.cpp
    interfac/graphics_page.cpp
    interfac/mouse_options.cpp
    interfac/remap_set.cpp
    game/pad_feedback.cpp
    game/unit_steer.cpp
    game/unit_stuck.cpp
    game/animal_pen.cpp
    game/mission_count.cpp
    uti3d/blkarrb.cpp
    uti3d/dlgres.cpp
    uti3d/esstate2.cpp
    uti3d/lflare.cpp
    uti3d/pathsp.cpp
    uti3d/spindex.cpp
    uti3d/video.cpp
    uti3d/videocap.cpp
    uti3d/water.cpp
    enparticle/particle.cpp
    enparticle/peffect.cpp
    enparticle/peffecta.cpp
    enparticle/pemitter.cpp
    enparticle/petoken.cpp
    enparticle/pevent.cpp
    enparticle/peventsq.cpp
    enpath/navgame.cpp
    enpath/navpool.cpp
    enpath/navsearch.cpp
    enpath/navsteer.cpp
    enpath/pathsp.cpp
    enpath/pathspr.cpp
    enphysic/collimgr.cpp
    enphysic/collis_b.cpp
    enphysic/collis_c.cpp
    enphysic/collis_i.cpp
    enphysic/collis_o.cpp
    enphysic/collis_s.cpp
    enphysic/collisio.cpp
    enphysic/physmath.cpp
    enphysic/physys.cpp
    enphysic/rbody.cpp
)
target_compile_definitions(openjoan_game PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)
# The player build (build_flavor.hpp): only the settings of openjoan_release.ini are read, the debug tools, logs and
# checks compile away. Presets linux-x64-release, vs2026-hook-x64-release; the debug builds leave it OFF.
option(JOAN_RELEASE "Release (player) build of openjoan.exe: debug settings and tools compiled out" OFF)
if(JOAN_RELEASE)
    target_compile_definitions(openjoan_game PUBLIC JOAN_RELEASE=1)
endif()
target_include_directories(openjoan_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(openjoan_game PUBLIC joan_warnings OpenAL::OpenAL joan_ffmpeg SDL3::SDL3 freetype)
if(WIN32)
    target_link_libraries(openjoan_game PUBLIC opengl32 gdi32 user32 winmm)
else()
    # OpenGL comes through SDL (SDL_GL_GetProcAddress); dladdr / backtrace (runtime/crash.cpp, runtime/profiler.cpp), threads.
    find_package(Threads REQUIRED)
    target_link_libraries(openjoan_game PUBLIC ${CMAKE_DL_LIBS} Threads::Threads)
endif()
if(NOT MSVC)
    # compiler.hpp: MSVC's C runtime names (_stricmp ...) in every file, as MSVC's own headers declare them. No
    # -fno-omit-frame-pointer (MSVC /O2 omits them too): a function that uses JOAN_RETURN_ADDRESS_SLOT or
    # JOAN_CANVAS_FRAME (__builtin_frame_address) gets one anyway, and on 32-bit x86 the register is worth more.
    target_compile_options(openjoan_game PUBLIC
        "$<$<COMPILE_LANGUAGE:CXX>:SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/compiler.hpp>")
    # The code was written against MSVC's code generation; GCC / Clang are told to keep the same rules where theirs
    # differ: no type-based alias analysis (the ported code reads one type through another all the time), signed
    # overflow wraps, no fused multiply-add contractions (MSVC /fp:precise), and on 32-bit x86 SSE2 arithmetic
    # instead of the x87 unit (MSVC /arch:SSE2: the float results the game's decisions depend on).
    target_compile_options(openjoan_game PUBLIC
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-strict-aliasing -fwrapv -ffp-contract=off>)
    # The layout checks of the game's structures (original/gen: static_assert(offsetof(...))) are on classes with
    # member functions, which GCC supports but warns about thousands of times.
    target_compile_options(openjoan_game PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-invalid-offsetof>)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_compile_options(openjoan_game PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:-msse2 -mfpmath=sse>)
    else()
        # x86-64: frame pointers kept, so compiler.hpp's stack_trace_from can walk them (the DWARF unwinder faults on
        # the code table's generated stubs); 15 other registers remain, the cost is negligible there.
        target_compile_options(openjoan_game PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:-fno-omit-frame-pointer>)
    endif()
    if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND NOT MINGW)
        # 32-bit Linux: a fixed-address executable, like MSVC's x86 code (absolute addresses, relocated by the loader).
        # Many distributions build position-independent executables by default, which on x86 keep the GOT address in
        # EBX in nearly every function - one of its 7 registers - and load each global through it. The image sits at
        # 0x08048000, above joan.exe's old range (runtime/image_runtime.hpp kOldHigh); openjoan_game goes only into openjoan.
        target_compile_options(openjoan_game PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:-fno-pie>)
    endif()
endif()
if(MSVC)
    # x87.hpp (used by misc_fp.cpp, date.cpp) emulates x87 rounding on top of SSE2 doubles.
    target_compile_options(openjoan_game PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:/arch:SSE2>)  # not for joan_re.rc
    # Build speed: the Visual Studio generator compiles the files of one project one after another unless /MP is
    # given (Ninja runs them in parallel anyway; /MP is harmless there).
    target_compile_options(openjoan_game PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/MP>)
    # Debug information in every configuration (/Z7: in the objects, no shared .pdb server needed with /MP); the
    # linker writes openjoan.pdb.
    target_compile_options(openjoan_game PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:/Z7>)
    # The game/_unplaced/ files hold up to ~1,100 functions each (plus their template instantiations): more than
    # the 65,279 sections of the default object format.
    target_compile_options(openjoan_game PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/bigobj>)
endif()
# Build speed: registry.hpp / data_names.hpp / fn_names.hpp (~1.5 MB of declarations) are included by ~480 of the
# ~500 module files; a precompiled header parses them once. -DJOAN_PCH=OFF compiles without it.
option(JOAN_PCH "Precompiled header for openjoan_game" ON)
if(JOAN_PCH)
    target_precompile_headers(openjoan_game PRIVATE <cstdint> <cstring> <string> <vector> <atomic>
                              registry.hpp data_names.hpp fn_names.hpp)
endif()

# Never the original install (the environment variable JOAN_GAME_DIR, read-only): a COPY of the game folder, e.g.
# $JOAN_WORK_DIR/game. The variable was JOAN_GAME_DIR in joan-re; renamed so that -DJOAN_GAME_DIR=$JOAN_GAME_DIR cannot
# overwrite the install (2026-10-29 Phase 1 review).
set(JOAN_INSTALL_DIR "" CACHE PATH "Game folder COPY; if set, the executable and its DLLs are copied there after each build")

# Standalone executable (docs/RUNTIME.md): installs the reimplementation (linked in) and runs the game. Since stage
# S7d nothing of joan.exe is at a fixed address: the executable is relocatable (ASLR) and DEP-compatible, nothing is
# reserved at joan.exe's old range 0x00400000..0x00a96000 and no synthetic PE header is written there.
# Data (stage S7b): joan.exe's .rdata / .data / .bss / .idata are ONE constinit object of ours (joan_image), relocated
# at startup (runtime/image_runtime.cpp, data_image.cpp). Its values are the committed typed source src/data/rdata.inc /
# data.inc included by src/data/image.cpp (src/data/README.md); src/data/dead.cpp lists the
# ranges left out. runtime/import_table.inc (re/gen_data_image.py) gives the imports (DLL / function names, committed).
# Code (stage S7c): no code range is generated any more; runtime/code_table.cpp's slots (runtime/code_slots.inc, re/gen_code_slots.py)
# lead every original code address to its replacement.
# Resources (Windows): runtime/loader/joan_re.rc, OpenJoan's own (version information and icon 102, ADR-0002).
if(MSVC)
    enable_language(RC)
endif()
set(JOAN_DATA_SOURCES data/image.cpp data/dead.cpp)
set(_joan_image_source data/image.cpp)
if(WIN32)
    set(JOAN_RESOURCE_SCRIPT runtime/loader/joan_re.rc)
else()
    set(JOAN_RESOURCE_SCRIPT "")  # resource scripts are Windows'
endif()
add_executable(openjoan WIN32 runtime/loader/joan_re.cpp runtime/loader/data_image.cpp runtime/image_runtime.cpp ${JOAN_DATA_SOURCES}
    ${JOAN_RESOURCE_SCRIPT})
# One build per game folder: openjoan.exe is the 32-bit or the 64-bit build, its SDL3.dll / soft_oal.dll / FFmpeg DLLs
# lie next to it and are found by Windows' default DLL search (both builds use the same file names, so a folder holds
# one of them).
set(_joan_exe_name openjoan)
set_target_properties(openjoan PROPERTIES OUTPUT_NAME ${_joan_exe_name})
# Where the third-party DLLs go: next to the exe.
set(_joan_dll_dir "$<TARGET_FILE_DIR:openjoan>")
set(_joan_game_dll_dir "${JOAN_INSTALL_DIR}")
# The shared libraries to copy: the DLL on Windows; elsewhere the file named as the dynamic loader looks for it (the
# soname: libSDL3.so.0, libopenal.so.1; openjoan's rpath is $ORIGIN).
if(WIN32)
    set(_joan_openal_file "$<TARGET_FILE:${JOAN_OPENAL_DLL_TARGET}>")
    set(_joan_sdl3_file "$<TARGET_FILE:${JOAN_SDL3_DLL_TARGET}>")
else()
    set(_joan_openal_file "$<TARGET_SONAME_FILE:${JOAN_OPENAL_DLL_TARGET}>")
    set(_joan_sdl3_file "$<TARGET_SONAME_FILE:${JOAN_SDL3_DLL_TARGET}>")
endif()
target_compile_definitions(openjoan PRIVATE UNICODE _UNICODE)
if(MSVC)
    # joan_image is one 0xa1000-byte constinit object with ~350k member initialisers: room for the constant evaluator
    # (constinit turns any shortfall into a compile error instead of a silent dynamic initialiser).
    set_property(SOURCE ${_joan_image_source} APPEND PROPERTY COMPILE_OPTIONS /constexpr:steps100000000)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_property(SOURCE ${_joan_image_source} APPEND PROPERTY COMPILE_OPTIONS -fconstexpr-steps=100000000)
endif()
target_link_libraries(openjoan PRIVATE openjoan_game)
if(MSVC)
    # /INCREMENTAL:NO: with incremental linking &f is an incremental-link thunk, not the function: code pointers
    #   should be what they look like.
    # /OPT:NOICF: identical-COMDAT folding would merge trivial replacements (two "return 0" functions) into one
    #   address; code values that our C++ compares must stay distinct (runtime/code_table.cpp alias check).
    # /DYNAMICBASE /NXCOMPAT (stage S7d, also the linker's defaults - said explicitly): ASLR and DEP. Every executable
    #   byte is in the image's .text or in the trap stub block (VirtualAlloc, PAGE_EXECUTE_READ); joan_image and
    #   joan_code are relocated data. No /BASE: the preferred base is the linker's default, Windows moves it anyway.
    # /LARGEADDRESSAWARE /HIGHENTROPYVA (the x64 linker's defaults, said explicitly): the image and every allocation may
    #   lie anywhere in the 64-bit address space (Windows builds are x64 only since the end of Phase 2).
    target_link_options(openjoan PRIVATE /DYNAMICBASE /NXCOMPAT /LARGEADDRESSAWARE /HIGHENTROPYVA /STACK:0x100000,0x1000
                        /INCREMENTAL:NO /OPT:NOICF /OPT:REF /DEBUG:FULL)
    # openjoan.pdb next to the exe: crash logs and replay divergence reports name our functions (dbghelp).
elseif(MINGW)
    # MinGW (llvm-mingw link tests): the entry point is wWinMain.
    target_link_options(openjoan PRIVATE -municode)
else()
    # Linux: -rdynamic puts our function names in the dynamic symbol table (crash logs, profiler::symbol_at through
    # dladdr); the shared libraries copied next to the executable (SDL3, OpenAL Soft) are found there ($ORIGIN).
    target_link_options(openjoan PRIVATE -rdynamic)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_options(openjoan PRIVATE -no-pie)  # fixed-address executable (openjoan_game: -fno-pie)
    endif()
    set_target_properties(openjoan PROPERTIES BUILD_RPATH "$ORIGIN" INSTALL_RPATH "$ORIGIN")
endif()
# The settings file of this build next to the exe as openjoan.ini (release: openjoan_release.ini, build_flavor.hpp).
if(JOAN_RELEASE)
    set(_joan_ini "${CMAKE_CURRENT_SOURCE_DIR}/openjoan_release.ini")
else()
    set(_joan_ini "${CMAKE_CURRENT_SOURCE_DIR}/openjoan.ini")
endif()
add_custom_command(TARGET openjoan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_joan_ini}" "$<TARGET_FILE_DIR:openjoan>/openjoan.ini"
    COMMENT "Copying the settings file next to ${_joan_exe_name}.exe")
if(JOAN_OPENAL_DLL_TARGET)
    # soft_oal.dll next to openjoan.exe (it is loaded at startup), and into the game folder below.
    add_custom_command(TARGET openjoan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_joan_dll_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_joan_openal_file} "${_joan_dll_dir}"
        COMMENT "Copying OpenAL Soft next to ${_joan_exe_name}.exe")
endif()
if(JOAN_FFMPEG_DLLS)
    # FFmpeg's DLLs next to openjoan.exe and into the game folder.
    add_custom_command(TARGET openjoan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_joan_dll_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${JOAN_FFMPEG_DLLS} "${_joan_dll_dir}"
        COMMENT "Copying FFmpeg next to ${_joan_exe_name}.exe")
    if(JOAN_INSTALL_DIR)
        add_custom_command(TARGET openjoan POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_joan_game_dll_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${JOAN_FFMPEG_DLLS} "${_joan_game_dll_dir}"
            COMMENT "Installing FFmpeg into ${_joan_game_dll_dir}")
    endif()
endif()
if(JOAN_SDL3_DLL_TARGET)
    # SDL3.dll next to openjoan.exe and into the game folder.
    add_custom_command(TARGET openjoan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_joan_dll_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_joan_sdl3_file} "${_joan_dll_dir}"
        COMMENT "Copying SDL3 next to ${_joan_exe_name}.exe")
    if(JOAN_INSTALL_DIR)
        add_custom_command(TARGET openjoan POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_joan_game_dll_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_joan_sdl3_file} "${_joan_game_dll_dir}"
            COMMENT "Installing SDL3 (SDL3.dll) into ${_joan_game_dll_dir}")
    endif()
endif()
if(JOAN_INSTALL_DIR AND JOAN_OPENAL_DLL_TARGET)
    add_custom_command(TARGET openjoan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_joan_game_dll_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_joan_openal_file} "${_joan_game_dll_dir}"
        COMMENT "Installing OpenAL Soft (soft_oal.dll) into ${_joan_game_dll_dir}")
endif()
if(JOAN_INSTALL_DIR)
    add_custom_command(TARGET openjoan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openjoan> "${JOAN_INSTALL_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CXX_COMPILER_ID:MSVC>:$<TARGET_PDB_FILE:openjoan>>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:$<TARGET_FILE:openjoan>> "${JOAN_INSTALL_DIR}"
        COMMENT "Installing ${_joan_exe_name}.exe (and ${_joan_exe_name}.pdb) into ${JOAN_INSTALL_DIR}")
endif()

# tests/data_image_reference_test (Phase 1 step 4): the committed data image (ADR-0001) against the reference joan.exe in
# $JOAN_GAME_DIR, and the loader's relocation code on copies of the image at many bases. Links the same objects as
# openjoan (image.cpp, dead.cpp, runtime/loader/data_image.cpp) without the game. Exit 77 = SKIPPED (no reference
# executable: CTest reports "Not Run / Skipped", the test does not fail a clone without the game).
if(JOAN_BUILD_DATA_TEST)
    add_executable(data_image_reference_test ${CMAKE_SOURCE_DIR}/tests/data_image_reference_test.cpp
        runtime/loader/data_image.cpp ${JOAN_DATA_SOURCES})
    target_include_directories(data_image_reference_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(data_image_reference_test PRIVATE joan_warnings)
    if(NOT MSVC)
        target_compile_options(data_image_reference_test PRIVATE -Wno-invalid-offsetof)
    endif()
    add_test(NAME data_image_reference_test COMMAND data_image_reference_test)
    set_tests_properties(data_image_reference_test PROPERTIES SKIP_RETURN_CODE 77)

    # The new pathfinding's search on small hand-built meshes (docs/PATHFINDING.md).
    find_package(Threads REQUIRED)
    add_executable(navsearch_test ${CMAKE_SOURCE_DIR}/tests/test_navsearch.cpp enpath/navpool.cpp enpath/navsearch.cpp enpath/navsteer.cpp)
    target_link_libraries(navsearch_test PRIVATE joan_warnings Threads::Threads)
    add_test(NAME navsearch_test COMMAND navsearch_test)
endif()
