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
#include "../../../lib/thermal/reading.hpp"

TEST_CASE("thermal::reading constructor")
{
  using namespace thermos;

  SECTION("initial values must be set")
  {
    thermal::reading reading;
    REQUIRE_FALSE( reading.dev.filled() );
    REQUIRE( reading.value == std::numeric_limits<int64_t>::min() );
    REQUIRE( reading.time == thermal::reading::reading_time_t() );
  }
}

TEST_CASE("thermal::reading::filled()")
{
  using namespace thermos;

  SECTION("empty thermal reading")
  {
    thermal::reading reading;

    REQUIRE_FALSE( reading.filled() );
  }

  SECTION("fully set data")
  {
    device dev;
    dev.name = "foo";
    dev.origin = "bar";
    REQUIRE( dev.filled() );

    thermal::reading reading;
    reading.dev = dev;
    reading.value = 42000;
    reading.time = std::chrono::system_clock::now();
    REQUIRE( reading.filled() );
  }

  SECTION("partially set data")
  {
    thermal::reading reading;

    SECTION("only device")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      reading.dev = dev;
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only value")
    {
      reading.value = 42000;
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only time")
    {
      reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( reading.filled() );
    }

    SECTION("only dev and value")
    {
      device dev;
      dev.name = "foo";
      dev.origin = "bar";

      reading.dev = dev;
      reading.value = 42000;
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

    SECTION("only value and time")
    {
      reading.value = 42000;
      reading.time = std::chrono::system_clock::now();
      REQUIRE_FALSE( reading.filled() );
    }
  }
}

TEST_CASE("thermal::reading::type()")
{
  using namespace thermos;

  SECTION("type is always temperature")
  {
    thermal::reading reading;
    REQUIRE( reading.type() == reading_type::temperature );
  }
}

TEST_CASE("thermal::reading::celsius()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    thermal::reading reading;
    REQUIRE( reading.celsius() == -9223372036854775.808 );

    reading.value = 42000;
    REQUIRE( reading.celsius() == 42.0 );

    reading.value = 55500;
    REQUIRE( reading.celsius() == 55.5 );

    reading.value = -40000;
    REQUIRE( reading.celsius() == -40.0 );

    reading.value = -273150;
    REQUIRE( reading.celsius() == -273.15 );
  }
}

TEST_CASE("thermal::reading::fahrenheit()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    thermal::reading reading;

    reading.value = 0; // 0 °C
    REQUIRE( reading.fahrenheit() == 32.0 );

    reading.value = 100000; // 100 °C
    REQUIRE( reading.fahrenheit() == 212.0 );

    reading.value = -40000; // -40 °C == -40 °F
    REQUIRE( reading.fahrenheit() == -40.0 );

    reading.value = -273150;
    REQUIRE( reading.fahrenheit() == -459.67 );
  }
}