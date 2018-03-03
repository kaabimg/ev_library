#pragma once

#include <egf/core/ChangeNotifier.hpp>
#include <ev/core/algorithm.hpp>

#include <functional>
#include <memory>

namespace evt {

template <typename T>
class PropertyExpression : public ChangeNotifier {
public:
    PropertyExpression(std::nullptr_t)
    {
    }

    template <typename...>
    PropertyExpression(ChangeNotifier&... dependencies)
    {
        d = std::make_shared<Data>();
        d->dependencies.reserve(sizeof(dependencies));

        ev::for_each(std::tuple<T&...>{dependencies},
                     [this](auto& dependency) {
                         d->dependencies.push_back(&dependency);
                         connect(&dependency, &ChangeNotifier::changed, this,
                                 &PropertyExpression::changed);
                         connect(&dependency, &ChangeNotifier::destroyed, this,
                                 &PropertyExpression::disable);
                     });
    }

    template <typename E>
    void setExpression(E&& exp)
    {
        d->expression = std::forward<E>(exp);
    }

    T operator()() const
    {
        return d->expression();
    }

private:
    void disable()
    {
        for (auto& d : d->dependencies)
            disconnect(d, &ChangeNotifier::changed, this, &PropertyExpression::changed);
    }

private:
    struct Data {
        std::function<T()> expression;
        std::vector<ChangeNotifier*> dependencies;
    };
    std::shared_ptr<Data> d;
};

template <typename T>
class Property : public ChangeNotifier {
public:
    Property()
    {
    }

    T operator()() const
    {
        return _value;
    }

    void operator=(PropertyExpression binding)
    {
        if (_binding)
            disconnect(&binding, &ChangeNotifier::changed, this, &Property::updateFromBinding);

        _binding = std::move(binding);
    }

private:
    void setValue(const T& val)
    {
        _value = val;
        Q_EMI changed();
    }

    void updateFromBinding()
    {
        setValue(_binding());
    }

private:
    T _value;
    PropertyExpression<T> _binding{nullptr};
};
}
