#pragma once
#include "logging.hpp"
#include <vector>
#include <ostream>
#include <string>


#define EV_CUSTOM_PRINTER(type) \
namespace ev { \
template <> \
struct print_impl_t<type> \
{ \
    static inline void print(std::ostream & out,const type & data);\
}; \
} \
void ev::print_impl_t<type>::print(std::ostream & ostream,const type & data)



#define EV_CUSTOM_TEMPLATE_PRINTER(type) \
namespace ev { \
template <typename T> \
struct print_impl_t <type<T>> \
{ \
    static inline void print(std::ostream & out,const type<T> & data);\
}; \
} \
template <typename T> \
void ev::print_impl_t<type<T>>::print(std::ostream & ostream,const type<T> & data)



///////////////////////////////////////////////
///
EV_CUSTOM_PRINTER(std::string)
{
    ostream << '\'';
    ostream << data.c_str();
    ostream << "' ";
}


////////////////////////////////////////////////
///
EV_CUSTOM_TEMPLATE_PRINTER(std::vector)
{
    ostream << '[';
    if(data.size())
    {
        ostream << ' ' ;
        ev::print_impl_t<T>::print(ostream,data[0]);
        for (int i = 1; i < data.size(); ++i) {
            ostream << " , ";
            ev::print_impl_t<T>::print(ostream,data[i]);
        }
        ostream << ']';
    }
}



