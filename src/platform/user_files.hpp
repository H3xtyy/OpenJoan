// SPDX-License-Identifier: GPL-3.0-or-later
// A place for the game's own files when the game folder cannot be written, as under C:\Program Files on Windows: Windows
// only redirects such writes for 32-bit programs without a manifest, so the 64-bit game gets "Error creating file". A file
// written while this is in use goes to the user's folder and is read back from there; everything else still comes from the
// game folder. A game folder that can be written changes nothing.
#pragma once

#include <filesystem>

namespace joan::platform::user_files {

/// Tests once whether the current folder (the game folder) can be written, and selects the user's folder if it cannot.
void detect();

/// The folder programs keep this user's data in, with OpenJoan's own folder in it (%LOCALAPPDATA%\\OpenJoan on Windows,
/// ~/.local/share/openjoan elsewhere); empty when the system does not say. Whether or not it is in use.
std::filesystem::path users_folder();

/// Uses `folder` for the user's files from now on; an empty path means none. For detect() and for tests.
void use_folder(const std::filesystem::path& folder);

/// Whether the user's folder is in use.
bool active();

/// The user's folder, empty when not in use.
const std::filesystem::path& folder();

/// Which file to read: the copy in the user's folder (`user`) if there is one, otherwise the game's (`game`).
std::filesystem::path for_read(const std::filesystem::path& game, const std::filesystem::path& user);

/// Where to write: `user`, with its folders created. A file that exists only in the game folder is copied there first, so that
/// writing part of it keeps the rest.
std::filesystem::path for_write(const std::filesystem::path& game, const std::filesystem::path& user);

}  // namespace joan::platform::user_files
