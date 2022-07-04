#!/bin/sh

# Script to create builds without SQLite support.
#
#  Copyright (C) 2022  Dirk Stolle
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

if [ ! -d build-no-sqlite ]
then
  mkdir build-no-sqlite
  if [ $? -ne 0 ]
  then
    echo "Failed to create build directory!"
    exit 1
  fi
fi

cd build-no-sqlite || exit 1

cmake .. -DNO_SQLITE=ON
if [ $? -ne 0 ]
then
  echo "Build error: CMake run failed."
  exit 1
fi

cmake --build . -j4
if [ $? -ne 0 ]
then
  echo "Build failed."
  exit 1
fi

ctest -V
