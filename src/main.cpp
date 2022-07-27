#include "main.hpp"
#include "library_utils.hpp"

#include "SettingsViewController.hpp"
#include "ModListViewController.hpp"
using namespace ModList;

#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
using namespace QuestUI;

#include "beatsaber-hook/shared/utils/utils-functions.h"

#include "GlobalNamespace/MainMenuViewController.hpp"
using namespace GlobalNamespace;

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/TextAnchor.hpp"
using namespace UnityEngine::UI;

#include "TMPro/TextMeshProUGUI.hpp"
#include "TMPro/TextAlignmentOptions.hpp"
using namespace TMPro;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static auto* logger = new Logger(modInfo);
    return *logger;
}

// Displays a modal view if mods fail to load showing why
MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &MainMenuViewController::DidActivate, void, MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    getLogger().info("MainMenuViewController_DidActivate");
    if(!firstActivation) {
        getLogger().info("Not first activation, not displaying modal");
        return;
    }

    if(!getConfig().config["showFailedModsOnGameStart"].GetBool()) {
        getLogger().info("Showing failed mods on game start is disabled! Returning");
        return;
    }

    getLogger().info("Checking for failed mods . . .");
    LibraryLoadInfo& modsLoadInfo = GetModsLoadInfo();

    std::unordered_map<std::string, std::string> failedMods;
    for(std::pair<std::string, std::optional<std::string>> modPair : modsLoadInfo) {
        if(modPair.second.has_value()) {
            failedMods[modPair.first] = *modPair.second;
        }
    }

    getLogger().info("%lu mods failed to load", failedMods.size());
    if(failedMods.empty()) {
        getLogger().info("All mods loaded successfully, not showing fail dialog");
        return;
    }

    getLogger().info("Constructing fail dialog . . .");

    HMUI::ModalView* modalView = BeatSaberUI::CreateModal(self->get_transform(), UnityEngine::Vector2(70.0f, 70.0f), [](HMUI::ModalView* modalView){
        getLogger().info("Fail dialog closed, destroying modal view!");
        UnityEngine::GameObject::Destroy(modalView->get_gameObject());
    });
    UnityEngine::Transform* modalTransform = modalView->get_transform();
    VerticalLayoutGroup* layout = BeatSaberUI::CreateVerticalLayoutGroup(modalTransform);
    UnityEngine::RectTransform* layoutTransform = layout->get_rectTransform();

    layout->set_padding(UnityEngine::RectOffset::New_ctor(2, 2, 4, 4));
    layout->set_childAlignment(UnityEngine::TextAnchor::UpperLeft);
    layout->set_childControlHeight(true);
    layout->set_childForceExpandHeight(false);
    layout->set_childControlWidth(false);
    layout->set_childForceExpandWidth(true);

    std::string failedModsText;
    // Make sure to adjust for the plural!
    if(failedMods.size() > 1) {
        failedModsText = string_format("%lu mods failed to load!", failedMods.size());
    }   else    {
        failedModsText = string_format("%lu mod failed to load!", failedMods.size());
    }

    TextMeshProUGUI* titleText = BeatSaberUI::CreateText(layoutTransform, failedModsText);
    titleText->set_fontSize(5.0f);
    titleText->set_alignment(TextAlignmentOptions::TopLeft);

    BeatSaberUI::CreateText(layoutTransform, "_______________________________");

    // Add the failed mods to the GUI
    for(std::pair<std::string, std::string> failedMod : failedMods) {
        TextMeshProUGUI* modText = BeatSaberUI::CreateText(layoutTransform, string_format("<color=red>%s</color> didn't load because: \n%s", failedMod.first.c_str(), failedMod.second.c_str()));
        modText->set_overflowMode(TextOverflowModes::Ellipsis);
        modText->set_fontSize(3.5f);

        BeatSaberUI::AddHoverHint(modText->get_gameObject(), failedMod.second); // Show the full fail reason in a hover hint, since there most likely won't be enough space in the modal view
    }

    getLogger().info("Showing fail dialog . . .");
    modalView->Show(true, false, nullptr);
}

void ConstructConfig() {
    ConfigDocument& config = getConfig().config;
    auto& alloc = config.GetAllocator();
    if(!config.HasMember("showFailedModsOnGameStart")) {
        config.AddMember("showFailedModsOnGameStart", true, alloc);
        getConfig().Write();
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    ConstructConfig(); // Add properties to the config if missing

    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    // Register our mod settings menu
    QuestUI::Init();
    custom_types::Register::AutoRegister();
    QuestUI::Register::RegisterMainMenuModSettingsViewController<ModListViewController*>(modInfo, "Loaded Mods");
    QuestUI::Register::RegisterModSettingsViewController<SettingsViewController*>(modInfo, "Mod List Settings");

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), MainMenuViewController_DidActivate);
    getLogger().info("Installed all hooks!");
}