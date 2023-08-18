#include "factory.h"

#include "gradient.h"
#include "image.h"
#include "paint.h"
#include "path.h"
#include "renderer.h"
#include "utils/godot_macros.hpp"

using namespace rive;

rcp<RenderBuffer> TvgFactory::makeBufferU16(Span<const uint16_t> data) {
    return DataRenderBuffer::Make(data);
}

rcp<RenderBuffer> TvgFactory::makeBufferU32(Span<const uint32_t> data) {
    return DataRenderBuffer::Make(data);
}

rcp<RenderBuffer> TvgFactory::makeBufferF32(Span<const float> data) {
    return DataRenderBuffer::Make(data);
}

rcp<RenderShader> TvgFactory::makeLinearGradient(
    float sx,
    float sy,
    float ex,
    float ey,
    const ColorInt colors[],  // [count]
    const float stops[],      // [count]
    size_t count
) {
    auto builder
        = TvgLinearGradientBuilder(sx, sy, ex, ey, colors, stops, count);
    return rive::make_rcp<TvgRenderLinearGradient>(builder);
}

rcp<RenderShader> TvgFactory::makeRadialGradient(
    float cx,
    float cy,
    float radius,
    const ColorInt colors[],  // [count]
    const float stops[],      // [count]
    size_t count
) {
    auto builder
        = TvgRadialGradientBuilder(cx, cy, radius, colors, stops, count);
    return rive::make_rcp<TvgRenderRadialGradient>(builder);
}

Ptr<RenderPath> TvgFactory::makeRenderPath(
    RawPath& raw_path, rive::FillRule fill_rule
) {
    return rivestd::make_unique<TvgRenderPath>(
        raw_path.points(),
        raw_path.verbs(),
        fill_rule
    );
}

Ptr<RenderPath> TvgFactory::makeEmptyRenderPath() {
    return rivestd::make_unique<TvgRenderPath>();
}

Ptr<RenderPaint> TvgFactory::makeRenderPaint() {
    return rivestd::make_unique<TvgRenderPaint>();
}

Ptr<RenderImage> TvgFactory::decodeImage(Span<const uint8_t> encoded) {
    uint8_t* data_ptr = const_cast<uint8_t*>(encoded.data());
    // TODO decoding...
    return rivestd::make_unique<TvgRenderImage>(
        // TvgRenderImage(data_ptr, data.size_bytes())
    );
}