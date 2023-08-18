#ifndef _RIVEEXTENSION_THORVG_GRADIENT_H_
#define _RIVEEXTENSION_THORVG_GRADIENT_H_

#include <thorvg.h>

#include <rive/renderer.hpp>
#include <vector>

#include "tvg_renderer/common.hpp"

struct GradientStop {
    unsigned int color;
    float stop;

    GradientStop() {}

    GradientStop(unsigned int color_value, float stop_value);
};

class TvgGradientBuilder {
   public:
    std::vector<GradientStop> stops;
    float sx, sy, ex, ey;

    TvgGradientBuilder() {}

    TvgGradientBuilder(float sx, float sy, float ex, float ey);
    TvgGradientBuilder(
        float sx_val,
        float sy_val,
        float ex_val,
        float ey_val,
        const uint32_t* colors,
        const float* stops_val,
        size_t stop_count
    );

    virtual ~TvgGradientBuilder() {}

    virtual void make(TvgPaint* paint) = 0;
};

class TvgRadialGradientBuilder : public TvgGradientBuilder {
   public:
    float radius;

    TvgRadialGradientBuilder() {}

    TvgRadialGradientBuilder(float sx, float sy, float ex, float ey);
    TvgRadialGradientBuilder(float cx, float cy, float r);
    TvgRadialGradientBuilder(
        float sx_val,
        float sy_val,
        float ex_val,
        float ey_val,
        const uint32_t* colors,
        const float* stops_val,
        size_t stop_count
    );
    TvgRadialGradientBuilder(
        float cx,
        float cy,
        float r,
        const uint32_t* colors,
        const float* stops_val,
        size_t stop_count
    );

    void make(TvgPaint* paint) override;
};

class TvgLinearGradientBuilder : public TvgGradientBuilder {
   public:
    TvgLinearGradientBuilder() {}

    TvgLinearGradientBuilder(float sx, float sy, float ex, float ey);
    TvgLinearGradientBuilder(
        float sx_val,
        float sy_val,
        float ex_val,
        float ey_val,
        const uint32_t* colors,
        const float* stops_val,
        size_t stop_count
    );

    void make(TvgPaint* paint) override;
};

class TvgRenderLinearGradient : public rive::RenderShader {
   protected:
    TvgLinearGradientBuilder _gradient;

   public:
    TvgRenderLinearGradient(TvgLinearGradientBuilder builder);
    TvgLinearGradientBuilder* gradient();
};

class TvgRenderRadialGradient : public rive::RenderShader {
   protected:
    TvgRadialGradientBuilder _gradient;

   public:
    TvgRenderRadialGradient(TvgRadialGradientBuilder builder);
    TvgRadialGradientBuilder* gradient();
};

#endif