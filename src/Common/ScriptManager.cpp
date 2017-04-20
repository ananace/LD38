#include "ScriptManager.hpp"
#include "Messages.hpp"

#include <as_addons/contextmgr/contextmgr.h>
#include <as_addons/debugger/debugger.h>
#include <as_addons/datetime/datetime.h>
#include <as_addons/scriptany/scriptany.h>
#include <as_addons/scriptarray/scriptarray.h>
#include <as_addons/scriptbuilder/scriptbuilder.h>
#include <as_addons/scriptdictionary/scriptdictionary.h>
#include <as_addons/scriptgrid/scriptgrid.h>
#include <as_addons/scripthandle/scripthandle.h>
#include <as_addons/scripthelper/scripthelper.h>
#include <as_addons/scriptmath/scriptmath.h>
#include <as_addons/scriptmath/scriptmathcomplex.h>
#include <as_addons/scriptstdstring/scriptstdstring.h>
#include <as_addons/serializer/serializer.h>
#include <as_addons/weakref/weakref.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <ctime>

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
    m_contextMgr.SetGetTimeCallback([]() -> asUINT { return clock(); });
}

Manager::~Manager()
{
    m_engine.Release()->ShutDownAndRelease();
}

void Manager::Initialize()
{
    m_engine->SetUserData(this, kManagerUserId);
    m_engine->SetMessageCallback(asFUNCTION(scriptMessage), nullptr, asCALL_CDECL);

    RegisterStdString(m_engine.Get());
    RegisterScriptAny(m_engine.Get());
    RegisterScriptArray(m_engine.Get(), true);
    RegisterScriptDateTime(m_engine.Get());
    RegisterScriptDictionary(m_engine.Get());
    RegisterScriptGrid(m_engine.Get());
    RegisterScriptHandle(m_engine.Get());
    RegisterScriptMath(m_engine.Get());
    RegisterScriptMathComplex(m_engine.Get());
    RegisterStdStringUtils(m_engine.Get());

    m_contextMgr.RegisterThreadSupport(m_engine.Get());
    m_contextMgr.RegisterCoRoutineSupport(m_engine.Get());
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
    // - Hot-reload
    // - Load/Reload events
    // - Better error reporting

    auto* mod = m_engine->GetModule(file.c_str(), asGM_ALWAYS_CREATE); if (!mod) return false;

    if (type == Script_Text)
    {
        AS_SOFTASSERT(mod->AddScriptSection(file.c_str(), reinterpret_cast<const char*>(fileData), length));
        AS_SOFTASSERT(mod->Build());
    }
    else
    {
        auto store = BytecodeStore(reinterpret_cast<const char*>(fileData), length);
        AS_SOFTASSERT(mod->LoadByteCode(&store, nullptr));
        AS_SOFTASSERT(mod->Build());
    }

    return true;
}

bool Manager::LoadScript(const Network::ScriptMsg& script)
{
    return LoadScript(script.ScriptName, script.GetBlockData(), script.GetBlockSize(), Script_Bytecode);
}

bool Manager::UnloadScript(const std::string& file)
{
    auto* mod = m_engine->GetModule(file.c_str(), asGM_ONLY_IF_EXISTS);
    if (!mod)
        return false;

    mod->Discard();
    return true;
}

