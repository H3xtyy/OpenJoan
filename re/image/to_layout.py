#!/usr/bin/env python3
"""Image typing -> class layouts (docs/X64.md image): turns what re/image/spec/i*.json found about the classes
of the data-image objects into a layout spec round, re/layout/spec/r5_image.json:

  * the classes the image review defined (CRT FILE / lconv, Win32 OVERLAPPED / MCI parms / SYSTEMTIME, small game
    singletons) are added;
  * a symbol that is really a field of a larger object (`part_of`) becomes a field of the class that owns that
    offset (descending into bases and embedded members), when the class has nothing there yet; its type is the
    spec's type, else the width the original reads it with;
  * a symbol typed as a pointer where the class has a same-size integer field: the field becomes that pointer;
  * class sizes stated by the review (re/image/spec notes) where the layout has none;
  * the fields listed in re/layout/add_fields/*.json (round P4: members the raw accesses of the game code need,
    re/a4/p4_fields.py), where no field / member / base of the class covers those bytes.
Classes that change are restated with "override": true (round r5). Report: re/image/out/to_layout.txt.

    re/image/to_layout.py [--dry-run]
"""
import argparse
import collections
import copy
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
LAYOUT = os.path.join(ROOT, 're', 'layout')
SIZES = {'u8': 1, 'i8': 1, 'char': 1, 'u16': 2, 'i16': 2, 'u32': 4, 'i32': 4, 'f32': 4, 'f64': 8, 'bool8': 1}
# sizes stated in the review notes (re/image/spec/i2.json, i3.json) for classes whose layout has none
# fields whose type the uses contradict (docs/X64.md round V reports): (class, offset, new type, why)
RETYPE = [('SysEn', 0x34, 'ptr:void', 'pp_windowed is hDeviceWindow (an HWND; d3dsys.cpp stores g_a50090)'),
          ('Actor', 0x5c, 'i32', 'record: a players-list id / handle (ptr_at(i32), set_active(i32)), not a pointer'),
          # round F (agents' reports, re/bind/round_f_notes.md): integer fields that hold pointers
          ('ScriptNode', 0x10, 'ptr:void', 'value: String* / u32* / Vec3* by node kind'),
          ('BodyState', 0xd0, 'ptr:void', 'sub: a layer pointer'),
          ('MessageBox', 0x2e, 'ptr:void', 'texture'),
          ('CampScreen', 0x181, 'ptr:void', 'widget / texture'), ('CampScreen', 0x1c3, 'ptr:void', 'widget / texture'),
          ('CampScreen', 0x195, 'ptr:void', 'widgets[6]'), ('CampScreen', 0x235, 'ptr:void', 'widgets[4]'),
          ('CampScreen', 0x225, 'ptr:void', 'texture handle'), ('CampScreen', 0x15d, 'ptr:ActorDef', 'actor def'),
          ('TalkPrompt', 0x0, 'ptr:void', 'texture'), ('VideoPlayer', 0x0, 'ptr:void', 'texture'),
          ('InventoryScreen', 0x0, 'ptr:void', 'texture'), ('ActorDef', 0x18c, 'ptr:Bone', 'batch_planes[80]: Bone*'),
          ('Letter', 0x0, 'ptr:void', 'texture'), ('Letter', 0x4, 'ptr:void', 'sprite sheet'),
          ('GoalPanel', 0x0, 'ptr:void', 'texture'),
          ('EndScreen', 0x0, 'ptr:void', 'texture'), ('EndScreen', 0x4, 'ptr:void', 'texture'),
          ('EndScreen', 0x8, 'ptr:void', 'texture'), ('EndScreen', 0xc, 'ptr:void', 'texture'),
          ('IngameFrame', 0x20, 'ptr:void', 'texture'), ('IngameFrame', 0x24, 'ptr:void', 'texture'),
          ('IngameFrame', 0x28, 'ptr:void', 'texture'), ('IngameFrame', 0x2c, 'ptr:void', 'texture'),
          ('Barracks', 0x0, 'ptr:void', 'tex_barr'), ('Barracks', 0x4, 'ptr:void', 'tex_bg'),
          ('Barracks', 0x8, 'ptr:void', 'tex_commele'),
          ('BoxTree', 0x0, 'ptr:BoxTree', 'parent'), ('Path', 0x1c, 'ptr:void', 'pointer'),
          ('Boid', 0x42, 'ptr:Boid', 'next boid'), ('EffectKey', 0x141, 'ptr:ActorDef', 'actor def'),
          ('VsEntry', 0x0, 'ptr:u8', 'key_format: pointer to the format bytes'),
          ('MessageLog', 0x1, 'ptr:void', 'res[4] textures'), ('MessageLog', 0x41, 'ptr:void', 'texture'),
          ('CombatScreen', 0x8, 'ptr:BmpRes', 'sprite sheet'), ('OptMenu', 0x18, 'ptr:BmpRes', 'sprite sheets'),
          ('GameOver', 0x9da, 'ptr:BmpRes', 'sprite sheet'),
          ('Unit', 0x2fd, 'ptr:ActorDef', 'actor def'), ('Unit', 0x30d, 'ptr:ActorDef', 'actor def'),
          ('Unit', 0x301, 'ptr:AnimSet', 'anim set'), ('Unit', 0x311, 'ptr:AnimSet', 'anim set'),
          # round G (round F reports)
          ('FileLoader', 0x124, 'ptr:void', 'context: the context object FileLoader::start passes on'),
          ('StripNode', 0x8, 'ptr:void', 'vertex data'), ('StripNode', 0xc, 'ptr:void', 'vertex data'),
          ('StripNode', 0x1c, 'ptr:void', 'index buffer'), ('StripNode', 0x24, 'ptr:Bone', 'bone'),
          ('ComboBox', 0x84, 'ptr:BmpRes', 'font: the sprite set'), ('ComboBox', 0x88, 'ptr:Font2d', 'font'),
          ('ScrollBox', 0x78, 'ptr:BmpRes', 'sprite set copied into Button::sprite_set'),
          ('Part', 0x1c, 'ptr:Scene', 'value: the scene of Actor::set_scene'),
          ('Zones', 0x74, 'ptr:void', 'pointer'),
          ('RWorldCell', 0x0, 'ptr:PathSpace', 'key_a'), ('RWorldCell', 0x4, 'ptr:PathSpace', 'key_b'),
          ('ActorDef', 0x8, 'ptr:void', 'anim def'), ('BuyScreen', 0x0, 'ptr:void', 'texture'),
          ('Res1', 0x0, 'ptr:void', 'texture'),
          # pointer fields that hold integers
          # round P3 (x64 compile blockers)
          ('ScrollBar', 0x0, 'ptr:BmpRes', 'owner: the sprite set (ScrollBar::build / init a0; MenuButton::setup, draw_named)'),
          ('CombatScreen', 0xc, 'ptr:BmpRes', 'sprite set 0xa57f30 handed to ScrollBar::build'),
          ('CamTrackNode', 0xd, 'i32', 'unit: a unit id'), ('BuyScreen', 0x239, 'i32', 'drag_item: an index'),
          # round P6 (the P5 layout requests re/a4/p5_requests, decided from joan.exe): integer fields that hold pointers
          ('Actor', 0x1c, 'ptr:Scene', 'scene_params0: Scene::nop / scene of the actor (playerctl.cpp)'),
          ('AnimBuf', 0x0, 'ptr:ModelSlot', 'data: the heap block of 0x21b-byte AnimScr records (animscr_load 0x00418f30)'),
          ('Credits', 0x2, 'ptr:void', 'images[12]: texture handles'),
          ('CampScreen', 0x1b3, 'ptr:Texture', '0x006099c0 result (objmenuw.cpp)'),
          ('CampScreen', 0x1b7, 'ptr:Texture', '0x006099c0 result (objmenuw.cpp)'),
          ('ItemRes', 0xc, 'ptr:ItemType', 'types: type_count * 0x25 ItemType records (ItemRes::load 0x0049f970)'),
          ('MainMenu', 0x15, 'ptr:Texture', 'images[7]: released through TextureManager::release'),
          ('MouseCursor', 0xd4, 'ptr:CursorRec', 'shape_table: i16_d2 8-byte CursorRec records'),
          ('OptMenu', 0x0, 'ptr:Texture', 'images[6]: 0x006099c0 results'),
          ('Path', 0x14, 'ptr:Billboard', 'blocks: Billboard::item 0x00637da0 on it (i_gate gate_build 0x0048d4b0)'),
          ('SaveMenu', 0x0, 'ptr:void', 'sprites: an image handle released by image_release'),
          ('SaveMenu', 0x10, 'ptr:void', 'sprites2: image handle'), ('SaveMenu', 0x14, 'ptr:void', 'image handle'),
          ('ShopStage', 0xdd, 'ptr:ActorDef', 'ModelEntry::actor_def stored and read back as SceneObject::def'),
          ('TrackNode', 0x20, 'ptr:TrackNode', 'parent (find_path open / closed lists)'),
          ('TrackNode', 0x28, 'ptr:TrackNode', 'next (find_path open / closed lists)'),
          ('UScrRes', 0x4, 'ptr:UnitScript', 'scripts: count * 0x28 UnitScript records'),
          ('UnitTable', 0x30, 'ptr:UnitTableEntry', 'entries: count << 2 bytes of {u16 id, u16 def}'),
          ('UpgradeRes', 0x4, 'ptr:UpgradeItem', 'items: 0x64-byte UpgradeItem records'),
          ('Water', 0x30, 'ptr:void', 'a sector / scene pointer ([[+0x30]+0x38] read as a Plane*)'),
          ('WorkScreen', 0x0, 'ptr:void', 'tex_a: texture handle released through 0x0060a550'),
          ('WorkScreen', 0x4, 'ptr:void', 'tex_b'), ('WorkScreen', 0x8, 'ptr:void', 'tex_c'),
          ('WorldSectorPtrArray', 0x0, 'ptr:SceneS', 'Cell::sectors elements are SceneS (Map::find_cell 0x0054a2c0: '
           'sector+0x20 box; uinput.cpp reads plane2 / object; World::sector -> SceneS::reset)'),
          # round P8 (the P7 layout requests re/a4/p7_requests, decided from joan.exe)
          ('Actor', 0x84, 'ptr:AnimDef', 'i32_84: the key-cache owner AnimDef::animate 0x005b5e40 compares with / stores '
           'its this (cache_owner void**)'),
          ('PathRec', 0x0, 'ptr:MeshVertex', 'a: Rec::set 0x0066a320 stores the edge start vertex (mesh_build 0x00667190)'),
          ('PathRec', 0x4, 'ptr:MeshVertex', 'b: the edge end vertex'),
          ('PathRec', 0xc, 'ptr:void', 'd: the triangle zone of the edge (Zone::link 0x00664130 argument)'),
          ('Buffer', 0xc, 'ptr:PfdZone', 'u32_c: PfdZone::open_next (the 0x00663b90 / 0x004ee410 search block is '
           'PfdZone+0x38 / TerrainBlock+0x38)'),
          ('Player', 0x1261, 'ptr:BmpRes', 'u32_1261: rts.cpp stores &g_a48f50 (a BmpRes), draw_named reads it as the set'),
          ('Scroll', 0x8e, 'ptr:void', 'callback: a code address called as i32(__cdecl*)(void*, u8) (scroll2d.cpp)'),
          ('Wall', 0xeb, 'ptr:void', 'u32_eb: a heap block freed through Mem::del 0x0058ed70 (Wall::release_487460)'),
          ('ListBox', 0xb2, 'ptr:void', 'u32_b2: item draw callback (set_b2 0x006b33f0 stores a code address, '
           '0x006f8970 calls it)'),
          ('World', 0x8e, 'ptr:SceneSPtr', 'entries: SceneS* (0x005ea460 add_node results; realloc cap << 2)'),
          ('BuyScreen', 0x4, 'ptr:void', 'sheet: img::addr(0x00a48810), a sprite sheet read as SpriteQueue::draw_named set'),
          ('MessageList', 0x40, 'ptr:Texture', 'i32_40: TextureManager::load result, released through 0x0060a550'),
          # x64 run under Wine (2026-10-18): the Ctl view's +0 is the vtable of the Control2D object (set_rect
          # 0x00628b00 calls through it); as an i32 the x64 vtable read took the following fields as its upper half
          ('Ctl', 0x0, 'ptr:void', 'vtable (Control2D +0)'),
          # round X64B (the X64A audit requests re/a4/x64a_requests x04 / x05 / x06): TextureManager::load 0x006099c0
          # results kept in u32 fields (released through 0x0060a550)
          ('GameOver', 0x9d2, 'ptr:Texture', 'image: gameover.cpp TextureManager::load result, released by 0x0060a550'),
          ('GameOver', 0x9d6, 'ptr:Texture', 'image2: as image'),
          ('Glory', 0xd18, 'ptr:Texture', 'u32_d18: i_vict.cpp sprite_load (TextureManager::load VICT)'),
          ('Glory', 0xd1c, 'ptr:Texture', 'u32_d1c: TextureManager::load COMMELE'),
          ('Player', 0x1265, 'ptr:void', 'rts_icon: rts.cpp TextureManager::load RTSICON result, released by 0x0060a550'),
          ('Ctl', 0x15, 'ptr:Texture', 'u32_15: popupwin.cpp TextureManager::release(u32_15) 0x0060a550')]
