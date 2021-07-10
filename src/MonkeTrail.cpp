#include "config.hpp"
#include "MonkeTrail.hpp"

extern Logger& getLogger();
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "gorilla-utils/shared/Utils/Player.hpp"
#include "Photon/Realtime/Player.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"

#include <map>
#include <deque>

DEFINE_TYPE(Trail::MonkeTrail);
namespace Trail
{
    bool moddedRoom = false;

    std::map<int, UnityEngine::LineRenderer*> monkeRenderer;
    std::map<int, std::deque<UnityEngine::Vector3>> monkeLines;
    int cooldownAmount = 10;
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

    void RenderPoints(UnityEngine::LineRenderer *cRend, std::deque<UnityEngine::Vector3> &points, UnityEngine::Vector3 pos) {
        int maxPoints = 50;
        switch (config.trailsize) {
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
        while (points.size() >= maxPoints) { points.pop_back(); }

        points.push_front(pos);

        cRend->set_positionCount(points.size());
        switch (config.trailwidth) {
          case 0:
            cRend->set_startWidth(0.05f);
            break;
          case 1:
            cRend->set_startWidth(0.1f);
            break;
          case 2:
            cRend->set_startWidth(0.2f);
            break;
          default:
            cRend->set_startWidth(0.3f);
        }
        cRend->set_endWidth(0.01f);

        int i = 0;
        for (auto pt : points) {
            cRend->SetPosition(i++, pt);
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
        cooldown = config.trailmode ? cooldownAmount / 2 : cooldownAmount;

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

        UnityEngine::Vector3 pos = get_transform()->get_position();
        auto &pts = monkeLines[playerId];

        RenderPoints(monkeRenderer[playerId], pts, pos);


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
        if (!moddedRoom) {
          auto self = Photon::Pun::PhotonNetwork::get_LocalPlayer();
          auto selfId = self->actorNumber;
          if (selfId != player->actorNumber) {
            return;
          }
        }

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
              }

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
