#pragma once

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include "modloader/shared/modloader.hpp"

using LibraryLoadInfo = std::unordered_map<std::string, std::optional<std::string>>;


// Attempts to load the SO file with path name.
// Returns nullopt if the load succeeded, otherwise returns the error from dlopen
std::optional<std::string> AttemptLoadLibrary(const std::string& name);

// Loops over all the SO files in the folder path, and attempts to load them
// Keys are library file names (with .so included), values are fail reasons, or nullopt if loading succeeded
LibraryLoadInfo AttemptLoadLibraries(const std::string& path);


// Gets (or finds) the libraries that failed to load in this instance of the game running
// Keys are library file names (with .so included), values are fail reasons (or nullopt if loading was successful)
LibraryLoadInfo& GetModloaderLibsLoadInfo();


// Gets the mods that failed to load in this instance of the game running
// Keys are mod SO file names (with .so included), values are fail reasons (or nullopt if loading was successful)
LibraryLoadInfo& GetModsLoadInfo();