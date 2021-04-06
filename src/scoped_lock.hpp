#pragma once
#include <lockable.h>
namespace ipc
{
    template <class T>
    class scoped_lock
    {
    public:
        scoped_lock(T& obj)
            : m_lockable(obj)
        {
            m_lockable.lock();
        }
        ~scoped_lock()
        {
            m_lockable.unlock();
        }

    private:
        T& m_lockable;
    };
} // namespace ipc