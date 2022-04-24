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

#include "Logger.hpp"
#include <thread>
#include "../lib/read.hpp"
#include "../lib/storage/csv.hpp"

namespace thermos
{

Logger::Logger(const std::string& fileName)
: file_name(fileName)
{
}

std::optional<std::string> Logger::log()
{
  while (true)
  {
    // Retrieve sensor data.
    const auto readings = read_all();
    if (!readings.has_value())
    {
      return readings.error();
    }
    const auto& readings_v = readings.value();
    if (readings_v.empty())
    {
      return "No temperature readings are available.";
    }

    // Store retrieved data.
    storage::csv storage;
    const auto opt = storage.save(readings_v, file_name);
    if (opt.has_value())
    {
      return opt;
    }

    // Wait before making the next iteration.
    constexpr auto interval = std::chrono::seconds(300);
    std::this_thread::sleep_for(interval);
  }
}

} // namespace