# round P6: fields / members replaced by others over the same bytes (splits of a wrong-width field, one-by-one names
# joined into an array, wrong records retyped). (class, lo, hi, [(name, off, type, count)], why): every field / member
# lying inside [lo, hi) is removed, then the new items are added (type: a scalar / ptr:X, or a class = embedded
# member). A field straddling lo / hi is an error. The removed names still referenced in src/ are reported
# (re/image/out/to_layout.txt "split: old name still used"; the compile gate then catches each one).
SPLIT = [
    # round X64B (re/a4/x64a_requests/x02.json): SaveEntry::set 0x0047e200 a14 (types 4 / 8 / 0x80) is the trigger sector
    ('SaveEntry', 0x3f, 0x43, [('scene', 0x3f, 'ptr:SceneS', 1)],
     'gfile3.cpp entry_set: +0x3f holds the trigger sector pointer (World::find_sector), the same object as Trigger::scene'),
    ('SceneS', 0x20, 0x24, [('inner_box', 0x20, 'f32', 6)],
     'grid_x / grid_y belong to the grid tiles (WorldSector); SceneS +0x20 is the inner box (Map::find_cell 0x0054a2c0 '
     'contains(sector+0x20); World::load_sector memcpy 0x18 bytes)'),
    ('WorldSector', 0x24, 0x28, [('owner', 0x24, 'ptr:SceneS', 1)],
     'set_24 0x0054dc50 stores the owner sector; build_ground compares it with Cell::sectors[i] and calls set_38 on it'),
    ('Cell', 0x8, 0x10, [('box', 0x8, 'f32', 6)], 'bounding box (Map::find_cell contains(cell+8), build_tree memcpy 0x18)'),
    ('CampScreen', 0x54, 0x80, [('menu_buttons', 0x54, 'ptr:Widget', 11)], 'p_54..p_7c indexed as one run by the original loops'),
    ('CampScreen', 0x1ad, 0x1b3, [('siege_flags', 0x1ad, 'u8', 6)], 'u8_1ad..u8_1b2 indexed by i (siege_help, objm_cannon)'),
    ('FlcHeader', 0x4, 0x11, [('frame_count', 0x4, 'u16', 1), ('frame', 0x6, 'u16', 1), ('pixels', 0x8, 'u8*', 1),
                              ('palette', 0xc, 'u8*', 1), ('loop', 0x10, 'u8', 1)],
     'Rec::export 0x005a99e0 fills the header (u8_4[13])'),
    ('Group', 0x3, 0x37, [('members', 0x3, 'i16', 20), ('class_counts', 0x2b, 'i16', 6)],
     'Group::add_unit / remove_member count per unit class at +0x2b + Unit::i32_104 * 2'),
    ('Hud', 0x60, 0x64, [('exp_len', 0x60, 'u8', 9)], 'digit count per exp entry indexed with exp_count (u32_60 unused)'),
    ('Input', 0x672, 0x6aa, [('clicks', 0x672, 'MouseClick', 2)], 'Table::get_67a / get_67e index the two clicks'),
    ('Joystick', 0xa, 0x5e, [('axis_offset', 0xa, 'u32', 10), ('button_offset', 0x32, 'u32', 11)],
     'DIJOYSTATE2 offsets of the axes / buttons (mousecr.cpp, u8_a[84])'),
    ('LightPreset', 0x42, 0x18c, [('lights', 0x42, 'LightDef', 9), ('params', 0x12c, 'LightParams', 1)],
     '9 lights then the 0x60-byte LightParams at +0x12c (0x00617ee0 constructs it at state+0x12c; the sky shader reads '
     'preset+0x12c as one block: camera3d.cpp)'),
    ('LightParams', 0x0, 0x60, [('vec_0', 0x0, 'f32', 3), ('vec_c', 0xc, 'f32', 3), ('vec_18', 0x18, 'f32', 3),
                                ('vec_24', 0x24, 'f32', 3), ('sun_dir', 0x30, 'f32', 3), ('scale_b', 0x3c, 'f32', 3),
                                ('f32_48', 0x48, 'f32', 1), ('scale_a', 0x4c, 'f32', 5)],
     'named like the LightPreset fields it absorbs (sun_dir +0x15c = params +0x30, ...); LightParams ctor 0x00617ee0'),
    ('LightSys', 0x198, 0x4c0, [('state_a', 0x198, 'LightState', 1), ('state_b', 0x32c, 'LightState', 1)],
     'the two 0x194-byte blend states (u8[404])'),
    ('LodEntry', 0x8, 0x10, [('levels', 0x8, 'LodLevel', 4)], 'four 8-byte levels (+0x8 + i * 8)'),
    ('MessageBox', 0x4, 0x44, [('title', 0x4, 'char', 64)],
     'Popup::set_title 0x006d52c0 strncpy 0x3f / memset 0x40 at +4; +0x2e..+0x43 were MessageList::init 0x0046c360 '
     '(on 0xa55a88, a MessageList)'),
    ('MouseCursor', 0x7a, 0xbe, [('shape', 0x7a, 'CursorShape', 3)],
     'the shapes start at +0x7a (memset(this+0x7a, 0, n*0x16) in MouseCursor::init; release frees +2..+0x16 of each)'),
    ('CursorShape', 0x0, 0x16, [('u16_0', 0x0, 'u16', 1), ('buf', 0x2, 'u8*', 5)], 'records start 2 bytes earlier (see MouseCursor)'),
    ('Order0b', 0xc2, 0xc8, [('drop_item', 0xc2, 'i16', 1), ('drop_count', 0xc4, 'i16', 1), ('u8_c6', 0xc6, 'u8', 2)],
     'update_48ba70 / spawn_follower read i16 item type / count (u8_c2[6])'),
    ('Particle', 0x3c, 0x46, [('rot_speed', 0x3c, 'f32', 1), ('self_angle', 0x40, 'f32', 1), ('self_rot_speed', 0x44, 'f32', 1)],
     'pemitter reads / writes three floats (u16_3c / ptr_42 unused)'),
    ('Player', 0x44, 0x50, [('rally_pos', 0x44, 'f32', 2), ('camera', 0x4c, 'CamParams', 1)],
     'the 0x28-byte camera block 0x4c..0x74 passed to CameraMotion::start / retarget (mission.cpp passed &rally_pos[2])'),
    ('Profiler', 0x24, 0x28, [('clock', 0x20, 'u32', 2)], 'read_clock 0x0045c990 writes a qword at +0x20 (node_24 unused)'),
    ('Queue', 0x249, 0x299, [('pairs', 0x249, 'QueuePair', 10)], 'push3 0x00414f50 / find_pair 0x00414fe0: 8-byte pairs'),
    ('Relation', 0x3, 0x4, [('attitude', 0x3, 'i8', 1)], 'Rel / Relation are one record: +3 is the attitude (2..4)'),
    ('SaveMenu', 0x2c, 0x20e8, [('slots', 0x2c, 'Button', 10)], 'slot0..slot9 indexed i * 0x346 + 0x2c'),
    ('Siege', 0x114, 0x2f04, [('queue', 0x114, 'SiegeOrder', 80)], '0x93-byte SiegeOrder records replace u8 queue[11760]'),
    ('SoundStreams3', 0x44, 0x50, [('videos', 0x44, 'ptr:Video', 3)], 'video_0..video_2 indexed by slot'),
    ('TexDirEntry', 0x4, 0x2d, [('name', 0x4, 'char', 33), ('name_count', 0x25, 'i32', 1), ('name_cap', 0x29, 'i32', 1)],
     'name is char[33], then the count / capacity of buf_2d (texdir.cpp)'),
    ('VertexF7', 0x0, 0x48, [('influence_pos', 0x0, 'f32', 6), ('influence_normal', 0x18, 'f32', 6), ('bone', 0x30, 'u16', 2),
                             ('weight', 0x34, 'u32', 2), ('tail', 0x3c, 'f32', 3)],
     'two influences: positions +k*0xc, normals +0x18, bone indices u16, weights (raw float bits) (plane.cpp 0x...4390)'),
    ('Anim', 0x44, 0x4a, [('i32_44', 0x44, 'i32', 1), ('i16_48', 0x48, 'i16', 1)],
     'help.cpp layout_4892d0 reads the i16 at +0x48 of each linked node (bytes_44[6])'),
    ('Buffer', 0x1c, 0x20, [('capacity', 0x1c, 'i16', 1), ('u8_1e', 0x1e, 'u8', 1), ('u8_1f', 0x1f, 'u8', 1)],
     'TerrainBlock +0x54 read / written as i16, +0x57 byte cleared (pathmesh.cpp)'),
    ('BuyScreen', 0x30, 0x34, [('items_b', 0x1a, 'UnitData', 1)],
     'UnitData ctor 0x00495950 / create 0x004959b0 / dtor on +0x1a; player_grid +0x30 is its grid_16'),
    ('Unit', 0x42e, 0x430, [('weapon_model_id', 0x42e, 'i32', 1)], 'written and read as a dword (unitupd.cpp)'),
    # round P8
    ('Water', 0x4, 0x1c, [('box', 0x4, 'f32', 6)],
     'Water::init 0x00546b50 copies 6 dwords to +4, Waters::find_at 0x00469d10 reads x0/y0 +4/+8, x1/y1 +0x10/+0x14 '
     '(fsubr / fadd dword), z at +0xc; motion / camera were CameraMan (0x005624c0 on g_a48cc0 = CameraFollow '
     'motions / camera)'),
    ('Top4', 0x0, 0x6, [('sample', 0x0, 'ptr:void', 1), ('rec_no', 0x4, 'u16', 1)],
     'the 0x28-byte Se::entries records (se_build 0x... mem_alloc count * 0x28) are Top4 lists with the sound in front: '
     '+0 the wave in memory (read_rec result, play_memory argument; 0 = not loaded), +4 the 1-based record number '
     '(play_resource argument); Top4List::clear / insert work on +6 / +8'),
    ('Streamer', 0x8, 0x20, [('lock', 0x8, 'CriticalSection', 1)],
     'InitializeCriticalSection / Enter / Leave on +8: the native CRITICAL_SECTION (24 bytes on x86, 40 on x64)'),
]
# round P8: classes whose base is replaced, keeping a copy of the old base's fields in [lo, hi) (the old base is
# larger than the object): (class, new base, old base, lo, hi, why)
REBASE = [('EditBox', 'Gadget', 'TextBox', 0x70, 0x91,
           'EditBox::init 0x0062d4f0 writes +0x70..+0x8f; the 0x91-byte edit box of SaveMenu (+0x440c) is followed by '
           'the name buffer at +0x449d: the multi-line part of TextBox2d (+0x91 line_starts .. +0x98) is not there')]
