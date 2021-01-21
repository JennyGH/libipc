#include "named_mutex_exception.h"
#include <string.h>
#if WIN32
#include <Windows.h>
#else
#endif // WIN32

static std::string get_error_message(int code);

ipc::named_mutex_exception::named_mutex_exception(int code)
    : std::runtime_error(get_error_message(code))
{
}

ipc::named_mutex_exception::~named_mutex_exception() throw()
{
}

std::string get_error_message(int code)
{
    std::string res;
#if WIN32
    LPVOID lpMsgBuf = NULL;
    DWORD  dwLength = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf,
        0,
        NULL
    );
    if (NULL != lpMsgBuf)
    {
        res.assign((LPCSTR)lpMsgBuf, dwLength);
        ::LocalFree(lpMsgBuf);
        lpMsgBuf = NULL;
        std::size_t length = res.size();
        while (length && (res[length - 1] == '\n' || res[length - 1] == '\r'))
        {
            res.erase(res.size() - 1);
            length--;
        }
    }
#else
    res.assign(::strerror(code));
#endif // WIN32
    return res;
}
