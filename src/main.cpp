#include "main.hpp"
#include "SettingsViewController.hpp"
using namespace ModList;

#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"

#include "beatsaber-hook/shared/utils/utils-functions.h"
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    // Register our mod settings menu
    QuestUI::Init();
    custom_types::Register::RegisterType<SettingsViewController>();
    QuestUI::Register::RegisterModSettingsViewController<SettingsViewController*>(modInfo);

    getLogger().info("Installing hooks...");
    // Install our hooks (none defined yet)
    getLogger().info("Installed all hooks!");
}

std::optional<std::string> checkSOFailReason(std::string name) {
    dlerror(); // Clear any preexisting errors
    dlopen(name.c_str(), RTLD_LOCAL | RTLD_NOW); // Attempt to open the SO file

    // Return an error if there was one
    char* error = dlerror();
    return error ? std::optional(std::string(error)) : std::nullopt;
}

std::vector<LibraryLoadInfo> checkLibraryLoadStatus(std::string path) {
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
            std::optional<std::string> failReason = checkSOFailReason(libraryPath);

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