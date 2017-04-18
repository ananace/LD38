#include "scriptnetwork.hpp"

#include <libyojimbo/yojimbo.h>

#include <limits>
#include <typeinfo>
#include <cstring>

namespace
{
    struct StreamWrapper
    {
        template<typename StreamType>
        StreamWrapper(StreamType& stream)
            : Stream(stream)
            , Type(Stream_Invalid)
        {
            auto type = typeid(StreamType);
            if (type == typeid(yojimbo::ReadStream))
                Type = Stream_Read;
            else if (type == typeid(yojimbo::WriteStream))
                Type = Stream_Write;
            else if (type == typeid(yojimbo::MeasureStream))
                Type = Stream_Measure;
        }

        template<typename StreamType>
        StreamType& get() { return static_cast<StreamType&>(Stream); }

        yojimbo::BaseStream& Stream;
        enum
        {
            Stream_Invalid,

            Stream_Read,
            Stream_Write,
            Stream_Measure
        } Type;
    };

    template<typename Stream, typename T>
    bool as_serialize_int_real(Stream& stream, T& val)
    {
        serialize_int(stream, val, 0, std::numeric_limits<T>::max());
        return true;
    }
    template<typename Stream>
    bool as_serialize_int_real(Stream& stream, int64_t& val)
    {
        serialize_bytes(stream, reinterpret_cast<uint8_t*>(&val), sizeof(int64_t));
        return true;
    }
    template<typename Stream>
    bool as_serialize_int_real(Stream& stream, uint64_t& val)
    {
        serialize_uint64(stream, val);
        return true;
    }

    template<typename T>
    bool as_serialize_int(StreamWrapper& stream, T& val)
    {
        if (stream.Type == StreamWrapper::Stream_Read)
            return as_serialize_int_real(stream.get<yojimbo::ReadStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Write)
            return as_serialize_int_real(stream.get<yojimbo::WriteStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Measure)
            return as_serialize_int_real(stream.get<yojimbo::MeasureStream>(), val);
        return false;
    }

    template<typename Stream, typename T>
    bool as_serialize_float_real(Stream& stream, T& val)
    {
        serialize_float(stream, val);
        return true;
    }
    template<typename Stream>
    bool as_serialize_float_real(Stream& stream, double& val)
    {
        serialize_double(stream, val);
        return true;
    }

    template<typename T>
    bool as_serialize_float(StreamWrapper& stream, T& val)
    {
        if (stream.Type == StreamWrapper::Stream_Read)
            return as_serialize_float_real(stream.get<yojimbo::ReadStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Write)
            return as_serialize_float_real(stream.get<yojimbo::WriteStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Measure)
            return as_serialize_float_real(stream.get<yojimbo::MeasureStream>(), val);
        return false;
    }

    template<typename Stream>
    bool as_serialize_bool_real(Stream& stream, bool& val)
    {
        serialize_bool(stream, val);
        return true;
    }

    bool as_serialize_bool(StreamWrapper& stream, bool& val)
    {
        if (stream.Type == StreamWrapper::Stream_Read)
            return as_serialize_bool_real(stream.get<yojimbo::ReadStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Write)
            return as_serialize_bool_real(stream.get<yojimbo::WriteStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Measure)
            return as_serialize_bool_real(stream.get<yojimbo::MeasureStream>(), val);
        return false;
    }

    template<typename Stream>
    bool as_serialize_string_real(Stream& stream, std::string& val)
    {
        serialize_string(stream, &val[0], UINT16_MAX);
        return true;
    }

    bool as_serialize_string(StreamWrapper& stream, std::string& val)
    {
        if (stream.Type == StreamWrapper::Stream_Read)
            return as_serialize_string_real(stream.get<yojimbo::ReadStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Write)
            return as_serialize_string_real(stream.get<yojimbo::WriteStream>(), val);
        else if (stream.Type == StreamWrapper::Stream_Measure)
            return as_serialize_string_real(stream.get<yojimbo::MeasureStream>(), val);
        return false;
    }

    bool as_serialize_object(asIScriptGeneric* gen)
    {
        StreamWrapper& stream = *reinterpret_cast<StreamWrapper*>(gen->GetObject());
        asIScriptEngine* eng = gen->GetEngine();

        void* ref = gen->GetArgAddress(0);
        int refType = gen->GetArgTypeId(0);

        auto ctx = eng->RequestContext();
        auto type = eng->GetTypeInfoById(refType);

        ctx->Prepare(type->GetMethodByDecl("bool Serialize(Stream@)"));
        ctx->SetObject(ref);
        ctx->SetArgAddress(0, &stream);

        ctx->Execute();

        bool ret = ctx->GetReturnByte();

        eng->ReturnContext(ctx);

        return ret;
    }
}

BEGIN_AS_NAMESPACE

void RegisterScriptNetwork(asIScriptEngine *engine)
{
    if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
        RegisterScriptNetwork_Generic(engine);
    else
        RegisterScriptNetwork_Native(engine);
}

void RegisterScriptNetwork_Native(asIScriptEngine* engine)
{
    int r;
    r = engine->RegisterObjectType("Stream", sizeof(StreamWrapper), asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);
    r = engine->RegisterInterface("ISerializable"); assert(r >= 0);
    r = engine->RegisterInterfaceMethod("ISerializable", "bool Serialize(Stream@)"); assert(r >= 0);

    r = engine->RegisterObjectMethod("Stream", "bool Serialize(bool&inout)", asFUNCTION(as_serialize_bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(uint8&inout)", asFUNCTION(as_serialize_int<uint8_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(int8&inout)", asFUNCTION(as_serialize_int<int8_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(uint16&inout)", asFUNCTION(as_serialize_int<uint16_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(int16&inout)", asFUNCTION(as_serialize_int<int16_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(uint32&inout)", asFUNCTION(as_serialize_int<uint32_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(int32&inout)", asFUNCTION(as_serialize_int<int32_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(uint64&inout)", asFUNCTION(as_serialize_int<uint64_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(int64&inout)", asFUNCTION(as_serialize_int<int64_t>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(float&inout)", asFUNCTION(as_serialize_float<float>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(double&inout)", asFUNCTION(as_serialize_float<double>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Stream", "bool Serialize(string&inout)", asFUNCTION(as_serialize_string), asCALL_CDECL_OBJFIRST); assert(r >= 0);

    r = engine->RegisterObjectMethod("Stream", "bool Serialize(ISerializable@)", asFUNCTION(as_serialize_object), asCALL_GENERIC); assert(r >= 0);
}

void RegisterScriptNetwork_Generic(asIScriptEngine* engine)
{
    int r;
    r = engine->RegisterObjectType("Stream", sizeof(StreamWrapper), asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);
    r = engine->RegisterInterface("ISerializable"); assert(r >= 0);
    r = engine->RegisterInterfaceMethod("ISerializable", "bool Serialize(Stream@)"); assert(r >= 0);

    r = engine->RegisterObjectMethod("Stream", "bool Serialize(ISerializable@)", asFUNCTION(as_serialize_object), asCALL_GENERIC); assert(r >= 0);
}

END_AS_NAMESPACE
