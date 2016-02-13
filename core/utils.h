#ifndef EV_UTILS_H
#define EV_UTILS_H

#include "types.h"

namespace ev { namespace core {



struct time_t
{
    void start();
    std::size_t elapsed_ms()const;

protected:
    static std::size_t current_time_ms();

private:
    std::size_t m_start {0};
};

}}



#endif // EV_UTILS_H
