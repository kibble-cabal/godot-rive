#ifndef _RIVEEXTENSION_FILE_READER_HPP_
#define _RIVEEXTENSION_FILE_READER_HPP_

// godot
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/string.hpp>

// stdlib
#include <iostream>
#include <sstream>

// rive
#include <rive/factory.hpp>
#include <rive/file.hpp>
#include <rive/file_asset_resolver.hpp>
#include <rive/span.hpp>

#include "utils/godot_macros.hpp"
#include "utils/out_redirect.hpp"
#include "utils/types.hpp"

using namespace godot;
using namespace rive;

Ptr<File> read_rive_file(String path, Factory *factory) {
    CerrRedirect errs = CerrRedirect();

    if (path.length() == 0) {
        GDERR("No file selected.");
        return nullptr;
    }
    if (!FileAccess::file_exists(path)) {
        GDERR("File not found at path: " + path);
        return nullptr;
    }

    PackedByteArray _bytes = FileAccess::get_file_as_bytes(path);
    const size_t length = _bytes.size();

    if (length < 1) {
        GDERR("File <" + path + "> contained 0 bytes.");
        return nullptr;
    }

    Span<const uint8_t> bytes
        = Span(const_cast<uint8_t *>(_bytes.ptr()), length);

    ImportResult result;

    Ptr<File> file = File::import(bytes, factory, &result);

    if (result != ImportResult::success) {
        GDERR("Failed to import.\nErrors: ", errs.str().c_str());
        return nullptr;
    }

    return file;
}

#endif
