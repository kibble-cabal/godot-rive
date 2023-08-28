#!/bin/bash

source build_common.sh

function help () {
    echo "\n${bold}Usage:${normal}"
    echo "    This script calls scons internally. Try running ${cyan}scons --help${normal} to see a full list of options."
    echo "\n${bold}Examples:${normal}"
    echo "    ${cyan}sh build_rive.sh platform=macosx               ${normal} - ${dim}build for MacOS${normal}"
    echo "    ${cyan}sh build_rive.sh target=template_release -j10  ${normal} - ${dim}build for release using 10 cores${normal}"
}

copy_sconstruct () {
    echo "Copying rive SConstruct..."
    cp SConstruct_rive thirdparty/rive-cpp/SConstruct
}

build () {
    cd thirdparty/rive-cpp
    scons "$@"
    cd -
}

# Run help command, if requested
show_help $1

# 1. Update rive-cpp submodule
git submodule update thirdparty/rive-cpp

# 2. Copy SConstruct file, if needed
if [[ -e "thirdparty/rive-cpp/skia/SConstruct" ]]; then
    if [[ "SConstruct_skia" -nt "thirdparty/rive-cpp/skia/SConstruct" ]]; then
        copy_sconstruct
    fi
else
    copy_sconstruct
fi

# 2. Build with scons
build "$@"