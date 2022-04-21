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

#include "read.hpp"
#if defined(__linux__) || defined(linux)
  #include "read_linux.hpp"
#endif

namespace thermos
{

nonstd::expected<std::vector<device>, std::string> read_all()
{
  #if defined(_WIN32) || defined(_WIN64)
    #warning There is no support for Windows OS yet.
    return nonstd::make_unexpected("There is no support for reading sensor data on Windows yet.");
  #elif defined(__linux__) || defined(linux)
    return thermos::linux_like::read_all();
  #else
    #error Unknown or unsupported operating system!
  #endif
}

} // namespace
