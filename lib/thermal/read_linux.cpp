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

#include "read_linux.hpp"
#if defined(__linux__) || defined(linux)
#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>

namespace thermos::linux_like::thermal
{

nonstd::expected<std::string, std::string> first_line(const std::filesystem::path& path)
{
  std::ifstream stream(path);
  if (!stream.good())
    return nonstd::make_unexpected("Cannot open file " + path.string() + ".");

  std::string line;
  if (!std::getline(stream, line))
    return nonstd::make_unexpected("Cannot read from file " + path.string() + ".");

  return line;
}

nonstd::expected<thermos::thermal::reading, std::string> read_thermal_device(const std::filesystem::path& device_directory)
{
  const std::filesystem::path type (device_directory / "type");
  std::error_code error;
  if (!std::filesystem::exists(type, error) || error)
    return nonstd::make_unexpected("File " + type.string() + " does not exist.");
  const std::filesystem::path temperature (device_directory / "temp");
  if (!std::filesystem::exists(temperature, error) || error)
    return nonstd::make_unexpected("File " + temperature.string() + " does not exist.");

  thermos::thermal::reading result;
  const auto maybe_name = first_line(type);
  if (!maybe_name.has_value())
    return nonstd::make_unexpected(maybe_name.error());
  result.dev.name = maybe_name.value();
  const auto maybe_temperature = first_line(temperature);
  if (!maybe_temperature.has_value())
    return nonstd::make_unexpected(maybe_temperature.error());
  try
  {
    std::size_t pos = 0;
    result.value = std::stoll(maybe_temperature.value(), &pos);
    if (pos < maybe_temperature.value().size())
      return nonstd::make_unexpected("File " + temperature.native() + " did not contain an integer value.");
  }
  catch (const std::exception& ex)
  {

    return nonstd::make_unexpected(ex.what());
  }
  result.dev.origin = temperature.native();
  result.time = std::chrono::system_clock::now();

  return result;
}

nonstd::expected<std::vector<thermos::thermal::reading>, std::string> read_all()
{
  auto thermal_devs = read_thermal();
  if (!thermal_devs.has_value())
    return thermal_devs;

  auto hwmon_devs = read_hwmon();
  if (!hwmon_devs.has_value())
    return hwmon_devs;

  thermal_devs.value().insert(thermal_devs.value().end(),
      std::make_move_iterator(hwmon_devs.value().begin()),
      std::make_move_iterator(hwmon_devs.value().end()));
  return thermal_devs;
}

nonstd::expected<std::vector<thermos::thermal::reading>, std::string> read_thermal()
{
  const std::filesystem::path thermal{"/sys/devices/virtual/thermal"};

  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(thermal, error);
  if (error)
    return nonstd::make_unexpected("Cannot iterate over directory " + thermal.native() + ".");

  std::vector<thermos::thermal::reading> readings;
  for (const auto& entry: iterator)
  {
    if (entry.is_directory(error) && !error)
    {
      auto reading = read_thermal_device(entry.path());
      if (reading.has_value())
      {
        readings.emplace_back(reading.value());
      }
    }
  }

  return readings;
}

nonstd::expected<std::vector<thermos::thermal::reading>, std::string> read_hwmon_devices(const std::filesystem::path& device_directory)
{
  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(device_directory, error);
  if (error)
    return nonstd::make_unexpected("Cannot iterate over directory " + device_directory.native() + ".");

  const std::regex label_exp("temp([0-9]+)_label");
  std::vector<thermos::thermal::reading> result;
  for (const auto& entry: iterator)
  {
    if (!entry.is_regular_file(error) || error)
    {
      continue;
    }

    const auto fn = entry.path().filename().native();
    // Only matching names are allowed, e. g. "temp0_label".
    if (!std::regex_match(fn, label_exp))
    {
      continue;
    }
    // Check whether related input file exists, e. g. "temp0_input".
    const auto input_name = fn.substr(0, fn.find("_label")) + "_input";
    auto path_input{entry.path()};
    path_input.replace_filename(input_name);
    if (!std::filesystem::exists(path_input, error) || error)
    {
      continue;
    }

    thermos::thermal::reading reading;
    const auto maybe_name = first_line(entry.path());
    if (!maybe_name.has_value())
      return nonstd::make_unexpected(maybe_name.error());
    reading.dev.name = maybe_name.value();
    const auto maybe_temperature = first_line(path_input);
    if (!maybe_temperature.has_value())
      return nonstd::make_unexpected(maybe_temperature.error());
    try
    {
      std::size_t pos = 0;
      reading.value = std::stoll(maybe_temperature.value(), &pos);
      if (pos < maybe_temperature.value().size())
        return nonstd::make_unexpected("File " + path_input.native() + " did not contain an integer value.");
    }
    catch (const std::exception& ex)
    {
      return nonstd::make_unexpected(ex.what());
    }
    reading.dev.origin = path_input.native();
    reading.time = std::chrono::system_clock::now();
    result.emplace_back(reading);
  }

  return result;
}

nonstd::expected<std::vector<thermos::thermal::reading>, std::string> read_hwmon()
{
  const std::filesystem::path hwmon{"/sys/class/hwmon"};

  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(hwmon, error);
  if (error)
    return nonstd::make_unexpected("Cannot iterate over directory " + hwmon.string() + ".");

  std::vector<thermos::thermal::reading> readings;
  for (const auto& entry: iterator)
  {
    if (entry.is_directory(error) && !error)
    {
      auto reads = read_hwmon_devices(entry.path() / "device");
      if (!reads.has_value())
        return nonstd::make_unexpected(reads.error());
      for(const auto& elem: reads.value())
      {
        readings.emplace_back(elem);
      }
      // Some kernel versions have the tempN_label and tempN_input files
      // directly in /sys/class/hwmon/hwmonN instead of the sub directory
      // /sys/class/hwmon/hwmonN/device.
      reads = read_hwmon_devices(entry.path());
      if (!reads.has_value())
        return nonstd::make_unexpected(reads.error());
      for(const auto& elem: reads.value())
      {
        readings.emplace_back(elem);
      }
    }
  }

  return readings;
}

} // namespace

#endif // Linux
