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

#ifndef THERMOS_GENERATE_TRACES_HPP
#define THERMOS_GENERATE_TRACES_HPP

#include <chrono>
#include <string>
#include <type_traits>
#include "../../third-party/nonstd/expected.hpp"
#include "../../lib/storage/db.hpp"
#include "../../lib/templating/template.hpp"
#include "../../lib/templating/vectorize.hpp"

namespace thermos
{

/** \brief Generates HTML code containing trace data for the plot.
 *
 * \param read_t        reading type, e.g. thermal::reading or load::reading
 * \param db_file_name  path to the SQLite database file
 *                      (Note: This file should have been created with the
                         thermos-logger program.)
 * \param tpl           a loaded template for graph generation
 * \param time_span     amount of time to cover in the generated graph
 * \param y_axis        y-axis configuration for the traces for use by plotly,
 *                      e. g. "yaxis: 'y2'," when mapping to the second y-axis
 * \return Returns an empty optional, if graph generation was successful.
 *         Returns an optional containing an error message otherwise.
 */
template<typename read_t>
nonstd::expected<std::string, std::string> generate_traces(const std::string& db_file_name, Template& tpl,
                                                           const std::chrono::hours time_span, const std::string& y_axis)
{
  static_assert(std::is_base_of<thermos::reading_base, read_t>::value,
                "read_t must be a reading type based on thermos::reading_base.");

  if (!tpl.load_section("trace"))
  {
    return nonstd::make_unexpected("Failed to load section 'trace' from template.");
  }

  std::string traces;
  storage::db the_db;
  std::vector<device> devs;
  auto opt = the_db.get_devices(devs, read_t().type(), db_file_name);
  if (opt.has_value())
  {
    return nonstd::make_unexpected(opt.value());
  }
  for (const auto& dev: devs)
  {
    std::vector<read_t> readings;
    opt = the_db.get_device_readings(dev, readings, db_file_name, time_span);
    if (opt.has_value())
    {
      return nonstd::make_unexpected(opt.value());
    }
    const auto vec_data = vectorize(readings);
    if (!vec_data.has_value())
    {
      return nonstd::make_unexpected(vec_data.error());
    }
    tpl.integrate("dates", vec_data.value().dates);
    tpl.integrate("values", vec_data.value().values);
    tpl.integrate("yaxis", y_axis);
    tpl.tag("name", dev.name);

    traces += tpl.generate().value();
  }

  return traces;
}

} // namespace

#endif // THERMOS_GENERATE_TRACES_HPP
