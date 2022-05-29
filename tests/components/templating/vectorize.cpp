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
#include "../../../lib/templating/vectorize.hpp"
#include "../storage/to_time.hpp"

TEST_CASE("vectorize")
{
  using namespace thermos;
  SECTION("CPU load readings")
  {
    std::vector<load::reading> data;

    SECTION("empty")
    {
      const auto vec = vectorize(data);
      REQUIRE( vec.has_value() );
      REQUIRE( vec.value().dates == "[]" );
      REQUIRE( vec.value().values == "[]" );
    }

    SECTION("actual data")
    {
      load::reading reading;
      reading.value = 450;
      reading.time = to_time(1999, 12, 31, 12, 34, 56);
      data.push_back(reading);
      reading.value = 26;
      reading.time = to_time(2022, 1, 2, 3, 4, 5);
      data.push_back(reading);

      const auto vec = vectorize(data);
      REQUIRE( vec.has_value() );
      REQUIRE( vec.value().dates == "[\"1999-12-31 12:34:56\",\"2022-01-02 03:04:05\"]" );
      REQUIRE( vec.value().values == "[450,26]" );
    }
  }

  SECTION("temperature readings")
  {
    std::vector<thermal::reading> data;

    SECTION("empty")
    {
      const auto vec = vectorize(data);
      REQUIRE( vec.has_value() );
      REQUIRE( vec.value().dates == "[]" );
      REQUIRE( vec.value().values == "[]" );
    }

    SECTION("actual data")
    {
      thermal::reading reading;
      reading.value = 24500;
      reading.time = to_time(2022, 4, 23, 14, 12, 12);
      data.push_back(reading);
      reading.value = 26000;
      reading.time = to_time(2022, 5, 23, 15, 16, 17);
      data.push_back(reading);

      const auto vec = vectorize(data);
      REQUIRE( vec.has_value() );
      REQUIRE( vec.value().dates == "[\"2022-04-23 14:12:12\",\"2022-05-23 15:16:17\"]" );
      REQUIRE( vec.value().values == "[24.5,26]" );
    }
  }
}
