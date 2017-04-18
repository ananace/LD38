#pragma once

#ifndef ANGELSCRIPT_H
#include <angelscript.h>
#endif

BEGIN_AS_NAMESPACE

void RegisterScriptNetwork(asIScriptEngine* engine);
void RegisterScriptNetwork_Native(asIScriptEngine* engine);
void RegisterScriptNetwork_Generic(asIScriptEngine* engine);

END_AS_NAMESPACE
