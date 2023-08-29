set -o errexit

bold=$(tput bold)
dim=$(tput dim)
normal=$(tput sgr0)
cyan=$(tput setaf 6)

# Runs help command, if requested
function show_help () {
    case $1 in
    -h | --help )
        help
        exit
        ;;
    esac;
}