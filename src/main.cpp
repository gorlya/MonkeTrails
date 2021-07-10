#include "config.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "modloader/shared/modloader.hpp"
#include "monkecomputer/shared/CommandRegister.hpp"
#include "monkecomputer/shared/Register.hpp"
#include "monkecomputer/shared/CustomQueues.hpp"
#include "monkecomputer/shared/GorillaUI.hpp"

#include "GlobalNamespace/GorillaTagger.hpp"
#include "GlobalNamespace/VRRig.hpp"
#include "GlobalNamespace/OVRInput.hpp"

#include "UnityEngine/Material.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

#include "Photon/Realtime/Player.hpp"

#include "Photon/Pun/PhotonNetwork.hpp"
#include "Photon/Pun/PhotonView.hpp"
#include "Photon/Pun/RpcTarget.hpp"
#include "Photon/Pun/PhotonMessageInfo.hpp"
#include "Photon/Realtime/Player.hpp"

#include "GorillaLocomotion/Player.hpp"

#include "custom-types/shared/register.hpp"
#include "MonkeTrail.hpp"
#include "UI/TrailSettingsView.hpp"

#include "gorilla-utils/shared/GorillaUtils.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include "gorilla-utils/shared/Utils/Player.hpp"
#include "gorilla-utils/shared/Callbacks/InRoomCallbacks.hpp"
#include "gorilla-utils/shared/Callbacks/MatchMakingCallbacks.hpp"
#include <map>
#include <deque>

Logger& getLogger()
{
    static Logger* logger = new Logger({ID, VERSION}, LoggerOptions(false, true));
    return *logger;
}

template<class T>
T max (T first, T second)
{
    return first > second ? first : second;
}

template<class T>
T max (T first, T second, T third)
{
    return max(max(first, second), third);
}


using namespace UnityEngine;

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
}

static int* indexptr = nullptr;
extern "C" void load()
{
    time_t t;
    srand(time(&t));

    if (!LoadConfig()) SaveConfig();

    GorillaUI::Innit();

    using namespace GlobalNamespace;

    using namespace Photon::Pun;
    using namespace Photon::Realtime;

    Logger& logger = getLogger();

    GorillaUtils::MatchMakingCallbacks::add_OnLeftRoom([&](){ Trail::ClearAll(); });
    GorillaUtils::MatchMakingCallbacks::add_OnJoinedRoom([&](){
      Trail::ClearAll();
			Il2CppObject* currentRoom = CRASH_UNLESS(il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "get_CurrentRoom"));
			if (currentRoom) {
				Trail::setModded(!CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(currentRoom, "get_IsVisible")));
			}

      GorillaUtils::Player::SetProperty<bool>(Photon::Pun::PhotonNetwork::get_LocalPlayer(), "trailEnabled", config.trailenabled);
      Trail::updateMonkes();
    });
    GorillaUtils::InRoomCallbacks::add_OnPlayerPropertiesUpdate([&](auto player, auto){
        Trail::markMonke(player);
    });

    custom_types::Register::RegisterType<Trail::MonkeTrail>();
    custom_types::Register::RegisterType<Trail::TrailSettingsView>();

    GorillaUI::Register::RegisterSettingsView<Trail::TrailSettingsView*>("Trail", VERSION);
    GorillaUI::Register::RegisterWatchView<Trail::TrailSettingsView*>("Trail", VERSION);

    GorillaUI::CommandRegister::RegisterCommand("marker", [](std::vector<std::string> args) -> std::string {

        return "Not implemented yet.";
    }, "Allows you to change the way markers work, using arguments to specify what is being changed\n");
}
