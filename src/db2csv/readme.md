# thermos-db2csv

`thermos-db2csv` is a command-line program that reads data from a SQLite 3
database that was created by [`thermos-logger`](../logger/readme.md) and writes
that data to a CSV file. In other words: It converts logged data from the
database format to the CSV format.

## Usage

```
thermos-db2csv [OPTIONS]

Writes thermal sensor data and CPU load from a SQLite 3 file to a CSV file.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -f FILE | --file FILE  - Sets the file name of the SQLite 3 database to read.
```

The name of the output file (CSV) is determined based in the input file, i. e.
it will be created in the same directory, but with the file extension `.csv`.

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
