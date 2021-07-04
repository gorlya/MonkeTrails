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

#include "UnityEngine/Material.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

#include "Photon/Realtime/Player.hpp"

#include "Photon/Pun/PhotonNetwork.hpp"
#include "Photon/Pun/PhotonView.hpp"
#include "Photon/Pun/RpcTarget.hpp"
#include "Photon/Pun/PhotonMessageInfo.hpp"

#include "GorillaLocomotion/Player.hpp"

#include "custom-types/shared/register.hpp"
#include "RayCastPlayerSelector.hpp"
#include "ColorGun.hpp"
#include "EnabledCache.hpp"
#include "MaterialColorCache.hpp"
#include "UI/PaintBallSettingsView.hpp"

#include "gorilla-utils/shared/GorillaUtils.hpp"
#include "gorilla-utils/shared/Callbacks/MatchMakingCallbacks.hpp"
#include "gorilla-utils/shared/Callbacks/InRoomCallbacks.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include "gorilla-utils/shared/Utils/RPC.hpp"

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

bool allowPaintBall = true;

MAKE_HOOK_OFFSETLESS(Player_Awake, void, GorillaLocomotion::Player* self)
{
    Player_Awake(self);

    Transform* leftHand = self->get_transform()->Find(il2cpp_utils::createcsstr("TurnParent/LeftHand Controller"));
    Transform* rightHand = self->get_transform()->Find(il2cpp_utils::createcsstr("TurnParent/RightHand Controller"));

    PaintBall::RayCastPlayerSelector* leftSelector = leftHand->get_gameObject()->AddComponent<PaintBall::RayCastPlayerSelector*>();
    leftHand->get_gameObject()->AddComponent<PaintBall::Gun*>();
    PaintBall::RayCastPlayerSelector* rightSelector = rightHand->get_gameObject()->AddComponent<PaintBall::RayCastPlayerSelector*>();
    rightSelector->isRight = true;
    PaintBall::RayCastPlayerSelector::useLeftHand = false;
}

