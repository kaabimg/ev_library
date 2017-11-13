#pragma once
#include "logging.hpp"
#include <vector>
#include <ostream>
#include <string>

////////////////////////////////////////////////
///
EV_CUSTOM_TEMPLATE_PRINTER(std::vector, ostream, data)
{
    ostream << '[';
    if (data.size()) {
        ostream << ' ';
        ev::print_impl<T>::print(ostream, data[0]);
        for (int i = 1; i < data.size(); ++i) {
            ostream << " , ";
            ev::print_impl<T>::print(ostream, data[i]);
        }
        ostream << ' ';
    }
    ostream << ']';
}
