/*
 -------------------------------------------------------------------------------
    This file is part of thermos.
    Copyright (C) 2022, 2023  Dirk Stolle

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
  const std::vector<std::chrono::hours> intervals = {
    std::chrono::hours(48),       // two days
    std::chrono::hours(7 * 24),   // seven days / one week
    std::chrono::hours(30 * 24),  // 30 days / one month
    std::chrono::hours(365 * 24)  // one year
  };
  for (const auto& time_span: intervals)
  {
    const std::string base_name = "graph_" + get_short_name(time_span) + ".html";
    const auto opt = generate_plot(db_file_name, tpl, time_span, intervals,
                                   output_directory / base_name);
    if (opt.has_value())
    {
      return opt;
    }
  }

  return std::nullopt;
}

nonstd::expected<std::string, std::string> generate_header(Template& tpl)
{
  if (!tpl.load_section("link_with_integrity"))
  {
    return nonstd::make_unexpected("Failed to load section 'link_with_integrity' from template.");
  }
  tpl.tag("url", "https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css");
  tpl.tag("hash", "sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65");
  auto scripts = tpl.generate().value();

  if (!tpl.load_section("script_with_integrity"))
  {
    return nonstd::make_unexpected("Failed to load section 'script_with_integrity' from template.");
  }
  tpl.tag("url", "https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.min.js");
  tpl.tag("hash", "sha384-cuYeSxntonz0PPNlHhBs68uyIAVpIIOZZ5JqeqvYYIcEL727kskC66kF92t6Xl2V");
  scripts += tpl.generate().value();

  if (!tpl.load_section("script"))
  {
    return "Failed to load section 'script' from template.";
  }
  tpl.tag("url", "https://cdn.plot.ly/plotly-basic-2.26.1.min.js");
  scripts += tpl.generate().value();

  if (!tpl.load_section("header"))
  {
    return nonstd::make_unexpected("Failed to load section 'header' from template.");
  }
  tpl.tag("title", "Temperature data plot");
  tpl.integrate("scripts", scripts);
  return tpl.generate().value();
}

std::optional<std::string> generate_plot(const std::string& db_file_name, Template& tpl,
                                         const std::chrono::hours time_span,
                                         const std::vector<std::chrono::hours>& all_time_spans,
                                         const std::filesystem::path& output)
{
  const auto header = generate_header(tpl);
  if (!header.has_value())
  {
    return header.error();
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
  tpl.tag("title", "Data of the last " + get_human_readable_span(time_span));
  tpl.integrate("traces", traces);
  const std::string graph = tpl.generate().value();

  const auto nav = generate_navigation(tpl, time_span, all_time_spans);
  if (!nav.has_value())
  {
    return nav.error();
  }

  if (!tpl.load_section("full"))
  {
    return "Failed to load section 'full' from template.";
  }
  tpl.integrate("header", header.value());
  tpl.integrate("content", nav.value() + graph);
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

nonstd::expected<std::string, std::string>
generate_navigation(Template& tpl, const std::chrono::hours current_time_span,
                    const std::vector<std::chrono::hours>& all_time_spans)
{
  if (!tpl.load_section("nav_item"))
  {
    return nonstd::make_unexpected("Failed to load section 'nav_item' from template.");
  }
  std::string items;
  for (const auto& time_span: all_time_spans)
  {
    const auto short_name = get_short_name(time_span);
    const std::string base_name = "graph_" + short_name + ".html";
    tpl.tag("url", base_name);
    tpl.tag("name", short_name);
    const bool active = (time_span == current_time_span);
    tpl.tag("active", active ? " active" : "");
    items += tpl.generate().value();
  }

  if (!tpl.load_section("navigation"))
  {
    return nonstd::make_unexpected("Failed to load section 'navigation' from template.");
  }
  tpl.integrate("items", items);
  return tpl.generate().value();
}

std::string get_human_readable_span(const std::chrono::hours time_span)
{
  if (time_span < std::chrono::hours(24))
  {
    if (time_span.count() == 1)
    {
      return "1 hour";
    }
    return std::to_string(time_span.count()) + " hours";
  }

  if (time_span < std::chrono::hours(365 * 24))
  {
    const auto days = time_span.count() / 24;
    auto str = std::to_string(days) + ((days == 1 ) ? " day" : " days");
    const auto mod = time_span.count() % 24;
    if (mod != 0)
    {
      str += " " + get_human_readable_span(std::chrono::hours(mod));
    }
    return str;
  }

  const auto years = time_span.count() / (365 * 24);
  auto str = std::to_string(years) + ((years == 1) ? " year" : " years");
  const auto remainder = time_span.count() % (365 * 24);
  if (remainder != 0)
  {
    str += " " + get_human_readable_span(std::chrono::hours(remainder));
  }
  return str;
}

std::string get_short_name(const std::chrono::hours time_span)
{
  if (time_span < std::chrono::hours(24))
  {
    return std::to_string(time_span.count()) + "h";
  }

  if (time_span < std::chrono::hours(365 * 24))
  {
    auto str = std::to_string(time_span.count() / 24) + "d";
    const auto mod = time_span.count() % 24;
    if (mod != 0)
    {
      str += " " + std::to_string(mod) + "h";
    }
    return str;
  }

  auto str = std::to_string(time_span.count() / (365 * 24)) + "y";
  const auto remainder = time_span.count() % (365 * 24);
  if (remainder != 0)
  {
    str += " " + get_short_name(std::chrono::hours(remainder));
  }
  return str;
}

} // namespace
