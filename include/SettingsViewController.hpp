#pragma once

#include "HMUI/ViewController.hpp"

#include "custom-types/shared/macros.hpp"

// Declare a ViewController to let us create UI in the mod settings menu
DECLARE_CLASS_CODEGEN(ModList, SettingsViewController, HMUI::ViewController,
    // Override DidActivate, which is called whenever you enter the menu
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
)