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
#include <sstream>
#include "../../lib/reading_type.hpp"

TEST_CASE("reading_type's to_string function")
{
  using namespace thermos;

  REQUIRE( to_string(reading_type::load) == "load" );
  REQUIRE( to_string(reading_type::temperature) == "temperature" );

  REQUIRE_THROWS( to_string(static_cast<reading_type>(25)) );
}

TEST_CASE("reading_type's stream operator <<")
{
  using namespace thermos;

  SECTION("load")
  {
    std::ostringstream stream;
    stream << reading_type::load;
    REQUIRE( stream.str() == "load" );
  }

  SECTION("temperature")
  {
    std::ostringstream stream;
    stream << reading_type::temperature;
    REQUIRE( stream.str() == "temperature" );
  }
}
