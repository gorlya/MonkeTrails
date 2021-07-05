#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/VRRig.hpp"

DECLARE_CLASS_CODEGEN(Marker, MonkeMarker, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(bool, isRight);
    DECLARE_METHOD(void, Update);
    DECLARE_METHOD(void, Awake);
    DECLARE_CTOR(ctor);

    DECLARE_STATIC_FIELD(UnityEngine::Transform*, markerEndPoint);

    REGISTER_FUNCTION(
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(Update);
        REGISTER_METHOD(Awake);
        REGISTER_FIELD(isRight);

        REGISTER_FIELD(markerEndPoint);
    )
)
