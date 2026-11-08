// Windows Media's IWMSyncReader (wmvcore.dll) for the game's movie / music player, on FFmpeg (media_file.hpp).
// See wm_reader.cpp.
#pragma once

#include <cstdint>

namespace joan::media {

/// [Video] Trace=1: every movie update (Video::update) and frame copy is logged to openjoan.log.
bool trace_video();
/// One line of that trace, to openjoan_video_ffmpeg.log.
void trace_line(const char* fmt, ...);
/// The address of our WMCreateSyncReader(IUnknown*, DWORD rights, IWMSyncReader** out) (stdcall), what the game
/// would get from GetProcAddress(wmvcore.dll, "WMCreateSyncReader").
std::uintptr_t create_sync_reader_address();

}  // namespace joan::media
