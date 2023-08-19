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

env["STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME"] = True

sources = []

# Add skia library
SKIA_SRC_DIR = "thirdparty/rive-cpp/skia/"
SKIA_BIN_DIR = path.join(SKIA_SRC_DIR, "dependencies/skia/out/static")
env.Alias("skia", SKIA_BIN_DIR)
env.Prepend(
    LIBS=["skia"],
    LIBPATH=[SKIA_BIN_DIR],
    CPPPATH=[
        SKIA_SRC_DIR,
        path.join(SKIA_SRC_DIR, "dependencies/skia"),
        path.join(SKIA_SRC_DIR, "dependencies/skia/include"),
        path.join(SKIA_SRC_DIR, "dependencies/skia/src"),
        path.join(SKIA_SRC_DIR, "renderer"),
        path.join(SKIA_SRC_DIR, "renderer/include"),
        path.join(SKIA_SRC_DIR, "renderer/src"),
    ]
)

# Add thorvg library
# TVG_SRC_DIR = "thirdparty/thorvg/"
# TVG_BIN_DIR = "thirdparty/thorvg/build/src"
# env.Alias("thorvg", TVG_BIN_DIR)
# env.Prepend(
#     LIBS=["thorvg"],
#     LIBPATH=[TVG_BIN_DIR],
#     CPPPATH=[
#         path.join(TVG_SRC_DIR, "inc"),
#         path.join(TVG_SRC_DIR, "src"),
#     ]
# )

# Add rive library
RIVE_SRC_DIR = "thirdparty/rive-cpp/"
RIVE_BIN_DIR = "demo/bin/rive"
env.Alias("rive", RIVE_BIN_DIR)
env.Prepend(
    LIBS=["rive"],
    LIBPATH=[RIVE_BIN_DIR],
    CPPPATH=[
        RIVE_SRC_DIR,
        path.join(RIVE_SRC_DIR, "include"),
        path.join(RIVE_SRC_DIR, "src")
    ]
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
