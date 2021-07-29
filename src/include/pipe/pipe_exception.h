#pragma once
#include <string>
#include <stdexcept>
namespace ipc
{
    class pipe_exception : public std::runtime_error
    {
    public:
        pipe_exception(int code, const std::string& name);
        ~pipe_exception() throw();

        const char* pipe_name() const;

    private:
        std::string m_pipe_name;
    };
}