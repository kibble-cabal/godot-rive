#include "gradient.h"

#include "paint.h"

std::vector<GradientStop> update_stops(
    const uint32_t* colors, const float* values, size_t count
) {
    std::vector<GradientStop> stops;
    stops.resize(count);
    for (int i = 0; i < count; i++) {
        stops[i] = GradientStop((unsigned int)colors[i], (float)values[i]);
    }
    return stops;
}

GradientStop::GradientStop(unsigned int color_value, float stop_value) {
    color = color_value;
    stop = stop_value;
}

TvgGradientBuilder::TvgGradientBuilder(
    float sx_value, float sy_value, float ex_value, float ey_value
)
    : sx(sx_value), sy(sy_value), ex(ex_value), ey(ey_value) {}

TvgGradientBuilder::TvgGradientBuilder(
    float sx_value,
    float sy_value,
    float ex_value,
    float ey_value,
    const uint32_t* colors,
    const float* stop_values,
    size_t stop_count
)
    : sx(sx_value), sy(sy_value), ex(ex_value), ey(ey_value) {
    stops = update_stops(colors, stop_values, stop_count);
}

TvgLinearGradientBuilder::TvgLinearGradientBuilder(
    float sx, float sy, float ex, float ey
)
    : TvgGradientBuilder(sx, sy, ex, ey) {}

TvgLinearGradientBuilder::TvgLinearGradientBuilder(
    float sx_value,
    float sy_value,
    float ex_value,
    float ey_value,
    const uint32_t* colors,
    const float* stop_values,
    size_t stop_count
) {
    sx = sx_value;
    sy = sy_value;
    ex = ex_value;
    ey = ey_value;
    stops = update_stops(colors, stop_values, stop_count);
}

TvgRadialGradientBuilder::TvgRadialGradientBuilder(
    float sx_value, float sy_value, float ex_value, float ey_value
) {
    sx = sx_value;
    sy = sy_value;
    ex = ex_value;
    ey = ey_value;
    radius = rive::Vec2D::distance(rive::Vec2D(sx, sy), rive::Vec2D(ex, ey));
}

TvgRadialGradientBuilder::TvgRadialGradientBuilder(
    float cx, float cy, float r
) {
    sx = cx, sy = cy, ex = cx + r, ey = cy + r, radius = r;
}

TvgRadialGradientBuilder::TvgRadialGradientBuilder(
    float sx_value,
    float sy_value,
    float ex_value,
    float ey_value,
    const uint32_t* colors,
    const float* stop_values,
    size_t stop_count
) {
    sx = sx_value;
    sy = sy_value;
    ex = ex_value;
    ey = ey_value;
    radius = rive::Vec2D::distance(rive::Vec2D(sx, sy), rive::Vec2D(ex, ey));
    stops = update_stops(colors, stop_values, stop_count);
}

TvgRadialGradientBuilder::TvgRadialGradientBuilder(
    float cx,
    float cy,
    float r,
    const uint32_t* colors,
    const float* stop_values,
    size_t stop_count
) {
    sx = cx, sy = cy, ex = cx + r, ey = cy + r, radius = r;
    stops = update_stops(colors, stop_values, stop_count);
}

void TvgRadialGradientBuilder::make(TvgPaint* paint) {
    paint->is_gradient = true;
    int num_stops = stops.size();

    paint->gradient_fill = tvg::RadialGradient::gen().release();
    static_cast<tvg::RadialGradient*>(paint->gradient_fill)
        ->radial(sx, sy, radius);

    tvg::Fill::ColorStop color_stops[num_stops];
    for (int i = 0; i < num_stops; i++) {
        ARGB value(stops[i].color);
        color_stops[i]
            = { stops[i].stop, value.r(), value.g(), value.b(), value.a() };
    }

    static_cast<tvg::RadialGradient*>(paint->gradient_fill)
        ->colorStops(color_stops, num_stops);
}

void TvgLinearGradientBuilder::make(TvgPaint* paint) {
    paint->is_gradient = true;
    int num_stops = stops.size();

    paint->gradient_fill = tvg::LinearGradient::gen().release();
    static_cast<tvg::LinearGradient*>(paint->gradient_fill)
        ->linear(sx, sy, ex, ey);

    tvg::Fill::ColorStop color_stops[num_stops];
    for (int i = 0; i < num_stops; i++) {
        ARGB value(stops[i].color);
        color_stops[i]
            = { stops[i].stop, value.r(), value.g(), value.b(), value.a() };
    }

    static_cast<tvg::LinearGradient*>(paint->gradient_fill)
        ->colorStops(color_stops, num_stops);
}

TvgRenderLinearGradient::TvgRenderLinearGradient(
    TvgLinearGradientBuilder gradient_value
) {
    _gradient = gradient_value;
}

TvgLinearGradientBuilder* TvgRenderLinearGradient::gradient() {
    return &_gradient;
}

TvgRenderRadialGradient::TvgRenderRadialGradient(
    TvgRadialGradientBuilder gradient_value
) {
    _gradient = gradient_value;
}

TvgRadialGradientBuilder* TvgRenderRadialGradient::gradient() {
    return &_gradient;
}
