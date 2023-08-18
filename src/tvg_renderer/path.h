#ifndef _RIVEEXTENSION_THORVG_PATH_H_
#define _RIVEEXTENSION_THORVG_PATH_H_

// thorvg
#include <thorvg.h>

// rive
#include <rive/renderer.hpp>

#include "tvg_renderer/common.hpp"

struct TvgRenderPath : public rive::RenderPath {
    Ptr<tvg::Shape> tvgShape;

    TvgRenderPath();
    TvgRenderPath(
        rive::Span<rive::Vec2D> points,
        rive::Span<rive::PathVerb> verbs,
        rive::FillRule fill_rule
    );

    void rewind() override;
    void addRenderPath(rive::RenderPath* path, const rive::Mat2D& transform)
        override;
    void fillRule(rive::FillRule value) override;
    void moveTo(float x, float y) override;
    void lineTo(float x, float y) override;
    void cubicTo(float ox, float oy, float ix, float iy, float x, float y)
        override;
    void close() override;
};

#endif