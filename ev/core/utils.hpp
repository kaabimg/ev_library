#pragma once

#include <chrono>

namespace ev
{
struct elapsed_time
{
    void restart() { _start = current_time_ms(); }
    std::size_t elapsed_ms() const { return current_time_ms() - _start; }
protected:
    static std::size_t current_time_ms()
    {
        using ms = std::chrono::milliseconds;
        return std::chrono::duration_cast<ms>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

private:
    std::size_t _start{current_time_ms()};
};

}  // ev
