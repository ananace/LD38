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
    void Disconnect();
    void Update();

    template<typename T>
    inline MessagePtr<T> CreatePacket()
    {
	return MessagePtr<T>(static_cast<T*>(m_clientWrapper.GetMsgFactory().Create(T::Type)), &m_clientWrapper.GetMsgFactory());
    }

    void Send(const MessagePtrBase& msg, int channel = Channel_Unreliable);
    inline void SendReliable(const MessagePtrBase& msg) {
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
        virtual void OnClientStateChange(yojimbo::ClientState previousState, yojimbo::ClientState currentState);
        virtual yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator);

    private:
        ClientCommon& m_client;
    };

    YojimboWrapper m_clientWrapper;
    friend class YojimboWrapper;
};

class ServerCommon : public Common
{
public:

private:
    class YojimboWrapper : public yojimbo::Server
    {
    public:

    private:
    };

    YojimboWrapper m_serverWrapper;
    friend class YojimboWrapper;
};

}
