#include <iostream>

#include "virtual_machine.h"


#include <ev/core/logging.h>


#include <list>

#include <functional>
#include <typeinfo>


int main()
{
    ev::vm::virtual_machine_t vm;
    std::string statement,input;

    std::cout << ">> ";
    std::getline(std::cin, input);

    while (input.size()) {

        if(*input.rbegin() == '\\'){
            *input.rbegin() = ' ';
            statement.append(input);
        }
        else {
            statement = input;
            vm.eval(statement);
            statement.clear();
        }

        std::cout << ">> ";

        std::getline(std::cin, input);
    }



    return 0;
}

