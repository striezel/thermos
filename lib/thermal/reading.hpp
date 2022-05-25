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

#ifndef THERMOS_THERMAL_READING_HPP
#define THERMOS_THERMAL_READING_HPP

#include "../device_reading.hpp"
#include "../reading_base.hpp"

namespace thermos::thermal
{

struct reading: public thermos::reading_base
{
  reading();

  /** \brief Gets the temperature in degrees Celsius, possibly rounded.
   *
   * \return Returns the temperature in ° C.
   */
  double celsius() const;

  /** \brief Gets the temperature in degrees Fahrenheit, possibly rounded.
   *
   * \return Returns the temperature in ° F.
   */
  double fahrenheit() const;

  /** \brief Gets the type of the reading, hinting at the implementation.
   *
   * \return Returns the type of the reading.
   */
  virtual reading_type type() const;
};

using device_reading = thermos::device_reading<reading>;

} // namespace

#endif // THERMOS_THERMAL_READING_HPP
