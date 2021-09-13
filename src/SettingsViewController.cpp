
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
using namespace QuestUI;

#include "TMPro/TextMeshProUGUI.hpp"
using namespace TMPro;

#include "modloader/shared/modloader.hpp"

DEFINE_TYPE(ModList, SettingsViewController);

void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(!(firstActivation && addedToHierarchy))  {return;}

    VerticalLayoutGroup* layout = BeatSaberUI::CreateVerticalLayoutGroup(this->get_rectTransform());
    layout->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(90.0f);
    layout->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
    layout->set_childControlHeight(true);
    layout->set_childForceExpandHeight(false);

    UnityEngine::UI::Toggle* toggle = BeatSaberUI::CreateToggle(layout->get_rectTransform(), "Show failed mods pop-up in main menu", getConfig().config["showFailedModsOnGameStart"].GetBool(), [](bool newValue){
        getConfig().config["showFailedModsOnGameStart"] = newValue;
        getConfig().Write(); // Save the config
    });
}