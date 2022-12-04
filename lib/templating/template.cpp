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

#include "template.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include "htmlspecialchars.hpp"

namespace thermos
{

const std::string Template::TAG_OPENER = "{{";
const std::string Template::TAG_CLOSER = "}}";
const std::string Template::INTEGRATE_OPENER = "{{>";
const std::string Template::INTEGRATE_CLOSER = "}}";

Template::Template()
: sections({}),
  tags({}),
  includes({}),
  tpl(std::nullopt)
{
}

bool Template::load_from_file(const std::filesystem::path& file_name)
{
  std::ifstream stream(file_name, std::ios::in | std::ios::binary);
  if (!stream.good())
    return false;
  std::string content;
  std::getline(stream, content,
               std::string::traits_type::to_char_type(std::string::traits_type::eof()));
  if (!stream)
    return false;
  return load_from_str(content);
}

bool Template::load_from_str(const std::string& content)
{
  if (content.empty())
  {
    return false;
  }

  std::regex pattern("<!--section-start::(.*?)-->((.|\\r|\\n)*?)<!--section-end::(.*?)-->");
  sections.clear();
  auto iter = std::sregex_iterator(content.begin(), content.end(), pattern);
  const auto iter_end = std::sregex_iterator();
  for (auto i = iter; i != iter_end; ++i)
  {
    const auto match = *i;
    // Section start and section end do not match.
    if (match[1].str() != match[4].str())
    {
      continue;
    }
    sections[match[1].str()] = match[2].str();
  }

  // Clear previously set tags and template, because they are not really valid
  // anymore.
  tags.clear();
  includes.clear();
  tpl = std::nullopt;

  return !sections.empty();
}

bool Template::load_section(const std::string& section_name)
{
  const auto iter = sections.find(section_name);
  if (iter == sections.end())
  {
    std::cerr << "There is no section '" << section_name << "'." << std::endl;
    return false;
  }

  tpl = iter->second;
  return true;
}

void Template::tag(const std::string& name, const std::string& replacement)
{
  tags[name] = replacement;
}

void Template::integrate(const std::string& name, const std::string& replacement)
{
  includes[name] = replacement;
}

std::optional<std::string> Template::generate() const
{
  if (!tpl.has_value())
  {
    return std::nullopt;
  }

  std::string out(tpl.value());
  for (const auto& [name, value]: tags)
  {
    const auto needle = TAG_OPENER + name + TAG_CLOSER;
    std::string::size_type pos = 0;
    while ((pos = out.find(needle, pos)) != std::string::npos)
    {
      out.replace(pos, needle.size(), htmlspecialchars(value));
      pos += value.size();
    }
  }
  for (const auto& [name, value]: includes)
  {
    const auto needle = INTEGRATE_OPENER + name + INTEGRATE_CLOSER;
    std::string::size_type pos = 0;
    while ((pos = out.find(needle, pos)) != std::string::npos)
    {
      out.replace(pos, needle.size(), value);
      pos += value.size();
    }
  }

  return out;
}

} // namespace
