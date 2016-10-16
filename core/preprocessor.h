#ifndef EV_PREPROCESSOR_H
#define EV_PREPROCESSOR_H


// data
#define EV_PRIVATE(type_name) \
    struct data_t;\
    data_t* d; \
    type_name(const type_name&) = delete;\
    type_name& operator=(const type_name &)= delete;

//


#define ev_forever while(true)

#define ev_unused(v) (void)v

#define ev_delete_all(container) for(auto e : container) delete e

#define _EV_CONCAT_IMPL(a,b) a##b

#define EV_CONCAT(a,b) _EV_CONCAT_IMPL(a,b)


#ifdef __COUNTER__
#define EV_ANONYMOUS_VARIABLE(name) EV_CONCAT(name,__COUNTER__)
#else
#define EV_ANONYMOUS_VARIABLE(name) EV_CONCAT(name,__LINE__)
#endif


#define EV_ARGS_1(a,...) a
#define EV_ARGS_2(a,b,...) b
#define EV_ARG_2_OR_1(...) EV_ARGS_2(__VA_ARGS__,__VA_ARGS__)


#endif // EV_PREPROCESSOR_H

