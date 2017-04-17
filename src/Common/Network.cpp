#include "Network.hpp"

#include <libyojimbo/yojimbo.h>

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
