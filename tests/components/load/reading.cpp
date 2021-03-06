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

TEST_CASE("load::reading constructor")
{
  using namespace thermos;

  SECTION("initial values must be set")
  {
    load::reading reading;
    REQUIRE( reading.value == std::numeric_limits<int64_t>::min() );
    REQUIRE( reading.time == load::reading::reading_time_t() );
  }
}

TEST_CASE("load::reading::type()")
{
  using namespace thermos;

  SECTION("type is always load")
  {
    load::reading reading;
    REQUIRE( reading.type() == reading_type::load );
  }
}

TEST_CASE("load::reading::percent()")
{
  using namespace thermos;

  SECTION("check some values")
  {
    load::reading reading;
    REQUIRE( reading.percent() == -9223372036854775808.0 );

    reading.value = 2400;
    REQUIRE( reading.percent() == 2400.00 );

    reading.value = 100;
    REQUIRE( reading.percent() == 100.0 );

    reading.value = 0;
    REQUIRE( reading.percent() == 0.0 );
  }
}
