#ifndef RIVEEXTENSION_H
#define RIVEEXTENSION_H

#include <thorvg.h>

// Stdlib
#include <vector>

// Godot
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>

// Rive
#include <rive/animation/state_machine_instance.hpp>
#include <rive/file.hpp>

// Extension
#include "tvg_renderer/factory.h"
#include "tvg_renderer/renderer.h"
#include "utils/out_redirect.hpp"
#include "utils/types.hpp"

using namespace godot;

struct Img {
    std::vector<uint32_t> bytes;
    Ref<Image> image;
    Ref<ImageTexture> texture;

    void reset();
    PackedByteArray get_byte_array();
};

struct Rive {
    Ptr<rive::File> file;
    Ptr<rive::Factory> factory;
    Ptr<tvg::SwCanvas> canvas;
    TvgRenderer renderer = TvgRenderer(nullptr);
    Ptr<rive::LinearAnimationInstance> animation;
    Ptr<rive::StateMachineInstance> state_machine;

    void reset();
};

class RiveViewer : public Control {
    GDCLASS(RiveViewer, Control);

   private:
    String path;
    Img img;
    Rive riv;
    float elapsed;
    CerrRedirect _err_output;

   protected:
    static void _bind_methods();

   public:
    RiveViewer();
    ~RiveViewer();

    void _draw() override;
    void _process(float delta);

    void test();
    String get_file_path();
    void set_file_path(String value);
    int width();
    int height();

    void _update_image_size();
    void _reset_target();
    void _update_target();
    void _load();
    void _render(float delta);

    rive::Factory* factory();
};

#endif