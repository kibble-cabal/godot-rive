#!/bin/bash

source build_common.sh

function make_dependencies () {
    echo "Making dependencies..."
    cd ../thirdparty/rive-cpp/skia/dependencies
    sh make_dependencies.sh
    cd -
}

# 1. Update rive-cpp submodule
cd ../
git submodule update thirdparty/rive-cpp
cd -

# 2. Make skia dependency, if it doesn't exist
if [[ -e "../thirdparty/rive-cpp/skia/dependencies/skia" ]]; then
    echo "Found dependencies!"
else
    make_dependencies
fi