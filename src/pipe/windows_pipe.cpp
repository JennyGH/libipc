#include "windows_pipe.h"
#include "pipe_exception.h"
#if WIN32
#include <Windows.h>

#define CORE to_pipe_ptr(m_core)

#define PIPE_PREFIX     "\\\\.\\pipe\\{8EF0E8DD-D266-4025-97D3-63408380ABE2}\\"

typedef struct tagPipe
{
    HANDLE              hPipe;
    SECURITY_DESCRIPTOR securityDescriptor;
    SECURITY_ATTRIBUTES securityAttributes;
    std::string         data;
} PIPE, *PPIPE;

static inline PPIPE to_pipe_ptr(void* ptr)
{
    return static_cast<PPIPE>(ptr);
}

static void try_close_handle(HANDLE& hPipe);

static DWORD receive_all(HANDLE hFile, std::string& container);

static BOOL wait_pipe(const std::string& pipeName, DWORD timeout);

ipc::windows_pipe::windows_pipe(const std::string & name)
    : m_core(new PIPE())
{
    // ------------ 这里主要解决不同会话的管道权限问题 ------------
    BYTE aclBuffer[1024] = { 0 };
    BYTE sidBuffer[100] = { 0 };
    PACL pacl = (PACL)&aclBuffer; // 声明一个ACL，长度是1024
    PSID psid = (PSID)&sidBuffer; // 声明一个SID，长度是100
    SID_NAME_USE snu;
    DWORD sidBufferSize = 100;
    DWORD domainBufferSize = 80;
    CHAR  domainBuffer[80];

    ::InitializeSecurityDescriptor(&CORE->securityDescriptor, SECURITY_DESCRIPTOR_REVISION);
    ::InitializeAcl(pacl, 1024, ACL_REVISION);
    ::LookupAccountNameA(0, ("everyone"), psid, &sidBufferSize, domainBuffer, &domainBufferSize, &snu);
    ::AddAccessAllowedAce(pacl, ACL_REVISION, GENERIC_ALL, psid);
    ::SetSecurityDescriptorDacl(&CORE->securityDescriptor, TRUE, pacl, FALSE);

    CORE->securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    CORE->securityAttributes.lpSecurityDescriptor = &CORE->securityDescriptor; // 把SD设置到SA中
    CORE->securityAttributes.bInheritHandle = FALSE;
    // ---------------------------------------------------------

    // 创建事件内核对象保持服务与应用进程的同步
    CORE->hPipe = ::CreateNamedPipeA(
        (PIPE_PREFIX + name).c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        0,
        0,
        NMPWAIT_WAIT_FOREVER,
        &CORE->securityAttributes
    );
}

ipc::windows_pipe::~windows_pipe()
{
    if (NULL != m_core)
    {
        try_close_handle(CORE->hPipe);
        delete CORE;
        m_core = NULL;
    }
}

void ipc::windows_pipe::send(const std::string& target, const void* data, unsigned int size)
{
    HANDLE hFile = NULL;
    BOOL   bSuccess = FALSE;
    bSuccess = WaitNamedPipeA((PIPE_PREFIX + target).c_str(), NMPWAIT_WAIT_FOREVER);
    //bSuccess = wait_pipe(PIPE_PREFIX + target, NMPWAIT_WAIT_FOREVER);
    if (!bSuccess)
    {
        throw ipc::pipe_exception(::GetLastError(), target);
    }
    hFile = ::CreateFileA(
        (PIPE_PREFIX + target).c_str(),
        GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL
    );
    if (NULL == hFile || INVALID_HANDLE_VALUE == hFile)
    {
        throw ipc::pipe_exception(::GetLastError(), target);
    }
    DWORD dwBytesWrote = 0;
    bSuccess = ::WriteFile(hFile, data, size, &dwBytesWrote, NULL); // 往管道写数据
    if (!bSuccess)
    {
        try_close_handle(hFile);
        throw ipc::pipe_exception(::GetLastError(), target);
    }
    bSuccess = ::FlushFileBuffers(hFile);
    bSuccess = ::DisconnectNamedPipe(hFile);
    try_close_handle(hFile);
}

std::string ipc::windows_pipe::receive()
{
    std::string res;
    BOOL bSuccess = FALSE;
    bSuccess = ::ConnectNamedPipe(CORE->hPipe, NULL);
    if (!bSuccess)
    {
        goto end;
    }
    receive_all(CORE->hPipe, res);
    bSuccess = ::DisconnectNamedPipe(CORE->hPipe);
    if (!bSuccess)
    {
        goto end;
    }
end:
    return res;
}

void try_close_handle(HANDLE& hPipe)
{
    if (NULL != hPipe)
    {
        ::CloseHandle(hPipe);
        hPipe = NULL;
    }
}

DWORD receive_all(HANDLE hFile, std::string& container)
{
    container.clear();
    DWORD dwReadBytes = 0;
    BOOL bSuccess = TRUE;
    do
    {
        BYTE buffer[256] = { 0 };
        DWORD dwCurrentReadBytes = ARRAYSIZE(buffer);
        bSuccess = ::ReadFile(hFile, buffer, dwCurrentReadBytes, &dwCurrentReadBytes, NULL); // 往管道写数据
        container.append(buffer, buffer + dwCurrentReadBytes);
        dwReadBytes += dwCurrentReadBytes;
        if (dwCurrentReadBytes < 256)
        {
            break;
        }
    } while (bSuccess);
    return dwReadBytes;
}

BOOL wait_pipe(const std::string & pipeName, DWORD timeout)
{
    BOOL bSuccess = FALSE;
    while (true)
    {
        bSuccess = WaitNamedPipeA(pipeName.c_str(), timeout);
        if (bSuccess)
        {
            return TRUE;
        }
        DWORD dwError = ::GetLastError();
        if (dwError == ERROR_FILE_NOT_FOUND && timeout == NMPWAIT_WAIT_FOREVER)
        {
            ::Sleep(1);
            continue;
        }
        break;
    }
    return bSuccess;
}


#endif // WIN32