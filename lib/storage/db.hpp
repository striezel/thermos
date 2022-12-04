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
#include <cmath>
#include "retrieve.hpp"
#include "store.hpp"
#include "../../third-party/nonstd/expected.hpp"
#include "../sqlite/database.hpp"
#include "utilities.hpp"

namespace thermos::storage
{

/** \brief Class for storing device readings in a SQLite 3 database.
 */
class db: public store, public retrieve
{
  public:
    /** \brief Saves thermal device readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<thermos::thermal::device_reading>& data, const std::string& file_name) final;


    /** \brief Saves CPU load readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<thermos::load::device_reading>& data, const std::string& file_name) final;


    /** \brief Loads device readings from a file.
     *
     * \param data        the vector where the device readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> load(std::vector<thermos::thermal::device_reading>& data, const std::string& file_name) final;


    /** \brief Loads CPU load readings from a file.
     *
     * \param data        the vector where the device readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> load(std::vector<thermos::load::device_reading>& data, const std::string& file_name) final;


    /** \brief Loads all available devices (NOT their readings) from a file.
     *
     * \param data        the vector where the devices shall be stored
     * \param type        type of the device's readings (thermal or load data)
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> get_devices(std::vector<thermos::device>& data, const thermos::reading_type type, const std::string& file_name) final;


    /** \brief Gets the internal id of a device from a file.
     *
     * \param dev         the device for which the id shall be retrieved
     * \param file_name   the database file from which the data shall be loaded
     * \return Returns the id, if the database contains a matching device.
     *         Returns zero, if the database does not contain a matching device.
     *         (Zero is not used as a valid device id.)
     *         Returns an error message otherwise.
     */
    nonstd::expected<int64_t, std::string> get_device_id(const thermos::device& dev, const std::string& file_name);


