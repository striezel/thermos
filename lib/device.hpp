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

#ifndef THERMOS_DEVICE_HPP
#define THERMOS_DEVICE_HPP

#include <string>

namespace thermos
{

/** A device (e. g. a thermal sensor) that can return temperature data. */
struct device
{
  device();

  /** \brief Checks whether this instance has valid data.
   *
   * \return Returns true, if this instance has valid data.
   * \remarks When this method returns false, then the data shall not be used.
   */
  bool filled() const;

  std::string name;   /**< name of the device */
  std::string origin; /**< origin / identifier of the device */
};

} // namespace

#endif // THERMOS_DEVICE_HPP
