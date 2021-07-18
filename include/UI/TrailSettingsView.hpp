#pragma once

#include "custom-types/shared/macros.hpp"
#include "monkecomputer/shared/ViewLib/View.hpp"
#include "monkecomputer/shared/InputHandlers/UISelectionHandler.hpp"
#include "monkecomputer/shared/InputHandlers/UIToggleInputHandler.hpp"

DECLARE_CLASS_CODEGEN(Trail, TrailSettingsView, GorillaUI::Components::View,
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation);
    DECLARE_INSTANCE_METHOD(void, Redraw);
    DECLARE_INSTANCE_METHOD(void, DrawHeader);
    DECLARE_INSTANCE_METHOD(void, DrawSettings);
    DECLARE_INSTANCE_METHOD(void, OnKeyPressed, int key);
    DECLARE_INSTANCE_METHOD(void, OnEnter, int key);

    public:
        GorillaUI::UISelectionHandler* settingSelector = nullptr;
        
)
