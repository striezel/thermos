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

#include "read_windows.hpp"
#include "calculator.hpp"
#if defined(_WIN32) || defined(_WIN64)

namespace thermos::windows::load
{

nonstd::expected<std::vector<thermos::load::device_reading>, std::string> read_all()
{
  static thermos::load::calculator calc;

  constexpr std::chrono::milliseconds wait{250};
  auto single_result = calc.fresh() ? calc.current(wait) : calc.current();
  if (!single_result.has_value())
  {
    return nonstd::make_unexpected(single_result.error());
  }

  std::vector<thermos::load::device_reading> vec;
  vec.push_back(single_result.value());
  return vec;
}

} // namespace

#endif // Windows
