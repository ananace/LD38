#include "Network.hpp"

#include <libyojimbo/yojimbo.h>

#include <chrono>

using namespace Network;

void Common::Initialize()
{
    if (!InitializeYojimbo())
        exit(EXIT_FAILURE);
}
void Common::Shutdown()
{
    ShutdownYojimbo();
}

Common::Common()
    : m_transport(m_allocator, yojimbo::Address(), kProtocolId, GetTime())
{
    auto& cfg = m_config.connectionConfig;

    cfg.numChannels = Channel_Max;
    {
        auto& chan = cfg.channel[Channel_Unreliable];
        chan.type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
        chan.disableBlocks = true;
    }
    {
        auto& chan = cfg.channel[Channel_Reliable];
        chan.type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
        chan.disableBlocks = true;
    }
    {
        auto& chan = cfg.channel[Channel_ReliableBlock];
        chan.type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
    }
}

double Common::GetTime()
{
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

ClientCommon::YojimboWrapper::YojimboWrapper(ClientCommon& client)
    : yojimbo::Client(client.m_allocator, client.m_transport, client.m_config, client.GetTime())
    , m_client(client)
{ }

void ClientCommon::YojimboWrapper::OnClientStateChange(yojimbo::ClientState previousState, yojimbo::ClientState currentState)
{
    if (currentState >= yojimbo::CLIENT_STATE_CONNECTED && previousState < yojimbo::CLIENT_STATE_CONNECTED)
	m_client.OnConnected();
    else if (currentState <= yojimbo::CLIENT_STATE_DISCONNECTED && previousState > yojimbo::CLIENT_STATE_DISCONNECTED)
	m_client.OnDisconnected(currentState);
}

yojimbo::MessageFactory* ClientCommon::YojimboWrapper::CreateMessageFactory(yojimbo::Allocator& aAllocator)
{
    return m_client.CreateMessageFactory(aAllocator);
}
