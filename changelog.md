# Version history of thermos

## Version 0.3.0 (2022-04-29)

`thermo-info` is renamed to `thermos-info`.

`thermo-logger` is renamed to `thermos-logger`.

`thermos-logger` can now alternatively write the logged data into a SQLite 3
database. The file format can be set by the new `--type` parameter. While
`--type csv` still writes data to a CSV file, specifying '--type db` will cause
the program to write the data as SQLite 3 database instead.

If no type is given, then the file is assumed to be a SQLite 3 database.

## Version 0.2.0 (2022-04-24)

`thermo-logger`, a command line application that continuously logs thermal
sensor data to a file, is introduced.

## Version 0.1.1 (2022-04-24)

The file name matching algorithm for reading thermal devices from
`/sys/class/hwmon/` on Linux systems has been improved, eliminating some false
positives.

## Version 0.1.0 (2022-04-23)

`thermo-info`, a command line application that just shows the current thermal
sensor readings when invoked, is introduced.
