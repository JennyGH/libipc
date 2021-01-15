#include <pipe.h>
#include <pipe_exception.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>

static const char data[] = "Hello";

int main()
try
{
    ipc::pipe pipe("test_pipe_sender");
    while (true)
    {
        try
        {
            pipe.send("test_pipe_receiver", data, sizeof(data) / sizeof(*data));
        }
        catch (const ipc::pipe_exception& ex)
        {
            printf("%s %s\n", ex.pipe_name(), ex.what());
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
catch (const ipc::pipe_exception& ex)
{
    printf("%s %s\n", ex.pipe_name(), ex.what());
}