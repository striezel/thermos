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
#include "../../src/lib/device.hpp"

TEST_CASE("device constructor")
{
  using namespace thermos;

  SECTION("initial values must be set")
  {
    device dev;
    REQUIRE( dev.type.empty() );
    REQUIRE( dev.millicelsius == std::numeric_limits<int64_t>::min() );
    REQUIRE( dev.origin.empty() );
  }
}

TEST_CASE("device::filled()")
{
  using namespace thermos;

  SECTION("empty device")
  {
    device dev;

    REQUIRE_FALSE( dev.filled() );
  }

  SECTION("fully set data")
  {
    device dev;
    dev.millicelsius = 42000;
    dev.type = "foo";
    dev.origin = "bar";

    REQUIRE( dev.filled() );
  }

  SECTION("partially set data")
  {
    device dev;

    SECTION("only type")
    {
      dev.type = "foo";
      REQUIRE_FALSE( dev.filled() );
    }

    SECTION("only temperature")
    {
      dev.millicelsius = 42000;
      REQUIRE_FALSE( dev.filled() );
    }

    SECTION("only origin")
    {
      dev.origin = "bar";
      REQUIRE_FALSE( dev.filled() );
    }

    SECTION("only type and temperature")
    {
      dev.type = "foo";
      dev.millicelsius = 42000;
      REQUIRE_FALSE( dev.filled() );
    }

    SECTION("only type and origin")
    {
      dev.type = "foo";
      dev.origin = "bar";
      REQUIRE_FALSE( dev.filled() );
    }

    SECTION("only temperature and origin")
    {
      dev.millicelsius = 42000;
      dev.origin = "bar";
      REQUIRE_FALSE( dev.filled() );
    }
  }
}

TEST_CASE("device::celsius()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    device dev;
    REQUIRE( dev.celsius() == -9223372036854775.808 );

    dev.millicelsius = 42000;
    REQUIRE( dev.celsius() == 42.0 );

    dev.millicelsius = 55500;
    REQUIRE( dev.celsius() == 55.5 );

    dev.millicelsius = -40000;
    REQUIRE( dev.fahrenheit() == -40.0 );

    dev.millicelsius = -273150;
    REQUIRE( dev.celsius() == -273.15 );
  }
}

TEST_CASE("device::fahrenheit()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    device dev;

    dev.millicelsius = 0; // 0 °C
    REQUIRE( dev.fahrenheit() == 32.0 );

    dev.millicelsius = 100000; // 100 °C
    REQUIRE( dev.fahrenheit() == 212.0 );

    dev.millicelsius = -40000; // -40 °C == -40 °F
    REQUIRE( dev.fahrenheit() == -40.0 );

    dev.millicelsius = -273150;
    REQUIRE( dev.fahrenheit() == -459.67 );
  }
}
