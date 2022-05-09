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
#include "../../lib/load/read.hpp"
#include "../../lib/thermal/read.hpp"
#include "../../lib/storage/factory.hpp"

namespace thermos
{

Logger::Logger(const std::string& fileName, const storage::type fileType)
: file_name(fileName),
  file_type(fileType)
{
}

std::optional<std::string> Logger::log()
{
  #if defined(THERMOS_NO_SQLITE)
  if (file_type == storage::type::db)
  {
    return std::string("This program was built without SQLite features, ")
      + "so db cannot be used as file type.";
  }
  #endif

  auto next = std::chrono::steady_clock::now();

  while (true)
  {
    // Retrieve thermal sensor data.
    const auto thermal_readings = thermal::read_all();
    if (!thermal_readings.has_value())
    {
      return thermal_readings.error();
    }
    const auto& thermal_readings_v = thermal_readings.value();
    if (thermal_readings_v.empty())
    {
      return "No temperature readings are available.";
    }

    // Retrieve CPU load data.
    const auto load_readings = load::read_all();
    if (!load_readings.has_value())
    {
      return load_readings.error();
    }
    const auto& load_readings_v = load_readings.value();
    if (load_readings_v.empty())
    {
      return "No CPU load data is available.";
    }

    // Store retrieved data.
    auto storage = storage::factory::create(file_type);
    auto opt = storage->save(thermal_readings_v, file_name);
    if (opt.has_value())
    {
      return opt;
    }
    opt = storage->save(load_readings_v, file_name);
    if (opt.has_value())
    {
      return opt;
    }

    // Wait before making the next iteration.
    constexpr auto interval = std::chrono::seconds(300);
    next += interval;
    std::this_thread::sleep_until(next);
  }
}

} // namespace
