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

#include "vectorize.hpp"
#include <sstream>
#include "../storage/utilities.hpp"

namespace thermos
{

vectorized_data::vectorized_data()
: dates(std::string()),
  values(std::string())
{
}

nonstd::expected<vectorized_data, std::string> vectorize(const std::vector<load::reading>& data)
{
  vectorized_data result;
  std::ostringstream dates;
  dates << "[\"";
  std::ostringstream values;
  values << "[";
  for (const load::reading& elem: data)
  {
    const auto maybe_string = storage::time_to_string(elem.time);
    if (!maybe_string.has_value())
    {
      return nonstd::make_unexpected(maybe_string.error());
    }
    dates << maybe_string.value() << "\",\"";
    values << elem.percent() << ',';
  }
  if (!data.empty())
  {
    result.dates = std::move(dates.str());
    const auto d_len = result.dates.length();
    result.dates[d_len - 2] = ']';
    result.dates.erase(d_len - 1, 1);
    result.values = std::move(values.str());
    result.values[result.values.length() - 1] = ']';
  }
  else
  {
    result.dates = result.values = "[]";
  }

  return result;
}

nonstd::expected<vectorized_data, std::string> vectorize(const std::vector<thermal::reading>& data)
{
  vectorized_data result;
  std::ostringstream dates;
  dates << "[\"";
  std::ostringstream values;
  values << "[";
  for (const thermal::reading& elem: data)
  {
    const auto maybe_string = storage::time_to_string(elem.time);
    if (!maybe_string.has_value())
    {
      return nonstd::make_unexpected(maybe_string.error());
    }
    dates << maybe_string.value() << "\",\"";
    values << elem.celsius() << ',';
  }
  if (!data.empty())
  {
    result.dates = std::move(dates.str());
    const auto d_len = result.dates.length();
    result.dates[d_len - 2] = ']';
    result.dates.erase(d_len - 1, 1);
    result.values = std::move(values.str());
    result.values[result.values.length() - 1] = ']';
  }
  else
  {
    result.dates = result.values = "[]";
  }

  return result;
}

} // namespace
