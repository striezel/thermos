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
#include "../../../src/lib/storage/csv.hpp"
#include "../../../src/lib/storage/db.hpp"
#include "../../../src/lib/storage/factory.hpp"

TEST_CASE("factory::create function")
{
  using namespace thermos::storage;

  SECTION("csv")
  {
    const auto ptr = factory::create(type::csv);
    REQUIRE_FALSE( ptr == nullptr );
    const csv* csv_ptr = dynamic_cast<csv*>(ptr.get());
    REQUIRE_FALSE( csv_ptr == nullptr );
    const db* db_ptr = dynamic_cast<db*>(ptr.get());
    REQUIRE( db_ptr == nullptr );
  }

  SECTION("db")
  {
    const auto ptr = factory::create(type::db);
    REQUIRE_FALSE( ptr == nullptr );
    const csv* csv_ptr = dynamic_cast<csv*>(ptr.get());
    REQUIRE( csv_ptr == nullptr );
    const db* db_ptr = dynamic_cast<db*>(ptr.get());
    REQUIRE_FALSE( db_ptr == nullptr );
  }

  SECTION("unsupported type returns null")
  {
    const type unsupported = static_cast<type>(static_cast<int>(type::db) + 20);

    const auto ptr = factory::create(unsupported);
    REQUIRE( ptr == nullptr );
  }
}
