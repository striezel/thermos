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
#include "../../../lib/sqlite/database.hpp"

#if !defined(THERMOS_NO_SQLITE)
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

TEST_CASE("sqlite::database::last_insert_id")
{
  using namespace thermos::sqlite;

  SECTION("id is zero when no insert has happened")
  {
    auto possible_db = database::open(":memory:");
    REQUIRE( possible_db.has_value() );
    auto& db = possible_db.value();

    REQUIRE( db.last_insert_id() == 0 );
  }

  SECTION("id changes after every insert")
  {
    auto possible_db = database::open(":memory:");
    REQUIRE( possible_db.has_value() );
    auto& db = possible_db.value();

    // Create table.
    REQUIRE( db.exec("CREATE TABLE foo (fooId INTEGER PRIMARY KEY NOT NULL, blah TEXT NOT NULL);") );
    // And insert a new record.
    REQUIRE( db.exec("INSERT INTO foo (fooId, blah) VALUES (42, 'some text');") );
    const auto first_id = db.last_insert_id();
    REQUIRE( first_id != 0 );
    REQUIRE( first_id == 42 );
    // Insert another record.
    REQUIRE( db.exec("INSERT INTO foo (blah) VALUES ('some other text');") );
    const auto second_id = db.last_insert_id();
    REQUIRE( second_id != 0 );
    REQUIRE( second_id != first_id );
    REQUIRE( second_id > first_id );
  }
}

TEST_CASE("sqlite::database::table_exists")
{
  using namespace thermos::sqlite;

  SECTION("existence checks")
  {
    auto possible_db = database::open(":memory:");
    REQUIRE( possible_db.has_value() );
    auto& db = possible_db.value();

    // Initially, no table exists.
    auto exists = db.table_exists("foo");
    REQUIRE( exists.has_value() );
    REQUIRE_FALSE( exists.value() );

    // Create table.
    REQUIRE( db.exec("CREATE TABLE foo (fooId INTEGER NOT NULL);") );
    // Table should exist now.
    exists = db.table_exists("foo");
    REQUIRE( exists.has_value() );
    REQUIRE( exists.value() );

    // Other table should not exist.
    exists = db.table_exists("blah");
    REQUIRE( exists.has_value() );
    REQUIRE_FALSE( exists.value() );
  }
}
#endif // SQLite feature guard
