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

#include "generator.hpp"
#include <fstream>
#include "../../lib/templating/template.hpp"
#include "generate_traces.hpp"

namespace thermos
{

std::optional<std::string> generate(const std::string& db_file_name, Template& tpl,
                                    const std::filesystem::path& output_directory)
{
  auto opt = generate_plot(db_file_name, tpl, std::chrono::hours(48), output_directory / "graph_48h.html");
  if (opt.has_value())
  {
    return opt;
  }
  opt = generate_plot(db_file_name, tpl, std::chrono::hours(7 * 24), output_directory / "graph_7d.html");
  if (opt.has_value())
  {
    return opt;
  }
  opt = generate_plot(db_file_name, tpl, std::chrono::hours(30 * 24), output_directory / "graph_30d.html");
  if (opt.has_value())
  {
    return opt;
  }
  return generate_plot(db_file_name, tpl, std::chrono::hours(365 * 24), output_directory / "graph_1y.html");
}

std::optional<std::string> generate_plot(const std::string& db_file_name, Template& tpl,
                                         const std::chrono::hours time_span, const std::filesystem::path& output)
{
  std::string header;
  {
    if (!tpl.load_section("script"))
    {
      return "Failed to load section 'script' from template.";
    }
    tpl.tag("path", "https://cdn.plot.ly/plotly-basic-2.12.1.min.js");
    const auto scripts = tpl.generate().value();

    if (!tpl.load_section("header"))
    {
      return "Failed to load section 'header' from template.";
    }
    tpl.tag("title", "Temperature data plot");
    tpl.integrate("scripts", scripts);
    header = tpl.generate().value();
  }

  auto maybe_traces = generate_traces<thermal::reading>(db_file_name, tpl, time_span, "yaxis: 'y2',");
  if (!maybe_traces)
  {
    return maybe_traces.error();
  }
  std::string traces {std::move(maybe_traces.value())};
  maybe_traces = generate_traces<load::reading>(db_file_name, tpl, time_span, "");
  if (!maybe_traces)
  {
    return maybe_traces.error();
  }
  traces += maybe_traces.value();

  if (!tpl.load_section("graph"))
  {
    return "Failed to load section 'graph' from template.";
  }
  tpl.tag("plotId", "id_1");
  tpl.tag("title", "Data of the last " + std::to_string(time_span.count()) + " hours");
  tpl.integrate("traces", traces);
  const std::string graph = tpl.generate().value();

  if (!tpl.load_section("full"))
  {
    return "Failed to load section 'full' from template.";
  }
  tpl.integrate("header", header);
  tpl.integrate("content", graph);
  const std::string full = tpl.generate().value();

  std::ofstream stream(output, std::ios::out | std::ios::binary | std::ios::trunc);
  stream.write(full.c_str(), full.length());
  if (!stream.good())
  {
    return "Failed to write generated template to file!";
  }
  stream.close();

  return std::nullopt;
}

} // namespace
