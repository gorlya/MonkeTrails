#pragma once

#include "UnityEngine/Material.hpp"
#include "UnityEngine/Color.hpp"
#include <optional>

namespace PaintBall::MaterialColorCache
{
    /// @brief adds color to map with mat as key
    /// Doesn't overwrite already added things
    /// @param mat key
    void add(UnityEngine::Material* mat);

    /// @brief gets the original color for a mat
    /// @param mat the key to use
    /// @return optional color stored at key, or nullopt
    std::optional<UnityEngine::Color> get(UnityEngine::Material* mat);

    void Reset();
}