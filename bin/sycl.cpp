
#include <ev/core/logging.hpp>
#include <CL/sycl.hpp>
#include <array>

using namespace cl::sycl;

constexpr access::mode sycl_read  = cl::sycl::access::mode::read;
constexpr access::mode sycl_write = cl::sycl::access::mode::write;

template <typename T>
class simple_add_t;

template <typename T, size_t size>
void simple_vadd(const std::array<T, size>& input1,
                 const std::array<T, size>& input2,
                 std::array<T, size>& result)
{
    queue device_queue;
    ev::debug()
        << "Running on device : "
        << device_queue.get_device().get_info<info::device::name>();

    range<1> item_count{size};
    buffer<T, 1> buffer_a(input1.data(), item_count);
    buffer<T, 1> buffer_b(input2.data(), item_count);
    buffer<T, 1> buffer_c(result.data(), item_count);

    device_queue.submit([&](handler& handler) {
        auto accessor_a = buffer_a.template get_access<sycl_read>(handler);
        auto accessor_b = buffer_b.template get_access<sycl_read>(handler);
        auto accessor_c = buffer_c.template get_access<sycl_write>(handler);
        handler.parallel_for<class simple_add_t<T>>(item_count,
            [=](id<1> wiID) {
                accessor_c[wiID] = accessor_a[wiID] + accessor_b[wiID];
            }
        );
    });
}

int main()
{
    for (const platform& platf : platform::get_platforms()) {

        ev::debug() << "Platform"<< platf.get_info<info::platform::name>();

        for(const device & dev : platf.get_devices())
        {
            ev::debug() << "    Devive"<<dev.get_info<info::device::name>();
        }
    }

    return 0;

    constexpr size_t array_size = 4;
    std::array<cl::sycl::cl_int, array_size> array_a = {{1, 2, 3, 4}},
                                             array_b = {{1, 2, 3, 4}}, array_c;

    std::array<cl::sycl::cl_float, array_size> array_d = {{1.f, 2.f, 3.f, 4.f}},
                                               array_e = {{1.f, 2.f, 3.f, 4.f}},
                                               array_f;

    simple_vadd<int, array_size>(array_a, array_b, array_c);
    simple_vadd<float, array_size>(array_d, array_e, array_f);
    for (unsigned int i = 0; i < array_size; i++) {
        if (array_c[i] != array_a[i] + array_b[i]) {
            ev::error() << "Error at index" << i;
            return 1;
        }
        if (array_f[i] != array_d[i] + array_e[i]) {
            ev::error() << "Error at index" << i;
            return 1;
        }
    }
    return 0;
}
