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

#include "calculator.hpp"
#if defined(_WIN32) || defined(_WIN64)
#include <thread>
#include <windows.h>
#pragma comment(lib, "kernel32.lib")

namespace thermos::load
{

calculator::calculator()
: previous_idle(0),
  previous_total(0)
{
}

bool calculator::fresh() const
{
  return (previous_idle == 0) && (previous_total == 0);
}

std::uint64_t file_time_to_u64(const FILETIME& ft)
{
  return (static_cast<std::uint64_t>(ft.dwHighDateTime) << 32)
        | static_cast<std::uint64_t>(ft.dwLowDateTime);
}

nonstd::expected<thermos::load::device_reading, std::string> calculator::current()
{
  FILETIME idle_time, kernel_time, user_time;
  if (GetSystemTimes(&idle_time, &kernel_time, &user_time) == 0)
  {
    return nonstd::make_unexpected("Failed to get system timing information.");
  }

  const auto now = std::chrono::system_clock::now();

  // Kernel time also includes idle time, so the total time is kernel + user time.
  const std::uint64_t total = file_time_to_u64(kernel_time) + file_time_to_u64(user_time);
  const std::uint64_t idle = file_time_to_u64(idle_time);

  const std::uint64_t total_delta = total - previous_total;
  const std::uint64_t idle_delta = idle - previous_idle;

  thermos::load::device_reading result;
  result.dev.name = "load";
  result.dev.origin = "GetSystemTimes";
  result.reading.time = now;

  if (total_delta != 0)
  {
    const double active = 1.0 - static_cast<double>(idle_delta) / static_cast<double>(total_delta);
    result.reading.value = static_cast<decltype(result.reading.value)>(active * 100.0);
  }
  else
  {
    result.reading.value = 0;
  }

  previous_idle = idle;
  previous_total = total;

  return result;
}

nonstd::expected<thermos::load::device_reading, std::string> calculator::current(const std::chrono::milliseconds ms)
{
  const auto one = current();
  if (!one.has_value())
  {
    return nonstd::make_unexpected(one.error());
  }
  std::this_thread::sleep_for(ms);
  return current();
}


} // namespace

#endif // Windows
