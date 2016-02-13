#include "utils.h"


#include <chrono>


using namespace ev::core;



std::size_t ev::core::time_t::current_time_ms()
{
    using ms = std::chrono::milliseconds;
    return std::chrono::duration_cast<ms>(std::chrono::system_clock::now().time_since_epoch()).count();

}

void ev::core::time_t::start()
{

    m_start = current_time_ms();
}

std::size_t ev::core::time_t::elapsed_ms() const
{
    return current_time_ms() - m_start;
}


