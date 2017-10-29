#pragma once
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
#include <boost/thread/future.hpp>

namespace ev {

template <typename T>
using future = boost::future<T>;

template <typename T>
using promise = boost::promise<T>;

template <typename T>
using packaged_task = boost::packaged_task<T>;
}
