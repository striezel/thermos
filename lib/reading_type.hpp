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

#ifndef THERMOS_READING_TYPE_HPP
#define THERMOS_READING_TYPE_HPP

#include <ostream>
#include <string>

namespace thermos
{

enum class reading_type
{
  /// temperature of CPU
  temperature,

  /// CPU load
  load
};


/** \brief Converts a reading_type to a string.
 *
 * \param t   the reading_type
 * \return Returns a string that identifies the reading_type.
 */
std::string to_string(const reading_type& t);


/** \brief Writes the value of a reading_type to an output stream.
 *
 * \param os   the stream to write to
 * \param t    the enumeration value to write
 * \return Returns a reference to the stream.
 */
std::ostream& operator<<(std::ostream& os, const reading_type& t);

} // namespace

#endif // THERMOS_READING_TYPE_HPP
