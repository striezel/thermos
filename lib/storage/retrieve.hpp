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

#ifndef THERMOS_STORAGE_RETRIEVE_HPP
#define THERMOS_STORAGE_RETRIEVE_HPP

#include <optional>
#include <string>
#include <vector>
#include "../../third-party/nonstd/expected.hpp"
#include "../load/reading.hpp"
#include "../thermal/reading.hpp"

namespace thermos::storage
{

/** \brief Interface for retrieving device readings from a file or database.
 */
class retrieve
{
  public:
    /** \brief Virtual destructor, because of derived classes.
     */
    virtual ~retrieve() = default;

    /** \brief Loads device readings from a file.
     *
     * \param data        the vector where the device readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    virtual std::optional<std::string> load(std::vector<thermos::thermal::reading>& data, const std::string& file_name) = 0;


    /** \brief Loads CPU load readings from a file.
     *
     * \param data        the vector where the device readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    virtual std::optional<std::string> load(std::vector<thermos::load::reading>& data, const std::string& file_name) = 0;
};

} // namespace

#endif // THERMOS_STORAGE_RETRIEVE_HPP