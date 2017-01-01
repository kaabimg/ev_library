#pragma once

#include <ev/core/preprocessor.hpp>
#include <ev/core/logging.hpp>
#include <ev/core/logging_helpers.hpp>
#include <ev/core/flags.hpp>

#include <exception>
#include <string>
#include <vector>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
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


EV_FLAGS(device_type_e)

inline std::ostream& operator << (std::ostream & os,ev::flags_t<device_type_e> flags)
{
    bool first = true;

    if( flags.test(device_type_e::default_) )
    {
        os << "default";
        first = false;
    }

    if( flags.test(device_type_e::cpu) )
    {
        if(!first) os << " | ";
        os << "cpu";
        first = false;
    }

    if( flags.test(device_type_e::gpu) )
    {
        if(!first) os << " | ";
        os << "gpu";
        first = false;
    }

    if( flags.test(device_type_e::accelerator) )
    {
        if(!first) os << " | ";
        os << "accelerator";
        first = false;
    }

    if( flags.test(device_type_e::custom) )
    {
        if(!first) os << " | ";
        os << "custom";
        first = false;
    }

    return os;
}



enum class memory_flags_e {
    read_write            = CL_MEM_READ_WRITE,
    write_only            = CL_MEM_WRITE_ONLY,
    read_only             = CL_MEM_READ_ONLY,
    use_host_ptr          = CL_MEM_USE_HOST_PTR,
    alloc_host_ptr        = CL_MEM_ALLOC_HOST_PTR,
    compy_host_ptr        = CL_MEM_COPY_HOST_PTR,
    host_write_only       = CL_MEM_HOST_WRITE_ONLY,
    host_read_only        = CL_MEM_HOST_READ_ONLY,
    host_no_access        = CL_MEM_HOST_NO_ACCESS,
    svm_fine_grain_buffer = CL_MEM_SVM_FINE_GRAIN_BUFFER,
    svm_atomics           = CL_MEM_SVM_ATOMICS,
    kernel_read_and_write = CL_MEM_KERNEL_READ_AND_WRITE,
};

EV_FLAGS(memory_flags_e)

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


inline void check_status(cl_int status)
{
    if(status != CL_SUCCESS)
    {
        throw cl_error_t(status);
    }
}


template <typename T>
inline cl_int dummy_retain_function (T ) {
    return CL_SUCCESS;
}

template <typename T>
inline cl_int dummy_release_function (T ) {
    return CL_SUCCESS;
}
template <typename T>
struct cl_object_handler_t {
    typedef cl_int (*retain_function_t)(T);
    typedef cl_int (*release_function_t)(T);

    static constexpr retain_function_t retain_function = dummy_retain_function<T>;
    static constexpr release_function_t release_function = dummy_release_function<T>;

};


#define EV_OCL_DECLARE_CLEAR_FUNCTION(cl_type,retain_func,release_func)  \
    template <> \
    struct cl_object_handler_t<cl_type> \
{ \
    typedef cl_int (*retain_function_t)(cl_type); \
    typedef cl_int (*release_function_t)(cl_type); \
    static constexpr retain_function_t retain_function = retain_func; \
    static constexpr release_function_t release_function = release_func; \
};


struct object_wrapper_base_t {
    enum class init_mode_e {
        create,
        reuse
    };
};

template <typename T>
class object_wrapper_t : public object_wrapper_base_t
{
public:
    using wrapper_type = object_wrapper_t<T>;

    object_wrapper_t(T d,init_mode_e mode = init_mode_e::create)
    {
        m_data = d;
        if(mode == init_mode_e::reuse) retain();
    }

    void set_data(T d,init_mode_e mode )
    {
        release();
        m_data = d;
        if(mode == init_mode_e::reuse) retain();
    }

    object_wrapper_t(const object_wrapper_t & rhs)
    {
        m_data = rhs.m_data;
        retain();
    }

    object_wrapper_t(object_wrapper_t && rhs)
    {
        m_data = rhs.m_data;
        rhs.m_data = nullptr;
    }

    object_wrapper_t& operator =(const object_wrapper_t & rhs)
    {
        release();
        m_data = rhs.m_data;
        retain();
        return *this;
    }

    object_wrapper_t& operator =(object_wrapper_t && rhs)
    {
        release();
        m_data = rhs.m_data;
        rhs.m_data = nullptr;
        return *this;
    }

    ~object_wrapper_t()
    {
        release();
    }

    bool is_valid()const {return m_data != nullptr;}
    T cl_object() const {return m_data;}

private:
    void retain()
    {
        if(m_data) cl_object_handler_t<T>::retain_function(m_data);
    }
    void release()
    {
        if(m_data) cl_object_handler_t<T>::release_function(m_data);
    }

private:
    T m_data = nullptr;
};


}}


