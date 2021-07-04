#include "UI/PaintBallSettingsView.hpp"
#include "monkecomputer/shared/ViewLib/CustomComputer.hpp"
#include "monkecomputer/shared/ViewLib/MonkeWatch.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"
#include "config.hpp"
#include "MaterialColorCache.hpp"

DEFINE_TYPE(PaintBall::PaintBallSettingsView);

extern Logger& getLogger();

using namespace GorillaUI;

namespace PaintBall
{
    void PaintBallSettingsView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);
        monkeModeSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);
        colorModeSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);

        settingSelector->max = 4;
        monkeModeSelector->max = 3;
        colorModeSelector->max = 2;

        monkeModeSelector->currentSelectionIndex = 2; // config.monkemode;
        colorModeSelector->currentSelectionIndex = config.alwayson;
    }

    void PaintBallSettingsView::DidActivate(bool firstActivation)
    {
        std::function<void(int)> fun = std::bind(&PaintBallSettingsView::OnEnter, this, std::placeholders::_1);
        settingSelector->selectionCallback = fun;
        Redraw();
    }

    void PaintBallSettingsView::OnEnter(int index)
    {
        if (index == 0) 
        {
            config.enabled ^= 1;
            GorillaUtils::Player::SetProperty<bool>(Photon::Pun::PhotonNetwork::get_LocalPlayer(), "paintballEnabled", config.enabled);
            if (!config.enabled)
            {
                MaterialColorCache::Reset();
            }
        }
        else if (index == 3)
        {
            MaterialColorCache::Reset();
        }
    }

    void PaintBallSettingsView::Redraw()
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
    
    void PaintBallSettingsView::DrawHeader()
    {
        text += "<color=#ffff00>== <color=#fdfdfd>PaintBall Settings</color> ==</color>\n";
    }
    
    void PaintBallSettingsView::DrawSettings()
    {
        text += "  Paintball is:\n";
        text += settingSelector->currentSelectionIndex == 0 ? " <color=#fd0000>></color> " : "   ";
        text += config.enabled ? "<color=#00fd00>enabled</color>" : "<color=#fd0000>disabled</color>";
        text += "\n";

        text += "  Paint Mode:\n";
        text += settingSelector->currentSelectionIndex == 1 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        switch (monkeModeSelector->currentSelectionIndex)
        {
            case 0:
                text += "MONKEY";
                break;
            case 1:
                text += "REST";
                break;
            case 2:
                text += "ALL";
                break;
            default:
                break;
        }
        text += " <color=#AADDAA>></color>";

        text += "\n";
        text += "  Always On:\n";
        text += settingSelector->currentSelectionIndex == 2 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        switch (colorModeSelector->currentSelectionIndex)
        {
            case 0:
                text += "OFF";
                break;
            case 1:
                text += "ON";
                break;
        }
        text += " <color=#AADDAA>></color>";

        text += "\n";
        text += "  Clean up:\n";
        text += settingSelector->currentSelectionIndex == 3 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        text += " RUN CLEANUP ";
        text += " <color=#AADDAA>></color>";
    }
    
    void PaintBallSettingsView::OnKeyPressed(int value)
    {
        EKeyboardKey key = (EKeyboardKey)value;
        if (!settingSelector->HandleKey(key)) // if it was not up/down/enter
        {
            switch (settingSelector->currentSelectionIndex)
            {
                case 0:
                    break;
                case 1:
                    monkeModeSelector->HandleKey(key);
                    break;
                case 2:
                    colorModeSelector->HandleKey(key);
                    break;
                default:
                    break;
            }

            // config.monkemode = monkeModeSelector->currentSelectionIndex;
            config.alwayson = colorModeSelector->currentSelectionIndex;
        }
        Redraw();
    }
}
