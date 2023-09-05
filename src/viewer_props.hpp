#ifndef _RIVEEXTENSION_VIEWER_PROPS_HPP_
#define _RIVEEXTENSION_VIEWER_PROPS_HPP_

// stdlib
#include <functional>

// godot-cpp
#include <godot_cpp/variant/builtin_types.hpp>
#include <godot_cpp/variant/variant.hpp>

// rive-cpp
#include <rive/layout.hpp>

using namespace godot;
using namespace std;

enum FIT { FILL = 1, CONTAIN = 2, COVER = 3, FIT_WIDTH = 4, FIT_HEIGHT = 5, NONE = 6, SCALE_DOWN = 9 };

static const char *FitEnumPropertyHint = "Fill:1,Contain:2,Cover:3,FitWidth:4,FitHeight:5,None:6,ScaleDown:7";

enum ALIGN {
    TOP_LEFT = 1,
    TOP_CENTER = 2,
    TOP_RIGHT = 3,
    CENTER_LEFT = 4,
    CENTER = 5,
    CENTER_RIGHT = 6,
    BOTTOM_LEFT = 7,
    BOTTOM_CENTER = 8,
    BOTTOM_RIGHT = 9
};

static const char *AlignEnumPropertyHint
    = "TopLeft:1,TopCenter:2,TopRight:3,CenterLeft:4,Center:5,CenterRight:6,BottomLeft:7,BottomCenter:8,BottomRight:9";

static rive::Fit convert(FIT fit) {
    switch (fit) {
        case FIT::COVER:
            return rive::Fit::cover;
        case FIT::FILL:
            return rive::Fit::fill;
        case FIT::FIT_HEIGHT:
            return rive::Fit::fitHeight;
        case FIT::FIT_WIDTH:
            return rive::Fit::fitWidth;
        case FIT::NONE:
            return rive::Fit::none;
        case FIT::SCALE_DOWN:
            return rive::Fit::scaleDown;
        case FIT::CONTAIN:
        default:
            return rive::Fit::contain;
    }
}

static rive::Alignment convert(ALIGN alignment) {
    switch (alignment) {
        case ALIGN::BOTTOM_CENTER:
            return rive::Alignment::bottomCenter;
        case ALIGN::BOTTOM_LEFT:
            return rive::Alignment::bottomLeft;
        case ALIGN::BOTTOM_RIGHT:
            return rive::Alignment::bottomRight;
        case ALIGN::CENTER:
            return rive::Alignment::center;
        case ALIGN::CENTER_LEFT:
            return rive::Alignment::centerLeft;
        case ALIGN::CENTER_RIGHT:
            return rive::Alignment::centerRight;
        case ALIGN::TOP_CENTER:
            return rive::Alignment::topCenter;
        case ALIGN::TOP_RIGHT:
            return rive::Alignment::topRight;
        case ALIGN::TOP_LEFT:
        default:
            return rive::Alignment::topLeft;
    }
}

template <typename... Args>
using Callback = function<void(Args...)>;

template <typename... Args>
struct PropEvent {
    std::vector<Callback<Args...>> listeners;

    void subscribe(Callback<Args...> listener) {
        listeners.push_back(listener);
    }

    void emit(Args... args) const {
        for (int i = 0; i < listeners.size(); i++) {
            auto listener = listeners[i];
            listener(args...);
        }
    }
};

struct ViewerProps {
   private:
    String _path;
    int _width = 1;
    int _height = 1;
    bool _disable_press = false;
    bool _disable_hover = false;
    bool _paused = false;
    int _artboard = -1;
    int _scene = -1;
    int _animation = -1;
    Dictionary _scene_properties;
    FIT _fit = FIT::CONTAIN;
    ALIGN _alignment = ALIGN::CENTER;

    /* Events */
    PropEvent<String> path_changed;
    PropEvent<> property_changed;
    PropEvent<> scene_properties_changed;
    PropEvent<int> artboard_changed;
    PropEvent<int> scene_changed;
    PropEvent<int> animation_changed;
    PropEvent<float, float> size_changed;

