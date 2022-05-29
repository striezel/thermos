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

#ifndef THERMOS_TEMPLATING_VECTORIZE_HPP
#define THERMOS_TEMPLATING_VECTORIZE_HPP

#include <string>
#include <vector>
#include "../../third-party/nonstd/expected.hpp"
#include "../load/reading.hpp"
#include "../thermal/reading.hpp"

namespace thermos
{

struct vectorized_data
{
  vectorized_data();

  std::string dates;
  std::string values;
};

/** \brief Converts the reading data to JSON array.
 *
 * \param data   the data to transform to JSON
 * \return Returns a structure containing JSON-ified data in case of success.
 *         Returns an error message otherwise.
 */
nonstd::expected<vectorized_data, std::string> vectorize(const std::vector<load::reading>& data);
nonstd::expected<vectorized_data, std::string> vectorize(const std::vector<thermal::reading>& data);

} // namespace

#endif // THERMOS_TEMPLATING_VECTORIZE_HPP
