#pragma once

#include <ev/core/preprocessor.hpp>
#include <ev/core/logging.hpp>

#include <exception>
#include <string>
#include <vector>

#include <CL/cl.h>

namespace ev { namespace ocl {


enum class device_type_e : unsigned long int {

    default_    = CL_DEVICE_TYPE_DEFAULT,
    cpu         = CL_DEVICE_TYPE_CPU,
    gpu         = CL_DEVICE_TYPE_GPU,
    accelerator = CL_DEVICE_TYPE_ACCELERATOR,
    custom      = CL_DEVICE_TYPE_CUSTOM,
    all         = CL_DEVICE_TYPE_ALL
};

inline std::ostream& operator << (std::ostream & os,device_type_e type)
{
    bool first = true;

    const auto test_flag = [type](device_type_e other){
        return (unsigned long int)type & (unsigned long int)other;
    };

    if( test_flag(device_type_e::default_) )
    {
        if(!first) os << " | ";
        os << "default";
        first = false;
    }

    if( test_flag(device_type_e::cpu) )
    {
        if(!first) os << " | ";
        os << "cpu";
        first = false;
    }

    if( test_flag(device_type_e::gpu) )
    {
        if(!first) os << " | ";
        os << "gpu";
        first = false;
    }

    if( test_flag(device_type_e::accelerator) )
    {
        if(!first) os << " | ";
        os << "accelerator";
        first = false;
    }

    if( test_flag(device_type_e::custom) )
    {
        if(!first) os << " | ";
        os << "custom";
        first = false;
    }

    return os;
}


/////////

class cl_error_t : public std::exception {
public:
    cl_error_t(cl_int error);
    ~cl_error_t()noexcept{}

    const char* what()const noexcept;
    cl_int error()const;

private:
    void build_error_string();
private:
    cl_int m_error;
    std::string m_error_string;
};


inline void check_status(cl_int error)
{
    if(error != CL_SUCCESS)
    {
        throw cl_error_t(error);
    }
}

template <typename T>
class object_wrapper_t
{
public:
    using wrapper_type = object_wrapper_t<T>;

    object_wrapper_t(T data):m_data{data}
    {}

    T cl_data() const {return m_data;}
protected:
    T m_data;
};


}}

