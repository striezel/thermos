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

#include <string>
#include <unordered_map>

namespace thermos
{

static const std::string TAG_OPENER = "{{";
static const std::string TAG_CLOSER = "}}";
static const std::string INTEGRATE_OPENER = "{{>";
static const std::string INTEGRATE_CLOSER = "}}";

class Template
{
  public:
    /**
     * Loads template from a string slice.
     *
     * @param content  content of the template
     * @return true, if template was loaded successfully; false otherwise
     */
    bool load_from_str(std::string content);


    /**
     * Loads a section of the template.
     *
     * @param section_name  name of the section to load
     * @return true if section was loaded, false otherwise
     */
    bool load_section(const std::string& section_name);


    /**
     * Sets the replacements text for a tag.
     * HTML entities in replacement are escaped during template generation.
     * Therefore it can also be used to show raw user input safely.
     *
     * @param name  name of the tag to replace
     * @param replacement the replacement
     */
    void tag(const std::string& name, const std::string& replacement);
  private:
    std::unordered_map<std::string, std::string> sections;
    std::unordered_map<std::string, std::string> tags;
    std::unordered_map<std::string, std::string> includes;
    std::optional<std::string> tpl;
};

} // namespace
