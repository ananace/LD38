#pragma once

#include <libyojimbo/yojimbo_config.h>
#include <libyojimbo/yojimbo_common.h>
#include <libyojimbo/yojimbo_allocator.h>

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
    yojimbo::DefaultAllocator   m_allocator;
    yojimbo::ClientServerConfig m_config;
};

}
