#include "named_mutex/named_mutex.h"
#if _MSC_VER
#include "named_mutex/windows_named_mutex.h"
typedef ipc::windows_named_mutex real_type;
#else
#include "named_mutex/unix_named_mutex.h"
typedef ipc::unix_named_mutex real_type;
#endif // _MSC_VER

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