# round P6: classes that are another class under a second name: (class, the class it becomes an alias of, why)
ALIAS = [('Rel', 'Relation', 'Nation::relationN and Relation are the same 0xf4 record (Relation::* methods, '
          'raise_to 0x004b8d60 / mark 0x004b8cf0 on both)'),
         # round P8
         ('Territory', 'ZoneDef', 'the territory list g_a52940 holds the ZoneDef records load_settlement 0x004f9330 '
          'fills (+4 name, +0x26 kind, +0x30 / +0x34 values[2..3] = map x / y, +0x38 owner, +0x3c script)'),
         ('PopupScreen', 'MessageBox', 'popup_buttons 0x006d3940 runs on the MessageBox 0x00a52df8 (+0x44c x / y, '
          '+0x450 font, +0x454 text box, buttons at +0x458)')]
MEMBER_ARRAYS = [('Game', 'end_conditions')]  # unused one-by-one names; Game indexes them with i * 0x1a
STATED_SIZES = {'UnitManager': 0x80, 'SysEn': 0x337, 'Config': 0x16a, 'UnitAttScreen': 0x6b3, 'Ending': 0x236,
                'SoundStreams3': 0x50, 'ListB': 0x32, 'Menu': 0xc,
                'ModelSlot': 0x21b,  # P6: the AnimScr record (AnimScr::data + i * 0x21b)
                # P8: the sector record (ctor 0x005ef800, raw load 0x00484d00 of 0xe4 bytes); Struct34::reset
                # 0x004ee410 memsets 0x34 bytes
                'SceneS': 0xe4, 'PathSearchState': 0x34}


