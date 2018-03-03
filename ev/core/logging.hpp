#pragma once

#include <sstream>
#include <iostream>

namespace ev {

enum class log_category { info, warning, error, debug };

struct log_message {
    std::string message;
    log_category category;
};

namespace detail {
// clang-format off
static const char* g_debug_prefix    = "[debug]";
static const char* g_info_prefix     = "[info]";
static const char* g_warning_prefix  = "[warning]";
static const char* g_error_prefix    = "[error]";
// clang-format on

inline const char* category_to_prefix(log_category category)
{
    switch (category) {
        case log_category::info: return detail::g_info_prefix;
        case log_category::warning: return detail::g_warning_prefix;
        case log_category::error: return detail::g_error_prefix;
        case log_category::debug: return detail::g_debug_prefix;
        default: nullptr;
    }
}
}
template <typename T>
struct print_impl {
    static inline void print(std::ostream& out, const T& t)
    {
        out << t;
    }
};

template <typename T>
struct logger {
    logger(T&)
    {
    }
    void log(const log_message&)
    {
        /// Override this template class
    }
};

template <>
struct logger<std::ostream> {
    logger(std::ostream& os) : _os(os)
    {
    }
    void log(const log_message& msg)
    {
        _os << detail::category_to_prefix(msg.category) + msg.message + '\n';
    }

private:
    std::ostream& _os;
};

template <typename Output>
struct printer {
    printer(Output& output, log_category c) : _logger{output}, _c(c)
    {
    }
    ~printer()
    {
        _logger.log({_ss.str(), _c});
    }

    template <typename T>
    printer& operator<<(const T& d)
    {
        _ss << ' ';
        print_impl<T>::print(_ss, d);
        return *this;
    }

private:
    std::stringstream _ss;
    log_category _c;
    logger<Output> _logger;
};

inline auto debug(auto& output)
{
    return printer{output, log_category::debug};
}
inline auto debug()
{
    return printer{std::cout, log_category::debug};
}

inline auto info(auto& output)
{
    return printer{output, log_category::info};
}
inline auto info()
{
    return printer{std::cout, log_category::info};
}

inline auto warning(auto& output)
{
    return printer{output, log_category::warning};
}
inline auto warning()
{
    return printer{std::cout, log_category::warning};
}

inline auto error(auto& output)
{
    return printer{output, log_category::error};
}
inline auto error()
{
    return printer{std::cout, log_category::error};
}

}  // ev

#define EV_CUSTOM_PRINTER(type, out, val)                             \
    namespace ev {                                                    \
    template <>                                                       \
    struct print_impl<type> {                                         \
        static inline void print(std::ostream& out, const type& val); \
    };                                                                \
    }                                                                 \
    inline void ev::print_impl<type>::print(std::ostream& out, const type& val)

///////////////////////////////////////////////
///
EV_CUSTOM_PRINTER(bool, ostream, data)
{
    ostream << (data ? "true" : "false");
}

///////////////////////////////////////////////
///
EV_CUSTOM_PRINTER(std::string, ostream, data)
{
    ostream << '\'';
    ostream << data.c_str();
    ostream << "'";
}
