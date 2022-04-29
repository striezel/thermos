#!/bin/sh

# Script to test logging failure.
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

# 1st parameter = executable path
if [ -z "$1" ]
then
  echo "First parameter must be executable file!"
  exit 1
fi
EXECUTABLE="$1"

# logging fails
"$EXECUTABLE" --type csv --file /tmp/some/place/not/here/foo.csv
if [ $? -ne 2 ]
then
  echo "Executable did not exit with code 2."
  exit 1
fi

# type is not specified
# (This is not an error, but logging will fail for other reasons (file cannot
# be created), and that shall result in exit code 2.)
"$EXECUTABLE" --file /tmp/some/place/not/here/foo.csv
if [ $? -ne 2 ]
then
  echo "Executable did not exit with code 2."
  exit 1
fi

exit 0
