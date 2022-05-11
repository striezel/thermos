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

#ifndef THERMOS_STORAGE_CSV_HPP
#define THERMOS_STORAGE_CSV_HPP

#include "store.hpp"
#include <fstream>
#include "utilities.hpp"

namespace thermos::storage
{

/** \brief Class for storing device readings as CSV in a file.
 */
class csv: public store
{
  public:
    /** \brief Saves device readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<thermos::thermal::reading>& data, const std::string& file_name) final
    {
      return save_impl(data, file_name);
    }

    /** \brief Saves CPU load readings to a file.
     *
     * \param data        the device readings that shall be stored
     * \param file_name   the file to which the data shall be saved
     * \return Returns an empty optional, if the data was written successfully.
     *         Returns an error message otherwise.
     */
    std::optional<std::string> save(const std::vector<thermos::load::reading>& data, const std::string& file_name) final
    {
      return save_impl(data, file_name);
    }

  private:
    template<typename T>
    std::optional<std::string> save_impl(const std::vector<T>& data, const std::string& file_name)
    {
      const char separator = ';';

      std::ofstream stream(file_name, std::ios_base::out | std::ios::app);
      if (!stream.good())
      {
        return "Failed to create or open file " + file_name + ".";
      }

      for(const auto& reading: data)
      {
        const auto time_string = time_to_string(reading.time);
        if (!time_string.has_value())
        {
          return time_string.error();
        }
        stream << reading.dev.name << separator << reading.dev.origin << separator
               << reading.type() << separator << reading.value << separator
               << time_string.value() << "\n";
      }
      stream.flush();
      stream.close();
      if (!stream.good())
      {
        return "Writing to " + file_name + " failed.";
      }
      return std::nullopt;
    }
};

} // namespace

#endif // THERMOS_STORAGE_CSV_HPP