    /** \brief Loads readings of a devices from a file.
     *
     * \param dev         the device for which the readings shall be retrieved
     * \param data        the vector where the readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \param time_span   the time span from which the data shall be included;
     *                    Settings this to e. g. two hours will retrieve the
     *                    data from the latest time up to two hours back.
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> get_device_readings(const thermos::device& dev, std::vector<load::reading>& data, const std::string& file_name, const std::chrono::hours time_span);
    std::optional<std::string> get_device_readings(const thermos::device& dev, std::vector<thermal::reading>& data, const std::string& file_name, const std::chrono::hours time_span);
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
    template<typename T>
    std::optional<std::string> insert_reading(sqlite::database& sql_db, const device_reading<T>& reading)
    {
      const auto dev_id = find_or_create_device(sql_db, reading.dev);
      if (!dev_id.has_value())
      {
        return dev_id.error();
      }
      const auto time_string = time_to_string(reading.reading.time);
      if (!time_string.has_value())
      {
        return time_string.error();
      }

      const std::string sql = "INSERT INTO reading (deviceId, type, date, value) VALUES ("
          + std::to_string(dev_id.value()) + ", '" + to_string(reading.reading.type())
          + "', '" + time_string.value() + "', " + std::to_string(reading.reading.value)
          + ");";
      if (!sql_db.exec(sql))
      {
        return "Could not insert new device reading into database!";
      }

      // Insertion was successful.
      return std::nullopt;
    }

    template<typename T>
    std::optional<std::string> save_impl(const std::vector<T>& data, const std::string& file_name)
    {
      // Open the database.
      auto maybe_db = prepare_db(file_name);
      if (!maybe_db.has_value())
      {
        return maybe_db.error();
      }
      auto& dbase = maybe_db.value();

      for(const auto& reading: data)
      {
        // Note: Potentially, this loop could be optimized by avoiding to look up
        //       the device ids repeatedly for the same devices. However, currently
        //       the code does not use multiple readings for the same device in one
        //       call to the save() method yet, so we should still be fine.
        auto insert = insert_reading(dbase, reading);
        if (insert.has_value())
        {
          return insert;
        }
      }

      return std::nullopt;
    }

    template<typename T>
    std::optional<std::string> load_impl(std::vector<T>& data, const std::string& file_name)
    {
      // Open the database.
      auto maybe_db = sqlite::database::open(file_name);
      if (!maybe_db.has_value())
      {
        return maybe_db.error();
      }
      auto& dbase = maybe_db.value();

      auto maybe_stmt = dbase.prepare("SELECT name, origin, reading.deviceId, date, value FROM reading JOIN device ON reading.deviceId=device.deviceId WHERE type=@t ORDER BY reading.deviceId ASC, date ASC;");
      if (!maybe_stmt.has_value())
      {
        return maybe_stmt.error();
      }
      auto& stmt = maybe_stmt.value();

      T dr;
      if (!stmt.bind(1, to_string(dr.reading.type())))
      {
        return "Failed to bind reading type to prepared statement.";
      }

      int last_device_id = -1;
      dr.dev.name = "uninitialized";
      dr.dev.origin = "uninitialized";
      int rc = -1;
      while ((rc = sqlite3_step(stmt.ptr())) == SQLITE_ROW)
      {
        const int current_device_id = sqlite3_column_int(stmt.ptr(), 2);
        if (current_device_id != last_device_id)
        {
          dr.dev.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 0)));
          dr.dev.origin = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 1)));
          last_device_id = current_device_id;
        }
        const std::string date(reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 3)));
        const auto maybe_time = string_to_time(date);
        if (!maybe_time.has_value())
        {
          return maybe_time.error();
        }
        dr.reading.time = maybe_time.value();
        dr.reading.value = sqlite3_column_int64(stmt.ptr(), 4);
        data.push_back(dr);
      }
      if (rc != SQLITE_DONE)
      {
        // An error occurred.
        return "Failed to retrieve data from database query.";
      }

      return std::nullopt;
    }

    template<typename read_t>
    std::optional<std::string> get_device_readings_impl(const thermos::device& dev, std::vector<read_t>& data, const std::string& file_name, const std::chrono::hours time_span)
    {
      const auto maybe_id = get_device_id(dev, file_name);
      if (!maybe_id.has_value())
      {
        return maybe_id.error();
      }

      auto maybe_db = sqlite::database::open(file_name);
      if (!maybe_db.has_value())
      {
        return maybe_db.error();
      }
      auto& dbase = maybe_db.value();

      std::string max_date;
      {
        auto maybe_stmt = dbase.prepare("SELECT MAX(date) FROM reading WHERE deviceId ="
                                       + std::to_string(maybe_id.value()) + " LIMIT 1;");
        if (!maybe_stmt.has_value())
        {
          return maybe_stmt.error();
        }
        auto& stmt = maybe_stmt.value();
        data.clear();
        const int rc = sqlite3_step(stmt.ptr());
        switch (rc)
        {
          case SQLITE_ROW:
               max_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 0));
               break;
          case SQLITE_DONE:
               // No data.
               return std::nullopt;
          default:
               return "Failed to retrieve maximum date value from database.";
        }
      }

      const auto hours = std::to_string(abs(static_cast<long long int>(time_span.count())));
      auto maybe_stmt = dbase.prepare("SELECT date, value FROM reading WHERE deviceId ="
          + std::to_string(maybe_id.value())
          + " AND type = @t AND date >= datetime(@max_d, '-" + hours + " hours');");
      if (!maybe_stmt.has_value())
      {
        return maybe_stmt.error();
      }
      auto& stmt = maybe_stmt.value();
      read_t r;
      if (!stmt.bind(1, to_string(r.type())) || !stmt.bind(2, max_date))
      {
        return "Could not bind reading data and maximum date to prepared statement!";
      }
      int rc = -1;
      while ((rc = sqlite3_step(stmt.ptr())) == SQLITE_ROW)
      {
        const std::string date(reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 0)));
        const auto maybe_time = string_to_time(date);
        if (!maybe_time.has_value())
        {
          return maybe_time.error();
        }
        r.time = maybe_time.value();
        r.value = sqlite3_column_int64(stmt.ptr(), 1);
        data.push_back(r);
      }
      if (rc != SQLITE_DONE)
      {
        // An error occurred.
        return "Failed to retrieve data from database query.";
      }

      return std::nullopt;
    }
};

} // namespace

#endif // SQLite feature guard

#endif // THERMOS_STORAGE_DB_HPP
