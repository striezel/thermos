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
#include "../../lib/storage/db.hpp"
#include "../../lib/templating/template.hpp"
#include "../../lib/templating/vectorize.hpp"

namespace thermos
{

std::optional<std::string> generate(const std::string& db_file_name, Template& tpl,
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

  if (!tpl.load_section("trace"))
  {
    return "Failed to load section 'trace' from template.";
  }
  std::string traces;

  storage::db the_db;
  std::vector<device> devs;
  auto opt = the_db.get_devices(devs, reading_type::temperature, db_file_name);
  if (opt.has_value())
  {
    return opt;
  }
  for (const auto& dev: devs)
  {
    std::vector<thermal::reading> readings;
    opt = the_db.get_device_readings(dev, readings, db_file_name, time_span);
    if (opt.has_value())
    {
      return opt;
    }
    const auto vec_data = vectorize(readings);
    if (!vec_data.has_value())
    {
      return vec_data.error();
    }
    tpl.integrate("dates", vec_data.value().dates);
    tpl.integrate("values", vec_data.value().values);
    tpl.integrate("yaxis", "yaxis: 'y2',");
    tpl.tag("name", dev.name);

    traces += tpl.generate().value();
  }

  opt = the_db.get_devices(devs, reading_type::load, db_file_name);
  if (opt.has_value())
  {
    return opt;
  }
  for (const auto& dev: devs)
  {
    std::vector<load::reading> readings;
    opt = the_db.get_device_readings(dev, readings, db_file_name, time_span);
    if (opt.has_value())
    {
      return opt;
    }
    const auto vec_data = vectorize(readings);
    if (!vec_data.has_value())
    {
      return vec_data.error();
    }
    tpl.integrate("dates", vec_data.value().dates);
    tpl.integrate("values", vec_data.value().values);
    tpl.integrate("yaxis", "");
    tpl.tag("name", dev.name);

    traces += tpl.generate().value();
  }

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