   public:
    /* Event handlers */
    void on_path_changed(Callback<String> callback) {
        path_changed.subscribe(callback);
    }

    void on_property_changed(Callback<> callback) {
        property_changed.subscribe(callback);
    }

    void on_scene_properties_changed(Callback<> callback) {
        scene_properties_changed.subscribe(callback);
    }

    void on_artboard_changed(Callback<int> callback) {
        artboard_changed.subscribe(callback);
    }

    void on_scene_changed(Callback<int> callback) {
        scene_changed.subscribe(callback);
    }

    void on_animation_changed(Callback<int> callback) {
        animation_changed.subscribe(callback);
    }

    void on_size_changed(Callback<float, float> callback) {
        size_changed.subscribe(callback);
    }

    /* Getters */

    String path() const {
        return _path;
    }

    int width() const {
        return std::max(_width, 1);
    }

    int height() const {
        return std::max(_height, 1);
    }

    int artboard() const {
        return _artboard;
    }

    int scene() const {
        return _scene;
    }

    int animation() const {
        return _animation;
    }

    FIT fit() const {
        return _fit;
    }

    rive::Fit rive_fit() const {
        return convert(_fit);
    }

    ALIGN alignment() const {
        return _alignment;
    }

    rive::Alignment rive_alignment() const {
        return convert(_alignment);
    }

    bool disable_press() const {
        return _disable_press;
    }

    bool disable_hover() const {
        return _disable_hover;
    }

    bool paused() const {
        return _paused;
    }

    Dictionary scene_properties() const {
        return _scene_properties;
    }

    Variant scene_property(String property) const {
        return _scene_properties.get(property, nullptr);
    }

    bool has_scene_property(String property) const {
        return _scene_properties.has(property);
    }

    /* Setters */

    void path(String value) {
        if (value != _path) {
            artboard(-1);
            _path = value;
            path_changed.emit(value);
        }
    }

    void width(int value) {
        if (value != _width) {
            _width = value;
            size_changed.emit(_width, _height);
        }
    }

    void height(int value) {
        if (value != _height) {
            _height = value;
            size_changed.emit(_width, _height);
        }
    }

    void size(int w, int h) {
        if (_width != w || _height != h) {
            _width = w, _height = h;
            property_changed.emit();
            size_changed.emit(_width, _height);
        }
    }

    void artboard(int index) {
        if (index != _artboard) {
            scene(-1);
            animation(-1);
            _artboard = index;
            artboard_changed.emit(index);
        }
    }

    void scene(int index) {
        if (index != _scene) {
            _scene_properties.clear();
            _scene = index;
            scene_changed.emit(index);
        }
    }

    void animation(int index) {
        if (index != _animation) {
            _animation = index;
            animation_changed.emit(index);
        }
    }

    void fit(FIT value) {
        if (value != _fit) {
            _fit = value;
            property_changed.emit();
        }
    }

    void alignment(ALIGN value) {
        _alignment = value;
        property_changed.emit();
    }

    void disable_press(bool value) {
        if (_disable_press != value) {
            _disable_press = value;
            property_changed.emit();
        }
    }

    void disable_hover(bool value) {
        if (_disable_hover != value) {
            _disable_hover = value;
            property_changed.emit();
        }
    }

    void paused(bool value) {
        if (_paused != value) {
            _paused = value;
            property_changed.emit();
        }
    }

    void scene_properties(Dictionary value) {
        if (_scene_properties != value) {
            _scene_properties = value;
            scene_properties_changed.emit();
        }
    }

    void clear_scene_properties() {
        if (!_scene_properties.is_empty()) {
            _scene_properties.clear();
            scene_properties_changed.emit();
        }
    }

    void scene_property(String property, Variant value) {
        if (_scene_properties.get(property, nullptr) != value) {
            _scene_properties[property] = value;
            scene_properties_changed.emit();
        }
    }
};

#endif