#pragma once

#include <ScriptManager.hpp>

class GameServer
{
public:
    GameServer() = default;
    GameServer(const GameServer&) = delete;
    ~GameServer() = default;

    GameServer& operator=(const GameServer&) = delete;

private:
    Script::Manager m_scripts;
};
