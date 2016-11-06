#ifndef EV_LOGGING_H
#define EV_LOGGING_H

#include <iostream>
namespace ev { namespace detail {

static const char * g_debug_prefix =    "[   debug]";
static const char * g_info_prefix =     "[    info]";
static const char * g_warning_prefix =  "[ warning]";
static const char * g_error_prefix =    "[   error]";
static const char * g_critical_prefix = "[critical]";

struct ev_print_object {

    bool first = true;


    ev_print_object(const char * prefix){
        std::cout << prefix;
    }

    ~ev_print_object(){std::cout << std::endl<<std::flush;}

    template <typename T>
    ev_print_object & operator << (T && d)
    {
        std::cout << " " << std::forward<T>(d);

        return *this;
    }
};


template <typename T>
inline void print_helper(ev_print_object & print_obj,const T & t){
    print_obj << t;
}

template <typename H,typename ... T>
inline void print_helper(ev_print_object & print_obj,const H & h,const T & ... t){
    print_obj << h;
    print_helper(print_obj,t...);
}

inline std::string create_indent(size_t level){
    std::string indent(level*4,' ');
    for(size_t i =0;i< level*4 ;i = i+2){
        indent[i] = '.';
    }
    return std::move(indent);
}

}


inline detail::ev_print_object debug(){
    return detail::ev_print_object(detail::g_debug_prefix);
}

inline detail::ev_print_object warning(){
    return detail::ev_print_object(detail::g_warning_prefix);
}
inline detail::ev_print_object error(){
    return detail::ev_print_object(detail::g_error_prefix);
}

inline detail::ev_print_object critical(){
    return detail::ev_print_object(detail::g_critical_prefix);
}

inline detail::ev_print_object info(){
    return detail::ev_print_object(detail::g_info_prefix);
}


template <typename H,typename ... T>
inline void debug(const H & h,const T & ... t){
    detail::ev_print_object print_obj(detail::g_debug_prefix);
    detail::print_helper(print_obj,h,t...);
}

} // ev



#define EV_TRACE(level)  ev::debug()<< ev::detail::create_indent(level) << __PRETTY_FUNCTION__

#endif // EV_LOGGING_H

