#include "config.hpp"
#include "MonkeTrail.hpp"

extern Logger& getLogger();
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/PrimitiveType.hpp"

#include <map>
#include <deque>

DEFINE_TYPE(Trail::MonkeTrail);
namespace Trail
{

    std::map<int, UnityEngine::LineRenderer*> monkeRenderer;
    std::map<int, std::deque<UnityEngine::Vector3>> monkeLines;
    int maxPoints = 50, cooldownAmount = 10;
    void MonkeTrail::ctor()
    {
        cooldown = 0;
    }

    void MonkeTrail::Awake()
    {

    }

    void RenderPoints(UnityEngine::LineRenderer *cRend, std::deque<UnityEngine::Vector3> &points, UnityEngine::Vector3 pos) {
        while (points.size() >= maxPoints) { points.pop_back(); }

        points.push_front(pos);

        cRend->set_positionCount(points.size());
        cRend->set_startWidth(0.2f);
        cRend->set_endWidth(0.01f);

        int i = 0;
        for (auto pt : points) {
            cRend->SetPosition(i++, pt);
        }
    }

    void MonkeTrail::Update()
    {

        if (!config.enabled || config.monkemode == 0) {
          auto &pts = monkeLines[playerId];
          pts.clear();
          if (markerEndPoint) { monkeRenderer[playerId]->set_positionCount(0); }

          return;
        }

        if (cooldown > 0) {
            cooldown--;
            return;
        }
        cooldown = cooldownAmount;

        if (!markerEndPoint)
        {
            UnityEngine::GameObject* point = UnityEngine::GameObject::CreatePrimitive(
                UnityEngine::PrimitiveType::Sphere);
            auto rend = monkeRenderer[playerId] = point->AddComponent<UnityEngine::LineRenderer*>();
            rend->set_material(material);

            monkeLines[playerId] = {};

            Object::DontDestroyOnLoad(point);

            markerEndPoint = point->get_transform();
        } else {
            auto rend = markerEndPoint->get_gameObject()->GetComponent<UnityEngine::LineRenderer*>();
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
}
