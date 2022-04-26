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

#include "database.hpp"
#include <iostream>

namespace thermos::sqlite
{

nonstd::expected<database, std::string> database::open(const std::string& fileName)
{
  sqlite3* dbPtr = nullptr;
  const int errorCode = sqlite3_open(fileName.c_str(), &dbPtr);
  if (errorCode != SQLITE_OK)
  {
    std::string message = "Error: Could not open database " + fileName
        + ": " + sqlite3_errmsg(dbPtr);
    sqlite3_close(dbPtr);
    return nonstd::make_unexpected(message);
  }

  return database({ dbPtr, sqlite3_close });
}

database::database(std::unique_ptr<sqlite3, decltype(&sqlite3_close)>&& the_handle)
: handle(std::move(the_handle))
{
}

bool database::exec(const std::string& sql)
{
  char * errorMessage = nullptr;
  if (sqlite3_exec(handle.get(), sql.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK)
  {
    std::cerr << "Error: Could not execute SQL statement in SQLite 3 database!" << std::endl
              << errorMessage << std::endl;
    sqlite3_free(errorMessage);
    return false;
  }

  return true;
}

std::string database::quote(const std::string& str)
{
  std::string result(str);
  std::string::size_type pos = 0;
  std::string::size_type found = std::string::npos;
  while ((found = result.find('\'', pos)) != std::string::npos)
  {
    result.replace(found, 1, "''");
    pos = found + 2;
  }

  return std::string("'").append(result).append("'");
}

nonstd::expected<statement, std::string> database::prepare(const std::string& sqlStmt)
{
  sqlite3_stmt * stmt = nullptr;
  const int errorCode = sqlite3_prepare_v2(handle.get(), sqlStmt.c_str(), sqlStmt.size(), &stmt, nullptr);
  if (errorCode != SQLITE_OK)
  {
    std::cerr << "Error: Could not create prepared statement for '" << sqlStmt << "'!\n"
              << sqlite3_errmsg(handle.get()) << std::endl;
    return statement({ nullptr, sqlite3_finalize });
  }

  return statement({ stmt, sqlite3_finalize });
}

} // namespace
