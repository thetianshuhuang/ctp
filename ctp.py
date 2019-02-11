"""
     _                         _ _
    | |_ _ _ __ _ _ _  ____ __(_) |___
    |  _| '_/ _` | ' \(_-< '_ \ | / -_)
     \__|_| \__,_|_||_/__/ .__/_|_\___|
    Transpiler for C    |_|       v1.0

Usage
-----
python transpile.py <target_file> <output_file>

Parameters
----------
target_file:
    Target file to transpile. The transpiler will follow #includes as
    appropriate. If a .h file is included, the corresponding .c file will
    be as well (if it exists).
output_file:
    Target file to save to. If no output file is specified, the output will
    be sent to stdout.

Configuration
-------------
Configuration options are specified in the code.

#define __NAME__ "Project name"
    Project name; is placed at the top of the output file as a large ascii art
    comment.
#define __AUTHOR__ "Author name[s]
    Project author; placed just under the name.
__header__.h
    If any file contains '#include "__header__.h"', the __header__.h file is
    loaded and placed at the top of the output file instead of the __AUTHOR__
    and __NAME__.
"""

from print import *
import os
import sys
import re
from util import *


__HEADER = """
     _                         _ _
    | |_ _ _ __ _ _ _  ____ __(_) |___
    |  _| '_/ _` | ' \(_-< '_ \ | / -_)
     \__|_| \__,_|_||_/__/ .__/_|_\___|
    Transpiler for C    |_|       v1.0

"""
__AUTHOR = "Tianshu Huang"
__HINT = """
Usage
-----
python transpile.py <target_file> <output_file>

Parameters
----------
target_file:
    Target file to transpile. The transpiler will follow #includes as
    appropriate. If a .h file is included, the corresponding .c file will
    be as well (if it exists).
output_file:
    Target file to save to. If no output file is specified, the output will
    be sent to stdout.

Configuration
-------------
Configuration options are specified in the code.

#define __NAME__ "Project name"
    Project name; is placed at the top of the output file as a large ascii art
    comment.
#define __AUTHOR__ "Author name[s]
    Project author; placed just under the name.
__header__.h
    If any file contains '#include "__header__.h"', the __header__.h file is
    loaded and placed at the top of the output file instead of the __AUTHOR__
    and __NAME__.
"""


def show_includes(std, usr):
    """Print found files"""

    print("libc includes [{i}]:".format(i=len(std)), BR + BLUE)
    print("    " + ", ".join(std))
    print("User files [{i}]:".format(i=len(usr)), BR + BLUE)
    print("    " + ", ".join(usr))


def make_title(content):
    """Make title string"""

    author = find_define(content, "__AUTHOR__")
    name = find_define(content, "__NAME__")

    # No name and no author -> no title
    if name is None and author is None:
        return ""
    # Author, but no name -> names only
    elif name is None:
        print("Found author: {a}".format(a=author), BR + BLUE)
        return "/* \n * {a}\n */\n\n".format(a=author)
    # Name, but no author -> treat author as empty string
    elif author is None:
        print('Found project "{n}"'.format(n=name), BR + BLUE)
        author = ""
    # Found author and name
    else:
        print('Found project "{n}":'.format(n=name), BR + BLUE)
        print('    Author: {a}'.format(a=author))

    title = putil.join(
        "/* \n * \n * \n * \n *",
        putil.clear_fmt(render(name, SLANT)))
    return (
        "\n".join(title.split("\n")[:-1]) +
        "\n * " + author + "\n */\n")


if __name__ == "__main__":

    print(__HEADER, RED, BOLD)

    # No args -> print hint and exit
    if len(sys.argv) < 2:
        print(__HINT)
        exit()

    target = sys.argv[1]
    base_dir = os.path.dirname(target)

    # Get file
    if len(sys.argv) >= 3:
        output = open(sys.argv[2], "w")
    else:
        output = sys.stdout

    std, usr = get_includes(target)
    show_includes(std, usr)

    # Handle header
    if '__header__.h' in usr:
        usr.remove('__header__.h')
        with open(os.path.join(base_dir, '__header__.h')) as headerfile:
            title = "".join(headerfile.readlines())
        print("__header__.h file found and loaded.", BR + BLUE)
    else:
        title = None

    content = (
        make_includes(std) +
        make_header(base_dir, usr) +
        make_c(base_dir, usr + [sys.argv[1]]))

    if title is None:
        title = make_title(content)

    output.write(title + content)

    print(
        "Transpiled {i} files to {o}.".format(
            i=len(std) + len(usr),
            o="sys.stdout" if len(sys.argv) < 3 else sys.argv[2]),
        BOLD)

    if len(sys.argv) >= 2:
        output.close()
