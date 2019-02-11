
from print import *
import os
import sys
import re


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
python transpile.py <target_file> <output_file> <options>

Parameters
----------
target_file:
    Target file to transpile. The transpiler will follow #includes as
    appropriate. If a .h file is included, the corresponding .c file will
    be as well (if it exists).
output_file:
    Target file to save to. If no output file is specified, the output will
    be sent to stdout.
options:
    See below.

Options
-------
header=<header_file_name>:
    Header to place at the top of the document. This file should contain
    any comment headers, as well as #defines that specify configuration
    options. If no header is specified, __header__.c is used if present.
name=<project_name>:
    Project name; is placed at the top of the file in a comment. Alternatively,
    use
        #define __NAME__ "This is the project name"
    to set the project name
author="<author_name>"
    Project author; placed at the top of the file under the project name. Can
    also be placed in a #define:
        #define __AUTHOR__ "Author Name (Author EID) | Coauthor Name"
"""


def get_file_includes(filename):
    """Fetch includes from a C file (.c or .h)

    Parameters
    ----------
    filename : str
        File to parse

    Returns
    -------
    [str[], str[]]
        [libc includes, user includes]
    """

    std_includes = []
    user_includes = []

    with open(filename) as f:
        includes = [
            line for line in f.readlines()
            if line.startswith("#include")]

        for inc in includes:
            std_includes += re.findall(r'<(.*)>', inc)
            user_includes += re.findall(r'"(.*)"', inc)

    return std_includes, user_includes


def get_includes(base_file):
    """Get includes referenced (recursively) by a base file

    Parameters
    ----------
    base_file : str
        File to follow links from

    Returns
    -------
    [str[], str[]]
        [libc includes, user includes]
    """

    std_includes = []
    user_includes = []
    file_stack = [os.path.basename(base_file)]

    base_dir = os.path.dirname(base_file)

    while len(file_stack) > 0:

        target = file_stack.pop()
        std_add, user_add = get_file_includes(os.path.join(base_dir, target))

        # Handle .c files
        if os.path.isfile(os.path.join(base_dir, target[:-2] + '.c')):
            std_add_c, user_add_c = get_file_includes(
                os.path.join(base_dir, target[:-2] + '.c'))
            std_add += std_add_c
            user_add += user_add_c

        std_includes = list(set(std_includes + std_add))
        for f in user_add:
            if f not in file_stack and f not in user_includes:
                user_includes.append(f)
                file_stack.append(f)

    return std_includes, user_includes


def make_includes(std_includes):
    """Make #include section

    Parameters
    ----------
    std_includes : str[]
        List of clib includes

    Returns
    -------
    str
        clib includes in string form
    """

    # Skip section if no std includes
    if len(std_includes) == 0:
        return ""

    out = "\n\n" + putil.join(
        "/* \n * \n * \n * \n */ ",
        putil.clear_fmt(render("libc includes", SM))) + '\n\n'

    for inc in std_includes:
        out += '#include <{s}>\n'.format(s=inc)

    return out


def get_contents(filelist):
    """Get contents of a list of files, and transpile into a single string

    Parameters
    ----------
    filelist : str[]
        List of files to transpile

    Returns
    -------
    str
        code transpiled into a single string
    """

    content = ""

    for filename in filelist:
        if os.path.isfile(filename):

            if content != "":
                content += "\n\n/*" + "*" * 77 + "*/\n\n"

            with open(filename) as file:
                lines = [
                    i for i in file.readlines()
                    if not i.startswith("#include")
                ]
                content += "".join(lines)
        else:
            print(
                "// [Warning] File {name} does not exist"
                .format(name=filename), BR + YELLOW)

    return content


def make_header(base_dir, user_includes):
    """Compile header section of code

    Parameters
    ----------
    base_dir : str
        Base directory
    user_includes : str[]
        List of user files to include in the header

    Returns
    -------
    str
        Header files transpiled into a single string
    """

    header = get_contents([
        os.path.join(base_dir, h_name) for h_name in user_includes])

    return (
        "\n\n" +
        putil.join(
            "/* \n * \n * \n * \n */ ",
            putil.clear_fmt(render("user headers", SM))) +
        "\n" + header)


def make_c(base_dir, user_includes):
    """Compile main (.c) section of code

    Parameters
    ----------
    base_dir : str
        Base directory
    user_includes : str[]
        List of user files to get corresponding .c files for

    Returns
    -------
    str
        Code files transpiled into a single string
    """

    header = get_contents([
        os.path.join(base_dir, h_name[:-2] + '.c')
        for h_name in user_includes])

    return (
        "\n\n" +
        putil.join(
            "/* \n * \n * \n * \n */ ",
            putil.clear_fmt(render("c code", SM))) +
        "\n" + header)


def find_define(s, dname):

    dvalue = re.findall(r'#define ' + dname + ' "(.*)"\n', s)
    if len(dvalue) == 0:
        dvalue = ""
    else:
        dvalue = dvalue[0]

    return dvalue


if __name__ == "__main__":

    print(__HEADER, RED, BOLD)

    if len(sys.argv) < 2:
        print(__HINT)
        exit()

    target = sys.argv[1]
    base_dir = os.path.dirname(target)

    if len(sys.argv) >= 3:
        output = open(sys.argv[2], "w")
    else:
        output = sys.stdout

    std, usr = get_includes(target)

    print("libc includes [{i}]:".format(i=len(std)))
    print("    " + ", ".join(std))
    print("User files [{i}]:".format(i=len(usr)))
    print("    " + ", ".join(usr))

    content = (
        make_includes(std) +
        make_header(base_dir, usr) +
        make_c(base_dir, usr + [sys.argv[1]]))

    author = find_define(content, "__AUTHOR__")
    name = find_define(content, "__NAME__")

    title = putil.join(
        "/* \n * \n * \n * \n *",
        putil.clear_fmt(render(name, SLANT)))

    content = (
        "\n".join(title.split("\n")[:-1]) +
        "\n * " + author + "\n */\n" + content)

    output.write(content)
    print(
        "Transpiled {i} files to {o}".format(
            i=len(std) + len(usr),
            o="sys.stdout" if len(sys.argv) < 3 else sys.argv[2]),
        BR + GREEN, BOLD)

    if len(sys.argv) >= 2:
        output.close()
