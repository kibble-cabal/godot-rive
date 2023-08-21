#!/usr/bin/env python
import os
import sys
from os import path


def glob_recursive(root_path, pattern):
    result_nodes = []
    paths = [root_path]
    while paths:
        path = paths.pop()
        all_nodes = Glob(f'{path}/*')
        # `srcnode()` must be used because `isdir()` doesn't work for entries in variant dirs which haven't been copied yet.
        paths.extend(entry for entry in all_nodes if entry.isdir()
                     or (entry.srcnode() and entry.srcnode().isdir()))
        result_nodes.extend(Glob(f'{path}/{pattern}'))
    return sorted(result_nodes)


env = SConscript("godot-cpp/SConstruct")

env["STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME"] = 1

sources = []

# Add rive library
env.Prepend(
    LIBPATH=[
        "thirdparty/rive-cpp/build/macosx/bin/debug",
        "thirdparty/rive-cpp/dependencies/macosx/cache/bin/debug",
    ],
    CPPPATH=[
        "thirdparty/rive-cpp/",
        "thirdparty/rive-cpp/include",
        "thirdparty/rive-cpp/src",
    ],
    LIBS=[
        "rive",
        "rive_harfbuzz",
        "rive_sheenbidi"
    ],
)

# Add skia's dependencies
env.Prepend(
    LIBPATH=[
        "thirdparty/rive-cpp/skia/renderer/build/macosx/bin/debug",
        "thirdparty/rive-cpp/skia/dependencies/skia_rive_optimized/out/static",
    ],
    CPPPATH=[
        "thirdparty/rive-cpp/skia/renderer",
        "thirdparty/rive-cpp/skia/dependencies/skia_rive_optimized",
        "thirdparty/rive-cpp/skia/dependencies/skia_rive_optimized/include",
        "thirdparty/rive-cpp/skia/dependencies/skia_rive_optimized/src",
    ],
    LIBS=[
        # "compression_utils_portable",
        # "jpeg",
        # "pathkit",
        # "png",
        "rive_skia_renderer",
        "skia",
        # "webp_sse41",
        # "webp",
        # "zlib",
    ],
)

# Add source files
env.Append(CPPPATH=["src/"])
sources += glob_recursive("src", "*.cpp")


if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/librive.{}.{}.framework/librive.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/librive{}{}".format(
            env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
