#ifndef EV_UTILS_H
#define EV_UTILS_H

#include <chrono>

namespace ev {

struct time_t
{

    void restart()
    {
        m_start = current_time_ms();
    }
    std::size_t elapsed_ms()const{
        return current_time_ms() - m_start;
    }


protected:
    static std::size_t current_time_ms(){
        using ms = std::chrono::milliseconds;
        return std::chrono::duration_cast<ms>(std::chrono::system_clock::now().time_since_epoch()).count();

    }

private:
    std::size_t m_start {current_time_ms() };
};





}//ev


#endif // EV_UTILS_H
