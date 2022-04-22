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
    REQUIRE( dev.name.empty() );
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
    dev.name = "foo";
    dev.origin = "bar";

    REQUIRE( dev.filled() );
  }

  SECTION("partially set data")
  {
    device dev;

    SECTION("only name")
    {
      dev.name = "foo";
      REQUIRE_FALSE( dev.filled() );
    }

    SECTION("only origin")
    {
      dev.origin = "bar";
      REQUIRE_FALSE( dev.filled() );
    }
  }
}
