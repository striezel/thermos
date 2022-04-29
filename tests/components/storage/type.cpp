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
#include <sstream>
#include "../../../src/lib/storage/type.hpp"

TEST_CASE("from_string function")
{
  using namespace thermos::storage;

  REQUIRE_FALSE( from_string("").has_value() );
  REQUIRE_FALSE( from_string("blah").has_value() );
  REQUIRE_FALSE( from_string("CSV").has_value() );
  REQUIRE_FALSE( from_string("DB").has_value() );

  REQUIRE( from_string("csv").has_value() );
  REQUIRE( from_string("csv").value() == type::csv );

  REQUIRE( from_string("db").has_value() );
  REQUIRE( from_string("db").value() == type::db );
}

TEST_CASE("type's stream operator <<")
{
  using namespace thermos::storage;

  SECTION("csv")
  {
    std::ostringstream stream;
    stream << type::csv;
    REQUIRE( stream.str() == "csv" );
  }

  SECTION("db")
  {
    std::ostringstream stream;
    stream << type::db;
    REQUIRE( stream.str() == "db" );
  }
}
