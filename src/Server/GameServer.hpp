#pragma once

#include <Network.hpp>
#include <Messages.hpp>
#include <ScriptManager.hpp>

class GameServer : public Network::ServerCommon
{
public:
    GameServer() = default;
    GameServer(const GameServer&) = delete;
    ~GameServer() = default;

    GameServer& operator=(const GameServer&) = delete;

    void Update();

private:
    void OnOnline();
    void OnOffline();
    void OnConnected(int connectionId);
    void OnDisconnected(int connectionId);
    void OnError(int connectionId, yojimbo::ServerClientError error);
    void OnConsume(int connectionId, yojimbo::Message* msg);

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) {
        return YOJIMBO_NEW(allocator, Network::LD38MessageFactory, allocator);
    }

    Script::Manager m_scripts;
};
