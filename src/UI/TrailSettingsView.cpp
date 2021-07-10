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

struct Selector {
  UISelectionHandler* handler;
  std::vector<std::string> options;
  int* value;
  std::string label;
};

namespace Trail
{

    Selector makeSelector(std::string label, std::vector<std::string> options, int *value) {
        auto selector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);
        selector->max = options.size();
        selector->currentSelectionIndex = *value;

        Selector ret = {};
        ret.handler = selector;
        ret.options = options;
        ret.value = value;
        ret.label = label;
        return ret;


    }

    std::vector<Selector> selectors;
    void TrailSettingsView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);
        selectors.emplace_back(makeSelector("Trail Mode:", { "ALL" }, &config.trailmode));
        selectors.emplace_back(makeSelector("Trail Length:", { "S", "M", "L"}, &config.trailsize));
        selectors.emplace_back(makeSelector("Trail Width:", { "S", "M", "L"}, &config.trailwidth));
        // selectors.emplace_back(makeSelector("Is Trail Public:", { "N", "Y" }, &config.enabled));

        settingSelector->max = selectors.size() + 1;
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
        else if (index == selectors.size())
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

    std::string renderSelector(Selector &selector, bool enabled) {
        std::string ret = "";
        ret += selector.label;
        ret += "  \n";
        ret += enabled ? " <color=#fd0000>></color> " : "   ";
        ret += "<color=#AADDAA><</color> ";

        int val = selector.handler->currentSelectionIndex;
        if (val < selector.options.size()) {
          ret += selector.options[val];
        } else {
          ret += "???";
        }

        ret += " <color=#AADDAA>></color>";
        ret += "\n";
        return ret;
    }

    void TrailSettingsView::DrawSettings()
    {
        int index = settingSelector->currentSelectionIndex;
        text += "  Trails are:\n";
        text += index == 0 ? " <color=#fd0000>></color> " : "   ";
        text += config.enabled ? "<color=#00fd00>enabled</color>" : "<color=#fd0000>disabled</color>";


        text += "\n";
        for (int i = 0; i < selectors.size(); i++) {
          text += renderSelector(selectors[i], index == i);
        }

        text += "  Clean up:\n";
        text += index == 5 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        text += " RUN CLEANUP ";
        text += " <color=#AADDAA>></color>";
    }

    void TrailSettingsView::OnKeyPressed(int value)
    {
        EKeyboardKey key = (EKeyboardKey)value;
        if (!settingSelector->HandleKey(key)) // if it was not up/down/enter
        {
            if (settingSelector->currentSelectionIndex < selectors.size()) {
              selectors[settingSelector->currentSelectionIndex].handler->HandleKey(key);
            }

        }

        for (auto &selector : selectors) {
          *selector.value = selector.handler->currentSelectionIndex;
        }

        Redraw();
        Trail::updateMonkes();
    }
}
