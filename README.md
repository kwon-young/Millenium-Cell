# Millenium-Cell

[![Travis Build Status](https://travis-ci.org/kwon-young/Millenium-Cell.svg)](https://travis-ci.org/kwon-young/Millenium-Cell)

# What is Millenium-Cell

Millenium-Cell is a cell morphogenesis simulation. It's a student project.

# Build instruction

Make sure you have installed all the dependencies :

* CMake
* Boost
* VTK

Additionnal dependency if you are building on ArchLinux :

* HDF5
* ODBC
* OpenMPI
* GDAL
* LibPQ

There may be other dependencies. If you have build error with undefined reference, search the web for the missing libraries and install them. Please add them to the list of dependencies above.

```shell
git clone https://github.com/kwon-young/Millenium-Cell.git
cd Millenium-Cell
mkdir build
cd build
cmake ..
make
./Millenium-Cell
```

* Coding style

The coding style is define in the `.clang-format`. Make sure to use `clang-format` command or use `git clang-format` if available before each commit. Moreover, It's a good idea to set it as a pre-commit action in `.git/hooks/pre-commit` as below. Don't forget to set it executable.

```python
#!/usr/bin/env python

import subprocess
output = subprocess.check_output(["git", "clang-format", "--diff"])

if output not in ['no modified files to format\n', 'clang-format did not modify any files\n']:
    print("Run git clang-format, then commit.\n")
    exit(1)
else:
    exit(0)
```

# TODO

* What reactive to use
* How to code a reaction
* How to represent reactive concentration (global vs local)
* How to model an energetic system
