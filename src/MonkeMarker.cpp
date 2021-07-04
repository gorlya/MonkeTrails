#include "config.hpp"
#include "MonkeMarker.hpp"

#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/GradientColorKey.hpp"
#include "UnityEngine/Gradient.hpp"
#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/VRRig.hpp"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include <deque>


DEFINE_TYPE(PaintBall::MonkeMarker);

using namespace UnityEngine;

extern Logger& getLogger();

extern bool allowPaintBall;
static int lCooldown = 0;
static int rCooldown = 0;

namespace PaintBall
{
    Transform* MonkeMarker::markerEndPoint = nullptr;
    LineRenderer *lRend = nullptr;
    LineRenderer *rRend = nullptr;
    std::deque<Vector3> lPoints;
    std::deque<Vector3> rPoints;
    int maxPoints = 1000;

    void MonkeMarker::ctor()
    {
        isRight = false;
    }

    void MonkeMarker::Awake()
    {
        if (!markerEndPoint)
        {
            GameObject* lpoint = GameObject::CreatePrimitive(PrimitiveType::Sphere);
            GameObject* rpoint = GameObject::CreatePrimitive(PrimitiveType::Sphere);

            lRend = lpoint->AddComponent<LineRenderer*>();
            rRend = rpoint->AddComponent<LineRenderer*>();

            Object::DontDestroyOnLoad(lpoint);
            Object::DontDestroyOnLoad(rpoint);
            markerEndPoint = lpoint->get_transform();

            Collider* col = lpoint->GetComponent<Collider*>();
            Object::DestroyImmediate(col);
            markerEndPoint->set_localScale(Vector3::get_one() * 0.1f);
        }
    }

    void RenderPoints(LineRenderer *cRend, std::deque<Vector3> &points, Vector3 pos, bool btnPressed) {
        while (points.size() >= maxPoints) { points.pop_back(); }

        if (btnPressed) {
            for (int i = 0; i < 5; i++) {
                if (points.size()) {
                    points.pop_back();
                }
            }
        }

        points.push_front(pos);

        cRend->set_positionCount(points.size());
        cRend->set_startWidth(0.02f);
        cRend->set_endWidth(0.01f);

        int i = 0;
        for (auto pt : points) {
            cRend->SetPosition(i++, pt);
        }
    }
    void MonkeMarker::Update()
    {
        if (!config.enabled) { return; }

        if (rCooldown > 0) { rCooldown--; }
        if (lCooldown > 0) { lCooldown--; }

        if (markerEndPoint != nullptr) {

            Transform* transform = get_transform();
            Vector3 pos = transform->get_position();
            bool rightBtn = GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryHandTrigger, GlobalNamespace::OVRInput::Controller::RTouch);
            bool leftBtn = GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryHandTrigger, GlobalNamespace::OVRInput::Controller::LTouch);
            bool rightGtn = GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryIndexTrigger, GlobalNamespace::OVRInput::Controller::RTouch);
            bool leftGtn = GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryIndexTrigger, GlobalNamespace::OVRInput::Controller::LTouch);
            if (rightGtn && leftGtn && (!leftBtn && !rightBtn)) {
                if (!isRight) {
                    return;
                }
                config.alwayson = !config.alwayson;
                rCooldown = 10;
                lCooldown = 10;
                return;
            }

            if (!rCooldown && (isRight && (rightBtn || config.alwayson)))
            {
                RenderPoints(rRend, rPoints, pos, rightGtn);
                rCooldown = 10;
            } 

            if (!lCooldown && (!isRight && (leftBtn || config.alwayson)))
            {
                RenderPoints(lRend, lPoints, pos, leftGtn);
                lCooldown = 10;
            }
        }
    }

}
