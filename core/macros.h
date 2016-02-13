#ifndef EV_MACROS_H
#define EV_MACROS_H


// data

#define EV_PRIVATE(type_name) \
    struct data_t;\
    data_t* d; \
    type_name(const type_name&) = delete;\
    type_name& operator=(const type_name &)= delete;


//

#define ev_forever while(true)

#define ev_unused(v) (void)v



#endif // EV_MACROS_H

