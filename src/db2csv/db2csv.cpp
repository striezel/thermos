/*
 -------------------------------------------------------------------------------
    This file is part of thermos.
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

#include "db2csv.hpp"
#include <filesystem>
#include <iostream>
#include "../ReturnCodes.hpp"
#include "../../lib/load/reading.hpp"
#include "../../lib/thermal/reading.hpp"
#include "../../lib/storage/csv.hpp"
#include "../../lib/storage/db.hpp"

namespace thermos
{

int db2csv(const std::string& db_path)
{
  std::error_code error;
  if (!std::filesystem::exists(db_path, error) || error)
  {
    std::cerr << "Error: File " << db_path << " does not exist." << std::endl;
    return thermos::rcInputOutputFailure;
  }

  storage::db db;
  storage::csv csv;

  std::string destination;

  // thermal data
  {
    std::vector<thermal::reading> data;
    auto opt_error = db.load(data, db_path);
    if (opt_error.has_value())
    {
      std::cerr << "Could not load thermal sensor data from database "
                << db_path << "!\nError: " << opt_error.value() << "\n";
      return thermos::rcInputOutputFailure;
    }

    destination = csv_name(db_path);
    opt_error = csv.save(data, destination);
    if (opt_error.has_value())
    {
      std::cerr << "Could not write thermal sensor data to file "
                << destination << "!\nError: " << opt_error.value() << "\n";
      return thermos::rcInputOutputFailure;
    }
  }

  // CPU load data
  {
    std::vector<load::reading> data;
    auto opt_error = db.load(data, db_path);
    if (opt_error.has_value())
    {
      std::cerr << "Could not load CPU load data from database "
                << db_path << "!\nError: " << opt_error.value() << "\n";
      return thermos::rcInputOutputFailure;
    }

    opt_error = csv.save(data, destination);
    if (opt_error.has_value())
    {
      std::cerr << "Could not write CPU load data to file "
                << destination << "!\nError: " << opt_error.value() << "\n";
      return thermos::rcInputOutputFailure;
    }
  }

  std::cout << "Data from " << db_path << " was written to " << destination
            << "." << std::endl;
  return 0;
}

std::string csv_name(const std::string& db_path)
{
  namespace fs = std::filesystem;

  fs::path path(db_path);
  const auto ext = path.extension();
  const auto stem = path.stem();

  fs::path csv(stem.native() + std::string(".csv"));
  path.replace_filename(csv);
  std::error_code error;
  uint_least32_t counter = 0;
  while (fs::exists(path, error) && !error)
  {
    ++counter;
    csv = stem.native() + std::string("_") + std::to_string(counter) + std::string(".csv");
    path.replace_filename(csv);
  }
  return path;
}

} // namespace
