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

std::string first_line(const std::filesystem::path& path)
{
  std::ifstream stream(path);
  if (!stream.good())
    return "";

  std::string line;
  if (!std::getline(stream, line))
    return "";

  return line;
}

thermos::device read_thermal_device(const std::filesystem::path& device_directory)
{
  const std::filesystem::path type (device_directory / "type");
  std::error_code error;
  if (!std::filesystem::exists(type, error) || error)
    return thermos::device();
  const std::filesystem::path temperature (device_directory / "temp");
  if (!std::filesystem::exists(temperature, error) || error)
    return thermos::device();

  thermos::device result;
  result.type = first_line(type);
  const auto temp = first_line(temperature);
  try
  {
    std::size_t pos = 0;
    result.millicelsius = std::stoll(temp, &pos);
    if (pos < temp.size())
      return thermos::device();
  }
  catch (const std::exception& ex)
  {
    return thermos::device();
  }

  return result;
}

std::vector<thermos::device> read_all()
{
  const auto thermal_devs = read_thermal();
  if (!thermal_devs.empty())
    return thermal_devs;

  return read_hwmon();
}

std::vector<thermos::device> read_thermal()
{
  const std::filesystem::path thermal{"/sys/devices/virtual/thermal"};

  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(thermal, error);
  if (error)
    return { };

  std::vector<thermos::device> devices;
  for (const auto& entry: iterator)
  {
    if (entry.is_directory(error) && !error)
    {
      auto dev = read_thermal_device(entry.path());
      if (dev.filled())
      {
        devices.emplace_back(dev);
      }
    }
  }

  return devices;
}

std::vector<thermos::device> read_hwmon_devices(const std::filesystem::path& device_directory)
{
  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(device_directory, error);
  if (error)
    return { };

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
    dev.type = first_line(entry.path());
    const auto input_name = fn.substr(0, fn.find("_label")) + "_input";
    auto path_mutable{entry.path()};
    const auto temp = first_line(path_mutable.replace_filename(input_name));
    try
    {
      std::size_t pos = 0;
      dev.millicelsius = std::stoll(temp, &pos);
      if (pos < temp.size())
        continue;
    }
    catch (const std::exception& ex)
    {
      return { };
    }
    result.emplace_back(dev);
  }

  return result;
}

std::vector<thermos::device> read_hwmon()
{
  const std::filesystem::path thermal{"/sys/class/hwmon"};

  std::error_code error;
  const auto iterator = std::filesystem::directory_iterator(thermal, error);
  if (error)
    return { };

  std::vector<thermos::device> devices;
  for (const auto& entry: iterator)
  {
    if (entry.is_directory(error) && !error)
    {
      auto devs = read_hwmon_devices(entry.path() / "device");
      for(const auto& elem: devs)
      {
        devices.emplace_back(elem);
      }
      // Some kernel versions have the tempN_label and tempN_input files
      // directly in /sys/class/hwmon/hwmonN instead of the sub directory
      // /sys/class/hwmon/hwmonN/device.
      devs = read_hwmon_devices(entry.path());
      for(const auto& elem: devs)
      {
        devices.emplace_back(elem);
      }
    }
  }

  return devices;
}

} // namespace

//#endif // Linux