MAKE_HOOK_OFFSETLESS(VRRig_InitializeNoobMaterial, void, GlobalNamespace::VRRig* self, float red, float green, float blue, Photon::Pun::PhotonMessageInfo info)
{
    // if not in room, run normal code
    if (!Photon::Pun::PhotonNetwork::get_InRoom())
    {
        VRRig_InitializeNoobMaterial(self, red, green, blue, info);
        return;
    }

    Photon::Pun::PhotonView* photonView = Photon::Pun::PhotonView::Get(self);

    Photon::Realtime::Player* player = photonView ? photonView->get_Owner() : nullptr;
    // if enabled and allowed
    auto neon = GorillaUtils::Player::GetProperty<bool>(player, "overrideNeon");
    bool overrideNeon = neon ? *neon : false;

    auto localNeon = GorillaUtils::Player::GetProperty<bool>(Photon::Pun::PhotonNetwork::get_LocalPlayer(), "overrideNeon");
    overrideNeon = overrideNeon && (localNeon ? *localNeon : false);
    
    float maxVal = max(red, green, blue);
    
    if (maxVal > 1.0f && !overrideNeon)
    {
        red /= maxVal;
        green /= maxVal;
        blue /= maxVal;
    }
    
    VRRig_InitializeNoobMaterial(self, red, green, blue, info);
    
    // does the player that is affected have the mod enabled?
    bool paintballEnabled = PaintBall::EnabledCache::get(player);
    
    getLogger().info("overrideNeon: %d, %d\npaintballEnabled: %d", (bool)neon, overrideNeon, paintballEnabled);

    // if enabled
    // sender player is not local player
    if (paintballEnabled && !info.Sender->Equals(Photon::Pun::PhotonNetwork::get_LocalPlayer())) 
    {
        self->InitializeNoobMaterialLocal(red, green, blue);
        getLogger().info("Overriding colors on other player");

        // if local player owns this rig, update the rest
        if (Photon::Pun::PhotonNetwork::get_LocalPlayer()->Equals(player)) GorillaUtils::RPC::RPC(photonView, "InitializeNoobMaterial", Photon::Pun::RpcTarget::Others, red, green, blue);
    }
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
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_InitializeNoobMaterial, il2cpp_utils::FindMethodUnsafe("", "VRRig", "InitializeNoobMaterial", 4));

    custom_types::Register::RegisterType<PaintBall::RayCastPlayerSelector>();
    custom_types::Register::RegisterType<PaintBall::Gun>();
    custom_types::Register::RegisterType<PaintBall::PaintBallSettingsView>();

    GorillaUI::Register::RegisterSettingsView<PaintBall::PaintBallSettingsView*>("Paint Ball", VERSION);
    GorillaUI::Register::RegisterWatchView<PaintBall::PaintBallSettingsView*>("Paint Ball", VERSION);
    GorillaUI::CustomQueues::add_queue("PAINTBALL_CASUAL", "paintball", "  <size=40>A queue for paintballers, shooting eachother with colors!\n    In this queue the paintball mod will remain enabled,\n    Allowing you to duke it out with paint!\n</size>");

    GorillaUI::CommandRegister::RegisterCommand("paint", [](std::vector<std::string> args) -> std::string {
        if (args.size() == 0 ) return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";

        if (args.size() == 1)
        {
            if (args[0].find("MODE") != std::string::npos)
            {
                switch (config.monkemode)
                {
                    case 0:
                        return "  Paint mode is configured to paint only gorillas";
                        break;
                    case 1:
                        return "  Paint mode is configured to paint everything but gorillas";
                        break;
                    case 2:
                        return "  Paint mode is configured to paint everything";
                        break;
                    default:
                        return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";
                }
            }
            else if (args[0].find("COLOR") != std::string::npos)
            {
                return config.random ? "  Paint color is configured to be random" : "  Paint color is configured to be copied from your own";
            }
            else if (args[0].find("ENABLE") != std::string::npos) 
            {
                config.enabled = true;
                return "  PaintBall is now enabled";
            }
            else if (args[0].find("DISABLE") != std::string::npos)
            { 
                config.enabled = false;
                return "  PaintBall is now disabled";
            }
            else 
            {
                return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]\n  PAINT [ENABLE/DISABLE]";
            }
        }
        else
        {
            char second = args[1][0];
            if (args[0].find("MODE") != std::string::npos)
            {
                switch (second)
                {
                    case 'M':
                        config.monkemode = 0;
                        return "  Paint mode is now configured to paint only gorillas";
                        break;
                    case 'R':
                        config.monkemode = 1;
                        return "  Paint mode is now configured to paint everything but gorillas";
                        break;
                    case 'A':
                        config.monkemode = 2;
                        return "  Paint mode is now configured to paint everything";
                        break;
                    default:
                        return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]";
                }
            }
            else if (args[0].find("COLOR") != std::string::npos)
            {
                switch (second)
                {
                    case 'R':
                        config.random = true;
                        break;
                    case 'O':
                        config.random = false;
                        break;
                    default:
                        return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";
                }
                return config.random ? "  Paint color is configured to be random" : "  Paint color is configured to be copied from your own";
            }
            else if (args[0].find("ENABLE") != std::string::npos) 
            {
                config.enabled = true;
                return "  PaintBall is now enabled";
            }
            else if (args[0].find("DISABLE") != std::string::npos)
            { 
                config.enabled = false;
                return "  PaintBall is now disabled";
            }
            else 
            {
                return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";
            }
        }

    }, "Allows you to change the way painting works, using arguments to specify what is being changed\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]\n  PAINT [ENABLE/DISABLE]");
    
    GorillaUtils::RegisterDisablingValue(ID, VERSION, allowPaintBall, true, {"MODDED", "PAINTBALL_CASUAL"});
    GorillaUtils::MatchMakingCallbacks::add_OnJoinedRoom([&](){
        if (!allowPaintBall)
        {
            PaintBall::MaterialColorCache::Reset();
        }

        GorillaUtils::Player::SetProperty<bool>(Photon::Pun::PhotonNetwork::get_LocalPlayer(), "paintballEnabled", config.enabled);
    });

    GorillaUtils::InRoomCallbacks::add_OnPlayerPropertiesUpdate([&](Photon::Realtime::Player* player, ExitGames::Client::Photon::Hashtable* changedProp) -> void {
        PaintBall::EnabledCache::add(player);
    });

    /*
    GorillaUI::CommandRegister::RegisterCommand("material", [](std::vector<std::string> args) -> std::string {
        if (args.size() == 0) return "  Invalid Argument!";
        static Il2CppString* changeMaterial = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("ChangeMaterial");

        GorillaTagger* gorillaTagger = GorillaTagger::get_Instance();

        VRRig* offlineRig = gorillaTagger->offlineVRRig;

        int count = offlineRig ? offlineRig->materialsToChangeTo->Length() : 0;
        int index = atoi(args[0].c_str());

        if (offlineRig)
        {
            if (index >= count)
            {
                return string_format("  Index out of range!\n  Only %d materials exist", count);
            }
            else
            {
                offlineRig->ChangeMaterial(index);
            }
        }

        if (PhotonNetwork::get_InRoom())
        {
            VRRig* myVRRig = gorillaTagger->myVRRig;

            count = myVRRig ? myVRRig->materialsToChangeTo->Length() : 0;

            if (myVRRig)
            {
                if (index >= count)
                {
                    return string_format("  Index out of range!\n  Only %d materials exist", count);
                }
                else
                {
                    myVRRig->ChangeMaterial(index);
                    if (!indexptr) indexptr = new int;
                    *indexptr = index;

                    Array<Il2CppObject*>* arr = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 1));
                    arr->values[0] = il2cpp_functions::value_box(classof(int), indexptr);

                    Array<Player*>* players = PhotonNetwork::get_PlayerList();
                    int playerCount = players->Length(); 
                    
                    for (int i = 0; i < playerCount; i++)
                    {
                        Player* player = players->values[i];
                        PhotonView::Get(myVRRig)->RPC(changeMaterial, player, arr);
                    }
                }
            }
        }
        return string_format("  Set material to %d out of %d", index, count);
    }, "forces your current material to whatever number you put if it is within range");
    */
}