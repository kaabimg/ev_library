
#include <ev/core/preprocessor.h>
#include <ev/core/logging.h>


int main ()
{
    ev::debug() << EV_ARG_COUNT(a,3) << EV_ARG_COUNT() ;
}

