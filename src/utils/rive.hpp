#ifndef _RIVEEXTENSION_RIVE_UTILITIES_HPP_
#define _RIVEEXTENSION_RIVE_UTILITIES_HPP_

#include <godot_cpp/variant/transform2d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <rive/math/mat2d.hpp>
#include <rive/math/transform_components.hpp>

static godot::Transform2D convert_transform(const rive::Mat2D& matrix) {
    rive::TransformComponents components = matrix.decompose();
    godot::Vector2 scale
        = godot::Vector2(components.scaleX(), components.scaleY());
    godot::Vector2 position = godot::Vector2(components.x(), components.y());
    float skew = components.skew();
    float rotation = components.rotation();
    return godot::Transform2D(rotation, scale, skew, position);
}

#endif