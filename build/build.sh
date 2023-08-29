#!/bin/bash

source build_common.sh

function help () {
    echo "\n${bold}Usage:${normal}"
    echo "    This script calls scons internally. Try running ${cyan}scons --help${normal} to see a full list of options."
    echo "\n${bold}Examples:${normal}"
    echo "    ${cyan}sh build.sh platform=macosx               ${normal} - ${dim}build for MacOS${normal}"
    echo "    ${cyan}sh build.sh target=template_release -j10  ${normal} - ${dim}build for release using 10 cores${normal}"
}

# Run help command, if requested
show_help $1

# 1. Build rive
echo "\n${bold}Building rive...${normal}"
sh build_rive.sh "$@"

# 2. Build skia
echo "\n${bold}Building skia...${normal}"
sh build_skia.sh "$@"

# 3. Build extension with scons
echo "\n${bold}Building extension...${normal}"
scons "$@"