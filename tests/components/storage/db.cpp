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
#include <array>
#include <filesystem>
#include <fstream>
#include "../../../lib/storage/db.hpp"
#include "to_time.hpp"

#if !defined(THERMOS_NO_SQLITE)
TEST_CASE("db storage: save thermal data")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    db store;
    const auto opt = store.save(data, "/path/may-not/exist/for-real.db");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "thermal-file-is-not-sqlite3.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;temperature;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;temperature;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    db store;
    const auto opt = store.save(data, file_name);
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Failed") != std::string::npos );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("normal write operation")
  {
    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.value = 60000;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-normal-thermal.db";

    db store;
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    // read data back to check file format
    std::array<char, 24> buffer;
    buffer.fill('\0');
    {
      std::ifstream stream(file_name);
      REQUIRE( stream.good() );
      stream.read(buffer.data(), buffer.size());
      REQUIRE( stream.good() );
      stream.close();
    }

    // First 16 bytes of SQLite 3 file are "SQLite format 3", followed by NUL.
    const std::string s3 = std::string(buffer.data(), 15);
    REQUIRE( s3 == "SQLite format 3" );
    REQUIRE( buffer[15] == '\0' );
    // Bytes at offset 18 are 0x01, 0x01, 0x00, 0x40, 0x20, 0x20.
    REQUIRE( buffer[18] == 0x01 );
    REQUIRE( buffer[19] == 0x01 );
    REQUIRE( buffer[20] == 0x00 );
    REQUIRE( buffer[21] == 0x40 );
    REQUIRE( buffer[22] == 0x20 );
    REQUIRE( buffer[23] == 0x20 );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("save appends to existing file")
  {
    std::vector<thermos::thermal::reading> data;
    thermal::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.value = 60000;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-append-existing-thermal.db";

    db store;
    // save first part of data
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    const auto size_one = std::filesystem::file_size(file_name);
    REQUIRE( size_one > 0 );

    // prepare data for append operation
    data.clear();
    reading.dev.name = "bar";
    reading.dev.origin = "somewhere else";
    reading.value = 43210;
    reading.time = to_time(2022, 4, 23, 20, 19, 18);
    data.push_back(reading);
    reading.value = 12345;
    reading.time = to_time(2022, 4, 23, 22, 23, 24);
    data.push_back(reading);
    // Generate more data to force resize.
    for (unsigned int i = 1; i < 60; ++i)
    {
      reading.dev.origin.append(" abc");
      reading.value += 100;
      reading.time = to_time(2022, 4, 23, 22, 24, i);
      data.push_back(reading);
    }

    const auto opt_append = store.save(data, file_name);
    REQUIRE_FALSE( opt_append.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    const auto size_two = std::filesystem::file_size(file_name);

    REQUIRE( std::filesystem::remove(file_name) );

    // Append means that the file size increased.
    REQUIRE( size_one < size_two );
  }
}

TEST_CASE("db storage: save CPU load data")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<thermos::load::reading> data;
    load::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    db store;
    const auto opt = store.save(data, "/path/may-not/exist/for-real-load.db");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "load-file-is-not-sqlite3.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;temperature;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;temperature;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    std::vector<thermos::load::reading> data;
    load::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "ori";
    reading.value = 42000;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);

    db store;
    const auto opt = store.save(data, file_name);
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Failed") != std::string::npos );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("normal write operation")
  {
    std::vector<thermos::load::reading> data;
    load::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.value = 2400;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.value = 600;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-normal-load.db";

    db store;
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    // read data back to check file format
    std::array<char, 24> buffer;
    buffer.fill('\0');
    {
      std::ifstream stream(file_name);
      REQUIRE( stream.good() );
      stream.read(buffer.data(), buffer.size());
      REQUIRE( stream.good() );
      stream.close();
    }

    // First 16 bytes of SQLite 3 file are "SQLite format 3", followed by NUL.
    const std::string s3 = std::string(buffer.data(), 15);
    REQUIRE( s3 == "SQLite format 3" );
    REQUIRE( buffer[15] == '\0' );
    // Bytes at offset 18 are 0x01, 0x01, 0x00, 0x40, 0x20, 0x20.
    REQUIRE( buffer[18] == 0x01 );
    REQUIRE( buffer[19] == 0x01 );
    REQUIRE( buffer[20] == 0x00 );
    REQUIRE( buffer[21] == 0x40 );
    REQUIRE( buffer[22] == 0x20 );
    REQUIRE( buffer[23] == 0x20 );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("save appends to existing file")
  {
    std::vector<thermos::load::reading> data;
    load::reading reading;
    reading.dev.name = "foo";
    reading.dev.origin = "origin is here";
    reading.value = 2400;
    reading.time = to_time(2022, 4, 23, 19, 18, 17);
    data.push_back(reading);
    reading.value = 600;
    reading.time = to_time(2022, 4, 23, 19, 20, 21);
    data.push_back(reading);

    const auto file_name = "storage-append-existing-load.db";

    db store;
    // save first part of data
    const auto opt = store.save(data, file_name);
    REQUIRE_FALSE( opt.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    const auto size_one = std::filesystem::file_size(file_name);
    REQUIRE( size_one > 0 );

    // prepare data for append operation
    data.clear();
    reading.dev.name = "bar";
    reading.dev.origin = "somewhere else";
    reading.value = 4321;
    reading.time = to_time(2022, 4, 23, 20, 19, 18);
    data.push_back(reading);
    reading.value = 1234;
    reading.time = to_time(2022, 4, 23, 22, 23, 24);
    data.push_back(reading);
    // Generate more data to force resize.
    for (unsigned int i = 1; i < 60; ++i)
    {
      reading.dev.origin.append(" abc");
      reading.value += 1;
      reading.time = to_time(2022, 4, 23, 22, 24, i);
      data.push_back(reading);
    }

    const auto opt_append = store.save(data, file_name);
    REQUIRE_FALSE( opt_append.has_value() );
    REQUIRE( std::filesystem::exists(file_name) );

    const auto size_two = std::filesystem::file_size(file_name);

    REQUIRE( std::filesystem::remove(file_name) );

    // Append means that the file size increased.
    REQUIRE( size_one < size_two );
  }
}

TEST_CASE("db storage: load thermal data")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<thermos::thermal::reading> data;
    db store;
    const auto opt = store.load(data, "/path/may-not/exist/for-real.db");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "thermal-file-is-not-sqlite3-retrieve.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;temperature;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;temperature;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    std::vector<thermos::thermal::reading> data;
    db store;
    const auto opt = store.load(data, file_name);
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Failed") != std::string::npos );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("normal read operation")
  {
    thermal::reading reading_one;
    reading_one.dev.name = "foo_therm";
    reading_one.dev.origin = "origin is here";
    reading_one.value = 42000;
    reading_one.time = to_time(2022, 4, 23, 19, 18, 17);
    thermal::reading reading_two;
    reading_two.dev.name = "foo_therm";
    reading_two.dev.origin = "origin is here";
    reading_two.value = 60000;
    reading_two.time = to_time(2022, 4, 23, 19, 20, 21);

    const auto file_name = "storage-normal-thermal-retrieve.db";
    {
      // Save thermal data.
      std::vector<thermos::thermal::reading> data;
      data.push_back(reading_one);
      data.push_back(reading_two);
      db store;
      const auto opt = store.save(data, file_name);
      REQUIRE_FALSE( opt.has_value() );
      // Add some CPU load data (should not be retrieved later).
      std::vector<thermos::load::reading> data2;
      load::reading reading_other;
      reading_other.dev.name = "foo";
      reading_other.dev.origin = "origin is here";
      reading_other.value = 2400;
      reading_other.time = to_time(2022, 4, 23, 19, 19, 19);
      data2.push_back(reading_other);
      const auto opt2 = store.save(data2, file_name);
      REQUIRE_FALSE( opt2.has_value() );
    }

    // Load data from file.
    std::vector<thermos::thermal::reading> data;
    db store;
    const auto opt = store.load(data, file_name);
    REQUIRE_FALSE( opt.has_value() );

    REQUIRE( std::filesystem::remove(file_name) );

    REQUIRE( data.size() == 2 );
    // Check first value.
    REQUIRE( reading_one.dev.name == data[0].dev.name );
    REQUIRE( reading_one.dev.origin == data[0].dev.origin );
    REQUIRE( reading_one.value == data[0].value );
    REQUIRE( reading_one.time == data[0].time );
    // Check second value.
    REQUIRE( reading_two.dev.name == data[1].dev.name );
    REQUIRE( reading_two.dev.origin == data[1].dev.origin );
    REQUIRE( reading_two.value == data[1].value );
    REQUIRE( reading_two.time == data[1].time );
  }
}

