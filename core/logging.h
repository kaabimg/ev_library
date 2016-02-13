#ifndef EV_DEBUG_H
#define EV_DEBUG_H

#include <iostream>
namespace ev { namespace core { namespace details {

static const char * g_debug_prefix =    "[debug   ]";
static const char * g_info_prefix =     "[info    ]";
static const char * g_warning_prefix =  "[warning ]";
static const char * g_error_prefix =    "[error   ]";
static const char * g_critical_prefix = "[critical]";

struct ev_print_object {

    bool first = true;


    ev_print_object(const char * prefix){
        std::cout << prefix;
    }

    ~ev_print_object(){std::cout << std::endl<<std::flush;}

    template <typename T>
    ev_print_object & operator << (const T & d)
    {
        std::cout << " " << d;

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


inline details::ev_print_object debug(){
    return details::ev_print_object(details::g_debug_prefix);
}

inline details::ev_print_object warning(){
    return details::ev_print_object(details::g_warning_prefix);
}
inline details::ev_print_object error(){
    return details::ev_print_object(details::g_error_prefix);
}

inline details::ev_print_object critical(){
    return details::ev_print_object(details::g_critical_prefix);
}

inline details::ev_print_object info(){
    return details::ev_print_object(details::g_info_prefix);
}


template <typename H,typename ... T>
inline void debug(const H & h,const T & ... t){
    details::ev_print_object print_obj(details::g_debug_prefix);
    details::print_helper(print_obj,h,t...);
}



}}



#define EV_TRACE(level)  ev::core::debug()<< ev::core::details::create_indent(level) << __PRETTY_FUNCTION__

#endif // EV_DEBUG_H

