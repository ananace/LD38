#include "GameServer.hpp"

int main(int argc, char** argv)
{
    GameServer server;

    while (true)
    {
        server.Update();
    }

    return 0;
}
