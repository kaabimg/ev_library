
#include <ev/core/utils.hpp>
#include <cmath>

#define EV_OCL_DEBUG_HEADER
#include <ev/experimental/ocl/ocl.hpp>

using namespace ev;

void transform_serial();
void transform_omp();
void transform_opencl_gpu();
void transform_opencl_cpu();

const size_t data_size = 50'000'000;

int main()
{
    transform_serial();
    transform_omp();
    transform_opencl_gpu();
    transform_opencl_cpu();
}

void transform_serial()
{
    std::vector<float> data;
    data.resize(data_size);

    for (int i = 0; i < data_size; ++i) {
        data[i] = i * 1.;
    }

    ev::time_t time;

    for (int i = 0; i < data_size; ++i) {
        data[i] = std::sin(data[i]);
    }
    for (int i = 0; i < data_size; ++i) {
        data[i] = std::sin(data[i]);
    }
    ev::debug() << __PRETTY_FUNCTION__ << "elapsed (ms)" << time.elapsed_ms();

    ev::debug() << data.size();
}

void transform_omp()
{
    std::vector<float> data;
    data.resize(data_size);

    for (int i = 0; i < data_size; ++i) {
        data[i] = i;
    }

    ev::time_t time;

#pragma omp parallel for
    for (int i = 0; i < data_size; ++i) {
        data[i] = std::sin(data[i]);
    }
#pragma omp parallel for
    for (int i = 0; i < data_size; ++i) {
        data[i] = std::sin(data[i]);
    }

    ev::debug() << __PRETTY_FUNCTION__ << "elapsed (ms)" << time.elapsed_ms();

    ev::debug() << data.size();
}

void transform_opencl_gpu()
{
    ev::time_t time;

    try
    {
        for (ocl::platform_t platform : ocl::get_platfomrs()) {
            std::vector<ocl::device_t> gpu_devices =
                platform.get_devices(ocl::device_type_e::gpu);

            if (gpu_devices.empty()) {
                continue;
            }

            std::vector<float> data;
            data.resize(data_size);

            for (int i = 0; i < data_size; ++i) {
                data[i] = i * 1.;
            }

            ev::time_t time;

            ocl::context_t context{gpu_devices};

            ocl::command_queue_t queue{context, context.devices().front()};

            ocl::buffer_t data_buffer = context.new_buffer<float>(
                data_size, ocl::memory_flags_e::read_write);

            ocl::program_t program = context.new_program_from_file(
                "/home/ghaith/data/development/workspace/examples/opencl/"
                "vector_add.cl");

            program.build();

            ocl::kernel_t transform = program.get_kenel("transform");

            queue << (data_buffer << data)
                  << transform(data_buffer)[ocl::nd_range_t{data_size}]
                  << transform(data_buffer)[ocl::nd_range_t{data_size}]
                  << (data_buffer >> data) << ocl::flush_t{};

            ev::debug() << __PRETTY_FUNCTION__ << "elapsed (ms)"
                        << time.elapsed_ms();
        }
    }
    catch (const std::exception& error)
    {
        ev::error(std::cerr) << error.what();
    }
}

void transform_opencl_cpu()
{
    try
    {
        for (ocl::platform_t platform : ocl::get_platfomrs()) {
            std::vector<ocl::device_t> gpu_devices =
                platform.get_devices(ocl::device_type_e::cpu);

            if (gpu_devices.empty()) {
                continue;
            }

            std::vector<float> data;
            data.resize(data_size);

            for (int i = 0; i < data_size; ++i) {
                data[i] = i * 1.;
            }

            ev::time_t time;

            ocl::context_t context{gpu_devices};

            ocl::command_queue_t queue{context, context.devices().front()};

            ocl::buffer_t data_buffer = context.new_buffer<float>(
                data_size, ocl::memory_flags_e::read_write);

            ocl::program_t program = context.new_program_from_file(
                "/home/ghaith/data/development/workspace/examples/opencl/"
                "vector_add.cl");

            program.build();

            ocl::kernel_t transform = program.get_kenel("transform");

            queue << (data_buffer << data)
                  << transform(data_buffer)[ocl::nd_range_t{data_size}]
                  << transform(data_buffer)[ocl::nd_range_t{data_size}]
                  << (data_buffer >> data) << ocl::flush_t{};

            ev::debug() << __PRETTY_FUNCTION__ << "elapsed (ms)"
                        << time.elapsed_ms();
        }
    }
    catch (const std::exception& error)
    {
        ev::error(std::cerr) << error.what();
    }
}
