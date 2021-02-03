#include "unix_pipe.h"
#include "pipe_exception.h"
#include "../system.h"
#if !WIN32
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define CORE to_pipe_ptr(m_core)

#define PIPE_PREFIX "/var/run/{3A165FEC-9E18-47FF-9CCE-5EDCB4CBE44A}."

typedef struct tagPipe
{
    int         fd;
    std::string name;
} pipe_t, *pipe_ptr_t;

static inline pipe_ptr_t to_pipe_ptr(void* ptr)
{
    return static_cast<pipe_ptr_t>(ptr);
}

static int read_all(int fd, std::string& container);

ipc::unix_pipe::unix_pipe(const std::string & name)
    : m_core(new pipe_t())
{
    CORE->name = PIPE_PREFIX + name;
    if (::access(CORE->name.c_str(), F_OK))
    {
        // File not exist.
        //printf("%s not exist.\n", CORE->name.c_str());
        // Create it.
        int rv = ::mkfifo(CORE->name.c_str(), 0666);
        //printf("mkfifo: %d\n", rv);
        if (0 != rv)
        {
            throw ipc::pipe_exception(system::get_last_error(), name);
        }
        CORE->fd = ::open(CORE->name.c_str(), O_RDWR);
    }
    else
    {
        CORE->fd = ::open(CORE->name.c_str(), O_CREAT | O_RDWR);
    }
    //printf("CORE->fd: %d\n", CORE->fd);
    if (-1 == CORE->fd)
    {
        throw ipc::pipe_exception(system::get_last_error(), name);
    }
}

ipc::unix_pipe::~unix_pipe()
{
    if (NULL != m_core)
    {
        ::close(CORE->fd);
        delete CORE;
        m_core = NULL;
    }
}

void ipc::unix_pipe::send(const std::string & target, const void * data, unsigned int size)
{
    std::string pipeName = PIPE_PREFIX + target;
    int fd = ::open(pipeName.c_str(), O_WRONLY);
    if (-1 == fd)
    {
        throw ipc::pipe_exception(system::get_last_error(), pipeName);
    }
    int rv = ::write(fd, data, size);
    if (-1 == rv)
    {
        ::close(fd);
        throw ipc::pipe_exception(system::get_last_error(), pipeName);
    }
    ::close(fd);
}

std::string ipc::unix_pipe::receive()
{
    if (-1 == CORE->fd)
    {
        throw ipc::pipe_exception(system::get_last_error(), CORE->name);
    }
    std::string res;
    if (-1 == read_all(CORE->fd, res))
    {
        throw ipc::pipe_exception(system::get_last_error(), CORE->name);
    }
    return res;
}

int read_all(int fd, std::string& container)
{
    container.clear();
    int totalReadSize = 0;
    while (true)
    {
        unsigned char buffer[256] = { 0 };
        int readSize = ::read(fd, buffer, sizeof(buffer) / sizeof(*buffer));
        if (readSize < 0)
        {
            return readSize;
        }
        totalReadSize += readSize;
        container.append(buffer, buffer + readSize);
        if (readSize < 256)
        {
            break;
        }
    }
    return totalReadSize;
}

#endif // !WIN32