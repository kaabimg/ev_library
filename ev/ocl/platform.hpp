#pragma once

#include "common.hpp"

namespace ev { namespace ocl {

class device_t;

class platform_t : public object_wrapper_t<cl_platform_id>
{
public:
    platform_t(cl_platform_id id)
        :wrapper_type{id}
    {}

    static std::vector<platform_t> get_platforms();

    std::vector<device_t> get_devices(
            flags_t<device_type_e> type = device_type_e::all
            )const;

    // properties
    std::string name()const;
    std::string profile()const;
    std::string version()const;
    std::string vendor()const;
    std::string extensions()const;

protected:
    std::string get_string_info(cl_platform_info)const;
};

inline std::vector<platform_t> get_platfomrs()
{
    return platform_t::get_platforms();
}



}}
