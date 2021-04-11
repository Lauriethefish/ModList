#pragma once
#include <optional>
#include <string>
#include <vector>

struct LibraryLoadInfo {
    std::string libraryName; // Name of the SO file with .so extension, not the full path
    std::optional<std::string> errorMessage; // nullopt if loading succeeded
};

// Attempts to load the SO file with path name.
// Returns nullopt if the load succeeded, otherwise returns the error from dlopen
std::optional<std::string> AttemptLoadLibrary(std::string name);

// Loops over all the SO files in the folder path, and attempts to load them
std::vector<LibraryLoadInfo> AttemptLoadLibraries(std::string path);
