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

#ifndef THERMOS_DEVICE_READING_HPP
#define THERMOS_DEVICE_READING_HPP

#include <chrono>
#include <cstdint>
#include <limits>
#include "device.hpp"
#include "reading_base.hpp"

namespace thermos
{

template<typename reading_t>
struct device_reading
{
  device_reading()
  : dev(device()),
    reading(reading_t())
  {
  }

  ~device_reading() = default;

  /** \brief Checks whether this instance has valid data.
   *
   * \return Returns true, if this instance has valid data.
   * \remarks When this method returns false, then the data shall not be used.
   */
  bool filled() const
  {
    return dev.filled()
      && (reading.value != std::numeric_limits<decltype(reading.value)>::min())
      && (reading.time != reading_base::reading_time_t());
  }


  device dev; /**< device from which the reading was obtained */
  reading_t reading; /**< value and time of the reading */
};

} // namespace

#endif // THERMOS_DEVICE_READING_HPP
