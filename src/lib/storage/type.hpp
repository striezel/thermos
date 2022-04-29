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

#ifndef THERMOS_STORAGE_TYPE_HPP
#define THERMOS_STORAGE_TYPE_HPP

#include <optional>
#include <ostream>
#include <string>

namespace thermos::storage
{

/// Enumeration to indicate the type of storage for device readings.
enum class type
{
  /// Store readings as CSV file.
  csv,

  /// Store readings in SQLite 3 database.
  db
};

/** \brief Converts a string value into the corresponding type.
 *
 * \param str   the string value, e. g. "csv" or "db"
 * \return Returns an optional containing the matching type on success.
 *         Returns an empty optional, if string did not match.
 */
std::optional<type> from_string(const std::string& str);

/** \brief Writes the value of a type enumeration to an output stream.
 *
 * \param os   the stream to write to
 * \param t    the enumeration value
 * \return Returns a reference to the stream.
 */
std::ostream& operator<<(std::ostream& os, const type& t);

} // namespace

#endif // THERMOS_STORAGE_TYPE_HPP
