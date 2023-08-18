#ifndef _RIVEEXTENSION_THORVG_CONVERSION_HPP_
#define _RIVEEXTENSION_THORVG_CONVERSION_HPP_

#include <thorvg.h>

// Rive
#include <rive/math/mat2d.hpp>
#include <rive/math/path_types.hpp>
#include <rive/math/vec2d.hpp>
#include <rive/shapes/paint/blend_mode.hpp>

static tvg::Point convert_point(rive::Vec2D point) {
    tvg::Point new_point;
    new_point.x = point.x;
    new_point.y = point.y;
    return new_point;
}

static tvg::PathCommand convert_verb(rive::PathVerb verb) {
    switch (verb) {
        case rive::PathVerb::close:
            return tvg::PathCommand::Close;
        case rive::PathVerb::cubic:
            return tvg::PathCommand::CubicTo;
        case rive::PathVerb::line:
            return tvg::PathCommand::LineTo;
        case rive::PathVerb::move:
            return tvg::PathCommand::MoveTo;
        case rive::PathVerb::quad:
        default:
            return tvg::PathCommand::CubicTo;
    }
}

static tvg::Point transform_coord(
    const tvg::Point pt, const rive::Mat2D& transform
) {
    tvg::Matrix m = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
    m.e11 = transform[0];
    m.e12 = transform[2];
    m.e13 = transform[4];
    m.e21 = transform[1];
    m.e22 = transform[3];
    m.e23 = transform[5];

    return { pt.x * m.e11 + pt.y * m.e12 + m.e13,
             pt.x * m.e21 + pt.y * m.e22 + m.e23 };
}

static tvg::BlendMethod convert_blend_mode(rive::BlendMode blend_mode) {
    switch (blend_mode) {
        case rive::BlendMode::colorBurn:
            return tvg::BlendMethod::ColorBurn;
        case rive::BlendMode::colorDodge:
            return tvg::BlendMethod::ColorDodge;
        case rive::BlendMode::darken:
            return tvg::BlendMethod::Darken;
        case rive::BlendMode::difference:
            return tvg::BlendMethod::Difference;
        case rive::BlendMode::exclusion:
            return tvg::BlendMethod::Exclusion;
        case rive::BlendMode::hardLight:
            return tvg::BlendMethod::HardLight;
        case rive::BlendMode::lighten:
            return tvg::BlendMethod::Lighten;
        case rive::BlendMode::multiply:
            return tvg::BlendMethod::Multiply;
        case rive::BlendMode::overlay:
            return tvg::BlendMethod::Overlay;
        case rive::BlendMode::screen:
            return tvg::BlendMethod::Screen;
        case rive::BlendMode::softLight:
            return tvg::BlendMethod::SoftLight;
        case rive::BlendMode::srcOver:
            return tvg::BlendMethod::SrcOver;

        // These blend modes are not currently supported in TVG
        case rive::BlendMode::hue:
        case rive::BlendMode::color:
        case rive::BlendMode::luminosity:
        case rive::BlendMode::saturation:
        default:
            return tvg::BlendMethod::Normal;
    }
}

#endif