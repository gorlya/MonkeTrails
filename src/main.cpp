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
#include "MonkeMarker.hpp"
#include "MonkeTrail.hpp"
#include "UI/MarkerSettingsView.hpp"

#include "gorilla-utils/shared/GorillaUtils.hpp"
#include "gorilla-utils/shared/Utils/Player.hpp"
#include "gorilla-utils/shared/Callbacks/InRoomCallbacks.hpp"
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

MAKE_HOOK_OFFSETLESS(Player_Awake, void, GorillaLocomotion::Player* self)
{
    Player_Awake(self);

    Transform* leftHand = self->get_transform()->Find(il2cpp_utils::createcsstr("TurnParent/LeftHand Controller"));
    Transform* rightHand = self->get_transform()->Find(il2cpp_utils::createcsstr("TurnParent/RightHand Controller"));

    Marker::MonkeMarker* leftSelector = leftHand->get_gameObject()->AddComponent<Marker::MonkeMarker*>();
    Marker::MonkeMarker* rightSelector = rightHand->get_gameObject()->AddComponent<Marker::MonkeMarker*>();
    rightSelector->isRight = true;

}

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
    INSTALL_HOOK_OFFSETLESS(logger, Player_Awake, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Awake", 0));

    GorillaUtils::InRoomCallbacks::add_OnPlayerPropertiesUpdate([&](auto player, auto){
        VRRig* monkeRig = GorillaUtils::Player::findPlayerVRRig(player);
        if (monkeRig != nullptr) {
            int playerId = player->actorNumber;
            UnityEngine::GameObject* monkeHead = monkeRig->headMesh;
            auto mat = monkeRig->mainSkin->get_material();
            Trail::MonkeTrail* trail = monkeHead->GetComponent<Trail::MonkeTrail*>();
            if (trail == nullptr) {
                trail = monkeHead->AddComponent<Trail::MonkeTrail*>();
            }

            if (trail != nullptr) {
                trail->playerId = playerId;
                trail->material = mat;
            }

        }
    });

    custom_types::Register::RegisterType<Marker::MonkeMarker>();
    custom_types::Register::RegisterType<Trail::MonkeTrail>();
    custom_types::Register::RegisterType<Marker::MarkerSettingsView>();

    GorillaUI::Register::RegisterSettingsView<Marker::MarkerSettingsView*>("Marker", VERSION);
    GorillaUI::Register::RegisterWatchView<Marker::MarkerSettingsView*>("Marker", VERSION);

    GorillaUI::CommandRegister::RegisterCommand("marker", [](std::vector<std::string> args) -> std::string {
        return "Not implemented yet.";
    }, "Allows you to change the way markers work, using arguments to specify what is being changed\n");
}
