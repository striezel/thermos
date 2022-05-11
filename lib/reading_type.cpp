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

#include "reading_type.hpp"
#include <stdexcept>

namespace thermos
{

std::string to_string(const reading_type& t)
{
  switch (t)
  {
    case reading_type::temperature:
         return "temperature";
    case reading_type::load:
         return "load";
    default:
         throw std::invalid_argument("Invalid reading_type value in to_string!");
  }
}

std::ostream& operator<<(std::ostream& os, const reading_type& t)
{
  return os << to_string(t);
}

} // namespace
