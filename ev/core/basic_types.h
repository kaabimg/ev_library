#pragma once


namespace ev {

using address_t = void*;


struct mem_block_t {
    address_t data;
    unsigned int size;
};

struct construct_inplace_t{};
struct empty_t{};

template <typename T = empty_t,bool cond = true>
struct loop_scoped_variable_t {
    template <typename ... A>
    loop_scoped_variable_t(A && ... d):data(std::forward<A>(d)...){}
    operator bool()const{return cond;}
    T data;
};

}
