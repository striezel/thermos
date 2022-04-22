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

#include "find_catch.hpp"
#include "../../src/lib/device_reading.hpp"

TEST_CASE("device_reading constructor")
{
  using namespace thermos;

  SECTION("initial values must be set")
  {
    device_reading reading;
    REQUIRE_FALSE( reading.dev.filled() );
    REQUIRE( reading.millicelsius == std::numeric_limits<int64_t>::min() );
    REQUIRE( reading.time == device_reading::reading_time_t() );
  }
}

TEST_CASE("device_reading::filled()")
{
  using namespace thermos;

  SECTION("empty device_reading")
  {
    device_reading reading;

    REQUIRE_FALSE( reading.filled() );
  }

  SECTION("fully set data")
  {
    device dev;
    dev.name = "foo";
    dev.origin = "bar";
    REQUIRE( dev.filled() );

    device_reading reading;
    reading.dev = dev;
    reading.millicelsius = 42000;
    reading.time = std::chrono::system_clock::now();
    REQUIRE( reading.filled() );
  }

  SECTION("partially set data")
  {
    device_reading reading;

    SECTION("only device")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      reading.dev = dev;
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only temperature")
    {
      reading.millicelsius = 42000;
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only time")
    {
      reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only dev and temperature")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      reading.dev = dev;
      reading.millicelsius = 42000;
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only dev and time")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      reading.dev = dev;
      reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only temperature and time")
    {
      reading.millicelsius = 42000;
      reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( reading.filled() );
    }
  }
}

TEST_CASE("device_reading::celsius()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    device_reading reading;
    REQUIRE( reading.celsius() == -9223372036854775.808 );

    reading.millicelsius = 42000;
    REQUIRE( reading.celsius() == 42.0 );

    reading.millicelsius = 55500;
    REQUIRE( reading.celsius() == 55.5 );

    reading.millicelsius = -40000;
    REQUIRE( reading.celsius() == -40.0 );

    reading.millicelsius = -273150;
    REQUIRE( reading.celsius() == -273.15 );
  }
}

TEST_CASE("device_reading::fahrenheit()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    device_reading reading;

    reading.millicelsius = 0; // 0 °C
    REQUIRE( reading.fahrenheit() == 32.0 );

    reading.millicelsius = 100000; // 100 °C
    REQUIRE( reading.fahrenheit() == 212.0 );

    reading.millicelsius = -40000; // -40 °C == -40 °F
    REQUIRE( reading.fahrenheit() == -40.0 );

    reading.millicelsius = -273150;
    REQUIRE( reading.fahrenheit() == -459.67 );
  }
}
