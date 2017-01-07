#pragma once

// data

#define EV_DISABLE_COPY(type_name)                   \
private:                                             \
    type_name(const type_name&) = delete;            \
    type_name& operator=(const type_name&) = delete; \
    type_name(type_name&& another)         = delete; \
    type_name& operator=(type_name&& another) = delete;

#define EV_PRIVATE(type_name)                        \
private:                                             \
    struct data_t;                                   \
    data_t* d;                                       \
    type_name(const type_name&) = delete;            \
    type_name& operator=(const type_name&) = delete; \
                                                     \
public:                                              \
    type_name(type_name&& another) {                 \
        data_t* tmp = d;                             \
        d           = another.d;                     \
        another.d   = tmp;                           \
    }                                                \
    type_name& operator=(type_name&& another) {      \
        data_t* tmp = d;                             \
        d           = another.d;                     \
        another.d   = tmp;                           \
        return *this;                                \
    }

#define EV_DEFAULT_CONSTRUCTORS(cls) \
    cls(const cls&) = default;       \
    cls(cls&&)      = default;

#define EV_DEFAULT_OPERATORS(cls)         \
    cls& operator=(const cls&) = default; \
    cls& operator=(cls&&) = default;

// loops and variables
#define ev_forever while (true)
#define ev_delete_all(container) \
    for (auto e : container) delete e
#define ev_unused(v) (void)v

// strings, concat
#define __EV_CONCAT(a, b) a##b
#define EV_CONCAT(a, b) __EV_CONCAT(a, b)

#define __EV_STRINGIFY(a) #a
#define EV_STRINGIFY(a) __EV_STRINGIFY(a)

// anonymous variable
//#ifdef __COUNTER__
#define EV_ANONYMOUS_VARIABLE(name) EV_CONCAT(name, __COUNTER__)
//#else
//#define EV_ANONYMOUS_VARIABLE(name) EV_CONCAT(name,__LINE__)
//#endif

// arguments
#define EV_ARG_1(a, ...) a
#define EV_ARG_2(a, b, ...) b
#define EV_ARG_2_OR_1(...) EV_ARGS_2(__VA_ARGS__, __VA_ARGS__)

// args count : between 1 and 10
#define EV_ARG_COUNT(...) \
    __EV_ARG_COUNT(0, __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __EV_ARG_COUNT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, count, \
                       ...)                                                \
    count
