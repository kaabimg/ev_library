#include <ev/core/object.hpp>
#include <ev/core/logging.hpp>

int main(int argc, char* argv[])
{
    ev::object_t obj;

    ev::member_property_t<ev::object_t, std::string, &ev::object_t::name,
                          &::ev::object_t::set_name>
        property;



    property.write(&obj,std::string("foo"));

    ev::debug() << ev::property_value_t(property.read(&obj)).as<std::string>();

    return 0;
}
