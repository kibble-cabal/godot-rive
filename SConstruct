#!/usr/bin/env python
import os
import sys


env = SConscript("godot-cpp/SConstruct")

sources = []

# Add thorvg library
thorvg_dir = "thirdparty/thorvg/"
env.Prepend(CPPPATH=[thorvg_dir + "inc"])
env.Prepend(
    CPPPATH=[
        thorvg_dir + "src/lib",
        thorvg_dir + "src/lib/sw_engine",
        thorvg_dir + "src/loaders/external_png",
        thorvg_dir + "src/loaders/jpg",
        thorvg_dir + "src/loaders/raw",
        thorvg_dir + "src/loaders/svg",
        thorvg_dir + "src/loaders/tvg",
        thorvg_dir + "src/savers/tvg",
    ]
)
env.Prepend(CPPPATH=["thirdparty/libpng"])  # Also requires libpng headers
env.Prepend(LIBPATH=["demo/bin/thorvg"])
env.Prepend(LIBS=["thorvg"])


# Add rive library
rive_dir = "thirdparty/rive-tizen/"
env.Prepend(
    CPPPATH=[
        rive_dir + "src",
        rive_dir + "src/renderer",
        rive_dir + "inc",
        rive_dir + "submodule/src",
        rive_dir + "submodule/include"
    ]
)
env.Prepend(LIBPATH="demo/bin/rive")
env.Prepend(LIBS=["rive_tizen"])


# Add source files
env.Append(CPPPATH=["src/"])
sources += Glob("src/*.cpp")


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

env.Alias("thorvg")
env.Alias("rive_tizen")

Default(library)
