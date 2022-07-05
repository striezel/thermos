/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for thermos.
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

#include "../find_catch.hpp"
#include <filesystem>
#include <fstream>
#include "../../../lib/templating/template.hpp"

TEST_CASE("Template class")
{
  using namespace thermos;

  SECTION("new is empty")
  {
    Template tpl;

    REQUIRE( tpl.sections.empty() );
    REQUIRE( tpl.tags.empty() );
    REQUIRE( tpl.includes.empty() );
    REQUIRE_FALSE( tpl.tpl.has_value() );
  }

  SECTION("tag")
  {
    Template tpl;

    tpl.tag("foo", "bar");
    const auto it = tpl.tags.find("foo");
    REQUIRE( it != tpl.tags.end() );
    REQUIRE( it->second == "bar" );
  }

  SECTION("integrate")
  {
    Template tpl;

    tpl.integrate("foo", "<b>bar</b>");
    const auto it = tpl.includes.find("foo");
    REQUIRE( it != tpl.includes.end() );
    REQUIRE( it->second == "<b>bar</b>" );
  }

  SECTION("load_from_file: file does not exist")
  {
    Template tpl;
    REQUIRE_FALSE( tpl.load_from_file("/this/file/does-not/exist.tpl") );
  }

  SECTION("load_from_file: single section")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><li><a href=\"{{url}}\">{{text}}</a></li><!--section-end::test-->");
    const auto file_name = "load_from_file_single_section.tpl";
    {
      std::ofstream stream(file_name, std::ios::out | std::ios::binary);
      stream.write(simple_template.c_str(), simple_template.length());
      REQUIRE( stream.good() );
      stream.close();
    }

    REQUIRE( tpl.load_from_file(file_name) );
    REQUIRE( std::filesystem::remove(file_name) );
    REQUIRE( tpl.sections.size() == 1 );
    const auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<li><a href=\"{{url}}\">{{text}}</a></li>" );
  }

  SECTION("load_from_file: new lines")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li><!--section-end::test-->");
    const auto file_name = "load_from_file_new_lines.tpl";
    {
      std::ofstream stream(file_name, std::ios::out | std::ios::binary);
      stream.write(simple_template.c_str(), simple_template.length());
      REQUIRE( stream.good() );
      stream.close();
    }
    REQUIRE( tpl.load_from_file(file_name) );
    REQUIRE( std::filesystem::remove(file_name) );
    REQUIRE( tpl.sections.size() == 1 );
    const auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li>" );
  }

  SECTION("load_from_file: multiple sections with new lines")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li><!--section-end::test-->\n<!--section-start::foo--><b>Info\nFoo\r\nBar\rBaz:</b> {{info}}<!--section-end::foo-->");
    const auto file_name = "load_from_file_multiple_sections_with_new_lines.tpl";
    {
      std::ofstream stream(file_name, std::ios::out | std::ios::binary);
      stream.write(simple_template.c_str(), simple_template.length());
      REQUIRE( stream.good() );
      stream.close();
    }
    REQUIRE( tpl.load_from_file(file_name) );
    REQUIRE( std::filesystem::remove(file_name) );
    REQUIRE( tpl.sections.size() == 2 );
    auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li>" );
    it = tpl.sections.find("foo");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<b>Info\nFoo\r\nBar\rBaz:</b> {{info}}" );
  }

  SECTION("load_from_str: single section")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><li><a href=\"{{url}}\">{{text}}</a></li><!--section-end::test-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.sections.size() == 1 );
    const auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<li><a href=\"{{url}}\">{{text}}</a></li>" );
  }

  SECTION("load_from_str: new lines")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li><!--section-end::test-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.sections.size() == 1 );
    const auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li>" );
  }

  SECTION("load_from_str: multiple sections")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><a href=\"{{url}}\">{{text}}</a><!--section-end::test-->\n<!--section-start::foo--><b>Info:</b> {{info}}<!--section-end::foo-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.sections.size() == 2 );
    auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<a href=\"{{url}}\">{{text}}</a>" );
    it = tpl.sections.find("foo");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<b>Info:</b> {{info}}" );
  }

  SECTION("load_from_str: multiple sections with new lines")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li><!--section-end::test-->\n<!--section-start::foo--><b>Info\nFoo\r\nBar\rBaz:</b> {{info}}<!--section-end::foo-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.sections.size() == 2 );
    auto it = tpl.sections.find("test");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<li>\n  <a href=\"{{url}}\">{{text}}</a>\r\n</li>" );
    it = tpl.sections.find("foo");
    REQUIRE( it != tpl.sections.end() );
    REQUIRE( it->second == "<b>Info\nFoo\r\nBar\rBaz:</b> {{info}}" );
  }

  SECTION("load_section")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><a href=\"{{url}}\">{{text}}</a><!--section-end::test-->");
    REQUIRE( tpl.load_from_str(simple_template) );

    // Section "test" should load just fine.
    REQUIRE( tpl.load_section("test") );
    REQUIRE( tpl.tpl.has_value() );
    REQUIRE( tpl.tpl.value() == "<a href=\"{{url}}\">{{text}}</a>" );
    // Section "something_else" does not exist, it should not be loaded.
    REQUIRE_FALSE( tpl.load_section("something_else") );
  }

  SECTION("generate: simple example")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><a href=\"{{url}}\">{{text}}</a><!--section-end::test-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.load_section("test") );
    tpl.tag("url", "http://localhost/");
    tpl.tag("text", "home");
    REQUIRE( tpl.generate() == "<a href=\"http://localhost/\">home</a>");
  }

  SECTION("generate: special characters")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test--><a href=\"{{url}}\">{{text}}</a><!--section-end::test-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.load_section("test") );
    tpl.tag("url", "http://localhost/");
    tpl.tag("text", "<< foo & \"bar\" >>");
    // Text should be escaped properly.
    REQUIRE( tpl.generate() == "<a href=\"http://localhost/\">&lt;&lt; foo &amp; &quot;bar&quot; &gt;&gt;</a>");
  }

  SECTION("generate: with includes / integrate")
  {
    Template tpl;

    const std::string simple_template("<!--section-start::test-->Blah {{>info}}<!--section-end::test-->");
    REQUIRE( tpl.load_from_str(simple_template) );
    REQUIRE( tpl.load_section("test") );
    tpl.integrate("info", "<b>Info: &quot;none&quot;</b>");
    // Text from info should not be escaped.
    REQUIRE( tpl.generate() == "Blah <b>Info: &quot;none&quot;</b>");
  }
}
