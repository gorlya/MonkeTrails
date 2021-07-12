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
  int changed = 0;
};

namespace Trail
{

    std::vector<Selector> selectors;
    void makeSelector(std::string label, std::vector<std::string> options, int *value) {
        auto selector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);
        selector->max = options.size();
        selector->currentSelectionIndex = *value;

        Selector ret = {};
        ret.handler = selector;
        ret.options = options;
        ret.value = value;
        ret.label = label;

        selectors.emplace_back(ret);
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


    void TrailSettingsView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);
        makeSelector("Trail Length:", { "S", "M", "L", "???"}, &config.trailsize);
        makeSelector("Trail Width:", { "S", "M", "L", "???"}, &config.trailwidth);
        makeSelector("Public Trail?", { "N", "Y" }, &config.trailpublic);

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
        int index = settingSelector->currentSelectionIndex;
        text += "  Trails are:\n";
        text += index == 0 ? " <color=#fd0000>></color> " : "   ";
        text += config.enabled ? "<color=#00fd00>enabled</color>" : "<color=#fd0000>disabled</color>";

        text += "\n";
        for (int i = 0; i < selectors.size(); i++) {
          text += renderSelector(selectors[i], index == i+1);
        }
    }

    void TrailSettingsView::OnKeyPressed(int value)
    {
        EKeyboardKey key = (EKeyboardKey)value;
        if (!settingSelector->HandleKey(key)) // if it was not up/down/enter
        {
            if (settingSelector->currentSelectionIndex <= selectors.size()) {
              selectors[settingSelector->currentSelectionIndex-1].handler->HandleKey(key);
            }

        }

        bool needBroadcast = false;
        for (auto &selector : selectors) {
          selector.changed = *selector.value != selector.handler->currentSelectionIndex;
          *selector.value = selector.handler->currentSelectionIndex;

          if (selector.changed) {
            if (selector.value == &config.trailsize || selector.value == &config.trailpublic) {
              needBroadcast = true;
            }
          }
        }

        for (auto &selector : selectors) {
          selector.changed = false;
        }


        if (needBroadcast) {
          if (config.trailpublic) {
              GorillaUtils::Player::SetProperty<int>(Photon::Pun::PhotonNetwork::get_LocalPlayer(),
                "trailSize", config.trailsize);
          } else {
              GorillaUtils::Player::SetProperty<int>(Photon::Pun::PhotonNetwork::get_LocalPlayer(),
                "trailSize", -1);

          }
        }

        Redraw();
        Trail::updateMonkes();
    }
}
