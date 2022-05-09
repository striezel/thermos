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
    /** \brief Saves thermal device readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<thermos::thermal::reading>& data, const std::string& file_name) final;


    /** \brief Saves CPU load readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<thermos::load::reading>& data, const std::string& file_name) final;
  private:
    /** \brief Ensures that the tables needed to save information exist.
     *
     * \param db   the database
     * \return Returns an empty optional, if tables existed or were created.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> ensure_tables_exist(sqlite::database& db);

    /** \brief Prepares a database file for writing.
     *
     * \param file_name   the file to which the data shall be saved
     * \return Returns an database object, if the database was created successfully.
     *         Returns an error message otherwise.
     */
    nonstd::expected<sqlite::database, std::string> prepare_db(const std::string& file_name);

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

    template<typename T>
    std::optional<std::string> save_impl(const std::vector<T>& data, const std::string& file_name)
    {
      // Open the database.
      auto maybe_db = prepare_db(file_name);
      if (!maybe_db.has_value())
      {
        return maybe_db.error();
      }
      auto& db = maybe_db.value();

      for(const auto& reading: data)
      {
        // Note: Potentially, this loop could be optimized by avoiding to look up
        //       the device ids repeatedly for the same devices. However, currently
        //       the code does not use multiple readings for the same device in one
        //       call to the save() method yet, so we should still be fine.
        auto insert = insert_reading(db, reading);
        if (insert.has_value())
        {
          return insert;
        }
      }

      return std::nullopt;
    }
};

} // namespace

#endif // SQLite feature guard

#endif // THERMOS_STORAGE_DB_HPP
