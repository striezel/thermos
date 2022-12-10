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
#include "../../../src/graph-generator/generator.hpp"

TEST_CASE("graph-generator: get_human_readable_span")
{
  using namespace thermos;

  REQUIRE( get_human_readable_span(std::chrono::hours(1)) == "1 hour" );
  REQUIRE( get_human_readable_span(std::chrono::hours(2)) == "2 hours" );
  REQUIRE( get_human_readable_span(std::chrono::hours(12)) == "12 hours" );
  REQUIRE( get_human_readable_span(std::chrono::hours(24)) == "1 day" );
  REQUIRE( get_human_readable_span(std::chrono::hours(48)) == "2 days" );
  REQUIRE( get_human_readable_span(std::chrono::hours(48 + 6)) == "2 days 6 hours" );
  REQUIRE( get_human_readable_span(std::chrono::hours(72)) == "3 days" );
  REQUIRE( get_human_readable_span(std::chrono::hours(720)) == "30 days" );

  REQUIRE( get_human_readable_span(std::chrono::hours(24 * 365)) == "1 year" );
  REQUIRE( get_human_readable_span(std::chrono::hours(24 * 365 + 24)) == "1 year 1 day" );
  REQUIRE( get_human_readable_span(std::chrono::hours(24 * 365 + 24 + 1)) == "1 year 1 day 1 hour" );
  REQUIRE( get_human_readable_span(std::chrono::hours(2 * 24 * 365)) == "2 years" );
  REQUIRE( get_human_readable_span(std::chrono::hours(2 * 24 * 365 + 24 * 150 )) == "2 years 150 days" );
  REQUIRE( get_human_readable_span(std::chrono::hours(2 * 24 * 365 + 24 * 150 + 10 )) == "2 years 150 days 10 hours" );
  REQUIRE( get_human_readable_span(std::chrono::hours(3 * 24 * 365)) == "3 years" );
}

TEST_CASE("graph-generator: get_short_name")
{
  using namespace thermos;

  REQUIRE( get_short_name(std::chrono::hours(1)) == "1h" );
  REQUIRE( get_short_name(std::chrono::hours(2)) == "2h" );
  REQUIRE( get_short_name(std::chrono::hours(12)) == "12h" );
  REQUIRE( get_short_name(std::chrono::hours(24)) == "1d" );
  REQUIRE( get_short_name(std::chrono::hours(48)) == "2d" );
  REQUIRE( get_short_name(std::chrono::hours(48 + 6)) == "2d 6h" );
  REQUIRE( get_short_name(std::chrono::hours(72)) == "3d" );
  REQUIRE( get_short_name(std::chrono::hours(720)) == "30d" );

  REQUIRE( get_short_name(std::chrono::hours(24 * 365)) == "1y" );
  REQUIRE( get_short_name(std::chrono::hours(24 * 365 + 24)) == "1y 1d" );
  REQUIRE( get_short_name(std::chrono::hours(24 * 365 + 24 + 1)) == "1y 1d 1h" );
  REQUIRE( get_short_name(std::chrono::hours(2 * 24 * 365)) == "2y" );
  REQUIRE( get_short_name(std::chrono::hours(2 * 24 * 365 + 24 * 150 )) == "2y 150d" );
  REQUIRE( get_short_name(std::chrono::hours(2 * 24 * 365 + 24 * 150 + 10 )) == "2y 150d 10h" );
  REQUIRE( get_short_name(std::chrono::hours(3 * 24 * 365)) == "3y" );
}
