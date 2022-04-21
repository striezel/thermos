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
//#if defined(__linux__) || defined(linux)
#include <filesystem>
#include <fstream>

namespace thermos::linux_like
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

nonstd::expected<thermos::device, std::string> read_thermal_device(const std::filesystem::path& device_directory)
{
  const std::filesystem::path type (device_directory / "type");
  std::error_code error;
  if (!std::filesystem::exists(type, error) || error)
    return nonstd::make_unexpected("File " + type.string() + " does not exist.");
  const std::filesystem::path temperature (device_directory / "temp");
  if (!std::filesystem::exists(temperature, error) || error)
    return nonstd::make_unexpected("File " + temperature.string() + " does not exist.");

  thermos::device result;
  const auto maybe_type = first_line(type);
  if (!maybe_type.has_value())
    return nonstd::make_unexpected(maybe_type.error());
  result.type = maybe_type.value();
  const auto maybe_temperature = first_line(temperature);
  if (!maybe_temperature.has_value())
    return nonstd::make_unexpected(maybe_temperature.error());
  try
  {
    std::size_t pos = 0;
    result.millicelsius = std::stoll(maybe_temperature.value(), &pos);
    if (pos < maybe_temperature.value().size())
      return nonstd::make_unexpected("File " + temperature.string() + " did not contain an integer value.");
  }
  catch (const std::exception& ex)
  {

    return nonstd::make_unexpected(ex.what());
  }

  return result;
}

nonstd::expected<std::vector<device>, std::string> read_all()
{
  const auto thermal_devs = read_thermal();
  if (thermal_devs.has_value() && !thermal_devs.value().empty())
    return thermal_devs;

  return read_hwmon();
}

nonstd::expected<std::vector<device>, std::string> read_thermal()
{
  const std::filesystem::path thermal{"/sys/devices/virtual/thermal"};

  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(thermal, error);
  if (error)
    return nonstd::make_unexpected("Cannot iterate over directory " + thermal.string() + ".");

  std::vector<thermos::device> devices;
  for (const auto& entry: iterator)
  {
    if (entry.is_directory(error) && !error)
    {
      auto dev = read_thermal_device(entry.path());
      if (dev.has_value())
      {
        devices.emplace_back(dev.value());
      }
    }
  }

  return devices;
}

nonstd::expected<std::vector<device>, std::string> read_hwmon_devices(const std::filesystem::path& device_directory)
{
  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(device_directory, error);
  if (error)
    return nonstd::make_unexpected("Cannot iterate over directory " + device_directory.string() + ".");

  std::vector<thermos::device> result;
  for (const auto& entry: iterator)
  {
    if (!entry.is_regular_file(error) || error)
    {
      continue;
    }

    const auto fn = entry.path().filename().generic_string();
    if ((fn.find("temp") != 0) || (fn.find("_label") <= 4))
    {
      continue;
    }

    thermos::device dev;
    const auto maybe_type = first_line(entry.path());
    if (!maybe_type.has_value())
      return nonstd::make_unexpected(maybe_type.error());
    dev.type = maybe_type.value();
    const auto input_name = fn.substr(0, fn.find("_label")) + "_input";
    auto path_mutable{entry.path()};
    const auto maybe_temperature = first_line(path_mutable.replace_filename(input_name));
    if (!maybe_temperature.has_value())
      return nonstd::make_unexpected(maybe_temperature.error());
    try
    {
      std::size_t pos = 0;
      dev.millicelsius = std::stoll(maybe_temperature.value(), &pos);
      if (pos < maybe_temperature.value().size())
        return nonstd::make_unexpected("File " + path_mutable.string() + " did not contain an integer value.");
    }
    catch (const std::exception& ex)
    {
      return nonstd::make_unexpected(ex.what());
    }
    result.emplace_back(dev);
  }

  return result;
}

nonstd::expected<std::vector<device>, std::string> read_hwmon()
{
  const std::filesystem::path hwmon{"/sys/class/hwmon"};

  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(hwmon, error);
  if (error)
    return nonstd::make_unexpected("Cannot iterate over directory " + hwmon.string() + ".");

  std::vector<thermos::device> devices;
  for (const auto& entry: iterator)
  {
    if (entry.is_directory(error) && !error)
    {
      auto devs = read_hwmon_devices(entry.path() / "device");
      if (!devs.has_value())
        return nonstd::make_unexpected(devs.error());
      for(const auto& elem: devs.value())
      {
        devices.emplace_back(elem);
      }
      // Some kernel versions have the tempN_label and tempN_input files
      // directly in /sys/class/hwmon/hwmonN instead of the sub directory
      // /sys/class/hwmon/hwmonN/device.
      devs = read_hwmon_devices(entry.path());
      if (!devs.has_value())
        return nonstd::make_unexpected(devs.error());
      for(const auto& elem: devs.value())
      {
        devices.emplace_back(elem);
      }
    }
  }

  return devices;
}

} // namespace

//#endif // Linux
