#ifndef _RIVEEXTENSION_TYPES_
#define _RIVEEXTENSION_TYPES_

#include <memory>

template <typename T>
using Ptr = std::unique_ptr<T>;

#endif