#include "application.hpp"
#include "object.hpp"
#include "session.hpp"

#include <qapplication.h>


using namespace ev::atk;
struct application_t::impl_t {

    impl_t(int a,char *argv[]):
        argc {a},
        qapp {argc,argv}
    {}

    int argc;
    qapplication qapp;


    session_t session;

    static application_t* instance;
};

application_t* application_t::impl_t::instance = nullptr;

application_t::application_t(int argc, char *argv[]):
    d (new impl_t(argc,argv))
{
    impl_t::instance = this;
}

application_t::~application_t()
{
    delete d;
}

int application_t::exec()
{
    return d->qapp.exec();
}

application_t *application_t::instance()
{
    return impl_t::instance;
}

session_t *application_t::session() const
{
    return &d->session;
}
