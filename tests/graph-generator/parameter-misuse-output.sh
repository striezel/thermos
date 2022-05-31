#!/bin/sh

# Script to test wrong values of parameter `--output`.
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
# 2nd parameter = HTML file path
if [ -z "$2" ]
then
  echo "Second parameter must be HTML file!"
  exit 1
fi
HTML_FILE="$2"
if [ ! -f "$HTML_FILE" ]
then
  echo "HTML file does not exist or is not a file!"
  exit 1
fi

# multiple occurrences of parameter
"$EXECUTABLE" --output /tmp/foo.html --output /tmp/foo.html --template /tmp/foo.tpl --file /tmp/foo.db
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1."
  exit 1
fi

# missing file name after `--output`
"$EXECUTABLE" --file /tmp/foo.db --template /tmp/foo.tpl --output
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1."
  exit 1
fi

# `--output` not specified
"$EXECUTABLE" --file /tmp/foo.db --template /tmp/foo.tpl
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1."
  exit 1
fi

# output file already exists
"$EXECUTABLE" --output "$HTML_FILE" --template /tmp/bar.tpl --file /tmp/foo.db
if [ $? -ne 3 ]
then
  echo "Executable did not exit with code 3."
  exit 1
fi

exit 0
