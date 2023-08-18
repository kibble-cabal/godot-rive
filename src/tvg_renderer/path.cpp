#include "path.h"

TvgRenderPath::TvgRenderPath() {
    tvgShape = tvg::Shape::gen();
}

TvgRenderPath::TvgRenderPath(
    rive::Span<rive::Vec2D> points,
    rive::Span<rive::PathVerb> verbs,
    rive::FillRule fill_rule
) {
    tvgShape = tvg::Shape::gen();
    fillRule(fill_rule);
    // convert path coords
    std::vector<tvg::Point> coords;
    coords.resize(points.size());
    for (int i = 0; i < points.size(); i++) {
        coords[i] = convert_point(points[i]);
    }
    const tvg::Point* coord_data = coords.data();
    tvgShape->pathCoords(&coord_data);
    // convert path commands
    std::vector<tvg::PathCommand> commands;
    commands.resize(verbs.size());
    for (int i = 0; i < verbs.size(); i++) {
        commands[i] = convert_verb(verbs[i]);
    }
    const tvg::PathCommand* command_data = commands.data();
    tvgShape->pathCommands(&command_data);
}

void TvgRenderPath::fillRule(rive::FillRule value) {
    switch (value) {
        case rive::FillRule::evenOdd:
            tvgShape->fill(tvg::FillRule::EvenOdd);
            break;
        case rive::FillRule::nonZero:
            tvgShape->fill(tvg::FillRule::Winding);
            break;
    }
}

void TvgRenderPath::rewind() {}

void TvgRenderPath::addRenderPath(
    rive::RenderPath* path, const rive::Mat2D& transform
) {
    const tvg::Point* pts;
    auto ptsCnt = static_cast<TvgRenderPath*>(path)->tvgShape->pathCoords(&pts);
    if (!pts) return;

    const tvg::PathCommand* cmds;
    auto cmdCnt
        = static_cast<TvgRenderPath*>(path)->tvgShape->pathCommands(&cmds);
    if (!cmds) return;

    // Capture the last coordinates
    tvg::Point* pts2;
    auto ptsCnt2 = tvgShape->pathCoords(const_cast<const tvg::Point**>(&pts2));

    tvgShape->appendPath(cmds, cmdCnt, pts, ptsCnt);

    // Immediate Transform for the newly appended
    tvg::Point* pts3;
    auto ptsCnt3 = tvgShape->pathCoords(const_cast<const tvg::Point**>(&pts3));

    for (unsigned i = ptsCnt2; i < ptsCnt3; ++i) {
        pts3[i] = transform_coord(pts3[i], transform);
    }
}

void TvgRenderPath::moveTo(float x, float y) {
    tvgShape->moveTo(x, y);
}

void TvgRenderPath::lineTo(float x, float y) {
    tvgShape->lineTo(x, y);
}

void TvgRenderPath::cubicTo(
    float ox, float oy, float ix, float iy, float x, float y
) {
    tvgShape->cubicTo(ox, oy, ix, iy, x, y);
}

void TvgRenderPath::close() {
    tvgShape->close();
}