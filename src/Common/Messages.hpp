#pragma once

// TODO:
// Generate this whole file
// Probably a small ruby program for it

#include <libyojimbo/yojimbo_message.h>
#include <libyojimbo/yojimbo_serialize.h>

#include <sstream>

namespace Network
{

class Vector2f : public yojimbo::Serializable
{
public:
    Vector2f() = default;
    Vector2f(float x, float y)
        : X(x)
        , Y(y)
    { }
    Vector2f(const Vector2f&) = default;
    Vector2f(Vector2f&&) = default;
    ~Vector2f() noexcept = default;

    Vector2f& operator=(const Vector2f&) = default;

    template<typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_float(stream, X);
        serialize_float(stream, Y);

        return true;
    }

    const std::string ToString() const
    {
        std::ostringstream oss;
        oss << "{\"type\": \"Network::Vector2f\", \"X\": " << X << ", \"Y\": " << Y << "}";
        return oss.str();
    }

    float X, Y;

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class HelloMsg : public yojimbo::Message
{
public:
    enum { Type = 0x0001 };

    HelloMsg() = default;
    HelloMsg(const std::string& name)
        : PlayerName(name)
    { }

    template<typename Stream>
    bool Serialize(Stream& stream)
    {
        auto len = PlayerName.length();
        serialize_int(stream, len, 0, 256);
        serialize_string(stream, &PlayerName[0], len);

        return true;
    }

    std::string PlayerName;

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class ScriptMsg : public yojimbo::BlockMessage
{
public:
    enum { Type = 0x0002 };

    ScriptMsg() = default;
    ScriptMsg(const std::string& name)
        : ScriptName(name)
    { }

    template<typename Stream>
    bool Serialize(Stream& stream)
    {
        auto len = ScriptName.length();
        serialize_int(stream, len, 0, 256);
        serialize_string(stream, &ScriptName[0], len);

        return true;
    }

    std::string ScriptName;

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

enum MessageTypes
{
    Message_Hello = HelloMsg::Type,
    Message_Script = ScriptMsg::Type,

    Message_Max
};

YOJIMBO_MESSAGE_FACTORY_START(LD38MessageFactory, yojimbo::MessageFactory, Message_Max)
    YOJIMBO_DECLARE_MESSAGE_TYPE(HelloMsg::Type, HelloMsg)
    YOJIMBO_DECLARE_MESSAGE_TYPE(ScriptMsg::Type, ScriptMsg)
YOJIMBO_MESSAGE_FACTORY_FINISH()

}
