/*
 -------------------------------------------------------------------------------
    This file is part of thermos, with earlier code from botvinnik Matrix bot.
    Copyright (C) 2020, 2022  Dirk Stolle

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

#ifndef THERMOS_SQLITE3_STATEMENT_HPP
#define THERMOS_SQLITE3_STATEMENT_HPP

#include <memory>
#include <string>
#include <sqlite3.h>

namespace thermos::sqlite
{

/// Forward declaration for use in friend declaration.
class database;

class statement
{
  public:
    /** \brief Binds a string parameter to a prepared statement.
     *
     * \param index  index of the parameter to bind (first parameter has index 1)
     * \param value  the value of the parameter
     * \return Returns whether the binding was successful.
     */
    bool bind(const int index, const std::string& value);


    /** \brief Binds an integer parameter to a prepared statement.
     *
     * \param index  index of the parameter to bind (first parameter has index 1)
     * \param value  the value of the parameter
     * \return Returns whether the binding was successful.
     */
    bool bind(const int index, const int64_t value);

    /** \brief Gets the internal pointer.
     *
     * \return Returns the internal pointer for the prepared statement.
     */
    sqlite3_stmt* ptr() const;
  private:
    friend database;
    statement(std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>&& s);

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt;
};

}

#endif // THERMOS_SQLITE3_STATEMENT_HPP
