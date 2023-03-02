
#include "SettingsViewController.hpp"
#include "main.hpp"
#include "library_utils.hpp"
using namespace ModList;

#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/TextAnchor.hpp"
using namespace UnityEngine::UI;

#include "bsml-lite/shared/BSML.hpp"
using namespace BSML::Lite;
using namespace BSML;

using namespace TMPro;

DEFINE_TYPE(ModList, SettingsViewController);

void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(!(firstActivation && addedToHierarchy))  {return;}

    VerticalLayoutGroup* layout = CreateVerticalLayoutGroup(this->get_rectTransform());
    layout->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(90.0f);
    layout->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
    layout->set_childControlHeight(true);
    layout->set_childForceExpandHeight(false);

    CreateToggle(layout->get_rectTransform(), "Show failed mods pop-up in main menu", getConfig().config["showFailedModsOnGameStart"].GetBool(), [](bool newValue){
        getConfig().config["showFailedModsOnGameStart"] = newValue;
        getConfig().Write(); // Save the config
    });
}