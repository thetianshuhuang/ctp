

import os


# Assemble includes
def get_includes(files):

    includes = []
    for f in files:
        with open(f) as file:
            includes += [
                line for line in file.readlines() if
                line.startswith("#include <")]
    return "".join(list(set(includes)))


def get_defines(files):
    defines = []
    for f in files:
        with open(f) as file:
            defines += [
                line for line in file.readlines() if
                line.startswith("#define")
            ]
    return "".join(defines)


def strip_newlines(lines):
    for idx, line in enumerate(lines):
        if idx < len(lines) - 2:
            if line == '\n' and lines[idx + 1] == '\n' and lines[idx + 2] == '\n':
                lines[idx] = ''


def get(files):

    lines = []

    for f in files:
        if lines != []:
            lines += ["\n\n/*" + "*" * 77 + "*/\n\n"]
        with open(f) as file:
            lines += [
                line for line in file.readlines() if
                not line.startswith("#")]

    strip_newlines(lines)

    return "".join(lines)


if __name__ == "__main__":
    files = [
        f for f in os.listdir(os.getcwd())
        if os.path.isfile(os.path.join(os.getcwd(), f))]
    c_files = [f for f in files if len(f) > 2 and f[-2:] == '.c']
    h_files = [f for f in files if len(f) > 2 and f[-2:] == '.h']
    files = c_files + h_files

    print(r"""/**
 *  ___ _  _  ___ _   _   _ ___  ___ ___
 * |_ _| \| |/ __| | | | | |   \| __/ __|
 *  | || .` | (__| |_| |_| | |) | _|\__ \
 * |___|_|\_|\___|____\___/|___/|___|___/
 */""")
    print(get_includes(files))

    print(r"""/**
 *  ___  ___ ___ ___ _  _ ___ ___
 * |   \| __| __|_ _| \| | __/ __|
 * | |) | _|| _| | || .` | _|\__ \
 * |___/|___|_| |___|_|\_|___|___/
 */""")
    print(get_defines(files))

    print(r"""/**
 *  _  _ ___   _   ___  ___ ___  ___
 * | || | __| /_\ |   \| __| _ \/ __|
 * | __ | _| / _ \| |) | _||   /\__ \
 * |_||_|___/_/ \_\___/|___|_|_\|___/
 */""")
    print(get(h_files))

    print(r"""/**
 *   ___    ___ ___  ___  ___
 *  / __|  / __/ _ \|   \| __|
 * | (__  | (_| (_) | |) | _|
 *  \___|  \___\___/|___/|___|
 */""")
    print(get(c_files))
