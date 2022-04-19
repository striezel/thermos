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

thermos::device read_single_device(const std::filesystem::path& device_directory)
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
      auto dev = read_single_device(entry.path());
      if (dev.filled())
      {
        devices.emplace_back(dev);
      }
    }
  }

  return devices;
}

} // namespace

#endif // Linux
