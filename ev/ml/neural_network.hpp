#pragma once
#include <cmath>
#include <vector>
namespace ev {
namespace ml {


struct input_neuron {
private:
    double _value = 0.0;
};

struct neuron {
private:
    std::vector<double> _weights;
    double _value = 0.0;
};

struct layer {
private:
    std::vector<neuron> _neurons;
};

struct neural_network {
private:
    std::vector<layer> _layers;
};
}
}
