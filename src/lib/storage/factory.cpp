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

#include "factory.hpp"
#include "csv.hpp"
#include "db.hpp"

namespace thermos::storage
{

std::unique_ptr<store> factory::create(const type t)
{
  switch (t)
  {
    case type::db:
         return std::make_unique<db>();
    case type::csv:
         return std::make_unique<csv>();
    default:
         // Any future unsupported type returns a null pointer.
         return nullptr;
  }
}

} // namespace
