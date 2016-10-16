#include <iostream>

#include <core/logging.h>
#include <core/shared_data.h>

#include <thread>

#include <vector>


int main()
{

    ev::synchronized_data_t<std::vector<int>> data;

    auto task = [&]()mutable {

        for (int i = 0; i < 100; ++i) {
            synchronized (data){
                data.push_back(i);
            }
        }
    };

    std::thread t1 {task},t2 {task};

    t1.join();
    t2.join();

    ev::debug() << "size"<<data->size();

    return 0;
}

