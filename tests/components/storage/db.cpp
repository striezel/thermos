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
#include "../../../src/lib/storage/db.hpp"
#include "to_time.hpp"

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

  // TODO: section "normal write operation"
  // TODO: section "save appends to existing file"
}
