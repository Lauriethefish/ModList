#include "library_utils.hpp"
#include "main.hpp"

#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

std::optional<std::string> AttemptLoadLibrary(std::string path) {
    dlerror(); // Clear any preexisting errors
    dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW); // Attempt to open the SO file

    // Return an error if there was one
    char* error = dlerror();
    return error ? std::optional(std::string(error)) : std::nullopt;
}

std::vector<LibraryLoadInfo> AttemptLoadLibraries(std::string path) {
    getLogger().info("Checking for libraries in path: " + path);
    std::vector<LibraryLoadInfo> result;

    struct dirent* dp;
    DIR* dir = opendir(path.c_str());
    while ((dp = readdir(dir)) != nullptr) { // Loop through all files until we run out
        // Find all of the files that end in .so
        if (strlen(dp->d_name) > 3 && !strcmp(dp->d_name + strlen(dp->d_name) - 3, ".so")) {        
            getLogger().debug("Checking library " + std::string(dp->d_name));

            // Make sure to use the modloader's temporary directory with the correct application ID
            std::string libraryPath = string_format("data/data/%s/%s", Modloader::getApplicationId().c_str(), dp->d_name);
            std::optional<std::string> failReason = AttemptLoadLibrary(libraryPath);

            // Add the load info to the vector
            LibraryLoadInfo loadInfo;
            loadInfo.libraryName = dp->d_name;
            loadInfo.errorMessage = failReason;
            result.push_back(loadInfo);
        }
    }
    closedir(dir); // Make sure to close the directory afterwards

    return result;
}