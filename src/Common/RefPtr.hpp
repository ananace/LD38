#pragma once

#include <type_traits>
#include <utility>

template<typename T>
class RefPtr
{
private:
    class has_refcounts
    {
        typedef char True;
        typedef long False;

        template<typename C> static True testAdd(decltype(&T::AddRef));
        template<typename C> static False testAdd(...);
        template<typename C> static True testRelease(decltype(&T::Release));
        template<typename C> static False testRelease(...);

    public:
        enum { value = (sizeof(testAdd<T>(0)) == sizeof(True) &&
                        sizeof(testRelease<T>(0) == sizeof(True))) };
    };

    static_assert(has_refcounts::value, "Type must be reference counted");

public:
    RefPtr()
        : m_refPtr(nullptr)
    { }
    RefPtr(std::nullptr_t)
        : m_refPtr(nullptr)
    { }
    RefPtr(T* ptr)
        : m_refPtr(ptr)
    { } // Assuming implicit initial reference exists
    RefPtr(const RefPtr& ptr)
        : m_refPtr(ptr.m_refPtr)
    { if (m_refPtr) m_refPtr->AddRef(); }
    template<typename Other>
    RefPtr(const RefPtr<Other>& ptr)
        : m_refPtr(static_cast<T*>(ptr.m_refPtr))
    { if (m_refPtr) m_refPtr->AddRef(); }
    RefPtr(RefPtr&& ptr)
        : m_refPtr(std::move(ptr.m_refPtr))
    { if (m_refPtr) m_refPtr->AddRef(); }
    ~RefPtr()
    {
        if (m_refPtr)
            m_refPtr->Release();
    }

    RefPtr& operator=(const RefPtr& rhs)
    {
        if (this == &rhs || m_refPtr == rhs.m_refPtr)
            return *this;

        Reset(rhs.m_refPtr);

        return *this;
    }

    template<typename Other>
    RefPtr& operator=(const RefPtr<Other>& rhs)
    {
        if (this == &rhs || m_refPtr == rhs.m_refPtr)
            return *this;

        Reset(static_cast<T*>(rhs.m_refPtr));

        return *this;
    }

    RefPtr& operator=(T* rhs)
    {
        if (m_refPtr == rhs)
            return *this;

        Reset(rhs);

        return *this;
    }

    T& operator*() const { return *m_refPtr; }
    T* operator->() const { return m_refPtr; }

    T* Get() const
    {
        return m_refPtr;
    }
    void Reset(T* newPtr = nullptr)
    {
        auto tmp = m_refPtr;
        m_refPtr = newPtr;

        if (m_refPtr)
            m_refPtr->AddRef();
        if (tmp)
            tmp->Release();
    }
    T* Release()
    {
        auto tmp = m_refPtr;
        m_refPtr = nullptr;
        return tmp;
    }

    bool operator!() const { return !m_refPtr; }
    bool IsValid() const { return m_refPtr; }

    bool operator==(const RefPtr& rhs) const
    { return m_refPtr == rhs.m_refPtr; }
    bool operator==(T* rhs) const
    { return m_refPtr == rhs; }
    friend bool operator==(const T* lhs, const RefPtr& rhs)
    { return lhs == rhs.m_refPtr; }

    bool operator!=(const RefPtr& rhs) const
    { return m_refPtr != rhs.m_refPtr; }
    bool operator!=(T* rhs) const
    { return m_refPtr != rhs; }
    friend bool operator!=(const T* lhs, const RefPtr& rhs)
    { return lhs != rhs.m_refPtr; }

    bool operator<(const RefPtr& rhs) const
    { return m_refPtr < rhs.m_refPtr; }

private:
    T* m_refPtr;
};
