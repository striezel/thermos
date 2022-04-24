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

#include "utilities.hpp"
#include <ctime>

namespace thermos::storage
{

nonstd::expected<std::string, std::string> time_to_string(const device_reading::reading_time_t& date_time)
{
  const std::time_t tt = std::chrono::system_clock::to_time_t(date_time);
  struct tm tm;
  #if !defined(_MSC_VER)
  // Note: localtime() is NOT thread-safe. Therefore we use localtime_r(), which
  // is thread-safe but may not be available on all platforms or compilers.
  struct tm* ptr = localtime_r(&tt, &tm);
  if (ptr == nullptr)
  {
    return nonstd::make_unexpected("Date conversion with localtime_r() failed!");
  }
  #else
  // MSVC does not have localtime_r, so we use localtime_s instead.
  const errno_t error = localtime_s(&tm, &tt);
  if (error != 0)
  {
    return nonstd::make_unexpected("Date conversion with localtime_s() failed!");
  }
  #endif
  const int realYear = tm.tm_year + 1900;
  const int realMonth = tm.tm_mon + 1;
  return // zero-padding for year
         std::string((realYear < 1000) + (realYear < 100) + (realYear < 10), '0')
         // year itself
         .append(std::to_string(realYear))
         .append("-")
         // month + padding
         .append(realMonth < 10, '0').append(std::to_string(realMonth))
         .append("-")
         // day + padding
         .append(tm.tm_mday < 10, '0').append(std::to_string(tm.tm_mday))
         .append(" ")
         // hour + padding
         .append(tm.tm_hour < 10, '0').append(std::to_string(tm.tm_hour))
         .append(":")
         // minute + padding
         .append(tm.tm_min < 10, '0').append(std::to_string(tm.tm_min))
         .append(":")
         // second
         .append(tm.tm_sec < 10, '0').append(std::to_string(tm.tm_sec));
}

} // namespace