TEST_CASE("db storage: load CPU load data")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<thermos::load::reading> data;

    db store;
    const auto opt = store.load(data, "/path/may-not/exist/for-real-load.db");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "load-file-is-not-sqlite3-retrieve.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;temperature;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;temperature;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    std::vector<thermos::load::reading> data;
    db store;
    const auto opt = store.load(data, file_name);
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Failed") != std::string::npos );

    REQUIRE( std::filesystem::remove(file_name) );
  }

  SECTION("normal read operation")
  {
    load::reading reading_one;
    reading_one.dev.name = "foo";
    reading_one.dev.origin = "origin is here";
    reading_one.value = 2400;
    reading_one.time = to_time(2022, 4, 23, 19, 18, 17);
    load::reading reading_two;
    reading_two.dev.name = "foo";
    reading_two.dev.origin = "origin is here";
    reading_two.value = 600;
    reading_two.time = to_time(2022, 4, 23, 19, 20, 21);

    const auto file_name = "storage-normal-load-retrieve.db";
    {
      // Save CPU load data.
      std::vector<thermos::load::reading> data;
      data.push_back(reading_one);
      data.push_back(reading_two);
      db store;
      const auto opt = store.save(data, file_name);
      REQUIRE_FALSE( opt.has_value() );
      // Add some thermal data (should not be retrieved later).
      std::vector<thermos::thermal::reading> data2;
      thermal::reading reading_other;
      reading_other.dev.name = "foo2";
      reading_other.dev.origin = "origin was here";
      reading_other.value = 24000;
      reading_other.time = to_time(2022, 4, 23, 19, 19, 19);
      data2.push_back(reading_other);
      const auto opt2 = store.save(data2, file_name);
      REQUIRE_FALSE( opt2.has_value() );
    }

    // Load data from file.
    std::vector<thermos::load::reading> data;
    db store;
    const auto opt = store.load(data, file_name);
    REQUIRE_FALSE( opt.has_value() );

    REQUIRE( std::filesystem::remove(file_name) );

    REQUIRE( data.size() == 2 );
    // Check first value.
    REQUIRE( reading_one.dev.name == data[0].dev.name );
    REQUIRE( reading_one.dev.origin == data[0].dev.origin );
    REQUIRE( reading_one.value == data[0].value );
    REQUIRE( reading_one.time == data[0].time );
    // Check second value.
    REQUIRE( reading_two.dev.name == data[1].dev.name );
    REQUIRE( reading_two.dev.origin == data[1].dev.origin );
    REQUIRE( reading_two.value == data[1].value );
    REQUIRE( reading_two.time == data[1].time );
  }
}

