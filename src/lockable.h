#pragma once
namespace ipc
{
    class lockable
    {
    public:
        virtual void lock()   = 0;
        virtual void unlock() = 0;
    };
} // namespace ipc