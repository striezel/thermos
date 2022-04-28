/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for thermos.
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

#include "../find_catch.hpp"
#include "../../../src/lib/sqlite/database.hpp"
#include "../../../src/lib/sqlite/statement.hpp"

TEST_CASE("sqlite::database::ptr")
{
  using namespace thermos::sqlite;

  SECTION("ptr() does not return nullptr")
  {
    auto db = database::open(":memory:");
    auto stmt = db.value().prepare("SELECT 1+2 AS sum WHERE sum=@blah;");
    REQUIRE( stmt.has_value() );

    REQUIRE_FALSE( stmt.value().ptr() == nullptr );
  }
}
