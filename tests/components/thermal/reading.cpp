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
    REQUIRE( reading.value == std::numeric_limits<int64_t>::min() );
    REQUIRE( reading.time == thermal::reading::reading_time_t() );
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
