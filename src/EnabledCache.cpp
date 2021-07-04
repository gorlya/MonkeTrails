#include "EnabledCache.hpp"

#include "gorilla-utils/shared/CustomProperties/Player.hpp"
#include <map>


extern Logger& getLogger();

namespace PaintBall::EnabledCache
{
    using Cache = std::map<Player*, bool>;
    
    Cache playerLikesPaintball = {};

    void add (Player* player)
    {
        auto prop = GorillaUtils::Player::GetProperty<bool>(player, "paintballEnabled");
        playerLikesPaintball[player] = prop ? *prop : false;
    }

    bool get (Player* player)
    {
        Cache::iterator it = playerLikesPaintball.find(player);
        if (it != playerLikesPaintball.end()) return it->second;
        return false;
    }
}