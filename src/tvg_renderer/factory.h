#ifndef _RIVEEXTENSION_THORVG_FACTORY_H_
#define _RIVEEXTENSION_THORVG_FACTORY_H_

#include <rive/factory.hpp>
#include <utils/factory_utils.hpp>

#include "utils/types.hpp"

using namespace rive;

class TvgFactory : public Factory {
    rcp<RenderBuffer> makeBufferU16(Span<const uint16_t>) override;
    rcp<RenderBuffer> makeBufferU32(Span<const uint32_t>) override;
    rcp<RenderBuffer> makeBufferF32(Span<const float>) override;
    rcp<RenderShader> makeLinearGradient(
        float sx,
        float sy,
        float ex,
        float ey,
        const ColorInt colors[],  // [count]
        const float stops[],      // [count]
        size_t count
    ) override;
    rcp<RenderShader> makeRadialGradient(
        float cx,
        float cy,
        float radius,
        const ColorInt colors[],  // [count]
        const float stops[],      // [count]
        size_t count
    ) override;
    Ptr<RenderPath> makeRenderPath(RawPath& raw_path, rive::FillRule fill_rule)
        override;
    Ptr<RenderPath> makeEmptyRenderPath() override;
    Ptr<RenderPaint> makeRenderPaint() override;
    Ptr<RenderImage> decodeImage(Span<const uint8_t>) override;
};

#endif
