#ifndef _RIVEEXTENSION_TYPES_
#define _RIVEEXTENSION_TYPES_

#include <memory>

template <typename T>
using Ptr = std::unique_ptr<T>;

struct ARGB {
    uint32_t color;

    ARGB(uint32_t value) {
        color = value;
    }

    ARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
        color = (a << 24) + (r << 16) + (g << 8) + b;
    }

    uint8_t a() {
        return (color >> 24) & 255;
    }

    uint8_t r() {
        return (color >> 16) & 255;
    }

    uint8_t g() {
        return (color >> 8) & 255;
    }

    uint8_t b() {
        return color & 255;
    }

    uint32_t rgba() {
        return (r() << 24) | (g() << 16) | (b() << 8) | a();
    }
};

struct RGBA {
    uint32_t color;

    RGBA(uint32_t value) {
        color = value;
    }

    RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        color = (r << 24) | (g << 16) | (b << 8) | a;
    }

    uint8_t r() {
        return (color >> 24) & 255;
    }

    uint8_t g() {
        return (color >> 16) & 255;
    }

    uint8_t b() {
        return (color >> 8) & 255;
    }

    uint8_t a() {
        return color & 255;
    }

    uint32_t argb() {
        return (a() << 24) | (r() << 16) | (g() << 8) | b();
    }
};

#endif