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

#ifndef THERMOS_READING_BASE_HPP
#define THERMOS_READING_BASE_HPP

#include <chrono>
#include <cstdint>
#include "reading_type.hpp"

namespace thermos
{

struct reading_base
{
  reading_base();
  virtual ~reading_base() = default;

  /// shorthand for time point type
  using reading_time_t = std::chrono::time_point<std::chrono::system_clock>;


  /** \brief Gets the type of the reading, hinting at the implementation.
   *
   * \return Returns the type of the reading.
   */
  virtual reading_type type() const = 0;

  int64_t value; /**< value of the reading; meaning depends on type */
  reading_time_t time; /**< time of the reading */
};

} // namespace

#endif // THERMOS_READING_BASE_HPP
