#include "library_utils.hpp"
#include "main.hpp"

#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

std::optional<std::string> AttemptLoadLibrary(const std::string& path) {
    dlerror(); // Clear any preexisting errors
    dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW); // Attempt to open the SO file

    // Return an error if there was one
    char* error = dlerror();
    return error ? std::optional(std::string(error)) : std::nullopt;
}

std::unordered_map<std::string, std::optional<std::string>> AttemptLoadLibraries(const std::string& path) {
    getLogger().info("Checking for libraries in path: %s", path.c_str());
    std::unordered_map<std::string, std::optional<std::string>> result;

    std::string modloaderDestinationPath = Modloader::getDestinationPath();

    struct dirent* dp;
    DIR* dir = opendir(path.c_str());
    while ((dp = readdir(dir)) != nullptr) { // Loop through all files until we run out
        // Find all of the files that end in .so
        if (strlen(dp->d_name) > 3 && !strcmp(dp->d_name + strlen(dp->d_name) - 3, ".so")) {        
            getLogger().debug("Checking library %s", dp->d_name);

            // Make sure to use the modloader's temporary directory with the correct application ID
            std::string libraryPath = modloaderDestinationPath + dp->d_name;
            std::optional<std::string> failReason = AttemptLoadLibrary(libraryPath);

            // Add the load info to the map
            result[dp->d_name] = failReason;
        }
    }
    closedir(dir); // Make sure to close the directory afterwards

    return result;
}

static std::optional<LibraryLoadInfo> failedLibraries;
static std::optional<LibraryLoadInfo> failedMods;

LibraryLoadInfo& GetModloaderLibsLoadInfo() {
    static std::string libsPath = string_format("sdcard/Android/data/%s/files/libs", Modloader::getApplicationId().c_str());
    
    if(!failedLibraries.has_value()) {
        failedLibraries = AttemptLoadLibraries(libsPath);
    }

    return *failedLibraries;
}

LibraryLoadInfo& GetModsLoadInfo() {
    static std::string modsPath = string_format("sdcard/Android/data/%s/files/mods", Modloader::getApplicationId().c_str());
    
    if(!failedMods.has_value()) {
        failedMods.emplace(AttemptLoadLibraries(modsPath));
    }

    return *failedMods;
}

