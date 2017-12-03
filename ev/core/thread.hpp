#pragma once
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
#define BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK

#include <boost/thread/future.hpp>
#include <boost/thread/latch.hpp>

namespace ev {

template <typename T>
using future = boost::future<T>;

template <typename T>
using promise = boost::promise<T>;

template <typename T>
using packaged_task = boost::packaged_task<T>;

using latch = boost::latch;
using boost::launch;
}
