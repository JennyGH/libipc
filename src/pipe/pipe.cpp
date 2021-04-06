#include "pipe.h"
#if _MSC_VER
#    include "windows_pipe.h"
typedef ipc::windows_pipe real_type;
#else
#    include "unix_pipe.h"
typedef ipc::unix_pipe real_type;
#endif // _MSC_VER

#define CONVERT_TO_REAL_PTR(ptr) static_cast<real_type*>(ptr)

#define CONVERT_TO_REAL_REF(ptr) (*(static_cast<real_type*>(ptr)))

#define CORE CONVERT_TO_REAL_PTR(m_core)

ipc::pipe::pipe(const std::string& name)
    : m_core(new real_type(name))
{
}

ipc::pipe::~pipe()
{
    if (NULL != m_core)
    {
        delete CORE;
        m_core = NULL;
    }
}

void ipc::pipe::send(
    const std::string& target,
    const void*        data,
    unsigned int       size)
{
    CORE->send(target, data, size);
}

void ipc::pipe::send(const std::string& target, const std::string& data)
{
    this->send(target, data.data(), data.length());
}

std::string ipc::pipe::receive()
{
    return CORE->receive();
}