TEST_CASE("db storage: load device list")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    std::vector<thermos::device> data;

    db store;
    const auto opt = store.get_devices(data, thermos::reading_type::load, "/path/may-not/exist/for-real-load.db");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "get-devices-file-is-not-sqlite3-retrieve.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;temperature;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;temperature;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    std::vector<thermos::device> data;
    db store;
    const auto opt = store.get_devices(data, thermos::reading_type::load, file_name);
    REQUIRE( std::filesystem::remove(file_name) );
    REQUIRE( opt.has_value() );
  }

  SECTION("normal query")
  {
    load::reading reading_one;
    reading_one.dev.name = "foo";
    reading_one.dev.origin = "origin is here";
    reading_one.value = 2400;
    reading_one.time = to_time(2022, 4, 23, 19, 18, 17);
    load::reading reading_two;
    reading_two.dev.name = "bar";
    reading_two.dev.origin = "baz is here";
    reading_two.value = 600;
    reading_two.time = to_time(2022, 4, 23, 19, 20, 21);

    const auto file_name = "storage-normal-get-devices.db";
    {
      // Save some CPU load data.
      std::vector<thermos::load::reading> data;
      data.push_back(reading_one);
      data.push_back(reading_two);
      db store;
      const auto opt = store.save(data, file_name);
      REQUIRE_FALSE( opt.has_value() );
    }

    {
      // Save some temperature data.
      std::vector<thermos::thermal::reading> data;
      thermal::reading reading_one;
      reading_one.dev.name = "foo's thermal sibling";
      reading_one.dev.origin = "origin was here";
      reading_one.value = 25000;
      reading_one.time = to_time(2022, 4, 23, 19, 18, 17);
      data.push_back(reading_one);
      reading_one.time = to_time(2022, 4, 23, 20, 19, 18);
      data.push_back(reading_one);
      db store;
      const auto opt = store.save(data, file_name);
      REQUIRE_FALSE( opt.has_value() );
    }

    // Load devices from file.
    std::vector<thermos::device> data;
    db store;
    const auto opt = store.get_devices(data, thermos::reading_type::load, file_name);
    REQUIRE_FALSE( opt.has_value() );

    REQUIRE( data.size() == 2 );
    // Check first value.
    // (Values are sorted by device name, so 2nd device is 1st now.)
    REQUIRE( reading_two.dev.name == data[0].name );
    REQUIRE( reading_two.dev.origin == data[0].origin );
    // Check second value.
    REQUIRE( reading_one.dev.name == data[1].name );
    REQUIRE( reading_one.dev.origin == data[1].origin );

    // Load thermal devices from file.
    const auto opt_thermal = store.get_devices(data, thermos::reading_type::temperature, file_name);
    REQUIRE_FALSE( opt_thermal.has_value() );

    REQUIRE( data.size() == 1 );
    // Check first value.
    REQUIRE( "foo's thermal sibling" == data[0].name );
    REQUIRE( "origin was here" == data[0].origin );

    REQUIRE( std::filesystem::remove(file_name) );
  }
}

