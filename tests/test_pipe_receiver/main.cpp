#include <pipe/pipe.h>
#include <pipe/pipe_exception.h>

int main()
try
{
    ipc::pipe pipe("test_pipe_receiver");

    while (true)
    {
        try
        {
            std::string received = pipe.receive();
            printf("received: %s\n", received.c_str());
        }
        catch (const ipc::pipe_exception& ex)
        {
            printf("%s %s\n", ex.pipe_name(), ex.what());
        }
    }

    return 0;
}
catch (const ipc::pipe_exception& ex)
{
    printf("%s %s\n", ex.pipe_name(), ex.what());
}