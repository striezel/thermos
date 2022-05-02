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

#ifndef THERMOS_STORAGE_FACTORY_HPP
#define THERMOS_STORAGE_FACTORY_HPP

#include <memory>
#include "store.hpp"
#include "type.hpp"

namespace thermos::storage
{

/** \brief Factory for store instances.
 */
struct factory
{
  /** \brief Creates a store instance based on the given type.
   *
   * \param t     type of the store instance to create
   * \return Returns a unique_ptr to the created instance.
   *         Returns nullptr, if the type is not supported.
   */
  static std::unique_ptr<store> create(const type t);
};

} // namespace

#endif // THERMOS_STORAGE_FACTORY_HPP
