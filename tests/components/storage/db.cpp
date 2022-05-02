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
#include <array>
#include <filesystem>
#include <fstream>
#include "../../../lib/storage/db.hpp"
#include "to_time.hpp"

#if !defined(THERMOS_NO_SQLITE)
TEST_CASE("db storage")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<device_reading> data;
    device_reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.millicelsius = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    db store;
    const auto opt = store.save(data, "/path/may-not/exist/for-real.db");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "file-is-not-sqlite3.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    std::vector<device_reading> data;
    device_reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.millicelsius = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    db store;
    const auto opt = store.save(data, file_name);
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Failed") != std::string::npos );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("normal write operation")
  {
    std::vector<device_reading> data;
    device_reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.millicelsius = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.millicelsius = 60000;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-normal.db";

    db store;
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    // read data back to check file format
    std::array<char, 24> buffer;
    buffer.fill('\0');
    {
      std::ifstream stream(file_name);
      REQUIRE( stream.good() );
      stream.read(buffer.data(), buffer.size());
      REQUIRE( stream.good() );
      stream.close();
    }

    // First 16 bytes of SQLite 3 file are "SQLite format 3", followed by NUL.
    const std::string s3 = std::string(buffer.data(), 15);
    REQUIRE( s3 == "SQLite format 3" );
    REQUIRE( buffer[15] == '\0' );
    // Bytes at offset 18 are 0x01, 0x01, 0x00, 0x40, 0x20, 0x20.
    REQUIRE( buffer[18] == 0x01 );
    REQUIRE( buffer[19] == 0x01 );
    REQUIRE( buffer[20] == 0x00 );
    REQUIRE( buffer[21] == 0x40 );
    REQUIRE( buffer[22] == 0x20 );
    REQUIRE( buffer[23] == 0x20 );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("save appends to existing file")
  {
    std::vector<device_reading> data;
    device_reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.millicelsius = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.millicelsius = 60000;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-append-existing.db";

    db store;
    // save first part of data
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    const auto size_one = std::filesystem::file_size(file_name);
    REQUIRE( size_one > 0 );

    // prepare data for append operation
    data.clear();
    reading.dev.name = "bar";
    reading.dev.origin = "somewhere else";
    reading.millicelsius = 43210;
    reading.time = to_time(2022, 4, 23, 20, 19, 18);
    data.push_back(reading);
    reading.millicelsius = 12345;
    reading.time = to_time(2022, 4, 23, 22, 23, 24);
    data.push_back(reading);
    // Generate more data to force resize.
    for (unsigned int i = 1; i < 60; ++i)
    {
      reading.dev.origin.append(" abc");
      reading.millicelsius += 100;
      reading.time = to_time(2022, 4, 23, 22, 24, i);
      data.push_back(reading);
    }

    const auto opt_append = store.save(data, file_name);
    REQUIRE_FALSE( opt_append.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    const auto size_two = std::filesystem::file_size(file_name);

    REQUIRE( std::filesystem::remove(file_name) );

    // Append means that the file size increased.
    REQUIRE( size_one < size_two );
  }
}
#endif // SQLite feature guard