def num(x):
    return int(x, 16) if isinstance(x, str) else int(x)


class load_symbols(dict):
    """spec name -> (address, ...): the address suffix of the name (the spec's names are those of the time;
    re/gen_data_curated.py may rename symbols since), else data_symbols.inc"""
    def __init__(self):
        super().__init__()
        for line in open(os.path.join(ROOT, 'src', 'data', 'data_symbols.inc')):
            m = re.match(r'JOAN_DATA_SYMBOL\((\w+), ([^,]+), (\w+), (0x[0-9a-f]+)u\)', line)
            if m:
                dict.__setitem__(self, m.group(3), (int(m.group(4), 16), m.group(2), m.group(1)))

    def __missing__(self, n):
        m = re.search(r'_([0-9a-f]{6})$', n)
        if not m:
            raise KeyError(n)
        return (int(m.group(1), 16), '', '')

    def __contains__(self, n):
        return dict.__contains__(self, n) or bool(re.search(r'_[0-9a-f]{6}$', n))

    def get(self, n, d=None):
        return self[n] if n in self else d


def field_name(sym, typ, off, taken):
    n = re.sub(r'_[0-9a-f]{6}$', '', sym)
    n = re.sub(r'^(get_|b_|g_|k)', '', n) if re.match(r'^(b_|g_a|k[A-Z]|get_)', n) else n
    n = re.sub(r'(?<!^)(?=[A-Z])', '_', n).lower().strip('_') if n and n[0].isupper() else n.lower()
    if not n or re.fullmatch(r'[0-9a-f]+', n) or n in ('a', 'b'):
        n = f"{typ.split(':')[0]}_{off:x}"
    base, k = n, 2
    while n in taken:
        n = f'{base}_{k}'
        k += 1
    taken.add(n)
    return n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()
    # the layout specs before this round (r5_image.json is this tool's output: re-running it starts from the same input)
    import glob
    sys.path.insert(0, LAYOUT)
    import check_spec as K
    paths = [p for p in sorted(glob.glob(os.path.join(LAYOUT, 'spec', '*.json'))) if not p.endswith('r5_image.json')]
    L, _, _, problems = K.load_specs(paths)
    if problems:
        sys.exit('\n'.join(problems))
    syms = load_symbols()
    facts = json.load(open(os.path.join(HERE, 'out', 'gfacts.json')))
    S, new_classes = {}, []
    for g in ('i1', 'i2', 'i3', 'i4'):
        s = json.load(open(os.path.join(HERE, 'spec', f'{g}.json')))
        S.update(s['symbols'])
        new_classes += s.get('classes', [])
    # round P6: classes the layout requests need (re/layout/add_fields/*.json "classes"; their fields come with the
    # file's "fields")
    for p in sorted(glob.glob(os.path.join(LAYOUT, 'add_fields', '*.json'))):
        for c in json.load(open(p)).get('classes', []):
            if c['class'] in L or c['class'] in {x['class'] for x in new_classes}:
                sys.exit(f"{os.path.basename(p)}: class {c['class']} exists already")
            new_classes.append(copy.deepcopy(c))
    known = {c['class'] for c in new_classes}
    classes = dict(L)
    for c in new_classes:
        classes[c['class']] = c
    changed = {}  # class -> restated spec
    log = collections.defaultdict(list)

    def spec(c):
        if c in known:
            return classes[c]
        if c not in changed:
            changed[c] = copy.deepcopy(classes[c])
            changed[c]['override'] = True
            changed[c].pop('defined_in', None)
        return changed[c]

    def size_of(c):
        e = changed.get(c) or classes.get(c)
        if not e:
            return 0
        if e.get('size'):
            return num(e['size'])
        ext = size_of(e['base']) if e.get('base') else 0
        for f in e.get('fields', []):
            ext = max(ext, num(f['off']) + f.get('size', 0) * int(f.get('count', 1) or 1))
        for m in e.get('members', []):
            ext = max(ext, num(m['off']) + size_of(m['class']) * int(m.get('count', 1) or 1))
        return ext

    def owner_at(c, off):
        """(class, offset in it, exact field or None, covering field or None) of the deepest class holding off."""
        for _ in range(30):
            e = classes.get(c)
            if e is None:
                return None
            for m in e.get('members', []):
                mo, ms = num(m['off']), size_of(m['class'])
                n = int(m.get('count', 1) or 1)
                if ms and mo <= off < mo + ms * n:
                    c, off = m['class'], (off - mo) % ms
                    break
            else:
                if e.get('base') and off < size_of(e['base']):
                    c = e['base']
                    continue
                exact = cover = None
                for f in e.get('fields', []):
                    fo, fs = num(f['off']), f.get('size', 0) * int(f.get('count', 1) or 1)
                    if fo == off:
                        exact = f
                    if fo <= off < fo + max(fs, 1):
                        cover = f
                return c, off, exact, cover
        return None

    def root(n):
        off = 0
        while 'part_of' in S.get(n, {}):
            o = S[n]['part_of']
            off += syms[n][0] - syms[o][0]
            n = o
        return n, off

    def width_type(n):
        f = facts.get(n, {})
        w = collections.Counter()
        for k, v in f.get('direct', {}).items():
            o, wd, kind = k.split('/')
            if o == '0x0' and wd != '0' and kind.startswith('r'):  # writes of al/ax are reported as dword
                w[int(wd)] += v
        if not w:
            return None
        wd = w.most_common(1)[0][0]
        return {1: 'u8', 2: 'i16', 4: 'i32', 8: 'f64'}.get(wd)

    for n, v in sorted(S.items(), key=lambda x: syms.get(x[0], (0,))[0]):
        if 'part_of' not in v or n not in syms:
            continue
        r, off = root(n)
        t = S[r].get('type', '')
        if not t.startswith('obj:'):
            log['owner is not an object (alias bases / scratch arrays)'].append(f'{n} -> {r} ({t})')
            continue
        cls = t[4:]
        osz = size_of(cls)
        if S[r].get('count', 1) and S[r].get('count', 1) > 1 and osz:
            off %= osz
        hit = owner_at(cls, off)
        if hit is None:
            log['owner class unknown'].append(f'{n} -> {cls}')
            continue
        c, o, exact, cover = hit
        typ = v.get('type') or width_type(n)
        if exact:
            if typ and typ.startswith('ptr') and not (exact['type'].startswith('ptr') or exact['type'].endswith('*')):
                if exact.get('size') == 4 and int(exact.get('count', 1) or 1) == 1:
                    f = next(x for x in spec(c)['fields'] if num(x['off']) == o)
                    log['field became a pointer'].append(f"{c}+{o:#x} {f['name']}: {f['type']} -> {typ} ({n})")
                    f['type'] = typ
                else:
                    log['pointer over a field of another size (left)'].append(
                        f"{c}+{o:#x} {exact['name']} {exact['type']}[{exact.get('size')}] vs {typ} ({n})")
            continue
        if cover:
            continue
        if not typ or typ in ('bytes', 'unknown') or typ.startswith('obj:'):
            log['no field there, type unknown (left)'].append(f'{c}+{o:#x} {n} {typ}')
            continue
        cnt = int(v.get('count', 1) or 1)
        sz = 4 if typ.startswith('ptr') else SIZES.get(typ)
        if sz is None:
            log['no field there, type unknown (left)'].append(f'{c}+{o:#x} {n} {typ}')
            continue
        e = spec(c)
        busy = [(num(f['off']), num(f['off']) + f.get('size', 0) * int(f.get('count', 1) or 1)) for f in e.get('fields', [])]
        busy += [(num(m['off']), num(m['off']) + size_of(m['class']) * int(m.get('count', 1) or 1)) for m in e.get('members', [])]
        if any(lo < o + sz * cnt and o < hi for lo, hi in busy):
            log['field would overlap (left)'].append(f'{c}+{o:#x} {n} {typ}x{cnt}')
            continue
        if e.get('size') and o + sz * cnt > num(e['size']):
            log['field beyond the class size (left)'].append(f'{c}+{o:#x} {n} {typ}x{cnt} size {e["size"]}')
            continue
        taken = {f['name'] for f in e.get('fields', [])} | {m['name'] for m in e.get('members', [])}
        f = {'off': f'{o:#x}', 'name': field_name(n, typ, o, taken), 'type': typ, 'size': sz}
        if cnt > 1:
            f['count'] = cnt
        e.setdefault('fields', []).append(f)
        e['fields'].sort(key=lambda x: num(x['off']))
        log['field added'].append(f"{c}+{o:#x} {f['name']} {typ}{'x%d' % cnt if cnt > 1 else ''} ({n})")
    for c, sz in STATED_SIZES.items():
        if c in classes and not classes[c].get('size'):
            if size_of(c) <= sz:
                spec(c)['size'] = f'{sz:#x}'
                log['size set'].append(f'{c} {sz:#x}')
            else:
                log['stated size smaller than the fields (left)'].append(f'{c} {sz:#x} < {size_of(c):#x}')
    for c, off, typ, why in RETYPE:
        e = spec(c)
        for f in e.get('fields', []):
            if num(f['off']) == off and f['type'] != typ:
                log['field retyped'].append(f"{c}+{off:#x} {f['name']}: {f['type']} -> {typ} ({why})")
                f['type'] = typ
    # round P6: splits / joins / record retypes over the same bytes (SPLIT) and second names (ALIAS)
    src_text = None
    for c, lo, hi, items, why in SPLIT:
        e = spec(c)
        gone = []
        for kind in ('fields', 'members'):
            keep = []
            for f in e.get(kind, []):
                fo = num(f['off'])
                fe = fo + (f.get('size', 0) if kind == 'fields' else size_of(f['class'])) * int(f.get('count', 1) or 1)
                if lo <= fo and fe <= hi:
                    gone.append(f['name'])
                elif fo < hi and lo < fe:
                    sys.exit(f'SPLIT {c} [{lo:#x}, {hi:#x}): {f["name"]} at {fo:#x}..{fe:#x} straddles the range')
                else:
                    keep.append(f)
            e[kind] = keep
        busy = [(num(f['off']), num(f['off']) + f.get('size', 0) * int(f.get('count', 1) or 1)) for f in e.get('fields', [])]
        busy += [(num(m['off']), num(m['off']) + size_of(m['class']) * int(m.get('count', 1) or 1)) for m in e.get('members', [])]
        for name, o, typ, cnt in items:
            if typ in SIZES or typ.startswith('ptr:') or typ == 'u8*':
                sz = 4 if typ.startswith('ptr:') or typ == 'u8*' else SIZES[typ]
                f = {'off': f'{o:#x}', 'name': name, 'type': typ, 'size': sz}
                kind = 'fields'
            elif typ in classes or typ in changed:
                sz = size_of(typ)
                f = {'off': f'{o:#x}', 'name': name, 'class': typ}
                kind = 'members'
            else:
                sys.exit(f'SPLIT {c}: unknown type {typ} for {name}')
            if cnt > 1:
                f['count'] = cnt
            if any(a < o + sz * cnt and o < b for a, b in busy):
                sys.exit(f'SPLIT {c}: {name} at {o:#x} overlaps a member left outside [{lo:#x}, {hi:#x})')
            busy.append((o, o + sz * cnt))
            e.setdefault(kind, []).append(f)
            e[kind].sort(key=lambda y: num(y['off']))
        log['split'].append(f"{c} [{lo:#x}, {hi:#x}): {', '.join(gone)} -> "
                            f"{', '.join(f'{n} {t}' + (f'[{k}]' if k > 1 else '') for n, _, t, k in items)} ({why})")
        # the replaced names must not be used any more (the compile gate catches each; this lists them)
        if src_text is None:
            src_text = {}
            for p in glob.glob(os.path.join(ROOT, 'src', '**', '*.[ch]pp'), recursive=True):
                if os.sep + 'gen' + os.sep not in p:
                    src_text[p] = open(p, errors='replace').read()
        new_names = {n for n, _, _, _ in items}
        for g in gone:
            if g in new_names or not re.fullmatch(r'\w+', g):
                continue
            hits = [os.path.relpath(p, ROOT) for p, t in src_text.items() if re.search(rf'(->|\.)\s*{g}\b', t)]
            if hits:
                log['split: old name still used (check each)'].append(f"{c}.{g}: {', '.join(sorted(hits)[:8])}")
    for c, base, why in ALIAS:
        e = spec(c)
        log['alias'].append(f"{c}: fields {', '.join(f['name'] for f in e.get('fields', []))} dropped, base {base} ({why})")
        e['fields'], e['members'], e['base'], e['size'] = [], [], base, None
    for c, base, old, lo, hi, why in REBASE:
        e = spec(c)
        if e.get('base') != old or e.get('fields') or e.get('members'):
            sys.exit(f'REBASE {c}: expected an empty class on base {old}')
        o = changed.get(old) or classes[old]
        if any(lo < num(f['off']) + f.get('size', 0) * int(f.get('count', 1) or 1) and num(f['off']) < lo
               for f in o.get('fields', [])) or size_of(base) != lo or o.get('members'):
            sys.exit(f'REBASE {c}: {old} does not split at {lo:#x} / {hi:#x}')
        keep = [copy.deepcopy(f) for f in o.get('fields', []) if lo <= num(f['off']) and
                num(f['off']) + f.get('size', 0) * int(f.get('count', 1) or 1) <= hi]
        e['base'], e['fields'], e['size'] = base, keep, f'{hi:#x}'
        log['rebase'].append(f"{c}: base {old} -> {base} + {old} fields {', '.join(f['name'] for f in keep)} "
                             f"(size {hi:#x}; {why})")
    # fields the raw accesses of the game code need (round P4: re/a4/p4_sites.py -> p4_fields.py ->
    # re/layout/add_fields/*.json): additive, only into bytes no field / member of the class covers
    for p in sorted(glob.glob(os.path.join(LAYOUT, 'add_fields', '*.json'))):
        for x in json.load(open(p))['fields']:
            c, o = x['class'], num(x['off'])
            if c not in classes:
                log['add_fields: class unknown (left)'].append(f"{c}+{o:#x} {x['name']}")
                continue
            if x.get('member'):  # a record by value (round P6): an embedded member
                if x['type'] not in classes and x['type'] not in changed:
                    log['add_fields: member class unknown (left)'].append(f"{c}+{o:#x} {x['name']} {x['type']}")
                    continue
                x = dict(x, size=size_of(x['type']))
            sz = x['size'] * int(x.get('count', 1) or 1)
            e = spec(c)
            busy = [(num(f['off']), num(f['off']) + f.get('size', 0) * int(f.get('count', 1) or 1)) for f in e.get('fields', [])]
            busy += [(num(m['off']), num(m['off']) + size_of(m['class']) * int(m.get('count', 1) or 1)) for m in e.get('members', [])]
            if e.get('base'):
                busy.append((0, size_of(e['base'])))
            if any(lo < o + sz and o < hi for lo, hi in busy):
                log['add_fields: would overlap (left)'].append(f"{c}+{o:#x} {x['name']}")
                continue
            if e.get('size') and o + sz > num(e['size']):
                log['add_fields: beyond the class size (left)'].append(f"{c}+{o:#x} {x['name']} size {e['size']}")
                continue
            taken = {f['name'] for f in e.get('fields', [])} | {m['name'] for m in e.get('members', [])}
            name, k = x['name'], 2
            while name in taken:
                name = f"{x['name']}_{k}"
                k += 1
            if x.get('member'):
                f, kind = {'off': f'{o:#x}', 'name': name, 'class': x['type']}, 'members'
            else:
                f, kind = {'off': f'{o:#x}', 'name': name, 'type': x['type'], 'size': x['size']}, 'fields'
            if int(x.get('count', 1) or 1) > 1:
                f['count'] = int(x['count'])
            e.setdefault(kind, []).append(f)
            e[kind].sort(key=lambda y: num(y['off']))
            log['add_fields: field added'].append(f"{c}+{o:#x} {name} {x['type']} ({os.path.basename(p)}: {x.get('why', '')})")
    # member arrays the specs spelled out one by one (x64: index them with the compiler's stride)
    for c, base in MEMBER_ARRAYS:
        e = spec(c)
        run = [m for m in e.get('members', []) if re.fullmatch(re.escape(base) + r'_\d+', m['name'])]
        if len(run) > 1:
            sz = size_of(run[0]['class'])
            offs = sorted(num(m['off']) for m in run)
            if all(b - a == sz for a, b in zip(offs, offs[1:])):
                e['members'] = [m for m in e['members'] if m not in run] + [
                    {'off': f'{offs[0]:#x}', 'name': base, 'class': run[0]['class'], 'count': len(run)}]
                e['members'].sort(key=lambda m: num(m['off']))
                log['member array'].append(f'{c}.{base}: {run[0]["class"]}[{len(run)}]')
    for c in changed.values():
        c['evidence'] = (c.get('evidence', '') + ' | r5: data-image review (re/image/spec, to_layout.py)').strip(' |')
    out = {'group': 'r5_image', 'classes': new_classes + [changed[k] for k in sorted(changed)],
           'functions': {}, 'notes': 'generated by re/image/to_layout.py from re/image/spec/i1..i4.json'}
    os.makedirs(os.path.join(HERE, 'out'), exist_ok=True)
    with open(os.path.join(HERE, 'out', 'to_layout.txt'), 'w') as w:
        for k, rows in log.items():
            w.write(f'== {k} ({len(rows)})\n' + ''.join(f'  {r}\n' for r in rows))
    for k, rows in log.items():
        print(f'{k}: {len(rows)}')
    print(f'{len(new_classes)} new classes, {len(changed)} restated')
    if not a.dry_run:
        json.dump(out, open(os.path.join(LAYOUT, 'spec', 'r5_image.json'), 'w'), indent=1)


if __name__ == '__main__':
    main()
