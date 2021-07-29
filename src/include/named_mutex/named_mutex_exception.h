#pragma once
#include <string>
#include <stdexcept>
namespace ipc
{
    class named_mutex_exception : public std::runtime_error
    {
    public:
        named_mutex_exception(int code);
        ~named_mutex_exception() throw();
    };
}