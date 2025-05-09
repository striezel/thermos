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
#include "../../lib/load/read.hpp"
#include "../../lib/thermal/read.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showVersion()
{
  thermos::GitInfos info;
  std::cout << "thermos-info, " << thermos::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << '\n';
  thermos::showLicenseInformation();
}

void showHelp()
{
  std::cout << "thermos-info [OPTIONS]\n"
            << "\n"
            << "Displays thermal sensor data and CPU load average.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help       - Shows this help message.\n"
            << "  -v | --version    - Shows version information.\n";
}

int main(int argc, char** argv)
{
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
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters.\n";
        return thermos::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  const auto thermal_readings = thermos::thermal::read_all();
  if (!thermal_readings.has_value() || thermal_readings.value().empty())
  {
    std::cout << "No temperature readings available!\n";
    if (!thermal_readings.has_value())
      std::cout << thermal_readings.error() << '\n';
  }
  else
  {
    std::cout << "Temperature data:\n";
    for (const auto& reading: thermal_readings.value())
    {
      std::cout << "Device '" << reading.dev.name << "': " << reading.reading.celsius() << " °C\n"
                << "  (from " << reading.dev.origin << ")\n";
    }
  }

  const auto load_readings = thermos::load::read_all();
  if (!load_readings.has_value() || load_readings.value().empty())
  {
    std::cout << "No CPU load data available!\n";
    if (!load_readings.has_value())
      std::cout << load_readings.error() << '\n';
  }
  else
  {
    std::cout << "\nCPU load:\n";
    for (const auto& reading: load_readings.value())
    {
      std::cout << "Device '" << reading.dev.name << "': " << reading.reading.percent() << " %\n"
                << "  (from " << reading.dev.origin << ")\n";
    }
  }

  return 0;
}
