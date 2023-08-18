#ifndef _RIVEEXTENSION_THORVG_RENDERER_H_
#define _RIVEEXTENSION_THORVG_RENDERER_H_

#include <thorvg.h>

// stdlib
#include <stack>
#include <vector>

// rive
#include <rive/renderer.hpp>

#include "tvg_renderer/common.hpp"

class TvgRenderer : public rive::Renderer {
   private:
    tvg::Canvas* _canvas;
    tvg::Shape* _clip_path = nullptr;
    tvg::Shape* _bg_clip_path = nullptr;
    rive::Mat2D _transform;
    std::stack<rive::Mat2D> _saved_transforms;

   public:
    TvgRenderer(tvg::Canvas* canvas);

    void save() override;
    void restore() override;
    void transform(const rive::Mat2D& transform) override;
    void drawPath(rive::RenderPath* path, rive::RenderPaint* paint) override;
    void clipPath(rive::RenderPath* path) override;
    void drawImage(const rive::RenderImage*, rive::BlendMode, float opacity)
        override;
    void drawImageMesh(
        const rive::RenderImage*,
        rive::rcp<rive::RenderBuffer> vertices_f32,
        rive::rcp<rive::RenderBuffer> uvCoords_f32,
        rive::rcp<rive::RenderBuffer> indices_u16,
        rive::BlendMode,
        float opacity
    ) override;
};

#endif