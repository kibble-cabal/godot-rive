#ifndef _RIVEEXTENSION_API_FILE_HPP_
#define _RIVEEXTENSION_API_FILE_HPP_

// stdlib
#include <string>

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

using namespace godot;

class RiveFile : public Resource {
    GDCLASS(RiveFile, Resource);

   private:
    rive::File *file;
    String path;

    friend class RiveViewer;

   protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("exists"), &RiveFile::exists);
        ClassDB::bind_method(D_METHOD("get_path"), &RiveFile::get_path);
        ClassDB::bind_method(D_METHOD("get_artboards"), &RiveFile::get_artboards);
        ClassDB::bind_method(D_METHOD("get_artboard_count"), &RiveFile::get_artboard_count);
        ClassDB::bind_method(D_METHOD("get_artboard_by_index", "index"), &RiveFile::get_artboard_by_index);
        ClassDB::bind_method(D_METHOD("get_artboard_by_name", "name"), &RiveFile::get_artboard_by_name);
    }

   public:
    static Ref<RiveFile> MakeRef(rive::File *file_value, String path_value) {
        if (!file_value) return nullptr;
        Ref<RiveFile> obj = memnew(RiveFile);
        obj->file = file_value;
        obj->path = path_value;
        return obj;
    }

    RiveFile() {}

    bool exists() const {
        return file != nullptr;
    }

    String get_path() const {
        return path;
    }

    TypedArray<RiveArtboard> get_artboards() const {
        TypedArray<RiveArtboard> artboards;
        for (int i = 0; i < file->artboardCount(); i++) {
            artboards.push_back(RiveArtboard::MakeRef(file, file->artboardAt(i).get()));
        }
        return artboards;
    }

    int get_artboard_count() const {
        return file ? file->artboardCount() : -1;
    }

    Ref<RiveArtboard> get_artboard_by_index(int index) const {
        if (file) return RiveArtboard::MakeRef(file, file->artboardAt(index).get());
        return nullptr;
    }

    Ref<RiveArtboard> get_artboard_by_name(String name) const {
        std::string artboard_name = (char *)name.ptrw();
        if (file) return RiveArtboard::MakeRef(file, file->artboardNamed(artboard_name).get());
        return nullptr;
    }

    String _to_string() const {
        Dictionary format_args;
        format_args["cls"] = get_class_static();
        format_args["path"] = path;
        return String("{cls}(\"{path}\")").format(format_args);
    }

    bool operator==(const RiveFile &other) const {
        return other.file == file;
    }

    bool operator!=(const RiveFile &other) const {
        return other.file != file;
    }
};

#endif