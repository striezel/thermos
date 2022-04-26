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

TEST_CASE("sqlite::database::quote")
{
  using namespace thermos::sqlite;

  SECTION("quote")
  {
    REQUIRE( database::quote("") == "''" );
    REQUIRE( database::quote("foo") == "'foo'" );
    REQUIRE( database::quote("foo bar") == "'foo bar'" );
    REQUIRE( database::quote("'") == "''''" );
    REQUIRE( database::quote("foo'bar") == "'foo''bar'" );
    REQUIRE( database::quote("\t \r\n \n \r") == "'\t \r\n \n \r'" );
  }
}

TEST_CASE("sqlite::database::open")
{
  using namespace thermos::sqlite;

  SECTION("opening an in-memory database is always successful")
  {
    const auto db = database::open(":memory:");
    REQUIRE( db.has_value() );
  }

  SECTION("opening a database that cannot be created fails")
  {
    const auto db = database::open("/does/not/exist.db");
    REQUIRE_FALSE( db.has_value() );
    REQUIRE( db.error().find("Could not open") != std::string::npos );
  }
}

TEST_CASE("sqlite::database::exec")
{
  using namespace thermos::sqlite;

  SECTION("successful statement")
  {
    auto db = database::open(":memory:");
    REQUIRE( db.has_value() );

    REQUIRE( db.value().exec("SELECT 1+2;") );
  }

  SECTION("failing statement")
  {
    auto db = database::open(":memory:");
    REQUIRE( db.has_value() );

    REQUIRE_FALSE( db.value().exec("SELECT syn) tax + error FROM foo;") );
  }
}

TEST_CASE("sqlite::database::prepare")
{
  using namespace thermos::sqlite;

  SECTION("successful statement preparation")
  {
    auto db = database::open(":memory:");
    REQUIRE( db.has_value() );

    auto stmt = db.value().prepare("SELECT 1+2 AS sum WHERE sum=@blah;");
    REQUIRE( stmt.has_value() );

    // bind some parameter successfully
    REQUIRE( stmt.value().bind(1, "foo") );

    // failing bind
    REQUIRE_FALSE( stmt.value().bind(2, "baz") );
  }
}
