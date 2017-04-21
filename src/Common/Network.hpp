#pragma once

#include "MessagePtr.hpp"

#include <libyojimbo/yojimbo_allocator.h>
#include <libyojimbo/yojimbo_client.h>
#include <libyojimbo/yojimbo_server.h>

#include <cstdint>

namespace Network
{

class Common
{
public:
    enum Channels
    {
        Channel_Unreliable = 0,
        Channel_Reliable = 1,
        Channel_ReliableBlock = 2,

        Channel_Max
    };

    enum : uint64_t
    {
        kProtocolId = 0xBABEFACEBEEFCAFE,
    };

    static void Initialize();
    static void Shutdown();

    Common();
    virtual ~Common() = default;

    virtual void Update() = 0;

protected:
    virtual yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& aAllocator) = 0;

    static double GetTime();

    yojimbo::DefaultAllocator   m_allocator;
    yojimbo::ClientServerConfig m_config;
    yojimbo::NetworkTransport   m_transport;
};

class ClientCommon : public Common
{
public:
    ClientCommon();
    virtual ~ClientCommon() = default;

    void Connect(const std::string& host, uint16_t port) { Connect(yojimbo::Address(host.c_str(), port)); }
    void Connect(const yojimbo::Address& address);
    void CancelConnect();
    void Disconnect();
    void Update();

    template<typename T>
    inline MessagePtr<T> CreateMessage() {
    return MessagePtr<T>(static_cast<T*>(m_clientWrapper.GetMsgFactory().Create(T::Type)), &m_clientWrapper.GetMsgFactory());
    }

    void Send(MessagePtrBase msg, int channel = Channel_Unreliable);
    inline void SendReliable(MessagePtrBase msg) {
        Send(msg, msg->IsBlockMessage() ? Channel_ReliableBlock : Channel_Reliable);
    }

    inline bool IsConnected() const { return m_clientWrapper.IsConnected(); }
    inline bool IsConnecting() const { return m_clientWrapper.IsConnecting(); }
    inline bool IsDisconnected() const { return m_clientWrapper.IsDisconnected(); }

protected:
    virtual void OnConnected() = 0;
    virtual void OnDisconnected(yojimbo::ClientState reason) = 0;
    virtual void OnConsume(yojimbo::Message* msg) = 0;

private:
    class YojimboWrapper : public yojimbo::Client
    {
    public:
        YojimboWrapper(ClientCommon& client);

    protected:
        virtual void OnClientStateChange(yojimbo::ClientState previousState, yojimbo::ClientState currentState) {
            if (currentState >= yojimbo::CLIENT_STATE_CONNECTED && previousState < yojimbo::CLIENT_STATE_CONNECTED)
                m_client.OnConnected();
            else if (currentState <= yojimbo::CLIENT_STATE_DISCONNECTED && previousState > yojimbo::CLIENT_STATE_DISCONNECTED)
                m_client.OnDisconnected(currentState);
        }
        virtual yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) {
            return m_client.CreateMessageFactory(allocator);
        }

    private:
        ClientCommon& m_client;
    };

    YojimboWrapper m_clientWrapper;
    uint64_t       m_clientId;
    friend class YojimboWrapper;
};

class ServerCommon : public Common
{
public:
    ServerCommon();
    virtual ~ServerCommon() = default;

    void Host(const yojimbo::Address& address);
    inline void Host(uint16_t port) {
        Host(yojimbo::Address("0.0.0.0", port));
    }
    inline void Host(const std::string& host, uint16_t port) {
        Host(yojimbo::Address(host.c_str(), port));
    }
    void Stop();
    void Update();

    template<typename T>
    MessagePtr<T> CreateMessage(int connectionId = 0)
    {
        yojimbo::MessageFactory* factory = nullptr;
        if (connectionId == 0)
            factory = m_globalFactory;
        else
            factory = &m_serverWrapper.GetMsgFactory(connectionId);

        return MessagePtr<T>(static_cast<T*>(factory->Create(T::Type)), factory);
    }

    void Send(int connectionId, const MessagePtrBase& msg, int channel = Channel_Unreliable);
    void SendAll(const MessagePtrBase& msg, int channel = Channel_Unreliable);
    inline void SendReliable(int connectionId, const MessagePtrBase& msg) {
        Send(connectionId, msg, msg->IsBlockMessage() ? Channel_ReliableBlock : Channel_Reliable);
    }
    inline void SendReliableAll(const MessagePtrBase& msg) {
        SendAll(msg, msg->IsBlockMessage() ? Channel_ReliableBlock : Channel_Reliable);
    }

    inline int GetConnectionId(uint64_t clientId) { return m_serverWrapper.FindClientIndex(clientId); }
    inline uint64_t GetClientId(int connectionId) { return m_serverWrapper.GetClientId(connectionId); }

protected:
    virtual void OnOnline() = 0;
    virtual void OnOffline() = 0;
    virtual void OnConnected(int connectionId) = 0;
    virtual void OnDisconnected(int connectionId) = 0;
    virtual void OnError(int connectionId, yojimbo::ServerClientError error) = 0;
    virtual void OnConsume(int connectionId, yojimbo::Message* msg) = 0;

private:
    void SendInternal(int connectionId, MessagePtrBase msg, int channel);

    class YojimboWrapper : public yojimbo::Server
    {
    public:
        YojimboWrapper(ServerCommon& server);

    protected:
        virtual void OnStart(int /*clients*/) {
            m_server.OnOnline();
        }
        virtual void OnStop() {
            m_server.OnOffline();
        }
        virtual void OnClientConnect(int client) {
            m_server.OnConnected(client);
        }
        virtual void OnClientDisconnect(int client) {
            m_server.OnDisconnected(client);
        }
        virtual void OnClientError(int client, yojimbo::ServerClientError error) {
            m_server.OnError(client, error);
        }

    private:
        ServerCommon& m_server;
    };

    YojimboWrapper           m_serverWrapper;
    yojimbo::MessageFactory* m_globalFactory;
    friend class YojimboWrapper;
};

}
