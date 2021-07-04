#pragma once

#include "Photon/Realtime/Player.hpp"
#include "ExitGames/Client/Photon/Hashtable.hpp"

namespace PaintBall::EnabledCache
{
    using Player = Photon::Realtime::Player;
    using Hashtable = ExitGames::Client::Photon::Hashtable;

    void add(Player* player);
    bool get(Player* player);
}