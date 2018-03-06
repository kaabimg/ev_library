#include <pybind11/embed.h>
namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};

    py::exec(R"(
def add(a,b):
    return a+b

print(add("34", 454))
    )");
}
