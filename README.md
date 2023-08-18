# Godot Rive

### An integration of Rive into Godot using GDExtension

> :warning: This extension is not yet finished. There are still a lot of bugs and unimplemented features.

This extensions adds [Rive](https://rive.app) support to Godot 4.

It makes use of the following third-party libraries:
- [`rive-cpp`](https://github.com/rive-app/rive-cpp)
- [`thorvg`](https://github.com/thorvg/thorvg)

## Building & installation

> :warning: These instructions may be incomplete. They are only tested on MacOS.

### 1. Build rive-cpp
1. Install scons, if not already installed
2. Update the submodule (`git submodule update thirdparty/rive-cpp`)
3. Copy the file `SConstruct-rive` into `thirdparty/rive-cpp`
4. Inside `thirdparty/rive-cpp`, run `scons` (run `scons --help` to see full list of options)

### 2. Build thorvg
1. Install meson and ninja, if not already installed
2. Update the submodule (`git submodule update thirdparty/thorvg`)
3. Follow [ThorVG's build instructions](https://github.com/thorvg/thorvg#installation)

### 3. Build godot-rive
1. Update the submodule (`git submodule update godot-cpp`)

### 4. Build the extension
1. Install scons, if not already installed
2. Inside the root directory, run `scons` (run `scons --help` to see full list of options)
3. You may be required to add the generated lib binary files from `rive-cpp` or `thorvg` to Godot's contents directory -- on MacOS, you would copy `demo/bin/rive/librive.dylib` and `demo/bin/thorvg/libthorvg.dylib` into the directory `Godot.app/Contents/MacOS`

## Roadmap
- [x] Load `.riv` files
- [x] Run and play Rive animations
- [ ] Raster image support
- [ ] Input events (hover, pressed, etc.)
- [ ] State machines
- [ ] Windows/Linux support
- [ ] iOS/Android support
- [ ] Any missing features
