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

#ifndef THERMOS_STORAGE_DB_HPP
#define THERMOS_STORAGE_DB_HPP

#if !defined(THERMOS_NO_SQLITE)
#include "store.hpp"
#include "../../third-party/nonstd/expected.hpp"
#include "../sqlite/database.hpp"

namespace thermos::storage
{

/** \brief Class for storing device readings in a SQLite 3 database.
 */
class db: public store
{
  public:
    /** \brief Saves device readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<device_reading>& data, const std::string& file_name) final;
  private:
    /** \brief Ensures that the tables needed to save information exist.
     *
     * \param db   the database
     * \return Returns an empty optional, if tables existed or were created.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> ensure_tables_exist(sqlite::database& db);

    /** \brief Finds a device in the database or creates it, if it is missing.
     *
     * \param db   the database
     * \param dev  the device to find or to create
     * \return Returns the id of the device used in the database in case of
     *         success. Returns an error message, if an error occurred.
     */
    nonstd::expected<int64_t, std::string> find_or_create_device(sqlite::database& db, const device& dev);

    /** \brief Inserts a new device reading into the database.
     *
     * \param db   the database
     * \param reading  the reading to insert
     * \return Returns an empty optional, if insertion was successful.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> insert_reading(sqlite::database& db, const device_reading& reading);
};

} // namespace

#endif // SQLite feature guard

#endif // THERMOS_STORAGE_DB_HPP
