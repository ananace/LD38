#pragma once

#include "RefPtr.hpp"

#include <angelscript.h>
#include <as_addons/contextmgr/contextmgr.h>

#ifndef NDEBUG
#include "ScriptException.hpp"

#define AS_ASSERT(f) do { int __r = (f); if (__r < 0) throw ASException(#f, __r, __FILE__, __LINE__); } while (false)
#else
#define AS_ASSERT(f) f
#endif
#define AS_SOFTASSERT(f) if ((f) <= 0) return false;

namespace sf { class InputStream; }
namespace Network { class ScriptMsg; }

namespace Script
{

class BytecodeStore : public asIBinaryStream
{
public:
    BytecodeStore() : mTellg(0) { }
    BytecodeStore(const char* data, size_t len) :
        mTellg(0)
    {
        mStore.assign(data, data + len);
    }

    void Read(void *ptr, asUINT size)
    {
        char* data = reinterpret_cast<char*>(ptr);

        for (uint32_t i = 0; i < size; ++i)
        {
            data[i] = mStore[mTellg + i];
        }

        mTellg += size;
    }
    void Write(const void *ptr, asUINT size)
    {
        const char* data = reinterpret_cast<const char*>(ptr);

        for (uint32_t i = 0; i < size; ++i)
            mStore.push_back(data[i]);
    }

private:
    std::vector<char> mStore;
    size_t mTellg;
};

class Manager
{
public:
    enum
    {
        kManagerUserId = 0x4567,
    };

    enum ScriptType
    {
        Script_Text,
        Script_Bytecode,

        Script_Autodetect
    };

    Manager();
    ~Manager();

    asIScriptEngine* operator->() { return m_engine.Get(); }
    const asIScriptEngine* operator->() const { return m_engine.Get(); }
    asIScriptEngine* GetEngine() { return m_engine.Get(); }
    const asIScriptEngine* GetEngine() const { return m_engine.Get(); }
    CContextMgr* GetContextMgr() { return &m_contextMgr; }
    const CContextMgr* GetContextMgr() const { return &m_contextMgr; }

    void Initialize();

    bool LoadScript(const std::string& file);
    bool LoadScript(const std::string& file, const void* fileData, size_t length, ScriptType type = Script_Autodetect);
    bool LoadScript(const Network::ScriptMsg& script);
    bool UnloadScript(const std::string& file);

private:
    RefPtr<asIScriptEngine> m_engine;
    CContextMgr m_contextMgr;
};

}
