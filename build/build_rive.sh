#!/bin/bash

source build_common.sh

SCONSTRUCT_ORIGIN="SConstruct.rive"
SCONSTRUCT_TARGET="../thirdparty/rive-cpp/SConstruct"

function help () {
    echo "\n${bold}Usage:${normal}"
    echo "    This script calls scons internally. Try running ${cyan}scons --help${normal} to see a full list of options."
    echo "\n${bold}Examples:${normal}"
    echo "    ${cyan}sh build_rive.sh platform=macosx               ${normal} - ${dim}build for MacOS${normal}"
    echo "    ${cyan}sh build_rive.sh target=template_release -j10  ${normal} - ${dim}build for release using 10 cores${normal}"
}

copy_sconstruct () {
    echo "Copying rive SConstruct..."
    cp "${SCONSTRUCT_ORIGIN}" "${SCONSTRUCT_TARGET}"
}

build () {
    cd ../thirdparty/rive-cpp
    scons "$@"
    cd -
}

# Run help command, if requested
show_help $1

# 1. Update rive-cpp submodule
cd ../
git submodule update thirdparty/rive-cpp
cd -

# 2. Copy SConstruct file, if needed
if [[ -e SCONSTRUCT_TARGET ]]; then
    if [[ SCONSTRUCT_ORIGIN -nt SCONSTRUCT_TARGET ]]; then copy_sconstruct 
    fi
else copy_sconstruct
fi

# 2. Build with scons
build "$@"