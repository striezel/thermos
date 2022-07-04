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

nonstd::expected<std::string, std::string> time_to_string(const thermal::reading::reading_time_t& date_time)
{
  const std::time_t tt = std::chrono::system_clock::to_time_t(date_time);
  struct tm tm;
  #if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__MINGW64__)
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

nonstd::expected<thermal::reading::reading_time_t, std::string> string_to_time(const std::string& value)
{
  // Value is something like '2020-04-04 12:34:56', so length is 19 chars.
  if (value.size() != 19 || value[4] != '-' || value[7] != '-' || value[13] != ':' || value[16] != ':')
  {
    return nonstd::make_unexpected("The given string is not a valid date/time, it must follow the pattern 'YYYY-MM-DD hh:mm:ss'.");
  }

  std::size_t pos;
  const int year = std::stoi(value, &pos);
  if (pos != 4 || year < 0)
  {
    return nonstd::make_unexpected(value + " is not a valid date/time. Maybe '" + value.substr(0, pos) + "' is not a valid year.");
  }
  const int month = std::stoi(value.substr(5, 2), &pos);
  if (pos != 2 || month > 12 || month < 1)
  {
    return nonstd::make_unexpected(value + " is not a valid date/time. Maybe '" + value.substr(5, 2) + "' is not a valid month.");
  }
  const int day = std::stoi(value.substr(8, 2), &pos);
  if (pos != 2 || day < 1 || day > 31)
  {
    return nonstd::make_unexpected(value + " is not a valid date/time. Maybe '" + value.substr(8, 2) + "' is not a valid day.");
  }

  const int hour = std::stoi(value.substr(11, 2), &pos);
  if (pos != 2 || hour < 0 || hour > 23)
  {
    return nonstd::make_unexpected(value + " is not a valid date/time. Maybe '" + value.substr(11, 2) + "' is not a valid hour.");
  }
  const int minute = std::stoi(value.substr(14, 2), &pos);
  if (pos != 2 || minute < 0 || minute > 59)
  {
    return nonstd::make_unexpected(value + " is not a valid date/time. Maybe '" + value.substr(14, 2) + "' is not a valid minute.");
  }
  const int second = std::stoi(value.substr(17, 2), &pos);
  if (pos != 2 || second < 0 || second > 59)
  {
    return nonstd::make_unexpected(value + " is not a valid date/time. Maybe '" + value.substr(17, 2) + "' is not a valid second.");
  }

  struct tm tm;
  tm.tm_sec = second;
  tm.tm_min = minute;
  tm.tm_hour = hour;
  tm.tm_mday = day;
  tm.tm_mon = month - 1;
  tm.tm_year = year - 1900;
  tm.tm_isdst = -1;
  const time_t tt = mktime(&tm);
  if (tt == static_cast<time_t>(-1))
  {
    return nonstd::make_unexpected("mktime() failed when converting " + value + " to time_t.");
  }
  return std::chrono::system_clock::from_time_t(tt);
}

} // namespace
