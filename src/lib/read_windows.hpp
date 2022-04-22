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

#ifndef THERMOS_READ_WINDOWS_HPP
#define THERMOS_READ_WINDOWS_HPP

#include <string>
#include <vector>
#include "../../third-party/nonstd/expected.hpp"
#include "device.hpp"

#if defined(_WIN32) || defined(_WIN64)
namespace thermos::windows
{

/** \brief Reads all thermal devices.
 *
 * \return Returns a vector containing the device readings, if successful.
 *         Returns an error message, if no readings were available.
 */
nonstd::expected<std::vector<device>, std::string> read_all();

/** \brief Reads thermal data from WMI.
 *
 * \return Returns a vector containing the device readings.
 *         Returns an error message, if no readings were available.
 * \remarks Note that this method requires administrative privileges to get
 *          actual data. Standard user accounts may get errors or an empty
 *          vector instead.
 */
nonstd::expected<std::vector<device>, std::string> read_wmi();

} // namespace
#endif // Windows

#endif // THERMOS_READ_WINDOWS_HPP
