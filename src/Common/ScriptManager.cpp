#include "ScriptManager.hpp"

#include <iostream>

using namespace Script;

namespace
{
    void scriptMessage(const asSMessageInfo* msg)
    {
        std::cerr << "Angelscript ";
        switch (msg->type)
        {
        case asMSGTYPE_ERROR: std::cerr << "error"; break;
        case asMSGTYPE_WARNING: std::cerr << "warning"; break;
        case asMSGTYPE_INFORMATION: std::cerr << "info"; break;
        default: std::cerr << "message"; break;
        }

        if (msg->section && msg->section[0] != 0)
            std::cerr << " (" << msg->section << ":" << msg->row << ":" << msg->col << ")";
        std::cerr << ";" << std::endl
            << ">  " << msg->message << std::endl;
    }
}

Manager::Manager()
    : m_engine(asCreateScriptEngine(ANGELSCRIPT_VERSION))
{
}

Manager::~Manager()
{
    m_engine.Release()->ShutDownAndRelease();
}

void Manager::Initialize()
{
    m_engine->SetUserData(this, kManagerUserId);
    m_engine->SetMessageCallback(asFUNCTION(scriptMessage), nullptr, asCALL_CDECL);
}
