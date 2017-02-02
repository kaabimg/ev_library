#pragma once

#include "../qtypes_fwd.hpp"
#include <ev/core/preprocessor.hpp>

#include <qobject.h>


#define atk_app ::ev::atk::application_t::instance()

namespace ev {
namespace atk {

class session_t;


enum class application_setting_e {

};

class application_t : public qobject {
    Q_OBJECT
public:
    application_t(int argc, char* argv[]);
    ~application_t();
    int exec();

    static application_t* instance();

    session_t* session() const;

private:
    EV_IMPL(application_t)
};
}
}
