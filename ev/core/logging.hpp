#pragma once

#include <iostream>

namespace ev { namespace detail {

static const char * g_info_prefix =     "[    info]";
static const char * g_warning_prefix =  "[ warning]";
static const char * g_error_prefix =    "[   error]";
static const char * g_critical_prefix = "[critical]";

}
template <typename T>
struct print_impl_t
{
    static inline void print(std::ostream & out,const T & t)
    {
        out << t;
    }
};


struct printer_t
{
    printer_t(std::ostream & out,const char * prefix = nullptr):stream(out){
        if(prefix)
            stream << prefix;
    }
    ~printer_t(){
        stream << std::endl;
    }

    template <typename T>
    printer_t & operator [] (const T & d)
    {
        stream << '[' ;
        print_impl_t<T>::print(stream,d);
        stream << ']';
        return *this;
    }

    template <typename T>
    printer_t & operator << (const T & d)
    {
        stream << ' ' ;
        print_impl_t<T>::print(stream,d);
        return *this;
    }
private:
    std::ostream & stream;
};



inline printer_t debug(std::ostream& out = std::cout){
    return printer_t(out);
}

inline printer_t warning(std::ostream& out = std::cout){
    return printer_t(out,detail::g_warning_prefix);
}
inline printer_t error(std::ostream& out = std::cout){
    return printer_t(out,detail::g_error_prefix);
}

inline printer_t critical(std::ostream& out = std::cout){
    return printer_t(out,detail::g_critical_prefix);
}

inline printer_t info(std::ostream& out = std::cout){
    return printer_t(out,detail::g_info_prefix);
}


} // ev


