# Godot Rive

### An integration of Rive into Godot using GDExtension

> :warning: This extension is not yet finished. There are still a lot of bugs and unimplemented features.

This extensions adds [Rive](https://rive.app) support to Godot 4.

It makes use of the following third-party libraries:
- [`rive-cpp`](https://github.com/rive-app/rive-cpp)
- [`skia`](https://github.com/google/skia) (included in `rive-cpp`)

## Building & installation

> :warning: These instructions may be incomplete. They are only tested on MacOS.

### 1. Build skia
1. In `thirdparty/rive-cpp/skia/renderer/build/build.sh`, change line 4 - replace `skia` with `skia_rive_optimized`
2. Inside `thirdparty/rive-cpp/skia/dependencies`, run `sh make_dependencies.sh`
3. Inside `thirdparty/rive-cpp/skia/dependencies`, run `sh make_skia_macos.sh` (if on MacOS)
4. Inside `thirdparty/rive-cpp/skia/dependendencies/skia_rive_optimized/out/static`, run `ninja -C .`

### 2. Build rive-cpp
1. Update the submodule (`git submodule update thirdparty/rive-cpp`)
2. Inside `thirdparty/rive-cpp`, run `sh build.sh`

### 3. Build renderer
1. Inside `thirdparty/rive-cpp/skia/renderer`, run `sh build.sh`

### 3. Build godot-cpp
1. Update the submodule (`git submodule update godot-cpp`)
2. Inside `godot-cpp`, run `scons` (or `scons --help`)

### 4. Build the extension
1. Install scons, if not already installed
2. Inside the root directory, run `scons` (or `scons --help` to see full list of options)
3. You may be required to add the generated lib binary files from `rive-cpp` to Godot's contents directory. Open up the demo project and look at the output to see you get an error - if so, on MacOS, you would copy `demo/bin/rive/librive.dylib` into the directory `Godot.app/Contents/MacOS`.

## Roadmap
- [x] Load `.riv` files
- [x] Run and play Rive animations
- [x] Raster image support
- [x] Input events (hover, pressed, etc.)
- [x] Alignment & size exported properties
- [ ] Optimization
- [ ] Multiple scenes
- [ ] Dynamic exported properties based on state machine
- [ ] Windows/Linux support
- [ ] iOS/Android support
- [ ] Editor preview
- [ ] Thumbnails
- [ ] Any missing features
- [ ] `.riv` ResourceLoader
