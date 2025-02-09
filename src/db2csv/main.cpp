/*
 -------------------------------------------------------------------------------
    This file is part of thermos.
    Copyright (C) 2022, 2024, 2025  Dirk Stolle

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

#include <iostream>
#include <sqlite3.h>
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "db2csv.hpp"

void showVersion()
{
  thermos::GitInfos info;
  std::cout << "thermos-db2csv, " << thermos::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << "\n"
            << "\n"
            << "Libraries:\n"
            << "SQLite " << sqlite3_libversion() << '\n';
  thermos::showLicenseInformation();
}

void showHelp()
{
  std::cout << "thermos-db2csv [OPTIONS]\n"
            << "\n"
            << "Writes thermal sensor data and CPU load from an SQLite 3 file to a CSV file.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -f FILE | --file FILE  - Sets the file name of the SQLite 3 database to read.\n";
}

int main(int argc, char** argv)
{
  std::string dbFile;

  if ((argc > 1) && (argv != nullptr))
  {
    for (int i = 1; i < argc; ++i)
    {
      if (argv[i] == nullptr)
      {
        std::cerr << "Error: Parameter at index " << i << " is null pointer!\n";
        return thermos::rcInvalidParameter;
      }
      const std::string param(argv[i]);
      if ((param == "-v") || (param == "--version"))
      {
        showVersion();
        return 0;
      } // if version
      else if ((param == "-?") || (param == "/?") || (param == "--help"))
      {
        showHelp();
        return 0;
      } // if help
      else if ((param == "--file") || (param == "-f"))
      {
        if (!dbFile.empty())
        {
          std::cerr << "Error: Database file was already set to " << dbFile
                    << "!\n";
          return thermos::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          dbFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param << "\".\n";
          return thermos::rcInvalidParameter;
        }
      } // if database file
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters.\n";
        return thermos::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  // File path must be set, otherwise we cannot log to that file.
  if (dbFile.empty())
  {
    std::cerr << "Error: No path for the database file has been specified.\n"
              << "Use the --file parameter to specify the file location,"
              << " e. g. as in\n\n\tthermos-db2csv --file data.db\n\n"
              << "to read the data from the file data.db in the current directory.\n";
    return thermos::rcInvalidParameter;
  }

  return thermos::db2csv(dbFile);
}
