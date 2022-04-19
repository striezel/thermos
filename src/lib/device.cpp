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

#include "device.hpp"
#include <cmath>
#include <limits>

namespace thermos
{

device::device()
: type(std::string()),
  millicelsius(std::numeric_limits<decltype(millicelsius)>::min())
{
}

bool device::filled() const
{
  return !type.empty() && (millicelsius != std::numeric_limits<decltype(millicelsius)>::min());
}

double device::celsius() const
{
  // Rounded to 1/100th degree Celsius.
  return std::round(static_cast<double>(millicelsius) / 10.0) / 100.0;
}

} // namespace
