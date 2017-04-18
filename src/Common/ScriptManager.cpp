#include "ScriptManager.hpp"
#include "Messages.hpp"

#include <iostream>
#include <fstream>
#include <vector>

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

bool Manager::LoadScript(const std::string& file)
{
    std::ifstream ifs(file);

    ifs.seekg(0, std::ios::end);
    size_t len = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> data(len);
    ifs.read(&data[0], len);

    return LoadScript(file, data.data(), len);
}

bool Manager::LoadScript(const std::string& file, const void* fileData, size_t length, ScriptType type)
{
    if (type == Script_Autodetect)
    {
        if (file.find_last_of(".as") == std::string::npos)
            type = Script_Bytecode;
        else
            type = Script_Text;
    }

    // TODO:

    int r;
    auto* mod = m_engine->GetModule(file.c_str(), asGM_ALWAYS_CREATE); if (!mod) return false;
    r = mod->AddScriptSection(file.c_str(), reinterpret_cast<const char*>(fileData), length); if (r <= 0) return false;
    r = mod->Build(); if (r <= 0) return false;

    return true;
}

bool Manager::LoadScript(const Network::ScriptMsg& script)
{
    return LoadScript(script.ScriptName, const_cast<Network::ScriptMsg&>(script).GetBlockData(), script.GetBlockSize(), Script_Bytecode);
}

bool Manager::UnloadScript(const std::string& file)
{
    auto* mod = m_engine->GetModule(file.c_str(), asGM_ONLY_IF_EXISTS);
    if (!mod)
        return false;

    mod->Discard();
    return true;
}