TEST_CASE("db storage: get_device_id")
{
  using namespace thermos;
  using namespace thermos::storage;

  SECTION("file cannot be opened / created")
  {
    device dev;
    dev.name = "foo";
    dev.origin = "bar";
    db store;
    const auto id = store.get_device_id(dev, "/path/may-not/exist/for-real/devices.db");
    REQUIRE_FALSE( id.has_value() );
    REQUIRE( id.error().find("Could not open") != std::string::npos );
  }

  SECTION("file is not in SQLite 3 format")
  {
    const auto file_name = "get-device-id_file-is-not-sqlite3.db";
    {
      std::ofstream stream(file_name);
      REQUIRE( stream.good() );
      stream << "foo;origin is here;temperature;42000;2022-04-23 19:18:17\n";
      stream << "foo;origin is here;temperature;60000;2022-04-23 19:20:21\n";
      REQUIRE( stream.good() );
      stream.close();
    }

    device dev;
    dev.name = "foo";
    dev.origin = "bar";
    db store;
    const auto id = store.get_device_id(dev, file_name);
    REQUIRE( std::filesystem::remove(file_name) );
    REQUIRE_FALSE( id.has_value() );
  }

  SECTION("normal query")
  {
    load::reading reading_one;
    reading_one.dev.name = "foo";
    reading_one.dev.origin = "bar";
    reading_one.value = 2400;
    reading_one.time = to_time(2022, 4, 23, 19, 18, 17);
    load::reading reading_two;
    reading_two.dev.name = "bar";
    reading_two.dev.origin = "baz is here";
    reading_two.value = 600;
    reading_two.time = to_time(2022, 4, 23, 19, 20, 21);

    const auto file_name = "storage-normal-get-device-id.db";
    {
      // Save some CPU load data.
      std::vector<thermos::load::reading> data;
      data.push_back(reading_one);
      data.push_back(reading_two);
      db store;
      const auto opt = store.save(data, file_name);
      REQUIRE_FALSE( opt.has_value() );
    }

    // Get first device id from file.
    {
      device dev_one;
      dev_one.name = "foo";
      dev_one.origin = "bar";
      db store;
      const auto id_one = store.get_device_id(dev_one, file_name);
      REQUIRE( id_one.has_value() );
      REQUIRE( id_one.value() == 1 );
    }
    // Get second device id from file.
    {
      device dev_two;
      dev_two.name = "bar";
      dev_two.origin = "baz is here";
      db store;
      const auto id_two = store.get_device_id(dev_two, file_name);
      REQUIRE( id_two.has_value() );
      REQUIRE( id_two.value() == 2 );
    }
    // Get device id zero for non-existing device.
    {
      device dev_none;
      dev_none.name = "nothing";
      dev_none.origin = "here";
      db store;
      const auto id_none = store.get_device_id(dev_none, file_name);
      REQUIRE( id_none.has_value() );
      REQUIRE( id_none.value() == 0 );
    }

    REQUIRE( std::filesystem::remove(file_name) );
  }
}
#endif // SQLite feature guard
