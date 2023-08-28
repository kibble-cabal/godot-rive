#!/bin/bash

source build_common.sh

function help () {
    echo "\n${bold}Usage:${normal}"
    echo "    This script calls scons internally. Try running ${cyan}scons --help${normal} to see a full list of options."
    echo "\n${bold}Examples:${normal}"
    echo "    ${cyan}sh build_skia.sh platform=macosx               ${normal} - ${dim}build for MacOS${normal}"
    echo "    ${cyan}sh build_skia.sh target=template_release -j10  ${normal} - ${dim}build for release using 10 cores${normal}"
}

function copy_sconstruct () {
    echo "Copying skia SConstruct..."
    cp SConstruct_skia thirdparty/rive-cpp/skia/SConstruct
}

function make_dependencies () {
    echo "Making dependencies..."
    cd thirdparty/rive-cpp/skia/dependencies
    sh make_dependencies.sh
    cd -
}

function build () {
    cd thirdparty/rive-cpp/skia
    scons "$@"
    cd -
}

# Run help command, if requested
show_help $1

# 1. Update rive-cpp submodule
git submodule update thirdparty/rive-cpp

# 2. Make skia dependency, if it doesn't exist
if [[ -e "thirdparty/rive-cpp/skia/dependencies/skia" ]]; then
    echo "Found dependencies!"
else
    make_dependencies
fi

# 3. Copy SConstruct file, if needed
if [[ -e "thirdparty/rive-cpp/skia/SConstruct" ]]; then
    if [[ "SConstruct_skia" -nt "thirdparty/rive-cpp/skia/SConstruct" ]]; then
        copy_sconstruct
    fi
else
    copy_sconstruct
fi

# 4. Build with scons
build "$@"