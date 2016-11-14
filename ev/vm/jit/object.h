#ifndef EV_VM_JIT_OBJECT_H
#define EV_VM_JIT_OBJECT_H

#include <memory>

namespace ev { namespace vm { namespace jit {


template <typename PrivateData>
struct object_t {

    using data_type = PrivateData;

    inline bool is_valid()const{return m_data.get() != nullptr;}
    inline operator bool ()const {return is_valid();}


protected:
    std::shared_ptr<PrivateData> m_data { nullptr };

    template <typename T,typename ... Arg>
    friend T create_object(Arg && ... args);
};


template <typename T,typename ... Arg>
inline  T create_object(Arg && ... args){
    T instance;
    instance.m_data = std::make_shared<typename T::data_type>(std::forward<Arg>(args)...);
    return instance;
}


}}}


#endif//EV_VM_JIT_OBJECT_H
