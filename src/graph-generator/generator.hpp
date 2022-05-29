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

#ifndef THERMOS_GENERATOR_HPP
#define THERMOS_GENERATOR_HPP

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include "../../lib/templating/template.hpp"

namespace thermos
{

/** \brief Generates a HTML file containing the plot.
 *
 * \param db_file_name  path to the SQLite database file
 *                      (Note: This file should have been created with the
                         thermos-logger program.)
 * \param tpl           a loaded template for graph generation
 * \param time_span     amount of time to cover in the generated graph
 * \param output        path where to save the generated file
 * \return Returns an empty optional, if graph generation was successful.
 *         Returns an optional containing an error message otherwise.
 */
std::optional<std::string> generate(const std::string& db_file_name, Template& tpl,
                                    const std::chrono::hours time_span, const std::filesystem::path& output);

} // namespace

#endif // THERMOS_GENERATOR_HPP
