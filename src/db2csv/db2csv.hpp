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

#ifndef THERMOS_DB2CSV_HPP
#define THERMOS_DB2CSV_HPP

#include <string>

namespace thermos
{

/** \brief Writes data from an SQLite 3 file to a CSV file.
 *
 * \param db_path   path to the database file
 * \return Returns zero, if operation was successful.
 *         Returns non-zero exit code, if an error occurred.
 */
int db2csv(const std::string& db_path);

/** \brief Generates a file name for the CSV file.
 *
 * \param db_path   the database path
 * \return Returns a suitable file name for the CSV file.
 */
std::string csv_name(const std::string& db_path);

} // namespace

#endif // THERMOS_DB2CSV_HPP
