#ifndef _RIVEEXTENSION_UTILS_MEMORY_HPP_
#define _RIVEEXTENSION_UTILS_MEMORY_HPP_

#include "utils/types.hpp"

template <typename InnerType>
static void unref(Ref<InnerType> value) {
    if (value != nullptr && value.is_valid()) value.unref();
};

template <typename InnerType>
static bool is_null(Ref<InnerType> value) {
    return value == nullptr || value.is_null();
};

template <typename InnerType>
void nullify(Ptr<InnerType> obj) {
    if (obj) {
        auto raw = obj.release();
        if (raw) obj.get_deleter()(raw);
    }
};

#endif