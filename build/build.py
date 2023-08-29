import argparse
import subprocess
from argparse import RawDescriptionHelpFormatter
from os.path import exists

RESET = "\033[0m"
BOLD = "\033[01m"
DISABLE = "\033[02m"
UNDERLINE = "\033[04m"
REVERSE = "\033[07m"
STRIKE = "\033[09m"
INVISIBLE = "\033[08m"
BLACK = "\033[30m"
RED = "\033[31m"
GREEN = "\033[32m"
ORANGE = "\033[33m"
BLUE = "\033[34m"
PURPLE = "\033[35m"
CYAN = "\033[36m"
LIGHT_GREY = "\033[37m"
DARK_GREY = "\033[90m"
YELLOW = "\033[93m"
PINK = "\033[95m"


def Dim(text: str) -> str:
    return f"{LIGHT_GREY}{text}{RESET}"


def Cyan(text: str) -> str:
    return f"{CYAN}{text}{RESET}"


def Bold(text: str) -> str:
    return f"{BOLD}{text}{RESET}"


def Red(text: str) -> str:
    return f"{RED}{text}{RESET}"


def Green(text: str) -> str:
    return f"{GREEN}{text}{RESET}"


def print_list(*items: tuple[str, str]):
    for label, value in items:
        print(Bold(f"{label}:"), Cyan(value))


def handle_fail(code: int):
    if code != 0:
        print(RESET, Red(f"Exited with error: {code}\n"))
    else:
        print(RESET)


def update_rive():
    subprocess.call(["git", "submodule", "update", "thirdparty/rive-cpp"], cwd="../")


def build_rive(platform: str, target: str):
    print(Bold("\nRunning Rive's build script..."))
    print_list(
        ("Platform", platform if len(platform) else "<auto>"),
        ("Target", target),
    )
    print(f"---{LIGHT_GREY}")
    update_rive()
    args = ["sh", "build.sh"]
    if len(platform):
        args.append("-p")
        args.append(platform)
    args.append(target)
    handle_fail(subprocess.call(args, cwd="../thirdparty/rive-cpp"))


def build_skia_dependencies() -> int:
    if not exists("../thirdparty/rive-cpp/skia/dependencies/skia"):
        return subprocess.call(
            ["sh", "make_dependencies.sh"],
            cwd="../thirdparty/rive-cpp/skia/dependencies",
        )
    else:
        print("Skia is already built!")
        return 0


def build_skia(platform: str, target: str):
    print(Bold("\nRunning Rive's skia build script..."))
    print_list(
        ("Platform", platform if len(platform) else "<auto>"),
        ("Target", target),
    )
    print(f"---{LIGHT_GREY}")
    update_rive()
    code = build_skia_dependencies()
    if code == 0:
        args = ["sh", "build.sh"]
        if len(platform):
            args.append("-p")
            args.append(platform)
        args.append(target)
        code = subprocess.call(args, cwd="../thirdparty/rive-cpp/skia/renderer")
    handle_fail(code)


def build_extension(
    platform: str = "",
    target: str = "",
    arch: str = "",
    unknown: list[str] = [],
):
    args: list[str] = ["scons"]
    if platform == "ios_sim":
        args.append(f"platform=ios")
        args.append(f"ios_simulator=yes")
    elif platform and len(platform):
        args.append(f"platform={platform}")
    if target and len(target):
        args.append(f"target={target}")
    if arch and len(arch):
        args.append(f"arch={arch}")
    args += unknown
    print(Bold("\nBuilding Rive Extension..."))
    print_list(
        ("Platform", platform or "<auto>"),
        ("Architecture", arch or "<auto>"),
        ("Target", target or "template_debug"),
        ("Other arguments", ", ".join(unknown)),
    )
    print(f"---{LIGHT_GREY}")
    code: int = subprocess.call(args)
    handle_fail(code)


# rive platform : godot platform
PLATFORM_MAP = {
    "macosx": "macos",
    "linux": "linux",
    "windows": "windows",
    "ios": "ios",
    "ios_sim": "ios_sim",
    "android": "android",
}

# rive target : godot target
TARGET_MAP = {
    "debug": "template_debug",
    "release": "template_release",
}

ARCHITECTURES = [
    "universal",
    "x86_32",
    "x86_64",
    "arm32",
    "arm64",
    "rv64",
    "ppc32",
    "ppc64",
    "wasm32",
]

EXAMPLES: list[tuple[str, str]] = [
    (
        "python build.py -j10 --platform=macos --target=debug",
        "builds debug for MacOS universal using 10 cores",
    ),
    (
        "python build.py --platform=macos --arch=arm64 --target=release",
        "builds release for M1 MacOS",
    ),
]

DESCRIPTION = [
    "This script builds Rive Extension and its dependencies.",
    "To see a full list of additional options provided by scons, run "
    + Cyan("scons --help"),
    Bold("\nExamples:"),
]

for example, text in EXAMPLES:
    DESCRIPTION.append(Cyan(f"  {example}"))
    DESCRIPTION.append(Dim(f"      {text}"))


parser = argparse.ArgumentParser(
    description="\n".join(DESCRIPTION), formatter_class=RawDescriptionHelpFormatter
)

parser.add_argument(
    "-p",
    "--platform",
    choices=PLATFORM_MAP.keys(),
    required=False,
    help=Dim(
        "Build for a specific platform. If not provided, will be inferred from the current OS."
    ),
)
parser.add_argument(
    "-a",
    "--arch",
    choices=ARCHITECTURES,
    required=False,
    default="",
    help=Dim(
        "Build for a specific CPU architecture. If not provided, will be inferred from the current OS."
    ),
)
parser.add_argument(
    "-c",
    "--clean",
    action="store_true",
    help=Dim("Cleans the build."),
)
parser.add_argument(
    "-t",
    "--target",
    choices=TARGET_MAP.keys(),
    required=False,
    default="debug",
    help=Dim("Build for debug or release. Defaults to debug."),
)

namespace, unknown = parser.parse_known_args()

platform: str = namespace.platform if namespace.platform else ""
target: str = (
    "clean" if namespace.clean else namespace.target if namespace.target else "debug"
)

godot_platform: str = PLATFORM_MAP[platform] if platform in PLATFORM_MAP else ""
godot_target: str = TARGET_MAP[target] if target in TARGET_MAP else "template_debug"

build_rive(platform, target)
build_skia(platform, target)
if target != "clean":
    build_extension(
        platform=godot_platform,
        target=godot_target,
        arch=namespace.arch,
        unknown=unknown,
    )

print(Bold("Build successful!\n"))
