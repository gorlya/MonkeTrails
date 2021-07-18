#include "config.hpp"
#include "MonkeTrail.hpp"

extern Logger& getLogger();
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "gorilla-utils/shared/Utils/Player.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include "Photon/Realtime/Player.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"

#include <map>
#include <deque>

DEFINE_TYPE(Trail, MonkeTrail);
namespace Trail
{
    bool moddedRoom = false;

    std::map<int, UnityEngine::LineRenderer*> monkeRenderer;
    std::map<int, std::deque<UnityEngine::Vector3>> monkeLines;
    int cooldownAmount = 5;
    void MonkeTrail::ctor()
    {
        cooldown = 0;
    }

    void MonkeTrail::OnDisable()
    {
        auto it = monkeRenderer.find(playerId);
        if (it != monkeRenderer.end()) {
          monkeRenderer.erase(it);
        }

        if (trailObject != nullptr) {
          Object::Destroy(trailObject);
        }

        trailObject = nullptr;

    }

    void RenderPoints(UnityEngine::LineRenderer *cRend, std::deque<UnityEngine::Vector3> &points, UnityEngine::Vector3 pos, int size) {
        int maxPoints = 50;
        switch (size) {
          case -1:
            maxPoints = 0;
            break;
          case 0:
            maxPoints = 5;
            break;
          case 1:
            maxPoints = 10;
            break;
          case 2:
            maxPoints = 20;
            break;
          default:
            maxPoints = 50;
        }

        points.push_front(pos);
        while (points.size() > maxPoints) { points.pop_back(); }


        cRend->set_positionCount(points.size());
        double sw = 0.3;
        switch (config.trailwidth) {
          case 0:
            sw = 0.05;
            break;
          case 1:
            sw = 0.1;
            break;
          case 2:
            sw = 0.2;
            break;
          default:
            sw = 0.3;
        }
        cRend->set_startWidth(sw);
        cRend->set_endWidth(0.01);

        int i = 0;
        for (auto pt : points) {
            cRend->SetPosition(i++, pt);
        }
    }

    void MonkeTrail::UpdateRenderer() {
        if (!trailObject)
        {
            trailObject = UnityEngine::GameObject::CreatePrimitive(
                UnityEngine::PrimitiveType::Sphere);
            auto rend = monkeRenderer[playerId] = trailObject->AddComponent<UnityEngine::LineRenderer*>();
            rend->set_material(material);

            monkeLines[playerId] = {};

            Object::DontDestroyOnLoad(trailObject);
        } else {
            auto rend = trailObject->GetComponent<UnityEngine::LineRenderer*>();
            rend->set_material(material);
        }

    }

    void MonkeTrail::Update()
    {
        if (!config.enabled) {
          return;
        }

        if (cooldown > 0) {
            cooldown--;
            return;
        }

        cooldown = cooldownAmount;

        UnityEngine::Vector3 pos = get_transform()->get_position();

        if (monkeLines.find(playerId) != monkeLines.end()) {
          auto &pts = monkeLines[playerId];
          RenderPoints(monkeRenderer[playerId], pts, pos, this->size);
        }


    }

    void Clear(int playerId)
    {
        auto lineRenderer = monkeRenderer.find(playerId);
        auto ml = monkeLines.find(playerId);
        if (lineRenderer != monkeRenderer.end()) {
            lineRenderer->second->set_positionCount(0);
        }
        if (ml != monkeLines.end()) {
            ml->second.clear();
        }
    }

    void ClearAll() {
        for (auto it : monkeLines) {
            it.second.clear();
        }
        for (auto it : monkeRenderer) {
            it.second->set_positionCount(0);
        }
    }

    void markMonke(Photon::Realtime::Player *player) {
        if (player == nullptr) { return; }

        int trailSize = -1;
        auto self = Photon::Pun::PhotonNetwork::get_LocalPlayer();
        auto selfId = self->actorNumber;

        bool isSelf = selfId == player->actorNumber;
        if (isSelf || moddedRoom) {
          trailSize = config.trailsize;
        } else {
          trailSize = GorillaUtils::Player::GetProperty<int>(player, "trailSize").value_or(-1);
        }

        getLogger().info("MARK MONKE: %i", trailSize);

        using namespace GlobalNamespace;
        VRRig* monkeRig = GorillaUtils::Player::findPlayerVRRig(player);
        if (monkeRig != nullptr) {
            int playerId = player->actorNumber;
            UnityEngine::GameObject* monkeHead = monkeRig->headMesh;
            if (monkeHead!= nullptr && monkeRig->mainSkin != nullptr) {
              auto mat = monkeRig->mainSkin->get_material();
              Trail::MonkeTrail* trail = monkeHead->GetComponent<Trail::MonkeTrail*>();
              if (trail == nullptr) {
                  trail = monkeHead->AddComponent<Trail::MonkeTrail*>();
              }

              if (trail != nullptr) {
                  trail->playerId = playerId;
                  trail->material = mat;
                  trail->size = trailSize;
              }

              trail->UpdateRenderer();

            }
        }
  }


  void updateMonkes() {
    using namespace GlobalNamespace;
    using namespace Photon::Pun;
    using namespace Photon::Realtime;

    Array<Player*>* players = PhotonNetwork::get_PlayerList();
    int playerCount = players->Length();

    for (int i = 0; i < playerCount; i++) {
      Player* player = players->values[i];
      markMonke(player);
    }
  }

  void setModded(bool v) {
    moddedRoom = v;
  }

}
