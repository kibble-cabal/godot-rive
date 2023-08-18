#!/usr/bin/env python
import os
import sys


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

sources = []

# Add thorvg library
thorvg_src_dir = "thirdparty/thorvg/"
thorvg_binary_dir = "thirdparty/thorvg/build/src"
thorvg_binary = thorvg_binary_dir + "/libthorvg.dylib"
env.Alias("thorvg", thorvg_binary_dir)
env.Prepend(
    LIBS=["thorvg"],
    LIBPATH=[thorvg_binary_dir],
    CPPPATH=[
        thorvg_src_dir + "inc",
        thorvg_src_dir + "src",
    ]
)

# Add rive library
rive_src_dir = "thirdparty/rive-cpp/"
rive_binary_dir = "demo/bin/rive"
rive_binary = rive_binary_dir + "/librive.dylib"
env.Alias("rive", rive_binary_dir)
env.Prepend(
    LIBS=["rive"],
    LIBPATH=[rive_binary_dir],
    CPPPATH=[
        rive_src_dir + "include",
        rive_src_dir + "src"
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

# env.Depends(
#     library,
#     rive_binary,
# )
#     Command(
#         "demo/bin/rive/librive.a",
#         rive_build_dir + "/librive.dylib",
#         Copy("$TARGET", "$SOURCE"),
#     )
# )

Default(library)
