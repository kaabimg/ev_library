
#include <ev/vm/virtual_machine.h>
#include <ev/core/logging.h>
#include <iostream>
int print(int v){
    ev::debug() << v;
    return v;
}

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
            try  {
                vm.eval(statement);
            }
            catch(const std::exception & e){
                ev::error()<<e.what();
            }

            statement.clear();
        }

        std::cout << ">> ";

        std::getline(std::cin, input);
    }



    return 0;
}

