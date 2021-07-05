#include "MaterialColorCache.hpp"
#include <map>
namespace Marker::MaterialColorCache
{
    using ColorCache = std::map<UnityEngine::Material*, UnityEngine::Color>;
    ColorCache colorCache = {};

    ColorCache::iterator find(UnityEngine::Material* mat)
    {
        return colorCache.find(mat);
    }

    void add(UnityEngine::Material* mat)
    {
        // if mat nullptr, return
        if (!mat) return;
        auto it = find(mat);
        // if not found
        if (it == colorCache.end()) colorCache[mat] = mat->get_color();    
    }

    std::optional<UnityEngine::Color> get(UnityEngine::Material* mat)
    {
        if (!mat) return std::nullopt;
        auto it = find(mat);
        // if not found, return nullopt
        if (it == colorCache.end()) return std::nullopt;
        return it->second;
    }

    void Reset()
    {
        for(auto& m : colorCache)
        {
            auto opt = il2cpp_utils::try_cast<UnityEngine::Material>(m.first);
            if (!opt) continue;
            UnityEngine::Material* mat = *opt;
            mat->set_color(m.second);
        }

        colorCache.clear();
    }
}