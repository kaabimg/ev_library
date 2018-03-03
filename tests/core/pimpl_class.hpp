#pragma once

#include <ev/core/pimpl.hpp>
#include <ev/core/flags.hpp>

enum class function_type : uint {
    none = 0,
    default_constructor = 1,
    custom_constructor = 1 << 1,
    destructor = 1 << 2,
    copy_constructor = 1 << 3,
    copy_assignment = 1 << 4,

    move_constructor = 1 << 5,
    move_assignment = 1 << 6,
};
EV_FLAGS(function_type)


using function_type_ptr = std::shared_ptr<ev::flags<function_type>>;

struct impl_base {
    function_type_ptr ftype =
        std::make_shared<ev::flags<function_type>>(function_type::none);
    std::string data;

    impl_base()
    {
        *ftype |= function_type::default_constructor;
    }
    impl_base(int)
    {
        *ftype |= function_type::custom_constructor;
    }
    ~impl_base()
    {
        *ftype |= function_type::destructor;
    }

    impl_base(const impl_base&)
    {
        *ftype |= function_type::copy_constructor;
    }

    impl_base(impl_base&&)
    {
        *ftype |= function_type::move_constructor;

    }

    impl_base& operator=(const impl_base&)
    {
        *ftype |= function_type::copy_assignment;
    }

    impl_base& operator=(impl_base&&)
    {
        *ftype |= function_type::move_assignment;
    }
};

class pimpl_type {
public:

    pimpl_type();
    ~pimpl_type();
    pimpl_type(int v);

    pimpl_type(const pimpl_type&);
    pimpl_type(pimpl_type&&);

    pimpl_type& operator=(const pimpl_type&);
    pimpl_type& operator=(pimpl_type&&);

    function_type_ptr ftype();

    const void* impl_ptr()const;

private:
    class impl;
    ev::pimpl<impl> d;
};

class bimpl_type {
public:
    bimpl_type();
    ~bimpl_type();
    bimpl_type(int v);

    bimpl_type(const bimpl_type&);
    bimpl_type(bimpl_type &&);

    bimpl_type& operator=(const bimpl_type&);
    bimpl_type& operator=(bimpl_type&&);

    function_type_ptr ftype();


private:
    class impl;
    ev::bimpl<impl, 48, 8> d;
};

class trivially_destructible_bimpl {
public:
    trivially_destructible_bimpl();

private:
    class impl;
    ev::bimpl<impl, 8, 4,true> d;
};

