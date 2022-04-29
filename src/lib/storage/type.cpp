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

#include "type.hpp"

namespace thermos::storage
{

std::optional<type> from_string(const std::string& str)
{
  if (str == "csv")
    return type::csv;
  if (str == "db")
    return type::db;

  // No match.
  return std::nullopt;
}

std::ostream& operator<<(std::ostream& os, const type& t)
{
  switch(t)
  {
    case type::csv:
         os << "csv";
         break;
    case type::db:
         os << "db";
         break;
  }

  return os;
}

} // namespace
