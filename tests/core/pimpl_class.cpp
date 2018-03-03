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

pimpl_type::pimpl_type(int v) : d(std::in_place, v)
{
}

pimpl_type::pimpl_type()
{
}

pimpl_type::~pimpl_type()
{
}

pimpl_type::pimpl_type(const pimpl_type& rhs) : d(rhs.d)
{
}

pimpl_type::pimpl_type(pimpl_type&& rhs) : d(std::move(rhs.d))
{
}

pimpl_type& pimpl_type::operator=(const pimpl_type& rhs)
{
    d = rhs.d;
    return *this;
}

pimpl_type& pimpl_type::operator=(pimpl_type&& rhs)
{
    d = std::move(rhs.d);
    return *this;
}

function_type_ptr pimpl_type::ftype()
{
    return d->ftype;
}

const void* pimpl_type::impl_ptr() const
{
    return d.operator->();
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

bimpl_type::bimpl_type(int v) : d{std::in_place, v}
{
}

bimpl_type::bimpl_type(const bimpl_type& rhs) : d(rhs.d)
{
}

bimpl_type::bimpl_type(bimpl_type&& rhs) : d(std::move(rhs.d))
{
}

bimpl_type& bimpl_type::operator=(const bimpl_type& rhs)
{
    d = rhs.d;
    return *this;
}

bimpl_type& bimpl_type::operator=(bimpl_type&& rhs)
{
    d = std::move(rhs.d);
    return *this;
}

function_type_ptr bimpl_type::ftype()
{
    return d->ftype;
}


struct trivially_destructible_bimpl::impl {
    int32_t a,b;
};

trivially_destructible_bimpl::trivially_destructible_bimpl()
{

}
