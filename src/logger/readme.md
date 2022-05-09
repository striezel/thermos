# thermos-logger

`thermos-logger` is a command-line program that continuously logs the thermal
sensor data to a CSV file or a SQLite 3 database.

## Usage

```
thermos-logger [OPTIONS]

Logs thermal sensor data and CPU load to a CSV or SQLite 3 file.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -f FILE | --file FILE  - Sets the file name of the log file to use during the
                           program run.
  -t TYPE | --type TYPE  - Sets the file type of the log file to use during the
                           program run. Allowed file types are:
                               csv
                               db
                           If the type is 'csv', then the readings are stored
                           as character-separated values (CSV). If the type is
                           'db', then the readings are stored in as SQLite 3
                           database.
                           If no type is given, then 'db' is assumed.
```

Once started the program runs indefinitely and logs new data every five minutes.
The only exception to that is when an error occurs. In that case the program
exits.

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
