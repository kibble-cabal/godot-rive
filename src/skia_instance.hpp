#ifndef _RIVEEXTENSION_SKIA_INSTANCE_HPP_
#define _RIVEEXTENSION_SKIA_INSTANCE_HPP_

// godot-cpp
#include <godot_cpp/variant/builtin_types.hpp>

// skia
#include <skia/dependencies/skia/include/core/SkBitmap.h>
#include <skia/dependencies/skia/include/core/SkCanvas.h>
#include <skia/dependencies/skia/include/core/SkSurface.h>

#include <skia/renderer/include/skia_factory.hpp>
#include <skia/renderer/include/skia_renderer.hpp>

// extension
#include "utils/types.hpp"
#include "viewer_props.hpp"

using namespace godot;
using namespace rive;

struct SkiaInstance {
    ViewerProps *props;
    sk_sp<SkSurface> surface;
    Ptr<SkiaRenderer> renderer;
    Ptr<SkiaFactory> factory = rivestd::make_unique<SkiaFactory>();

    void set_props(ViewerProps *props_value) {
        props = props_value;
        if (props) {
            props->on_size_changed([this](float w, float h) { on_size_changed(w, h); });
        }
    }

    SkImageInfo image_info() const {
        return SkImageInfo::Make(
            props ? props->width() : 1,
            props ? props->height() : 1,
            SkColorType::kRGBA_8888_SkColorType,
            SkAlphaType::kUnpremul_SkAlphaType
        );
    }

    PackedByteArray bytes() const {
        SkPixmap pixmap;
        PackedByteArray bytes;
        if (!surface) return bytes;
        if (!surface->peekPixels(&pixmap)) return bytes;
        SkImageInfo info = surface->imageInfo();
        size_t bytes_per_pixel = info.bytesPerPixel(), row_bytes = pixmap.rowBytes();
        bytes.resize(row_bytes * info.height());
        for (int y = 0; y < info.height(); y++) {
            for (int x = 0; x < info.width(); x++) {
                int offset = y * row_bytes + x * bytes_per_pixel;
                auto addr = pixmap.addr32(x, y);
                bytes.encode_u32(offset, *addr);
            }
        }
        return bytes;
    }

    void clear() {
        if (surface && renderer) surface->getCanvas()->clear(SkColors::kTransparent);
    }

   private:
    void on_size_changed(float w, float h) {
        surface = SkSurface::MakeRaster(image_info());
        renderer = rivestd::make_unique<SkiaRenderer>(surface->getCanvas());
    }
};

#endif