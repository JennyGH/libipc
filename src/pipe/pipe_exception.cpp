#include "pipe_exception.h"
#include "system.h"
#include <string.h>

ipc::pipe_exception::pipe_exception(int code, const std::string & name)
    : std::runtime_error(system::get_error_message(code))
    , m_pipe_name(name)
{
}

ipc::pipe_exception::~pipe_exception() throw()
{
}

const char * ipc::pipe_exception::pipe_name() const
{
    return this->m_pipe_name.c_str();
}