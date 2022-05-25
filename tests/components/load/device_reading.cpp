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
#include "../../../lib/load/reading.hpp"

TEST_CASE("load::device_reading constructor")
{
  using namespace thermos;

  SECTION("initial values must be set")
  {
    load::device_reading dev_reading;
    REQUIRE_FALSE( dev_reading.dev.filled() );
    REQUIRE( dev_reading.reading.value == std::numeric_limits<int64_t>::min() );
    REQUIRE( dev_reading.reading.time == load::reading::reading_time_t() );
  }
}

TEST_CASE("load::device_reading::filled()")
{
  using namespace thermos;

  SECTION("empty load reading")
  {
    load::device_reading dev_reading;

    REQUIRE_FALSE( dev_reading.filled() );
  }

  SECTION("fully set data")
  {
    device dev;
    dev.name = "foo";
    dev.origin = "bar";
    REQUIRE( dev.filled() );

    load::device_reading dev_reading;
    dev_reading.dev = dev;
    dev_reading.reading.value = 800;
    dev_reading.reading.time = std::chrono::system_clock::now();
    REQUIRE( dev_reading.filled() );
  }

  SECTION("partially set data")
  {
    load::device_reading dev_reading;

    SECTION("only device")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      dev_reading.dev = dev;
      REQUIRE_FALSE( dev_reading.filled() );
    }

    SECTION("only value")
    {
      dev_reading.reading.value = 800;
      REQUIRE_FALSE( dev_reading.filled() );
    }

    SECTION("only time")
    {
      dev_reading.reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( dev_reading.filled() );
    }

    SECTION("only dev and value")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      dev_reading.dev = dev;
      dev_reading.reading.value = 800;
      REQUIRE_FALSE( dev_reading.filled() );
    }

    SECTION("only dev and time")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      dev_reading.dev = dev;
      dev_reading.reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( dev_reading.filled() );
    }

    SECTION("only value and time")
    {
      dev_reading.reading.value = 800;
      dev_reading.reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( dev_reading.filled() );
    }
  }
}
