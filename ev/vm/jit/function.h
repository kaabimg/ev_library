#ifndef EV_VM_JIT_FUNCTION_H
#define EV_VM_JIT_FUNCTION_H

#include "utils.h"
#include "value.h"
#include "object.h"


namespace ev { namespace vm { namespace jit {

struct block_t;

struct compilation_scope_t
{
    virtual value_t find_variable(const std::string & name) const = 0;
};



struct function_private_t;
struct function_t : object_t<function_private_t>, compilation_scope_t
{
    const function_creation_info_t& info()const;
    block_t new_block(const std::string & name);
    bool finalize();

    operator function_data_t()const;

    value_t find_variable(const std::string & name) const override;

protected:


    friend class module_t;
};


struct block_private_t;
struct block_t : object_t<block_private_t>, compilation_scope_t
{

    value_t find_variable(const std::string & name) const override;
    void set_as_insert_point();
    void set_return(value_t);

protected:

    friend class function_t;

};



}}}


#endif//EV_VM_JIT_FUNCTION_H
