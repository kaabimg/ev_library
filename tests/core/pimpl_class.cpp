#include "pimpl_class.hpp"

struct pimpl_type::impl : impl_base {
    using impl_base::impl_base;
    using impl_base::operator=;

    impl(const impl& rhs) : impl_base(rhs)
    {
    }

    impl& operator=(const impl& rhs)
    {
        impl_base::operator=(rhs);
    }
};

pimpl_type::pimpl_type(int v) : _impl(std::in_place, v)
{
}

pimpl_type::pimpl_type()
{
}

pimpl_type::~pimpl_type()
{
}

pimpl_type::pimpl_type(const pimpl_type& rhs) : _impl(rhs._impl)
{
}

pimpl_type::pimpl_type(pimpl_type&& rhs) : _impl(std::move(rhs._impl))
{
}

pimpl_type& pimpl_type::operator=(const pimpl_type& rhs)
{
    _impl = rhs._impl;
    return *this;
}

pimpl_type& pimpl_type::operator=(pimpl_type&& rhs)
{
    _impl = std::move(rhs._impl);
    return *this;
}

function_type_ptr pimpl_type::ftype()
{
    return _impl->ftype;
}

const void* pimpl_type::impl_ptr() const
{
    return _impl.operator->();
}

struct bimpl_type::impl : impl_base {
    using impl_base::impl_base;
    using impl_base::operator=;

    impl(const impl& rhs) : impl_base(rhs)
    {
    }
    impl(impl&& rhs) : impl_base(std::move(rhs))
    {
    }

    impl& operator=(const impl& rhs)
    {
        impl_base::operator=(rhs);
        return *this;
    }

    impl& operator=(impl&& rhs)
    {
        impl_base::operator=(std::move(rhs));
        return *this;
    }
};

bimpl_type::bimpl_type()
{
}

bimpl_type::~bimpl_type()
{
}

bimpl_type::bimpl_type(int v) : _impl{std::in_place, v}
{
}

bimpl_type::bimpl_type(const bimpl_type& rhs) : _impl(rhs._impl)
{
}

bimpl_type::bimpl_type(bimpl_type&& rhs) : _impl(std::move(rhs._impl))
{
}

bimpl_type& bimpl_type::operator=(const bimpl_type& rhs)
{
    _impl = rhs._impl;
    return *this;
}

bimpl_type& bimpl_type::operator=(bimpl_type&& rhs)
{
    _impl = std::move(rhs._impl);
    return *this;
}

function_type_ptr bimpl_type::ftype()
{
    return _impl->ftype;
}
