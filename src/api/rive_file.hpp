#ifndef _RIVEEXTENSION_API_FILE_HPP_
#define _RIVEEXTENSION_API_FILE_HPP_

// stdlib
#include <string>
#include <vector>

// godot-cpp
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

// rive-cpp
#include <rive/animation/state_machine_instance.hpp>
#include <rive/file.hpp>

// extension
#include "api/rive_artboard.hpp"
#include "utils/read_rive_file.hpp"

using namespace godot;

class RiveFile : public Resource {
    GDCLASS(RiveFile, Resource);

    friend class RiveViewerBase;
    friend class RiveInstance;

   private:
    Ptr<rive::File> file;
    String path = "";

    Instances<RiveArtboard> artboards = Instances<RiveArtboard>([this](int index) -> Ref<RiveArtboard> {
        if (file && file->artboardCount() > index && index >= 0)
            return RiveArtboard::MakeRef(
                file.get(),
                file->artboardAt(index),
                index,
                file->artboardNameAt(index).c_str()
            );
        else return nullptr;
    });

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveFile::exists);
        ClassDB::bind_method(D_METHOD("get_path"), &RiveFile::get_path);
        ClassDB::bind_method(D_METHOD("get_artboards"), &RiveFile::get_artboards);
        ClassDB::bind_method(D_METHOD("get_artboard_names"), &RiveFile::get_artboard_names);
        ClassDB::bind_method(D_METHOD("get_artboard_count"), &RiveFile::get_artboard_count);
        ClassDB::bind_method(D_METHOD("get_artboard", "index"), &RiveFile::get_artboard);
        ClassDB::bind_method(D_METHOD("find_artboard", "name"), &RiveFile::find_artboard);
        ClassDB::bind_method(D_METHOD("reset_artboard", "index"), &RiveFile::reset_artboard);
    }

    void _instantiate_artboards() {
        if (exists())
            for (int i = 0; i < file->artboardCount(); i++) {
                auto ab = get_artboard(i);
                if (ab.is_null() || !ab->exists()) throw RiveException("Failed to instantiate artboard.");
            }
    }

    String _get_artboard_property_hint() const {
        PackedStringArray hints;
        hints.append("None:-1");
        if (file)
            artboards.for_each([&hints](Ref<RiveArtboard> artboard, int i) {
                if (artboard->exists()) hints.append(artboard->get_name() + ":" + std::to_string(i).c_str());
            });
        return String(",").join(hints);
    }

   public:
    static Ref<RiveFile> MakeRef(Ptr<rive::File> file_value, String path_value) {
        if (!file_value) return nullptr;
        Ref<RiveFile> obj = memnew(RiveFile);
        obj->file = std::move(file_value);
        obj->path = path_value;
        return obj;
    }

    static Ref<RiveFile> Load(String path, rive::Factory *factory) {
        try {
            Ptr<rive::File> file = read_rive_file(path, factory);
            if (file != nullptr) {
                auto file_wrapper = RiveFile::MakeRef(std::move(file), path);
                GDPRINT("Successfully imported <", path, ">!");
                return file_wrapper;
            } else throw RiveException("Unable to import <" + path + ">");
        } catch (RiveException error) {
            error.report();
        }
        return nullptr;
    }

    RiveFile() {}

    bool exists() const {
        return file != nullptr;
    }

    String get_path() const {
        return path;
    }

    TypedArray<RiveArtboard> get_artboards() const {
        return artboards.get_list();
    }

    PackedStringArray get_artboard_names() const {
        PackedStringArray names;
        artboards.for_each([&names](Ref<RiveArtboard> artboard, int _) { names.append(artboard->get_name()); });
        return names;
    }

    int get_artboard_count() const {
        return artboards.get_size();
    }

    Ref<RiveArtboard> get_artboard(int index) {
        return artboards.get(index);
    }

    Ref<RiveArtboard> find_artboard(String name) const {
        return artboards.find([name](Ref<RiveArtboard> artboard, int _) { return artboard->get_name() == name; });
    }

    Ref<RiveArtboard> reset_artboard(int index) {
        return artboards.reinstantiate(index);
    }

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["path"] = path;
        return String("{cls}(\"{path}\")").format(format_args);
    }

    bool operator==(const RiveFile &other) const {
        return other.path == path;
    }

    bool operator!=(const RiveFile &other) const {
        return other.path != path;
    }
};

#endif