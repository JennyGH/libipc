#pragma once
#include <string>
namespace ipc
{
    class pipe
    {
    public:
        pipe(const std::string& name);
        ~pipe();

        void send(const std::string& target, const void* data, unsigned int size);

        void send(const std::string& target, const std::string& data);

        std::string receive();

    private:
        void* m_core;
    };
}