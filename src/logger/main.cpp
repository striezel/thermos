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

#include <iostream>
#if !defined(THERMOS_NO_SQLITE)
#include <sqlite3.h>
#endif
#include "../../lib/storage/type.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "Logger.hpp"

#if !defined(THERMOS_NO_SQLITE)
constexpr thermos::storage::type defaultFileType = thermos::storage::type::db;
#else
constexpr thermos::storage::type defaultFileType = thermos::storage::type::csv;
#endif

void showVersion()
{
  thermos::GitInfos info;
  std::cout << "thermos-logger, " << thermos::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << "\n"
            << "\n"
  #if !defined(THERMOS_NO_SQLITE)
            << "Libraries:\n"
            << "SQLite " << sqlite3_libversion() << std::endl;
  #else
            << "Note: This version was built without SQLite support, so the SQLite-related\n"
            << "features are not available." << std::endl;
  #endif
}

void showHelp()
{
  using namespace thermos::storage;

  std::cout << "thermos-logger [OPTIONS]\n"
            << "\n"
            << "Logs thermal sensor data and CPU load to a CSV or SQLite 3 file.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -f FILE | --file FILE  - Sets the file name of the log file to use during the\n"
            << "                           program run.\n"
            << "  -t TYPE | --type TYPE  - Sets the file type of the log file to use during the\n"
            << "                           program run. Allowed file types are:\n"
            << "                               " << type::csv << "\n"
            << "                               " << type::db << "\n"
            << "                           If the type is '" << type::csv << "', then the readings are stored\n"
            << "                           as character-separated values (CSV). If the type is\n"
            << "                           '" << type::db << "', then the readings are stored in a SQLite 3\n"
            << "                           database.\n"
            << "                           If no type is given, then '" << defaultFileType << "' is assumed.\n";
}

int main(int argc, char** argv)
{
  std::string logFile;
  std::optional<thermos::storage::type> fileType = std::nullopt;

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
        if (!logFile.empty())
        {
          std::cerr << "Error: Log file was already set to " << logFile << "!"
                    << std::endl;
          return thermos::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          logFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param <<"\"." << std::endl;
          return thermos::rcInvalidParameter;
        }
      } // if log file
      else if ((param == "--type") || (param == "-t"))
      {
        if (fileType.has_value())
        {
          std::cerr << "Error: File type was already set to "
                    << fileType.value() << "!" << std::endl;
          return thermos::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          fileType = thermos::storage::from_string(std::string(argv[i+1]));
          if (!fileType.has_value())
          {
            std::cerr << "Error: '" << std::string(argv[i+1]) << "' is not a "
                      << "valid file type.\nAllowed types are:\n"
                      << "\t" << thermos::storage::type::csv
                      << "\n\t" << thermos::storage::type::db
                      << "\nPlease use one of them.\n";
            return thermos::rcInvalidParameter;
          }
          #if defined(THERMOS_NO_SQLITE)
          if (fileType.value() == thermos::storage::type::db)
          {
            std::cerr << "Error: This program was compiled without support for"
                      << " SQLite 3 databases. Therefore you cannot use the "
                      << "file type " << thermos::storage::type::db
                      << " here.\n";
            return thermos::rcInvalidParameter;
          }
          #endif
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file type after \""
                    << param <<"\"." << std::endl;
          return thermos::rcInvalidParameter;
        }
      } // if file type
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return thermos::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  // File path must be set, otherwise we cannot log to that file.
  if (logFile.empty())
  {
    std::cerr << "Error: No path for the log file has been specified.\n"
              << "Use the --file parameter to specify the log file destination,"
              << " e. g. as in\n\n\tthermos-logger --file data.csv\n\n"
              << "to log the data to the file data.csv in the current directory."
              << std::endl;
    return thermos::rcInvalidParameter;
  }

  if (!fileType.has_value())
  {
    fileType = defaultFileType;
  }

  thermos::Logger logger(logFile, fileType.value());
  const auto opt = logger.log();
  if (opt.has_value())
  {
    std::cerr << "Error: Logging failed or was aborted.\n"
              << opt.value() << std::endl;
    return thermos::rcLoggingFailure;
  }

  return 0;
}
