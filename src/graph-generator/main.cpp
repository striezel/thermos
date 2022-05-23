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
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showVersion()
{
  thermos::GitInfos info;
  std::cout << "thermos-graph-generator, " << thermos::version << "\n"
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
  std::cout << "thermos-graph-generator [OPTIONS]\n"
            << "\n"
            << "Generates graphs from logged data.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -f FILE | --file FILE  - Sets the file name of the log file to use to\n"
            << "                            generate the graphs.\n";
}

int main(int argc, char** argv)
{
  std::string logFile;

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
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return thermos::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  std::cout << "Not implemented yet!" << std::endl;
  return 0;
}
