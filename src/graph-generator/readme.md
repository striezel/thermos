# thermos-graph-generator

`thermos-graph-generator` is a command-line program that creates graphs from the
date logged by `thermos-logger`.

## Usage

```
thermos-graph-generator [OPTIONS]

Generates graphs from logged data.

options:
  -? | --help               - Shows this help message.
  -v | --version            - Shows version information.
  -f FILE | --file FILE     - Sets the file name of the log file to use to
                              generate the graphs. This must be a database
                              file (SQLite) and not a CSV file.
  -t FILE | --template FILE - Sets the file name of the template file to use
                              to generate the graphs.
  -o DIR | --output DIR     - Sets the destination of the generated files to
                              the directory DIR.
```

_Note:_ This program is not completely implemented yet.

## Copyright and Licensing

Copyright 2022, 2025  Dirk Stolle

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
