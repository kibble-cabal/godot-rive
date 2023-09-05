#ifndef _RIVEEXTENSION_TYPES_
#define _RIVEEXTENSION_TYPES_

#include <functional>
#include <memory>

template <typename T>
using Ptr = std::unique_ptr<T>;

template <typename R, typename... Args>
using Fn = std::function<R(Args...)>;

template <typename... Args>
using Callback = Fn<void, Args...>;

#endif