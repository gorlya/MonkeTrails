#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/VRRig.hpp"

DECLARE_CLASS_CODEGEN(Trail, MonkeTrail, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(int, playerId);
    DECLARE_INSTANCE_FIELD(int, cooldown);
    DECLARE_INSTANCE_FIELD(int, size);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, material);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, trailObject);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, UpdateRenderer);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_CTOR(ctor);
)
namespace Trail {
    void Clear(int);
    void ClearAll();
    void markMonke(Photon::Realtime::Player *player);
    void updateMonkes();
    void setModded(bool);
}
