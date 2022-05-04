/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#ifndef THERMOS_STORAGE_UTILITIES_HPP
#define THERMOS_STORAGE_UTILITIES_HPP

#include "../../third-party/nonstd/expected.hpp"
#include "../thermal/reading.hpp"

namespace thermos::storage
{

/** \brief Translates a time point to readable format 'YYYY-MM-DD HH:ii:ss'.
 *
 * \param date_time  the time point to transform to string
 * \return Returns a string representing the time point. It is similar to SQL
 *         dates, e. g. "2020-05-25 13:37:00" could be a return value.
 *         If transformation fails, then an error message is returned.
 */
nonstd::expected<std::string, std::string> time_to_string(const thermal::reading::reading_time_t& date_time);

} // namespace

#endif // THERMOS_STORAGE_UTILITIES_HPP
