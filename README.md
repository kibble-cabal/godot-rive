# Godot Rive

### An integration of Rive into Godot using GDExtension

> :warning: This extension is not yet finished. There are still a lot of bugs and unimplemented features.

This extensions adds [Rive](https://rive.app) support to Godot 4.

It makes use of the following third-party libraries:
- [`rive-cpp`](https://github.com/rive-app/rive-cpp)
- [`skia`](https://github.com/google/skia) (included in `rive-cpp`)

## Building

> :warning: These instructions may be incomplete. They are only tested on MacOS.

### 1. Build rive-cpp
1. Install scons, if not already installed
2. Update the submodule (`git submodule update thirdparty/rive-cpp`)
3. Copy the file `SConstruct_rive` into `thirdparty/rive-cpp`
4. Inside `thirdparty/rive-cpp`, run `scons` (run `scons --help` to see full list of options)

### 2. Build skia
1. Inside `thirdparty/rive-cpp/skia/dependencies`, run `sh make_dependencies.sh`
2. Copy the file `SConstruct_skia` into `thirdparty/rive-cpp/skia`
3. Inside `thirdparty/rive-cpp/skia`, run `scons` (or `scons --help` to see full list of options)

### 3. Build godot-cpp
1. Update the submodule (`git submodule update godot-cpp`)

### 4. Build the extension
1. Install scons, if not already installed
2. Inside the root directory, run `scons` (or `scons --help` to see full list of options)


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
