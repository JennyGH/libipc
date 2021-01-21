#include "named_mutex.h"
#if WIN32
#include "windows_named_mutex.h"
typedef ipc::windows_named_mutex real_type;
#else
#include "unix_named_mutex.h"
typedef ipc::unix_named_mutex real_type;
#endif // WIN32

#define CONVERT_TO_REAL_PTR(ptr)\
static_cast<real_type*>(ptr)

#define CONVERT_TO_REAL_REF(ptr)\
(*(static_cast<real_type*>(ptr)))

#define CORE CONVERT_TO_REAL_PTR(m_core)

ipc::named_mutex::named_mutex(const std::string & name)
    : m_core(new real_type(name))
{
}

ipc::named_mutex::~named_mutex()
{
    if (NULL != m_core)
    {
        delete CORE;
        m_core = NULL;
    }
}

void ipc::named_mutex::lock()
{
    CORE->lock();
}

void ipc::named_mutex::unlock()
{
    CORE->unlock();
}
