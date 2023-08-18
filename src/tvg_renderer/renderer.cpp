#include "renderer.h"

// rive
#include <rive/math/vec2d.hpp>
#include <rive/shapes/paint/color.hpp>

#include "image.h"
#include "paint.h"
#include "path.h"

TvgRenderer::TvgRenderer(tvg::Canvas* canvas) {
    _canvas = canvas;
}

void TvgRenderer::save() {
    _saved_transforms.push(_transform);
}

void TvgRenderer::restore() {
    // Check shouldn't be needed, but safest to check
    if (_saved_transforms.size() > 0) {
        _transform = _saved_transforms.top();
        _saved_transforms.pop();
    }
}

void TvgRenderer::transform(const rive::Mat2D& transform) {
    _transform = _transform * transform;
}

void TvgRenderer::drawPath(rive::RenderPath* path, rive::RenderPaint* paint) {
    auto tvg_shape = static_cast<TvgRenderPath*>(path)->tvgShape.get();
    auto tvg_paint = static_cast<TvgRenderPaint*>(paint)->paint();

    /* OPTIMIZE ME: Stroke / Fill Paints required to draw separately.
       thorvg doesn't need to handle both, we can avoid one of them rendering...
     */

    tvg_shape->blend(convert_blend_mode(tvg_paint->blend_mode));

    if (tvg_paint->style == rive::RenderPaintStyle::fill) {
        if (!tvg_paint->is_gradient)
            tvg_shape->fill(
                tvg_paint->color[0],
                tvg_paint->color[1],
                tvg_paint->color[2],
                tvg_paint->color[3]
            );
        else {
            tvg_shape->fill(std::unique_ptr<tvg::Fill>(
                tvg_paint->gradient_fill->duplicate()
            ));
        }
    } else if (tvg_paint->style == rive::RenderPaintStyle::stroke) {
        tvg_shape->stroke(tvg_paint->cap);
        tvg_shape->stroke(tvg_paint->join);
        tvg_shape->stroke(tvg_paint->thickness);

        if (!tvg_paint->is_gradient)
            tvg_shape->stroke(
                tvg_paint->color[0],
                tvg_paint->color[1],
                tvg_paint->color[2],
                tvg_paint->color[3]
            );
        else {
            tvg_shape->stroke(std::unique_ptr<tvg::Fill>(
                tvg_paint->gradient_fill->duplicate()
            ));
        }
    }

    if (_clip_path) {
        _clip_path->fill(255, 255, 255, 255);
        tvg_shape->composite(
            std::unique_ptr<tvg::Shape>(
                static_cast<tvg::Shape*>(_clip_path->duplicate())
            ),
            tvg::CompositeMethod::ClipPath
        );
        _clip_path = nullptr;
    }

    tvg_shape->transform({ _transform[0],
                           _transform[2],
                           _transform[4],
                           _transform[1],
                           _transform[3],
                           _transform[5],
                           0,
                           0,
                           1 });

    if (_bg_clip_path) {
        _bg_clip_path->fill(255, 255, 255, 255);
        Ptr<tvg::Scene> scene = tvg::Scene::gen();
        scene->push(Ptr<tvg::Paint>(tvg_shape->duplicate()));
        scene->composite(
            std::unique_ptr<tvg::Shape>(
                static_cast<tvg::Shape*>(_bg_clip_path->duplicate())
            ),
            tvg::CompositeMethod::ClipPath
        );
        _canvas->push(move(scene));
        _bg_clip_path = nullptr;
    } else _canvas->push(Ptr<tvg::Paint>(tvg_shape->duplicate()));
}

void TvgRenderer::clipPath(rive::RenderPath* path) {
    // Note: ClipPath transform matrix is calculated by transfrom matrix in
    // addRenderPath function
    if (_bg_clip_path == nullptr) {
        _bg_clip_path = static_cast<TvgRenderPath*>(path)->tvgShape.get();
        _bg_clip_path->transform({ _transform[0],
                                   _transform[2],
                                   _transform[4],
                                   _transform[1],
                                   _transform[3],
                                   _transform[5],
                                   0,
                                   0,
                                   1 });
    } else {
        _clip_path = static_cast<TvgRenderPath*>(path)->tvgShape.get();
        _clip_path->transform({ _transform[0],
                                _transform[2],
                                _transform[4],
                                _transform[1],
                                _transform[3],
                                _transform[5],
                                0,
                                0,
                                1 });
    }
}

void TvgRenderer::drawImage(
    const rive::RenderImage* image, rive::BlendMode blend_mode, float opacity
) {
    GDERR("Trying to draw image (not yet implemented).");
    // const TvgRenderImage* tvg_image = static_cast<const
    // TvgRenderImage*>(image); Ptr<tvg::Picture> picture = tvg::Picture::gen();
    // tvg::Result result = picture->load(
    //     tvg_image->dataw(),
    //     tvg_image->width(),
    //     tvg_image->height(),
    //     false
    // );

    // if (result != tvg::Result::Success) {
    //     GDERR("Error loading image: ", as_string(result).c_str());
    //     return;
    // }

    // picture->blend(convert_blend_mode(blend_mode));
    // picture->opacity(opacity);

    // _canvas->push(Ptr<tvg::Paint>(picture->duplicate()));
}

void TvgRenderer::drawImageMesh(
    const rive::RenderImage* image,
    rive::rcp<rive::RenderBuffer> vertices_f32,
    rive::rcp<rive::RenderBuffer> uvCoords_f32,
    rive::rcp<rive::RenderBuffer> indices_u16,
    rive::BlendMode blend_mode,
    float opacity
) {
    GDERR("Trying to draw image mesh (not yet implemented).");
}