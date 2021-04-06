#include <named_mutex/named_mutex.h>
#include <scoped_lock.hpp>
#include <thread>
#include <stdio.h>
#include <stdlib.h>

int main()
try
{
    ipc::named_mutex mutex("test_named_mutex");

    ipc::scoped_lock<ipc::named_mutex> lock(mutex);

    // mutex.lock();
    return 0;
}
catch (const std::exception& ex)
{
    printf("%s\n", ex.what());
}