#pragma once

#include <libyojimbo/yojimbo_message.h>
#include <type_traits>

template<typename T>
class MessagePtr
{
public:
    static_assert(std::is_base_of<yojimbo::Message, T>::value,
                  "Type must be a libyojimbo message");

    typedef T message_t;
    typedef yojimbo::MessageFactory factory_t;

    MessagePtr()
        : m_messagePtr(nullptr)
        , m_factoryPtr(nullptr)
    { }
    MessagePtr(std::nullptr_t)
        : m_messagePtr(nullptr)
        , m_factoryPtr(nullptr)
    { }
    MessagePtr(message_t* message, factory_t* factory)
        : m_messagePtr(message)
        , m_factoryPtr(factory)
    { } // Should come with an implicit ref from creation
    MessagePtr(const MessagePtr& copy)
        : m_messagePtr(copy.m_messagePtr)
        , m_factoryPtr(copy.m_factoryPtr)
    { AddRef(); }
    template<typename Other>
    MessagePtr(const MessagePtr<Other>& copy)
        : m_messagePtr(static_cast<message_t*>(copy.m_messagePtr))
        , m_factoryPtr(copy.m_factoryPtr)
    { AddRef(); }
    MessagePtr(MessagePtr&& move)
        : m_messagePtr(std::move(move.m_messagePtr))
        , m_factoryPtr(std::move(move.m_factoryPtr))
    { }
    ~MessagePtr()
    {
        Release();

        m_messagePtr = nullptr;
        m_factoryPtr = nullptr;
    }

    MessagePtr& operator=(const MessagePtr& rhs)
    {
        Assign(rhs);
        return *this;
    }

    template<typename Other>
    MessagePtr& operator=(const MessagePtr<Other>& rhs)
    {
        Assign(rhs);
        return *this;
    }

    MessagePtr& operator=(message_t* message)
    {
        if (m_messagePtr == message)
            return *this;

        assert(m_factoryPtr);

        auto tmp = m_messagePtr;
        m_messagePtr = message;
        if (m_messagePtr)
            m_factoryPtr->AddRef(m_messagePtr);
        if (tmp)
            m_factoryPtr->Release(tmp);
        return *this;
    }

    message_t& operator*() const { return *m_messagePtr; }
    message_t* operator->() const { return m_messagePtr; }

    message_t* Get()
    {
        AddRef();
        return m_messagePtr;
    }
    void Reset(message_t* ptr = nullptr)
    {
        assert(m_factoryPtr);

        auto tmp = m_messagePtr;
        m_messagePtr = ptr;

        if (m_messagePtr)
            m_factoryPtr->AddRef(m_messagePtr);
        if (tmp)
            m_factoryPtr->Release(tmp);
    }

    bool operator!() const { return !m_messagePtr; }
    bool IsValid() const { return m_messagePtr; }

    bool operator==(const MessagePtr& rhs) const
    { return m_messagePtr == rhs.m_messagePtr; }
    bool operator==(message_t* rhs) const
    { return m_messagePtr == rhs; }
    friend bool operator==(const message_t* lhs, const MessagePtr& rhs)
    { return lhs == rhs.m_messagePtr; }

    bool operator!=(const MessagePtr& rhs) const
    { return m_messagePtr != rhs.m_messagePtr; }
    bool operator!=(message_t* rhs) const
    { return m_messagePtr != rhs; }
    friend bool operator!=(const message_t* lhs, const MessagePtr& rhs)
    { return lhs != rhs.m_messagePtr; }

    bool operator<(const MessagePtr& rhs) const
    { return m_messagePtr < rhs.m_messagePtr; }

    /* The "safe bool idiom"
     *
     * To avoid issues with classes containing a bool operator, where
     * comparisons like `A == B` might give erroneous results.
     */
private:
    typedef message_t* MessagePtr::*unspecified_bool_type;

public:
    operator unspecified_bool_type() const {
        return IsValid() ? m_messagePtr : nullptr;
    }

protected:
    void AddRef()
    {
        assert(m_factoryPtr);
        if (m_messagePtr)
            m_factoryPtr->AddRef(m_messagePtr);
    }

    void Release()
    {
        assert(m_factoryPtr);
        if (m_messagePtr)
            m_factoryPtr->Release(m_messagePtr);
    }

private:
    template<class Other>
    void Assign(const MessagePtr<Other>& ptr)
    {
        if (this == &ptr || m_messagePtr == ptr.m_messagePtr)
            return;

        auto tmpFact = m_factoryPtr;
        auto tmpMsg = m_messagePtr;

        m_messagePtr = ptr.m_messagePtr;
        m_factoryPtr = ptr.m_factoryPtr;

        if (m_messagePtr)
            m_factoryPtr->AddRef(m_messagePtr);
        if (tmpMsg)
            tmpFact->Release(tmpMsg);
    }

    message_t* m_messagePtr;
    factory_t* m_factoryPtr;

    template<class Other>
    friend class MessagePtr;
};

typedef MessagePtr<yojimbo::Message> MessagePtrBase;
