# thermo-info

`thermo-info` is a command-line program that show the current thermal sensor
data.

## Usage

```
thermo-info [OPTIONS]

Reads and displays thermal sensor data.

options:
  -? | --help       - Shows this help message.
  -v | --version    - Shows version information.
```

A possible output could be:

```
Device 'pch_skylake': 57.5 °C
  (from /sys/devices/virtual/thermal/thermal_zone2/temp)
Device 'acpitz': 46 °C
  (from /sys/devices/virtual/thermal/thermal_zone0/temp)
Device 'B0D4': 46 °C
  (from /sys/devices/virtual/thermal/thermal_zone3/temp)
Device 'INT3400 Thermal': 20 °C
  (from /sys/devices/virtual/thermal/thermal_zone1/temp)
Device 'x86_pkg_temp': 42 °C
  (from /sys/devices/virtual/thermal/thermal_zone4/temp)
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
