#ifndef _RIVEEXTENSION_THORVG_PAINT_H_
#define _RIVEEXTENSION_THORVG_PAINT_H_

#include <thorvg.h>

// Rive
#include <rive/renderer.hpp>

// Extension
#include "tvg_renderer/common.hpp"

struct TvgPaint {
   public:
    uint8_t color[4];
    float thickness = 1.0f;
    tvg::Fill* gradient_fill = nullptr;
    tvg::StrokeJoin join = tvg::StrokeJoin::Bevel;
    tvg::StrokeCap cap = tvg::StrokeCap::Butt;
    rive::RenderPaintStyle style = rive::RenderPaintStyle::fill;
    rive::BlendMode blend_mode = rive::BlendMode::srcOver;
    bool is_gradient = false;
};

class TvgRenderPaint : public rive::RenderPaint {
   private:
    TvgPaint _paint;
    TvgGradientBuilder* _gradient_builder = nullptr;

   public:
    TvgPaint* paint() {
        return &_paint;
    }

    void style(rive::RenderPaintStyle style) override;
    void color(unsigned int value) override;
    void thickness(float value) override;
    void join(rive::StrokeJoin value) override;
    void cap(rive::StrokeCap value) override;
    void blendMode(rive::BlendMode value) override;
    void shader(rive::rcp<rive::RenderShader>) override;
    void invalidateStroke() override;

    void linear_gradient(float sx, float sy, float ex, float ey);
    void radial_gradient(float sx, float sy, float ex, float ey);
    void add_stop(unsigned int color, float stop);
    void complete_gradient();
};

#endif