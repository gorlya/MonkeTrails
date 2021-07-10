#include "UI/TrailSettingsView.hpp"
#include "monkecomputer/shared/ViewLib/CustomComputer.hpp"
#include "monkecomputer/shared/ViewLib/MonkeWatch.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"
#include "MonkeTrail.hpp"
#include "config.hpp"

#include <vector>

DEFINE_TYPE(Trail::TrailSettingsView);

extern Logger& getLogger();

using namespace GorillaUI;

namespace Trail
{
    void TrailSettingsView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);
        trailModeSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);
        trailSizeSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);
        trailWidthSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);

        settingSelector->max = 5;
        trailModeSelector->max = 2;
        trailSizeSelector->max = 4;
        trailWidthSelector->max = 4;

        trailModeSelector->currentSelectionIndex = config.trailmode;;
        trailSizeSelector->currentSelectionIndex = config.trailsize;
        trailWidthSelector->currentSelectionIndex = config.trailwidth;
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
            Trail::ClearAll();
        }
        else if (index == 4)
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

    std::string renderSelector(auto modeSelector, std::string label, std::vector<std::string> options, bool enabled) {
        std::string ret = "";
        ret += label;
        ret += "  \n";
        ret += enabled ? " <color=#fd0000>></color> " : "   ";
        ret += "<color=#AADDAA><</color> ";

        int val = modeSelector->currentSelectionIndex;
        if (val < options.size()) {
          ret += options[val];
        } else {
          ret += "???";
        }

        ret += " <color=#AADDAA>></color>";
        ret += "\n";
        return ret;
    }

    void TrailSettingsView::DrawSettings()
    {
        text += "  Trails are:\n";
        text += settingSelector->currentSelectionIndex == 0 ? " <color=#fd0000>></color> " : "   ";
        text += config.enabled ? "<color=#00fd00>enabled</color>" : "<color=#fd0000>disabled</color>";

        int index = settingSelector->currentSelectionIndex;

        text += "\n";
        text += renderSelector(trailModeSelector, "Trail Mode:", { "ALL" }, index == 1);
        text += renderSelector(trailSizeSelector, "Trail Length:", { "S", "M", "L"}, index == 2);
        text += renderSelector(trailWidthSelector, "Trail Width:", { "S", "M", "L"}, index == 3);

        text += "  Clean up:\n";
        text += settingSelector->currentSelectionIndex == 4 ? " <color=#fd0000>></color> " : "   ";
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
                case 1:
                  trailModeSelector->HandleKey(key);
                  break;
                case 2:
                  trailSizeSelector->HandleKey(key);
                  break;
                case 3:
                  trailWidthSelector->HandleKey(key);
                  break;
                default:
                  break;
            }

        }
        config.trailmode = trailModeSelector->currentSelectionIndex;
        config.trailsize = trailSizeSelector->currentSelectionIndex;
        config.trailwidth = trailWidthSelector->currentSelectionIndex;
        Redraw();
        Trail::updateMonkes();
    }
}
