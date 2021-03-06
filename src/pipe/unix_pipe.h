#pragma once
#include <string>
namespace ipc
{
#if !_MSC_VER
    class unix_pipe
    {
    public:
        unix_pipe(const std::string& name);
        ~unix_pipe();

        void send(
            const std::string&  target,
            const void*         data,
            unsigned int        size
        );

        std::string receive();

    private:
        void* m_core;
    };
#endif // !_MSC_VER
}