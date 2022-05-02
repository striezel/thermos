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

#if !defined(THERMOS_NO_SQLITE)
#include "db.hpp"
#include "utilities.hpp"

namespace thermos::storage
{

std::optional<std::string> db::save(const std::vector<device_reading>& data, const std::string& file_name)
{
  // Open the database.
  auto maybe_db = sqlite::database::open(file_name);
  if (!maybe_db.has_value())
  {
    return maybe_db.error();
  }
  auto& db = maybe_db.value();
  // Make sure the schema is correct.
  const auto existence = ensure_tables_exist(db);
  if (existence.has_value())
  {
    return existence;
  }

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

std::optional<std::string> db::ensure_tables_exist(sqlite::database& db)
{
  auto exists = db.table_exists("device");
  if (!exists.has_value())
  {
    return exists.error();
  }
  if (!exists.value())
  {
    const std::string statement = R"SQL(
        CREATE TABLE device (
          deviceId INTEGER PRIMARY KEY NOT NULL,
          name TEXT NOT NULL,
          origin TEXT NOT NULL
        );
        CREATE TABLE reading (
          readingId INTEGER PRIMARY KEY NOT NULL,
          deviceId INTEGER NOT NULL,
          date TEXT,
          value INTEGER
        );
        )SQL";
    if (!db.exec(statement))
      return "Failed to create tables.";
  }

  return std::nullopt;
}

nonstd::expected<int64_t, std::string> db::find_or_create_device(sqlite::database& db, const device& dev)
{
  {
    auto maybe_stmt = db.prepare("SELECT deviceId FROM device WHERE origin = @ori AND name = @name LIMIT 1;");
    if (!maybe_stmt.has_value())
    {
      return nonstd::make_unexpected(maybe_stmt.error());
    }
    auto& stmt = maybe_stmt.value();
    if (!stmt.bind(1, dev.origin) || !stmt.bind(2, dev.name))
    {
      return nonstd::make_unexpected("Could not bind device data to prepared statement!");
    }

    const int rc = sqlite3_step(stmt.ptr());
    if (rc == SQLITE_ROW)
    {
      return sqlite3_column_int(stmt.ptr(), 0);
    }
    if (rc != SQLITE_DONE)
    {
      // An error occurred.
      return nonstd::make_unexpected("Failed to retrieve data from database query.");
    }
    // End of statement scope ensures statement is finalized.
  }

  // SQLITE_DONE means "no more data", so the device does not exist yet.
  auto maybe_stmt = db.prepare("INSERT INTO device (origin, name) VALUES (@ori, @name);");
  if (!maybe_stmt.has_value())
  {
    return nonstd::make_unexpected(maybe_stmt.error());
  }
  auto& stmt = maybe_stmt.value();
  if (!stmt.bind(1, dev.origin) || !stmt.bind(2, dev.name))
  {
    return nonstd::make_unexpected("Could not bind device data to prepared statement!");
  }
  const auto ret = sqlite3_step(stmt.ptr());
  if ((ret != SQLITE_OK) && (ret != SQLITE_DONE))
  {
    std::string message{"Error: Could not insert device data for '"};
    message.append(dev.name).append("' / '").append(dev.origin)
           .append("' into database!");
    return nonstd::make_unexpected(message);
  }
  return db.last_insert_id();
}

std::optional<std::string> db::insert_reading(sqlite::database& db, const device_reading& reading)
{
  const auto dev_id = find_or_create_device(db, reading.dev);
  if (!dev_id.has_value())
  {
    return dev_id.error();
  }
  const auto time_string = time_to_string(reading.time);
  if (!time_string.has_value())
  {
    return time_string.error();
  }

  const std::string sql = "INSERT INTO reading (deviceId, date, value) VALUES ("
      + std::to_string(dev_id.value()) + ", '" + time_string.value() + "', "
      + std::to_string(reading.millicelsius) + ");";
  if (!db.exec(sql))
  {
    return "Could not insert new device reading into database!";
  }

  // Insertion was successful.
  return std::nullopt;
}

} // namespace
#endif // SQLite
