/*
 -------------------------------------------------------------------------------
    This file is part of thermos.
    Copyright (C) 2022  Dirk Stolle

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
 -------------------------------------------------------------------------------
*/

#include "device_reading.hpp"
#include <cmath>
#include <limits>

namespace thermos
{

device_reading::device_reading()
: dev(device()),
  millicelsius(std::numeric_limits<decltype(millicelsius)>::min()),
  time(reading_time_t())
{
}

bool device_reading::filled() const
{
  return dev.filled()
      && (millicelsius != std::numeric_limits<decltype(millicelsius)>::min())
      && (time != reading_time_t());
}

double device_reading::celsius() const
{
  // Rounded to 1/100th degree Celsius.
  return std::round(static_cast<double>(millicelsius) / 10.0) / 100.0;
}

double device_reading::fahrenheit() const
{
  // Convert from millicelsius to Fahrenheit.
  const double f = static_cast<double>(millicelsius) * 0.0018 + 32.0;
  // Rounded to 1/100th degree Fahrenheit.
  return std::round(f * 100.0) / 100.0;
}

} // namespace
