# thermos - tools to get the system temperature

[![GitHub CI Clang status](https://github.com/striezel/thermos/workflows/Clang/badge.svg)](https://github.com/striezel/thermos/actions)
[![GitHub CI GCC status](https://github.com/striezel/thermos/workflows/GCC/badge.svg)](https://github.com/striezel/thermos/actions)
[![GitLab pipeline status](https://gitlab.com/striezel/thermos/badges/main/pipeline.svg)](https://gitlab.com/striezel/thermos/)

This repository contains code to read the thermal sensors of a system.

_Note:_ This code is still in an early stage of development. So far it has only
very few features.

## Building from source

### Prerequisites

To build thermos from source you need a C++ compiler with support for C++17,
CMake 3.8 or later.

It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All of that can usually be installed be typing

```bash
# on Debian, Ubuntu, etc.
apt-get install cmake g++ git
```

or

```bash
# on Fedora, etc.
yum install cmake gcc-c++ git
```

into a root terminal.

### Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

    git clone https://gitlab.com/striezel/thermos.git
    cd thermos

That's it, you should now have the current source code on your machine.

### Build process

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

```bash
mkdir build
cd build
cmake ../
make -j4
```

## Copyright and Licensing

Copyright 2022  Dirk Stolle

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
