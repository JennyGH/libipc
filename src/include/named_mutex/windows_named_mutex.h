#pragma once
#include <string>
namespace ipc
{
#if _MSC_VER
    class windows_named_mutex
    {
    public:
        windows_named_mutex(const std::string& name);
        ~windows_named_mutex();

        void lock();

        void unlock();

    private:
        void* m_core;
    };
#endif // _MSC_VER
}