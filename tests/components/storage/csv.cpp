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
#include <filesystem>
#include <fstream>
#include "../../../lib/storage/csv.hpp"
#include "to_time.hpp"

TEST_CASE("csv storage")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    csv store;
    const auto opt = store.save(data, "/path/may-not/exist/for-real.csv");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Failed to create or open file") != std::string::npos );
  }

  SECTION("normal write operation")
  {
    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.value = 60000;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-normal.csv";

    csv store;
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    // read data back to check file content
    std::string line;
    {
      std::ifstream stream(file_name);
      REQUIRE( stream.good() );
      std::getline(stream, line);
      REQUIRE( stream.good() );
      REQUIRE( line == "foo;origin is here;42000;2022-04-23 19:18:17" );
      std::getline(stream, line);
      REQUIRE( stream.good() );
      REQUIRE( line == "foo;origin is here;60000;2022-04-23 19:20:21" );
      stream.close();
    }

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("save appends to existing file")
  {
    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.value = 60000;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-append-existing.csv";

    csv store;
    // save first part of data
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    // prepare data for append operation
    data.clear();
    reading.dev.name = "bar";
    reading.dev.origin = "somewhere else";
    reading.value = 43210;
    reading.time = to_time(2022, 4, 23, 20, 19, 18);
    data.push_back(reading);
    reading.value = 12345;
    reading.time = to_time(2022, 4, 23, 22, 23, 24);
    data.push_back(reading);

    const auto opt_append = store.save(data, file_name);
    REQUIRE_FALSE( opt_append.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    // read data back to check file content
    std::string line;
    {
      std::ifstream stream(file_name);
      REQUIRE( stream.good() );
      std::getline(stream, line);
      REQUIRE( stream.good() );
      REQUIRE( line == "foo;origin is here;42000;2022-04-23 19:18:17" );
      std::getline(stream, line);
      REQUIRE( stream.good() );
      REQUIRE( line == "foo;origin is here;60000;2022-04-23 19:20:21" );
      std::getline(stream, line);
      REQUIRE( stream.good() );
      REQUIRE( line == "bar;somewhere else;43210;2022-04-23 20:19:18" );
      std::getline(stream, line);
      REQUIRE( stream.good() );
      REQUIRE( line == "bar;somewhere else;12345;2022-04-23 22:23:24" );
      stream.close();
    }

    REQUIRE( std::filesystem::remove(file_name) );
  }
}
