# Godot Rive

### An integration of Rive into Godot using GDExtension

> :warning: This extension is not yet finished. There are still a lot of bugs and unimplemented features.

This extensions adds [Rive](https://rive.app) support to Godot 4.

It makes use of the following third-party libraries:
- [`rive-cpp`](https://github.com/rive-app/rive-cpp)
- [`skia`](https://github.com/google/skia) (included in `rive-cpp`)

## Building

> :warning: These instructions may be incomplete. They are only tested on MacOS.  You may have to modify `build.sh`, `build_skia.sh`, and `build_rive.sh` for your system.

The following must be installed:
- [git](https://git-scm.com/)
- [scons](https://scons.org/)
- [ninja](https://ninja-build.org/)

To build, run the following commands (from the root directory):
```bash
cd build
sh build.sh
```

For more information, run:
```bash
sh build.sh --help
```

## Installation

> :warning: If you are not on M1 MacOS, you will need to build the extension yourself.

1. Create a folder in your project called `bin`, and copy the contents of `demo/bin` into the new folder
2. Update the paths in `rive.gdextension` to match your project folder structure

## Roadmap
- [x] Load `.riv` files
- [x] Run and play Rive animations
- [x] Raster image support
- [x] Input events (hover, pressed, etc.)
- [x] Alignment & size exported properties
- [x] Multiple scenes/artboards
- [x] Dynamic exported properties based on state machine
- [x] API for interaction during runtime
- [x] Add error handling
- [x] Add signals for event listeners (hover, pressed, etc)
- [x] Disable/enable event listeners (hover, pressed, etc) in API and editor
- [ ] Optimization
- [ ] Editor preview
- [ ] Windows/Linux support
- [ ] iOS/Android support
- [ ] Any missing features
- [ ] `.riv` ResourceLoade (thumbnails)
