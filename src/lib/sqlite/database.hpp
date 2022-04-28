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

#ifndef THERMOS_SQLITE3_DATABASE_HPP
#define THERMOS_SQLITE3_DATABASE_HPP

#include <memory>
#include <string>
#include <sqlite3.h>
#include "../../../third-party/nonstd/expected.hpp"
#include "statement.hpp"

namespace thermos::sqlite
{

class database
{
  public:
    /** \brief Opens a new SQLite 3 database connection.
     *
     * If a file with the given name already exists, it will be opened.
     * If no such file exists, it will be created.
     * \param fileName   name of the database file to open / create
     * \return Returns database connection.
     *         If no database could be opened / created, an error message is returned.
     */
    static nonstd::expected<database, std::string> open(const std::string& fileName);


    /** \brief Executes an SQL statement.
     *
     * \param sql   SQL statement to execute
     * \return Returns true, if execution of the statement was successful.
     *         Returns false otherwise.
     */
    bool exec(const std::string& sql);

    /** \brief Escapes a string and surrounds it with quotes for use in an SQL statement.
     *
     * \param str  the string to escape
     * \return Returns the properly escaped and quoted string.
     */
    static std::string quote(const std::string& str);


    /** \brief Creates a prepared statement.
     *
     * \param sqlStmt   SQL for the prepared statement
     * \return Returns the prepared statement.
     *         If no statement could be prepared, an error message is returned.
     */
    nonstd::expected<statement, std::string> prepare(const std::string& sqlStmt);

    /** \brief Gets the row id of the latest successful insert operation.
     *
     * \return Returns the row id of the latest successful insert operation.
     *         Returns zero, if no insert operation took place yet.
     */
    int64_t last_insert_id() const;

    /** \brief Checks whether a certain table exists.
     *
     * \param table   name of the table
     * \return Returns whether the table exists.
     *         If an error occurred, an message is returned instead.
     */
    nonstd::expected<bool, std::string> table_exists(const std::string& table);
  private:
    database(std::unique_ptr<sqlite3, decltype(&sqlite3_close)>&& the_handle);

    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> handle;
};

}

#endif // THERMOS_SQLITE3_DATABASE_HPP
