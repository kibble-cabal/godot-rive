#ifndef _RIVEEXTENSION_THORVG_UTILS_HPP_
#define _RIVEEXTENSION_THORVG_UTILS_HPP_

// Stdlib
#include <string>

// Thorvg
#include <thorvg.h>

static std::string as_string(tvg::Result result) {
    switch (result) {
        case tvg::Result::FailedAllocation:
            return "FailedAllocation";
        case tvg::Result::InsufficientCondition:
            return "InsufficientCondition";
        case tvg::Result::InvalidArguments:
            return "InvalidArguments";
        case tvg::Result::MemoryCorruption:
            return "MemoryCorruption";
        case tvg::Result::NonSupport:
            return "NonSupport";
        case tvg::Result::Success:
            return "Success";
        case tvg::Result::Unknown:
            return "Unknown";
    }
}

#define _TVGCATCH_SELECT(_1, _2, _3, m_macro, ...) m_macro

#define _TVGCATCH_3_ARGS(result, msg, return_val)                        \
    if (result != tvg::Result::Success) {                                \
        GDERR("[Thorvg] Error: ", as_string(result).c_str(), "\n", msg); \
        return return_val;                                               \
    }

#define _TVGCATCH_2_ARGS(result, msg)                                    \
    if (result != tvg::Result::Success) {                                \
        GDERR("[Thorvg] Error: ", as_string(result).c_str(), "\n", msg); \
        return;                                                          \
    }

#define _TVGCATCH_1_ARG(result)                               \
    if (result != tvg::Result::Success) {                     \
        GDERR("[Thorvg] Error: ", as_string(result).c_str()); \
        return;                                               \
    }

#define TVGCATCH(...)     \
    _TVGCATCH_SELECT(     \
        __VA_ARGS__,      \
        _TVGCATCH_3_ARGS, \
        _TVGCATCH_2_ARGS, \
        _TVGCATCH_1_ARG,  \
    )                     \
    (__VA_ARGS__)

#endif