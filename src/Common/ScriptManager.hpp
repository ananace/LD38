#pragma once

#include "RefPtr.hpp"

#include <angelscript.h>

#ifndef NDEBUG
#include "ScriptException.hpp"

#define AS_ASSERT(f) do { int __r = (f); if (__r < 0) throw ASException(#f, __r, __FILE__, __LINE__); } while (false)
#else
#define AS_ASSERT(f) f
#endif

namespace sf { class InputStream; }

namespace Script
{

class Manager
{
public:
    enum
    {
        kManagerUserId = 0x4567,
    };

    Manager();
    ~Manager();

    asIScriptEngine* operator->() { return m_engine.Get(); }
    const asIScriptEngine* operator->() const { return m_engine.Get(); }
    asIScriptEngine* GetEngine() { return m_engine.Get(); }
    const asIScriptEngine* GetEngine() const { return m_engine.Get(); }

    void Initialize();

private:
    RefPtr<asIScriptEngine> m_engine;
};

}
