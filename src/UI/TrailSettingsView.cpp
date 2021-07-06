#include "UI/TrailSettingsView.hpp"
#include "monkecomputer/shared/ViewLib/CustomComputer.hpp"
#include "monkecomputer/shared/ViewLib/MonkeWatch.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"
#include "MonkeTrail.hpp"
#include "config.hpp"

DEFINE_TYPE(Trail::TrailSettingsView);

extern Logger& getLogger();

using namespace GorillaUI;

namespace Trail
{
    void TrailSettingsView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);

        settingSelector->max = 2;
    }

    void TrailSettingsView::DidActivate(bool firstActivation)
    {
        std::function<void(int)> fun = std::bind(&TrailSettingsView::OnEnter, this, std::placeholders::_1);
        settingSelector->selectionCallback = fun;
        Redraw();
    }

    void TrailSettingsView::OnEnter(int index)
    {
        if (index == 0) 
        {
            config.enabled ^= 1;
        }
        else if (index == 3)
        {
            Trail::ClearAll();
        }
    }

    void TrailSettingsView::Redraw()
    {
        text = "";

        DrawHeader();
        DrawSettings();

        if (computer)
            CustomComputer::Redraw();
        else if (watch)
            MonkeWatch::Redraw();
        SaveConfig();
    }
    
    void TrailSettingsView::DrawHeader()
    {
        text += "<color=#ffff00>== <color=#fdfdfd>Trail Settings</color> ==</color>\n";
    }
    
    void TrailSettingsView::DrawSettings()
    {
        text += "  Trails are:\n";
        text += settingSelector->currentSelectionIndex == 0 ? " <color=#fd0000>></color> " : "   ";
        text += config.enabled ? "<color=#00fd00>enabled</color>" : "<color=#fd0000>disabled</color>";
        text += "\n";
        text += "  Clean up:\n";
        text += settingSelector->currentSelectionIndex == 1 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        text += " RUN CLEANUP ";
        text += " <color=#AADDAA>></color>";
    }
    
    void TrailSettingsView::OnKeyPressed(int value)
    {
        EKeyboardKey key = (EKeyboardKey)value;
        if (!settingSelector->HandleKey(key)) // if it was not up/down/enter
        {
            switch (settingSelector->currentSelectionIndex)
            {
                default:
                    break;
            }

        }
        Redraw();
    }
}
