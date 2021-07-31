
#include "SettingsViewController.hpp"
#include "main.hpp"
#include "library_utils.hpp"
using namespace ModList;

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/TextAnchor.hpp"
using namespace UnityEngine::UI;

#include "TMPro/TextAlignmentOptions.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
using namespace QuestUI::BeatSaberUI;

#include "modloader/shared/modloader.hpp"

DEFINE_TYPE(ModList, SettingsViewController);

struct ListItem {
    std::string content;
    std::string hoverHint;
};

void CreateListWithTitle(UnityEngine::Transform* parent, std::string title, std::vector<ListItem> content) {
    VerticalLayoutGroup* layout = CreateVerticalLayoutGroup(parent);
    layout->set_spacing(0.5);

    // Create a layout for displaying the title.
    VerticalLayoutGroup* titleLayout = CreateVerticalLayoutGroup(layout->get_rectTransform());
    CreateText(titleLayout->get_rectTransform(), title)->set_alignment(TMPro::TextAlignmentOptions::Center);
    CreateText(titleLayout->get_rectTransform(), "____________")->set_alignment(TMPro::TextAlignmentOptions::Center); // This is a botch, but works alright.
    titleLayout->GetComponent<LayoutElement*>()->set_minWidth(25.0); // Make sure the list has a set width.

    // Create a layout for the list itself
    VerticalLayoutGroup* listLayout = CreateVerticalLayoutGroup(layout->get_rectTransform());
    listLayout->GetComponent<LayoutElement*>()->set_minWidth(25.0); // Make sure the list has a set width.
    listLayout->GetComponent<LayoutElement*>()->set_minHeight(60.0); // Make sure the list takes up most of the space

    
    // Add some padding so that the messages aren't totally squished
    titleLayout->set_padding(UnityEngine::RectOffset::New_ctor(1, 1, 1, 1));
    listLayout->set_padding(UnityEngine::RectOffset::New_ctor(1, 1, 1, 1));

    // Make sure the list items are in the top left
    listLayout->set_childAlignment(UnityEngine::TextAnchor::UpperLeft);
    listLayout->set_childForceExpandHeight(false);
    listLayout->set_childControlHeight(true);

    // Create a line of text for each in the list
    for(ListItem element : content) {
        TMPro::TextMeshProUGUI* text = CreateText(listLayout->get_rectTransform(), element.content);
        // Add a hover hint if there is one
        if(element.hoverHint != "") {
            AddHoverHint(text->get_gameObject(), element.hoverHint);
        }
        text->set_fontSize(2.3f);
    }
}

void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(!(firstActivation && addedToHierarchy))  {return;}

    // Allow the lists to be scrolled
    UnityEngine::GameObject* scrollView = CreateScrollableSettingsContainer(get_rectTransform());

    HorizontalLayoutGroup* mainLayout = CreateHorizontalLayoutGroup(scrollView->get_transform());
    mainLayout->set_childAlignment(UnityEngine::TextAnchor::MiddleCenter); // The lists should be centred

    // Check to see which libraries loaded/failed to load
    getLogger().info("Checking library load info.");

    // Find the path with the correct application ID
    std::string librariesPath = string_format("/sdcard/Android/data/%s/files/libs", Modloader::getApplicationId().c_str());
    LibraryLoadInfo libraryLoadInfo = AttemptLoadLibraries(librariesPath);
    std::vector<ListItem> librariesList;

    for(std::pair<std::string, std::optional<std::string>> libraryLoadPair : libraryLoadInfo) {
        if(libraryLoadPair.second.has_value()) {
            // If there was an error loading the library, display it in red
            getLogger().debug("Adding failed library %s", libraryLoadPair.first.c_str());
            ListItem item;
            item.content = "<color=red>" + libraryLoadPair.first + " (failed)";
            item.hoverHint = *libraryLoadPair.second; // Allow you to hover over the mod to see the fail reason
            librariesList.push_back(item);
        }   else    {
            // Otherwise, make the library name green
            getLogger().debug("Adding successful library %s", libraryLoadPair.first.c_str());
            ListItem item;
            item.content = "<color=green>" + libraryLoadPair.first;
            librariesList.push_back(item);
        }
        
    }

    getLogger().info("Adding loaded mods . . .");
    // Find the list of all loaded mods
    std::vector<ListItem> loadedMods;
    for(const std::pair<std::string, const Mod>& modEntry : Modloader::getMods()) {
        const Mod& mod = modEntry.second;
        getLogger().info("Adding mod %s", mod.info.id.c_str());
        ListItem item;
        item.content = "<color=green>" + mod.info.id + "</color><color=white> v" + mod.info.version;
        loadedMods.push_back(item);
    }

    // Find the info about why the libraries in the mods directory loaded/didn't load
    // Make sure to find the mods path with the correct application ID
    std::string modsPath = string_format("sdcard/Android/data/%s/files/mods", Modloader::getApplicationId().c_str());
    LibraryLoadInfo modsLoadInfo = AttemptLoadLibraries(modsPath);

    std::vector<ListItem> failedMods;
    getLogger().info("Checking for failed mods . . .");
    for(std::pair<std::string, std::optional<std::string>> modLoadPair : modsLoadInfo) {
        // If there was an error loading the library, add it to the list in red
        if(modLoadPair.second.has_value()) {
            getLogger().debug("Adding failed mod %s", modLoadPair.first.c_str());
            ListItem item;
            item.content = "<color=red>" + modLoadPair.first + " (failed)";
            item.hoverHint = *modLoadPair.second; // Allow you to hover over the mod to see the fail reason
            failedMods.push_back(item);
        }
    }

    // Create lists for each group
    CreateListWithTitle(mainLayout->get_rectTransform(), "Loaded Mods", loadedMods);
    CreateListWithTitle(mainLayout->get_rectTransform(), "Failed Mods", failedMods);
    CreateListWithTitle(mainLayout->get_rectTransform(), "Libraries", librariesList);
}