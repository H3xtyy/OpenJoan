// C++ exceptions of the game (MSVC 6 `throw` / `try` in the original, plain C++ here).
//
// The original throws two types, both through _CxxThrowException 0x007b71ac with these ThrowInfos:
//   char*              0x009efbf8, catchable types {char* ".PAD", void* ".PAX"}  (error messages)
//   EventSystemToken*  0x009f0168, catchable types {EventSystemToken* ".PAVEventSystemToken@@", void* ".PAX"}
//                      (the event-script lexer's bad string token, ScriptLexer::classify 0x00662330)
// and catches them in three functions: ScriptStatementCompile::compile 0x00649280 (EventSystemToken*, char*, ...),
// ParticleSystem::compile 0x00677390 and its sequence parser 0x00678320 (char*, ...). Our ports throw the same types
// with a C++ `throw` (`throw msg;` with msg a char*, `throw token;` with token an EventSystemToken*) and catch them with
// `catch (char*)` / `catch (EventSystemToken*)` / `catch (...)`: the catchable-type lists above are exactly what any
// C++ compiler gives these two pointer types (no base classes), so every handler matches what it matched in the
// original, and no catcher names void*. Pointer exceptions have no destructor (ThrowInfo pmfnUnwind 0).
// Every frame between a throw and its catcher is our C++ (no original code is left): the unwind runs the destructors
// of our locals and nothing else, so a local whose cleanup the original performs in an unwind funclet must have an
// RAII owner that calls the same destructor; no other cleanup may depend on normal return.
// MSVC 6's catch (...) also took structured exceptions (access violations ...); the ports do not: see the catchers
// (uti3d/esstate2.cpp, game/unknown_psystem.cpp) for why none can occur on their paths.
#pragma once

// The game's token class (+0 text, +4 kind, +8 line: joan::orig::as_token). Only its pointer is thrown and caught; the
// fields are read through as_token, so the class itself stays empty.
class EventSystemToken {};
