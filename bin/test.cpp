#include <ev/core/execution_context.hpp>
#include <ev/core/executor.hpp>

#define MAX 100
int main()
{
    ev::execution_context context;

    auto producer = [](ev::execution_context context) {
        for (int i = 0; i < MAX; ++i) {
            ev::debug() << "writing" << i;
            ev::debug(context) << i;
        }
    };

    auto consumer = [](ev::execution_context context) {
        int size = 0;
        int read = 0;
        while (size < MAX) {
            size = context.subitem_count();

            if (read != size) {
                for (int j = read; j < size; ++j)
                    ev::debug() << "reading in" << std::this_thread::get_id() << ':'
                                << context.subitem_at(j).message().message;
                read = size;
            }
        }
    };

    ev::executor executor;

    auto p = executor.async(producer, context);
    auto c = executor.async(consumer, context);
    auto c2 = executor.async(consumer, context);

    p.wait();
    c.wait();
    c2.wait();
}
