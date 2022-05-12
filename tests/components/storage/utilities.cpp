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
#include "../../../lib/storage/utilities.hpp"

TEST_CASE("time_to_string function")
{
  using namespace thermos::storage;

  SECTION("now() can be transformed to string")
  {
    const auto now = std::chrono::system_clock::now();
    const auto now_string = time_to_string(now);
    REQUIRE( now_string.has_value() );
    REQUIRE_FALSE( now_string.value().empty() );
  }

  SECTION("specific dates can be transformed to string")
  {
    SECTION("2022-04-23 19:08:01")
    {
      std::tm tm{};
      tm.tm_year = 2022 - 1900; // 2022
      tm.tm_mon = 4 - 1; // April
      tm.tm_mday = 23; // 23rd
      tm.tm_hour = 19;
      tm.tm_min = 8;
      tm.tm_sec = 1;
      tm.tm_isdst = -1; // no info on daylight saving time
      const std::time_t tt = std::mktime(&tm);
      REQUIRE( tt != static_cast<std::time_t>(-1) );

      const auto date_time = std::chrono::system_clock::from_time_t(tt);
      const auto date_time_string = time_to_string(date_time);
      REQUIRE( date_time_string.has_value() );
      REQUIRE_FALSE( date_time_string.value().empty() );
      REQUIRE( date_time_string.value() == "2022-04-23 19:08:01" );
    }

    SECTION("time with leading zeroes")
    {
      std::tm tm{};
      tm.tm_year = 1990 - 1900; // 1990
      tm.tm_mon = 5 - 1; // May
      tm.tm_mday = 1; // 1st
      tm.tm_hour = 6;
      tm.tm_min = 7;
      tm.tm_sec = 8;
      tm.tm_isdst = -1; // no info on daylight saving time
      const std::time_t tt = std::mktime(&tm);
      REQUIRE( tt != static_cast<std::time_t>(-1) );

      const auto date_time = std::chrono::system_clock::from_time_t(tt);
      const auto date_time_string = time_to_string(date_time);
      REQUIRE( date_time_string.has_value() );
      REQUIRE_FALSE( date_time_string.value().empty() );
      REQUIRE( date_time_string.value() == "1990-05-01 06:07:08" );
    }

    SECTION("time without leading zeroes")
    {
      std::tm tm{};
      tm.tm_year = 2030 - 1900; // 2030
      tm.tm_mon = 12 - 1; // December
      tm.tm_mday = 24; // 24th
      tm.tm_hour = 19;
      tm.tm_min = 20;
      tm.tm_sec = 35;
      tm.tm_isdst = -1; // no info on daylight saving time
      const std::time_t tt = std::mktime(&tm);
      REQUIRE( tt != static_cast<std::time_t>(-1) );

      const auto date_time = std::chrono::system_clock::from_time_t(tt);
      const auto date_time_string = time_to_string(date_time);
      REQUIRE( date_time_string.has_value() );
      REQUIRE_FALSE( date_time_string.value().empty() );
      REQUIRE( date_time_string.value() == "2030-12-24 19:20:35" );
    }
  }
}

