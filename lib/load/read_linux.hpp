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

#ifndef THERMOS_READ_LOAD_LINUX_HPP
#define THERMOS_READ_LOAD_LINUX_HPP

#include <string>
#include <vector>
#include "../../third-party/nonstd/expected.hpp"
#include "reading.hpp"

#if defined(__linux__) || defined(linux)
namespace thermos::linux_like::load
{

/** \brief Reads all CPU load data.
 *
 * \return Returns a vector containing the device readings, if successful.
 *         Returns an error message, if no readings were available.
 */
nonstd::expected<std::vector<thermos::load::device_reading>, std::string> read_all();

} // namespace
#endif // Linux

#endif // THERMOS_READ_LOAD_LINUX_HPP
