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
#include <iostream>
#include <regex>

namespace thermos
{

bool Template::load_from_str(std::string content)
{
  if (content.empty())
  {
    return false;
  }

  std::regex pattern("<!--section-start::(.*?)-->(.*?)<!--section-end::(.*?)-->");
  // TODO!
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

} // namespace