TEST_CASE("string_to_time function")
{
  using namespace thermos::storage;

  SECTION("specific dates can be transformed to time")
  {
    SECTION("2022-04-23 19:08:01")
    {
      std::tm tm{};
      tm.tm_year = 2022 - 1900; // 2022
      tm.tm_mon = 4 - 1; // April
      tm.tm_mday = 23; // 23rd
      tm.tm_hour = 19;
      tm.tm_min = 8;
      tm.tm_sec = 1;
      tm.tm_isdst = -1; // no info on daylight saving time
      const std::time_t tt = std::mktime(&tm);
      REQUIRE( tt != static_cast<std::time_t>(-1) );
      const auto expected_date_time = std::chrono::system_clock::from_time_t(tt);

      const auto maybe_time = string_to_time("2022-04-23 19:08:01");
      REQUIRE( maybe_time.has_value() );
      REQUIRE( maybe_time.value() == expected_date_time );
    }

    SECTION("time with leading zeroes")
    {
      std::tm tm{};
      tm.tm_year = 1990 - 1900; // 1990
      tm.tm_mon = 5 - 1; // May
      tm.tm_mday = 1; // 1st
      tm.tm_hour = 6;
      tm.tm_min = 7;
      tm.tm_sec = 8;
      tm.tm_isdst = -1; // no info on daylight saving time
      const std::time_t tt = std::mktime(&tm);
      REQUIRE( tt != static_cast<std::time_t>(-1) );
      const auto expected_date_time = std::chrono::system_clock::from_time_t(tt);

      const auto maybe_time = string_to_time("1990-05-01 06:07:08");
      REQUIRE( maybe_time.has_value() );
      REQUIRE( maybe_time.value() == expected_date_time );
    }

    SECTION("time without leading zeroes")
    {
      std::tm tm{};
      tm.tm_year = 2030 - 1900; // 2030
      tm.tm_mon = 12 - 1; // December
      tm.tm_mday = 24; // 24th
      tm.tm_hour = 19;
      tm.tm_min = 20;
      tm.tm_sec = 35;
      tm.tm_isdst = -1; // no info on daylight saving time
      const std::time_t tt = std::mktime(&tm);
      REQUIRE( tt != static_cast<std::time_t>(-1) );
      const auto expected_date_time = std::chrono::system_clock::from_time_t(tt);

      const auto maybe_time = string_to_time("2030-12-24 19:20:35");
      REQUIRE( maybe_time.has_value() );
      REQUIRE( maybe_time.value() == expected_date_time );
    }
  }

  SECTION("test with invalid strings")
  {
    SECTION("input too long/short")
    {
      auto t = string_to_time("2022-05-01 06:07:08f");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("pattern") != std::string::npos );

      t = string_to_time("2022-05-01 06:07");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("pattern") != std::string::npos );
    }

    SECTION("invalid year")
    {
      auto t = string_to_time("-990-05-01 06:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid year") != std::string::npos );

      t = string_to_time("199Z-05-01 06:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid year") != std::string::npos );
    }

    SECTION("invalid month")
    {
      auto t = string_to_time("1990-14-01 06:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid month") != std::string::npos );

      t = string_to_time("1990--1-01 06:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid month") != std::string::npos );
    }

    SECTION("invalid day")
    {
      auto t = string_to_time("1990-12-32 06:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid day") != std::string::npos );

      t = string_to_time("1990-01-00 06:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid day") != std::string::npos );
    }

    SECTION("invalid hour")
    {
      auto t = string_to_time("1990-12-31 25:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid hour") != std::string::npos );

      t = string_to_time("1990-01-01 -1:07:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid hour") != std::string::npos );
    }

    SECTION("invalid minute")
    {
      auto t = string_to_time("1990-12-31 23:60:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid minute") != std::string::npos );

      t = string_to_time("1990-01-01 01:-1:08");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid minute") != std::string::npos );
    }

    SECTION("invalid second")
    {
      auto t = string_to_time("1990-12-31 23:10:61");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid second") != std::string::npos );

      t = string_to_time("1990-01-01 01:01:-1");
      REQUIRE_FALSE( t.has_value() );
      REQUIRE( t.error().find("not a valid second") != std::string::npos );
    }
  }
}

TEST_CASE("time_to_string and string_to_time roundtrip")
{
  using namespace thermos::storage;

  SECTION("now() can be transformed forth and back")
  {
    // Round to seconds, because both functions only work with precision to
    // the seconds. Milliseconds or smaller are not considered.
    const auto now = std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now());
    // Convert time to string and back.
    const auto now_time = string_to_time(time_to_string(now).value()).value();

    REQUIRE( now == now_time );
  }

  SECTION("2030-12-24 19:20:35 comes back the same")
  {
    const auto s = time_to_string(string_to_time("2030-12-24 19:20:35").value());
    REQUIRE( s.has_value() );
    REQUIRE( s.value() == "2030-12-24 19:20:35");
  }
}
