# Version history of thermos

## Next Version (2022-05-??)

On Linux systems, the information about thermal readings does now contain values
from both `/sys/devices/virtual/thermal` and `/sys/class/hwmon`, not just one of
them.

## Version 0.5.0 (2022-05-13)

`thermos-db2csv`, a command line application that converts logged data of
`thermos-logger` from the SQLite 3 database format into the CSV format, is
added.

A typo in the help text of `thermos-logger` is fixed.

## Version 0.4.0 (2022-05-11)

`thermos-info` and `thermos-logger` show and log information about the CPU load,
too.

CPU load is given in percent. Zero percent means the system is idle. Non-zero
percentages have a different meaning, depending on the operating system. On
Windows systems, a load of 100 % means that all cores are completely busy. On
Linux systems, a load of 100 % means that one core is completely busy. So the
interpretation of load values depends on the number of cores in that case. For
example, imagine a system with four CPU cores. If all four cores are completely
busy, then the CPU load will be 400 % on Linux, but only 100 % on Windows. If
only one core is busy on the same system, then the CPU load will be 100 % on Linux, but only 25 % on Windows.

In other words: CPU load scales with the number of CPU cores on Linux, but it
does not on Windows.

## Version 0.3.0 (2022-04-29)

`thermo-info` is renamed to `thermos-info`.

`thermo-logger` is renamed to `thermos-logger`.

`thermos-logger` can now alternatively write the logged data into a SQLite 3
database. The file format can be set by the new `--type` parameter. While
`--type csv` still writes data to a CSV file, specifying `--type db` will cause
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
