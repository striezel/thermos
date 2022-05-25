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

#include "read_linux.hpp"
#if defined(__linux__) || defined(linux)
#include <filesystem>
#include <fstream>

namespace thermos::linux_like::load
{

nonstd::expected<std::vector<thermos::load::device_reading>, std::string> read_proc_loadavg()
{
  const std::filesystem::path loadavg ("/proc/loadavg");
  std::error_code error;
  if (!std::filesystem::exists(loadavg, error) || error)
    return nonstd::make_unexpected("File " + loadavg.string() + " does not exist.");

  std::ifstream stream(loadavg);
  double load = -1.0;
  if (!(stream >> load))
  {
    return nonstd::make_unexpected("Error while reading load average (1 min) from /proc/loadavg.");
  }
  const auto now = std::chrono::system_clock::now();
  std::vector<thermos::load::device_reading> result;
  thermos::load::device_reading data;
  data.dev.name = "loadavg1";
  data.dev.origin = "/proc/loadavg_1";
  data.reading.time = now;
  data.reading.value = static_cast<int64_t>(load * 100.0);
  result.push_back(data);
  if (!(stream >> load))
  {
    return nonstd::make_unexpected("Error while reading load average (5 min) from /proc/loadavg.");
  }
  data.dev.name = "loadavg5";
  data.dev.origin = "/proc/loadavg_5";
  data.reading.time = now;
  data.reading.value = static_cast<int64_t>(load * 100.0);
  result.push_back(data);
  if (!(stream >> load))
  {
    return nonstd::make_unexpected("Error while reading load average (15 min) from /proc/loadavg.");
  }
  data.dev.name = "loadavg15";
  data.dev.origin = "/proc/loadavg_15";
  data.reading.time = now;
  data.reading.value = static_cast<int64_t>(load * 100.0);
  result.push_back(data);

  return result;
}

nonstd::expected<std::vector<thermos::load::device_reading>, std::string> read_all()
{
  return read_proc_loadavg();
}

} // namespace

#endif // Linux
