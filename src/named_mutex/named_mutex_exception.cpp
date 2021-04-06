#include "named_mutex_exception.h"
#include "system.h"

ipc::named_mutex_exception::named_mutex_exception(int code)
    : std::runtime_error(system::get_error_message(code))
{
}

ipc::named_mutex_exception::~named_mutex_exception() throw() {}