#pragma once

// TODO:
// Generate this whole file
// Probably a small ruby program for it

#include <yojimbo_serialize.h>

#include <stringstream>

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
    enum { Type = 0x0001 }

    template<typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_string(stream, PlayerName, 256);

        return true;
    }

    std::string PlayerName;
};

enum MessageTypes
{
    Message_Hello = HelloMsg::Type,

    Message_Max
};

YOJIMBO_MESSAGE_FACTORY_START(LD38MessageFactory, yojimbo::MessageFactory, Message_Max);
    YOJIMBO_DECLARE_MESSAGE_TYPE(HelloMsg::Type, HelloMsg);
YOJIMBO_MESSAGE_FACTORY_FINISH();

}
