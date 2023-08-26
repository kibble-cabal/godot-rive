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

#include "rive_exceptions.hpp"
#include "utils/godot_macros.hpp"
#include "utils/out_redirect.hpp"
#include "utils/types.hpp"

using namespace godot;
using namespace rive;

Ptr<File> read_rive_file(String path, Factory *factory) {
    CerrRedirect errs = CerrRedirect();
    try {
        if (path.get_extension().to_lower() != "riv") throw RiveException("No .riv path provided.").no_report();
        if (!FileAccess::file_exists(path)) throw RiveException("File <" + path + "> not found.");

        PackedByteArray _bytes = FileAccess::get_file_as_bytes(path);
        const size_t length = _bytes.size();
        if (length < 1) throw RiveException("File <" + path + "> contained 0 bytes.");
        Span<const uint8_t> bytes = Span(const_cast<uint8_t *>(_bytes.ptr()), length);

        ImportResult result;
        Ptr<File> file = File::import(bytes, factory, &result);
        if (result != ImportResult::success)
            throw RiveException(String("Failed to import.\nErrors: ") + String(errs.str().c_str()));

        return file;
    } catch (RiveException error) {
        error.report();
        return nullptr;
    }
}

#endif
