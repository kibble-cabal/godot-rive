#include "rive_extension.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "rive_tizen.hpp"

using namespace godot;

#define GDPRINT godot::UtilityFunctions::print
#define GDERR godot::UtilityFunctions::printerr

RiveNode::RiveNode() {
    // Initialize any variables here.
}

RiveNode::~RiveNode() {
    // Add your cleanup here.
}

void RiveNode::test() {
    if (filename.length() == 0) {
        GDPRINT("No file selected.");
        return;
    }
    if (!FileAccess::file_exists(filename)) {
        GDERR("File not found at path: " + filename);
        return;
    }

    PackedByteArray _bytes = FileAccess::get_file_as_bytes(filename);
    const char32_t *fname = filename.ptr();

    const size_t length = _bytes.size();

    if (length == 0) {
        GDERR("File " + filename + " contained 0 bytes.");
        return;
    }

    // uint8_t *bytes = new uint8_t[length];
    // bytes
    uint8_t *bytes = const_cast<uint8_t *>(_bytes.ptr());

    // uint8_t *bytes = _bytes.ptr();
    // Load Rive File
    // FILE *fp = fopen(fname, "r");

    // fseek(fp, 0, SEEK_END);
    // size_t length = ftell(fp);
    // fseek(fp, 0, SEEK_SET);

    // uint8_t *bytes = new uint8_t[length];
    // if (fread(bytes, 1, length, fp) != length) {
    //     delete[] bytes;
    //     GDPRINT("failed to read all of %s\n", filename);
    //     return;
    // }

    auto reader = rive::BinaryReader(bytes, length);
    rive::File *file = nullptr;
    auto result = rive::File::import(reader, &file);
    if (result != rive::ImportResult::success) {
        delete[] bytes;
        GDPRINT("failed to import %s\n", filename);
        return;
    }
}