
#include <ev/core/logging.hpp>
#include <ev/ocl/ocl.hpp>
using namespace ev;
int main()
{


    try {


    for (ocl::platform_t platform : ocl::platform_t::get_all())
    {
        ev::debug() << "Platform" << platform.name();
        ev::debug() << "    profile   " << platform.profile();
        ev::debug() << "    vendor    " << platform.vendor();
        ev::debug() << "    version   " << platform.version();
        ev::debug() << "    extensions" << platform.extensions();


        const char * idt = "****    ";
        for(ocl::device_t device : platform.get_devices(ocl::device_type_e::all))
        {
            ev::debug() << idt << "Device" << device.name();
            ev::debug() << idt << "    Available" << device.is_available();
            ev::debug() << idt << "    Vendor   " << device.vendor();
            ev::debug() << idt << "    Version  " << device.version();
            ev::debug() << idt << "    Driver   " << device.driver_version();
            ev::debug() << idt << "    Profile  " << device.profile();
            ev::debug() << idt << "    Type     " << device.type();
            ev::debug() << idt << "    C. units " << device.max_compute_units();
            ev::debug() << idt << "    Max Freq " << device.max_clock_frequency();


        }
    }
    }
    catch(const ocl::cl_error_t & error)
    {
        ev::error() << error.what();
    }

}
