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

#include <filesystem>
#include <iostream>
#if !defined(THERMOS_NO_SQLITE)
#include <sqlite3.h>
#endif
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "generator.hpp"

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
            << "SQLite " << sqlite3_libversion() << '\n';
  #else
            << "Note: This version was built without SQLite support, so the SQLite-related\n"
            << "features are not available.\n";
  #endif
  thermos::showLicenseInformation();
}

void showHelp()
{
  std::cout << "thermos-graph-generator [OPTIONS]\n"
            << "\n"
            << "Generates graphs from logged data.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help               - Shows this help message.\n"
            << "  -v | --version            - Shows version information.\n"
            << "  -f FILE | --file FILE     - Sets the file name of the log file to use to\n"
            << "                              generate the graphs. This must be a database\n"
            << "                              file (SQLite) and not a CSV file.\n"
            << "  -t FILE | --template FILE - Sets the file name of the template file to use\n"
            << "                              to generate the graphs.\n"
            << "  -o DIR | --output DIR     - Sets the destination of the generated files to\n"
            << "                              the directory DIR.\n";
}

int check_directory(const std::filesystem::path& destination)
{
  try
  {
    if (!std::filesystem::exists(destination))
    {
      if (!std::filesystem::create_directories(destination))
      {
        std::cerr << "Error: Directory " << destination << " could not be created.\n";
        return thermos::rcInputOutputFailure;
      }
    }
    else
    {
      if (!std::filesystem::is_directory(destination))
      {
        std::cerr << "Error: " << destination << " is not a directory.\n";
        return thermos::rcInputOutputFailure;
      }
    }
  }
  catch(const std::filesystem::filesystem_error& ex)
  {
    std::cerr << "Error: " << ex.code().message() << "\n";
    return thermos::rcInputOutputFailure;
  }

  return 0;
}

int main(int argc, char** argv)
{
  std::string logFile;
  std::string templateFile;
  std::filesystem::path destination;

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
          std::cerr << "Error: Log file was already set to " << logFile
                    << "!\n";
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
                    << param << "\".\n";
          return thermos::rcInvalidParameter;
        }
      } // if log file
      else if ((param == "--template") || (param == "-t"))
      {
        if (!templateFile.empty())
        {
          std::cerr << "Error: Template file was already set to "
                    << templateFile << "!\n";
          return thermos::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          templateFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param << "\".\n";
          return thermos::rcInvalidParameter;
        }
      } // if template file
      else if ((param == "--output") || (param == "-o"))
      {
        if (!destination.empty())
        {
          std::cerr << "Error: Output file was already set to " << destination
                    << "!\n";
          return thermos::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          destination = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param << "\".\n";
          return thermos::rcInvalidParameter;
        }
      } // if output file
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters.\n";
        return thermos::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  if (logFile.empty())
  {
    std::cerr << "Error: You have to specify a log file to get the data from.\n"
              << "Use the parameter --file to specify a file, e.g.\n\n"
              << "    thermos-graph-generator --file data.db ...\n";
    return thermos::rcInvalidParameter;
  }

  if (templateFile.empty())
  {
    std::cerr << "Error: You have to specify a template file for graph generation.\n"
              << "Use the parameter --template to specify a file, e.g.\n\n"
              << "    thermos-graph-generator --template graph.tpl ...\n";
    return thermos::rcInvalidParameter;
  }

  if (destination.empty())
  {
    std::cerr << "Error: You have to specify an output path for graph generation.\n"
              << "Use the parameter --output to specify a file, e.g.\n\n"
              << "    thermos-graph-generator --output graph.html ...\n";
    return thermos::rcInvalidParameter;
  }

  thermos::Template tpl;
  if (!tpl.load_from_file(templateFile))
  {
    std::cerr << "Failed to load template from " << templateFile << "!\n";
    return thermos::rcInputOutputFailure;
  }

  if (int code = check_directory(destination))
  {
    return code;
  }

  const auto opt = thermos::generate(logFile, tpl, destination);
  if (opt.has_value())
  {
    std::cerr << "Error: Template generation failed!\n" << opt.value() << "\n";
    return thermos::rcInputOutputFailure;
  }

  return 0;
}
