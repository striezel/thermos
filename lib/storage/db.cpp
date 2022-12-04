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

namespace thermos::storage
{

nonstd::expected<sqlite::database, std::string> db::prepare_db(const std::string& file_name)
{
  // Open the database.
  auto maybe_db = sqlite::database::open(file_name);
  if (!maybe_db.has_value())
  {
    return nonstd::make_unexpected(maybe_db.error());
  }
  auto& db_ = maybe_db.value();
  // Make sure the schema is correct.
  const auto existence = ensure_tables_exist(db_);
  if (existence.has_value())
  {
    return nonstd::make_unexpected(existence.value());
  }

  return std::move(db_);
}

std::optional<std::string> db::save(const std::vector<thermos::thermal::device_reading>& data, const std::string& file_name)
{
  return save_impl(data, file_name);
}

std::optional<std::string> db::save(const std::vector<thermos::load::device_reading>& data, const std::string& file_name)
{
  return save_impl(data, file_name);
}

std::optional<std::string> db::load(std::vector<thermos::thermal::device_reading>& data, const std::string& file_name)
{
  return load_impl<thermos::thermal::device_reading>(data, file_name);
}

std::optional<std::string> db::load(std::vector<thermos::load::device_reading>& data, const std::string& file_name)
{
  return load_impl<thermos::load::device_reading>(data, file_name);
}

std::optional<std::string> db::ensure_tables_exist(sqlite::database& dbase)
{
  auto exists = dbase.table_exists("device");
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
          type TEXT,
          date TEXT,
          value INTEGER
        );
        )SQL";
    if (!dbase.exec(statement))
      return "Failed to create tables.";
  }

  return std::nullopt;
}

nonstd::expected<int64_t, std::string> db::find_or_create_device(sqlite::database& dbase, const device& dev)
{
  {
    auto maybe_stmt = dbase.prepare("SELECT deviceId FROM device WHERE origin = @ori AND name = @name LIMIT 1;");
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
  auto maybe_stmt = dbase.prepare("INSERT INTO device (origin, name) VALUES (@ori, @name);");
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
  return dbase.last_insert_id();
}

std::optional<std::string> db::get_devices(std::vector<thermos::device>& data, const thermos::reading_type type, const std::string& file_name)
{
  // Open the database.
  auto maybe_db = sqlite::database::open(file_name);
  if (!maybe_db.has_value())
  {
    return maybe_db.error();
  }
  auto& dbase = maybe_db.value();

  auto maybe_stmt = dbase.prepare(R"(SELECT deviceId, name, origin FROM device JOIN
                                       (SELECT DISTINCT deviceId AS devid, type FROM reading WHERE reading.type=@t)
                                       ON device.deviceId = devid ORDER BY name ASC;)");
  if (!maybe_stmt.has_value())
  {
    return maybe_stmt.error();
  }
  auto& stmt = maybe_stmt.value();
  if (!stmt.bind(1, to_string(type)))
  {
    return "Could not bind reading type to prepared statement!";
  }

  data.clear();
  device dev;
  dev.name = "uninitialized";
  dev.origin = "uninitialized";
  int rc = -1;
  while ((rc = sqlite3_step(stmt.ptr())) == SQLITE_ROW)
  {
    dev.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 1)));
    dev.origin = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt.ptr(), 2)));
    data.push_back(dev);
  }

  if (rc != SQLITE_DONE)
  {
    // An error occurred.
    return "Failed to retrieve data from database query.";
  }

  return std::nullopt;
}

nonstd::expected<int64_t, std::string> db::get_device_id(const thermos::device& dev, const std::string& file_name)
{
  auto maybe_db = sqlite::database::open(file_name);
  if (!maybe_db.has_value())
  {
    return nonstd::make_unexpected(maybe_db.error());
  }
  auto& dbase = maybe_db.value();
  auto maybe_stmt = dbase.prepare("SELECT deviceId FROM device WHERE name=@nom AND origin=@ori LIMIT 1;");
  if (!maybe_stmt.has_value())
  {
    return nonstd::make_unexpected(maybe_stmt.error());
  }
  auto& stmt = maybe_stmt.value();
  if (!stmt.bind(1, dev.name) || !stmt.bind(2, dev.origin))
  {
    return nonstd::make_unexpected("Could not bind device data to prepared statement!");
  }
  const int rc = sqlite3_step(stmt.ptr());
  switch (rc)
  {
    case SQLITE_ROW:
         return sqlite3_column_int64(stmt.ptr(), 0);
    case SQLITE_DONE:
         return static_cast<std::int64_t>(0);
    default:
         return nonstd::make_unexpected("Failed to retrieve deviceId from database.");
  }
}

std::optional<std::string> db::get_device_readings(const thermos::device& dev, std::vector<load::reading>& data, const std::string& file_name, const std::chrono::hours time_span)
{
  return get_device_readings_impl(dev, data, file_name, time_span);
}

std::optional<std::string> db::get_device_readings(const thermos::device& dev, std::vector<thermal::reading>& data, const std::string& file_name, const std::chrono::hours time_span)
{
  return get_device_readings_impl(dev, data, file_name, time_span);
}

} // namespace
#endif // SQLite
