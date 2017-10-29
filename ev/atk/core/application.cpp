#include "application.hpp"
#include "object.hpp"
#include "session.hpp"
#include "../interfaces/system_interface.hpp"

#include <qapplication.h>

using namespace ev::atk;
struct application_t::impl {
    impl(int a, char* argv[]) : argc{a}, qapp{argc, argv}
    {
    }

    int argc;
    qapplication qapp;
    session_t session;
    system_interface_t* system_interface = nullptr;

    static application_t* instance;
};

application_t* application_t::impl::instance = nullptr;

application_t::application_t(int argc, char* argv[]) : d(new impl(argc, argv))
{
    impl::instance = this;
}

application_t::~application_t()
{
    delete d;
}

int application_t::exec()
{
    return d->qapp.exec();
}

application_t* application_t::instance()
{
    return impl::instance;
}

session_t* application_t::session() const
{
    return &d->session;
}

system_interface_t* application_t::system_interface() const
{
    return d->system_interface;
}

void application_t::set_system_interface(system_interface_t * interface)
{
    d->system_interface = interface;
}
