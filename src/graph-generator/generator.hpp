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
#include <vector>
#include "../../third-party/nonstd/expected.hpp"
#include "../../lib/templating/template.hpp"

namespace thermos
{

/** \brief Generates the HTML file containing the plots in the given directory.
 *
 * \param db_file_name  path to the SQLite database file
 *                      (Note: This file should have been created with the
                         thermos-logger program.)
 * \param tpl                 a loaded template for graph generation
 * \param output_directory    path of directory where to save the generated files
 * \return Returns an empty optional, if graph generation was successful.
 *         Returns an optional containing an error message otherwise.
 */
std::optional<std::string> generate(const std::string& db_file_name, Template& tpl,
                                    const std::filesystem::path& output_directory);


/** \brief Generates a single HTML file containing the plot.
 *
 * \param db_file_name  path to the SQLite database file
 *                      (Note: This file should have been created with the
                         thermos-logger program.)
 * \param tpl           a loaded template for graph generation
 * \param time_span     amount of time to cover in the generated graph
 * \param all_time_spans container with all time spans in the navigation
 * \param output        path where to save the generated file
 * \return Returns an empty optional, if graph generation was successful.
 *         Returns an optional containing an error message otherwise.
 */
std::optional<std::string> generate_plot(const std::string& db_file_name, Template& tpl,
                                         const std::chrono::hours time_span,
                                         const std::vector<std::chrono::hours>& all_time_spans,
                                         const std::filesystem::path& output);

/** \brief Generates the navigation for a single HTML file.
 *
 * \param tpl            a loaded template for graph generation
 * \param time_span      amount of time to cover in the generated graph
 * \param all_time_spans container with all time spans in the navigation
 * \return Returns the HTML code, if navigation generation was successful.
 *         Returns an error message otherwise.
 */
nonstd::expected<std::string, std::string>
generate_navigation(Template& tpl, const std::chrono::hours current_time_span,
                    const std::vector<std::chrono::hours>& all_time_spans);

/** \brief Gets a short name for a time span, e. g. "2d" for two days / 48 hours.
 *
 * \param time_span      amount of time
 * \return Returns the short name for the time span.
 */
std::string get_short_name(const std::chrono::hours time_span);

} // namespace

#endif // THERMOS_GENERATOR_HPP
