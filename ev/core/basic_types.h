#ifndef EV_BASIC_TYPES_H
#define EV_BASIC_TYPES_H


namespace ev {

using address_t = void*;


struct mem_block_t {
    address_t data;
    unsigned int size;
};

struct construct_inplace_t{};



}


#endif // EV_BASIC_TYPES_H
