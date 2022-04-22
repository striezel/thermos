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

#include <iostream>
#include "../lib/read.hpp"

int main()
{
  const auto readings = thermos::read_all();
  if (!readings.has_value() || readings.value().empty())
  {
    std::cout << "No temperature readings available!\n";
    if (!readings.has_value())
      std::cout << readings.error() << std::endl;
    return 0;
  }

  for (const auto& reading: readings.value())
  {
    std::cout << "Device '" << reading.dev.name << "': " << reading.celsius() << " °C\n"
              << "  (from " << reading.dev.origin << ")\n";
  }

  return 0;
}
