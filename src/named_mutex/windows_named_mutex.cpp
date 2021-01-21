#include "windows_named_mutex.h"
#include "named_mutex_exception.h"
#if WIN32
#include <Windows.h>

#define CORE to_real_type_ptr(m_core)

typedef struct tagNamedMutex {
    HANDLE hMutex;
} NAMED_MUTEX, *PNAMED_MUTEX;

static inline PNAMED_MUTEX to_real_type_ptr(void* ptr)
{
    return static_cast<PNAMED_MUTEX>(ptr);
}

static void try_close_handle(HANDLE& handle)
{
    if (NULL != handle)
    {
        ::CloseHandle(handle);
        handle = NULL;
    }
}

ipc::windows_named_mutex::windows_named_mutex(const std::string & name)
    : m_core(new tagNamedMutex())
{
    CORE->hMutex = ::CreateMutexA(NULL, FALSE, name.empty() ? NULL : name.c_str());
}

ipc::windows_named_mutex::~windows_named_mutex()
{
    if (NULL != m_core)
    {
        try_close_handle(CORE->hMutex);
        delete CORE;
        m_core = NULL;
    }
}

void ipc::windows_named_mutex::lock()
{
    ::WaitForSingleObject(CORE->hMutex, INFINITE);
}

void ipc::windows_named_mutex::unlock()
{
    ::ReleaseMutex(CORE->hMutex);
}
#endif // WIN32