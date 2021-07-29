#pragma once
#include <string>
#include <lockable.h>
namespace ipc
{
    class named_mutex : public lockable
    {
    public:
        named_mutex(const std::string& name);
        ~named_mutex();

        virtual void lock();

        virtual void unlock();

    private:
        void* m_core;
    };
}