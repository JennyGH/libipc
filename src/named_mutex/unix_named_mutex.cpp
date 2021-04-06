#include "unix_named_mutex.h"
#include "named_mutex_exception.h"
#include "../system.h"
#if !_MSC_VER
#include <errno.h>     /* For errno constants */
#include <fcntl.h>     /* For O_* constants */
#include <sys/stat.h>  /* For mode constants */
#include <semaphore.h> /* For sem_* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONSOLE(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define CORE to_real_type_ptr(m_core)

typedef struct tagSemophare {
    sem_t*      sem;
    bool        created;
    bool        opened;
    std::string name;
} semophare_t, *semophare_ptr_t;

static inline semophare_ptr_t to_real_type_ptr(void* ptr)
{
    return static_cast<semophare_ptr_t>(ptr);
}

ipc::unix_named_mutex::unix_named_mutex(const std::string & name)
    : m_core(new semophare_t())
{
    int err = 0;
    CORE->sem = NULL;
    CORE->created = false;
    CORE->opened = false;
    CORE->name = name;
    // Try to create semophare.
    CORE->sem = ::sem_open(name.empty() ? NULL : name.c_str(), O_CREAT | O_EXCL, 0666, 1);
    if (NULL != CORE->sem)
    {
        CONSOLE("Created semophare.");
        CORE->created = true;
        CORE->opened = true;
        return;
    }
    err = system::get_last_error();
    CONSOLE("Fail to open semophare of \"%s\" with oflag=(O_CREAT | O_EXCL), because: %s(%d).", name.c_str(), system::get_error_message(err).c_str(), err);
    // Try to open existed semophare.
    CORE->sem = ::sem_open(name.empty() ? NULL : name.c_str(), O_EXCL);
    if (NULL != CORE->sem)
    {
        CONSOLE("Opened existed semophare.");
        CORE->opened = true;
        return;
    }
    err = system::get_last_error();
    CONSOLE("Fail to open semophare of \"%s\" with oflag=O_EXCL, because: %s(%d).", name.c_str(), system::get_error_message(err).c_str(), err);
}

ipc::unix_named_mutex::~unix_named_mutex()
{
    int err = 0;
    if (NULL == CORE)
    {
        return;
    }
    if (CORE->opened  && NULL != CORE->sem) // If opened in this process, close it and remove it.
    {
        int rv = 0;
        rv = ::sem_close(CORE->sem);
        err = system::get_last_error();
        CONSOLE("Try to close semophare of \"%s\", rv: %d, because: %s(%d).", CORE->name.c_str(), rv, system::get_error_message(err).c_str(), err);
        rv = ::sem_unlink(CORE->name.empty() ? NULL : CORE->name.c_str());
        err = system::get_last_error();
        CONSOLE("Try to remove semophare of \"%s\", rv: %d, because: %s(%d).", CORE->name.c_str(), rv, system::get_error_message(err).c_str(), err);
    }
    //else if (CORE->created && NULL != CORE->sem) // If only created in this process, remove it.
    //{
    //    int rv = ::sem_unlink(CORE->name.empty() ? NULL : CORE->name.c_str());
    //    err = system::get_last_error();
    //    CONSOLE("Try to remove semophare of \"%s\", rv: %d, because: %s(%d).", CORE->name.c_str(), rv, system::get_error_message(err).c_str(), err);
    //}
    CORE->sem = NULL;
}

void ipc::unix_named_mutex::lock()
{
    if (NULL == CORE)
    {
        return;
    }
    if (NULL == CORE->sem)
    {
        return;
    }
    ::sem_wait(CORE->sem);
}

void ipc::unix_named_mutex::unlock()
{
    if (NULL == CORE)
    {
        return;
    }
    if (NULL == CORE->sem)
    {
        return;
    }
    ::sem_post(CORE->sem);
}
#endif // !_MSC_VER
