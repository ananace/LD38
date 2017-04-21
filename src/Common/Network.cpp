#include "Network.hpp"

#include <libyojimbo/yojimbo.h>

#include <chrono>
#include <vector>

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

ClientCommon::ClientCommon()
    : m_clientWrapper(*this)
{
    yojimbo::RandomBytes(reinterpret_cast<uint8_t*>(&m_clientId), sizeof(m_clientId));
}

void ClientCommon::Connect(const yojimbo::Address& address)
{
    m_clientWrapper.InsecureConnect(m_clientId, address);
}

void ClientCommon::CancelConnect()
{
    if (IsConnecting())
        m_clientWrapper.Disconnect(yojimbo::CLIENT_STATE_DISCONNECTED, false);
}

void ClientCommon::Disconnect()
{
    m_clientWrapper.Disconnect();
}

void ClientCommon::Update()
{
    if (IsDisconnected())
        return;

    m_clientWrapper.SendPackets();
    m_transport.WritePackets();

    m_transport.ReadPackets();
    m_clientWrapper.ReceivePackets();

    m_clientWrapper.CheckForTimeOut();

    for (int i = 0; i < Channel_Max; ++i)
    {
        yojimbo::Message* msg = m_clientWrapper.ReceiveMsg(i);
        if (!msg)
            break;

        OnConsume(msg);
    }

    m_clientWrapper.AdvanceTime(GetTime());
    m_transport.AdvanceTime(GetTime());
}

void ClientCommon::Send(MessagePtrBase msg, int channel)
{
    msg.AddRef();
    m_clientWrapper.SendMsg(msg.Get(), channel);
}

ClientCommon::YojimboWrapper::YojimboWrapper(ClientCommon& client)
    : yojimbo::Client(client.m_allocator, client.m_transport, client.m_config, client.GetTime())
    , m_client(client)
{ }

ServerCommon::ServerCommon()
    : m_serverWrapper(*this)
    , m_globalFactory(nullptr)
{ }

void ServerCommon::Host(const yojimbo::Address& address)
{
    if (!m_globalFactory)
        m_globalFactory = CreateMessageFactory(m_allocator);

    m_transport.Reset();
    m_serverWrapper.SetServerAddress(address);
    m_serverWrapper.Start();
}
void ServerCommon::Stop()
{
    m_serverWrapper.Stop();
}

void ServerCommon::Update()
{
    m_serverWrapper.SendPackets();
    m_transport.WritePackets();

    m_transport.ReadPackets();
    m_serverWrapper.ReceivePackets();

    m_serverWrapper.CheckForTimeOut();

    for (int cli = 0; cli < m_serverWrapper.GetMaxClients(); ++cli)
    {
        if (!m_serverWrapper.IsClientConnected(cli))
            continue;

        for (int i = 0; i < Channel_Max; ++i)
        {
            yojimbo::Message* msg = m_serverWrapper.ReceiveMsg(cli, i);
            if (!msg)
                break;

            OnConsume(cli, msg);
        }
    }

    m_serverWrapper.AdvanceTime(GetTime());
    m_transport.AdvanceTime(GetTime());
}

void ServerCommon::Send(int connection, const MessagePtrBase& msg, int channel)
{
    if (!m_serverWrapper.IsClientConnected(connection) || channel < 0 || channel >= Channel_Max)
        return;

    SendInternal(connection, msg, channel);
}

void ServerCommon::SendAll(const MessagePtrBase& msg, int channel)
{
    for (int i = 0; i < m_serverWrapper.GetMaxClients(); ++i)
    {
        if (!m_serverWrapper.IsClientConnected(i))
            continue;

        SendInternal(i, msg, channel);
    }
}

void ServerCommon::SendInternal(int connection, MessagePtrBase msg, int channel)
{
    auto& factory = m_serverWrapper.GetMsgFactory(connection);
    yojimbo::Message* toSend = nullptr;
    if (msg.BelongsTo(factory))
    {
        msg.AddRef();
        toSend = msg.Get();
    }
    else
    {
        // TODO: Improve by making some kind of generalized message copier
        yojimbo::MeasureStream strm;
        msg->SerializeInternal(strm);

        auto& alloc = m_serverWrapper.GetClientAllocator(connection);
        std::vector<uint8_t> data(strm.GetBytesProcessed());

        yojimbo::WriteStream write(&data[0], data.size(), alloc);
        msg->SerializeInternal(write);

        toSend = factory.Create(msg->GetType());

        yojimbo::ReadStream read(&data[0], data.size());
        toSend->SerializeInternal(read);
    }

    m_serverWrapper.SendMsg(connection, toSend, channel);
}

ServerCommon::YojimboWrapper::YojimboWrapper(ServerCommon& server)
    : yojimbo::Server(server.m_allocator, server.m_transport, server.m_config, server.GetTime())
    , m_server(server)
{ }

