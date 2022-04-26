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

#include "statement.hpp"

namespace thermos::sqlite
{

statement::statement(std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>&& s)
: stmt(std::move(s))
{
}

bool statement::bind(const int index, const std::string& value)
{
  return sqlite3_bind_text(stmt.get(), index, value.c_str(), static_cast<int>(value.size()), SQLITE_TRANSIENT) == SQLITE_OK;
}

bool statement::bind(const int index, const int64_t value)
{
  return sqlite3_bind_int64(stmt.get(), index, value) == SQLITE_OK;
}

} // namespace
