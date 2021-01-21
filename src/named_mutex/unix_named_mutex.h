#pragma once
#include <string>
namespace ipc
{
#if !WIN32
    class unix_named_mutex
    {
    public:
        unix_named_mutex(const std::string& name);
        ~unix_named_mutex();

        void lock();

        void unlock();

    private:
        void* m_core;
    };
#endif // !WIN32
}