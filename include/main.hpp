#pragma once
#include <string>
#include <optional>

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
const Logger& getLogger();

// Attemps to load the SO file with the specified name, and returns if there was an error loading it
// Useful for finding the reason that mods aren't loading.
std::optional<std::string> tryLoadingLibrary(std::string name);

struct LibraryLoadInfo {
    std::string libraryName;
    std::optional<std::string> errorMessage = std::nullopt;
};

// Attempts to load all of the libraries in the folder path, and returns if there were any errors
std::vector<LibraryLoadInfo> checkLibraryLoadStatus(std::string path);