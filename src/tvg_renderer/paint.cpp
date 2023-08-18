#include "paint.h"

#include <exception>

#include "gradient.h"

void TvgRenderPaint::style(rive::RenderPaintStyle style) {
    _paint.style = style;
}

void TvgRenderPaint::color(unsigned int value) {
    RGBA color(value);
    _paint.color[0] = color.r();
    _paint.color[1] = color.g();
    _paint.color[2] = color.b();
    _paint.color[3] = color.a();
}

void TvgRenderPaint::thickness(float value) {
    _paint.thickness = value;
}

void TvgRenderPaint::join(rive::StrokeJoin value) {
    switch (value) {
        case rive::StrokeJoin::round:
            _paint.join = tvg::StrokeJoin::Round;
            break;
        case rive::StrokeJoin::bevel:
            _paint.join = tvg::StrokeJoin::Bevel;
            break;
        case rive::StrokeJoin::miter:
            _paint.join = tvg::StrokeJoin::Miter;
            break;
    }
}

void TvgRenderPaint::cap(rive::StrokeCap value) {
    switch (value) {
        case rive::StrokeCap::butt:
            _paint.cap = tvg::StrokeCap::Butt;
            break;
        case rive::StrokeCap::round:
            _paint.cap = tvg::StrokeCap::Round;
            break;
        case rive::StrokeCap::square:
            _paint.cap = tvg::StrokeCap::Square;
            break;
    }
}

void TvgRenderPaint::linear_gradient(float sx, float sy, float ex, float ey) {
    _gradient_builder = new TvgLinearGradientBuilder(sx, sy, ex, ey);
}

void TvgRenderPaint::radial_gradient(float sx, float sy, float ex, float ey) {
    _gradient_builder = new TvgRadialGradientBuilder(sx, sy, ex, ey);
}

void TvgRenderPaint::add_stop(unsigned int color, float stop) {
    _gradient_builder->stops.emplace_back(GradientStop(color, stop));
}

void TvgRenderPaint::complete_gradient() {
    _gradient_builder->make(&_paint);
    delete _gradient_builder;
}

void TvgRenderPaint::blendMode(rive::BlendMode value) {
    _paint.blend_mode = value;
}

void TvgRenderPaint::shader(rive::rcp<rive::RenderShader> shader) {
    TvgRenderLinearGradient *linear_gradient = nullptr;
    TvgRenderRadialGradient *radial_gradient = nullptr;

    try {
        rive::RenderShader *shader_ptr = shader.get();
        linear_gradient = dynamic_cast<TvgRenderLinearGradient *>(shader_ptr);
        radial_gradient = dynamic_cast<TvgRenderRadialGradient *>(shader_ptr);
    } catch (std::bad_cast &err) {
        GDERR("Bad cast! ", err.what());
    }

    if (linear_gradient != nullptr) {
        linear_gradient->gradient()->make(&_paint);
    } else if (radial_gradient != nullptr) {
        radial_gradient->gradient()->make(&_paint);
    } else {
        GDERR(
            "Encountered unknown RenderShader type (in "
            "TvgRenderPaint::shader)"
        );
    }
}

void TvgRenderPaint::invalidateStroke() {
    // GDERR("TvgRenderPaint::invalidateStroke not yet implemented.");
}