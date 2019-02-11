# CTP
C Transpiler --- for submitting multi-file C projects as one file.

CTP recursively follows #includes to grab all source code referenced by a main file, and transpiles it into a single .c file. Folders can be used by specifying relative filepaths in #includes (i.e. ```#include "module/filename.h"```). .c files are assumed to be associated with a .h file with the same name; .c files without a corresponding .h file that is included somewhere in the dependency tree are ignored.

## Dependencies
- Python 3 (Python 2 is not supported)
- print:
```shell
pip install printtools
```

## Usage
```shell
python ctp.py <target_file> <output_file>
```

## Parameters
- target_file: Target file to transpile. The transpiler will follow #includes as appropriate. If a .h file is included, the corresponding .c file will be as well (if it exists).
- output_file: Target file to save to. If no output file is specified, the output will be sent to stdout.

## Configuration
Configuration options are specified in the code using special #defines.

- ```#define __NAME__ "Project name"```: Project name; is placed at the top of the output file as a large ascii art comment.
- ```#define __AUTHOR__ "Author name[s]```: Project author; placed just under the name.
- ```#include "__header__.h"```, ```__header__.h```: If any file contains ```#include "__header__.h"```, the ```__header__.h``` file is loaded and placed at the top of the output file instead of the ```__AUTHOR__``` and ```__NAME__```.
