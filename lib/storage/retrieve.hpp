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

#include <chrono>
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
    virtual std::optional<std::string> load(std::vector<thermos::thermal::device_reading>& data, const std::string& file_name) = 0;


    /** \brief Loads CPU load readings from a file.
     *
     * \param data        the vector where the device readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    virtual std::optional<std::string> load(std::vector<thermos::load::device_reading>& data, const std::string& file_name) = 0;


    /** \brief Loads all available devices (NOT their readings) from a file.
     *
     * \param data        the vector where the devices shall be stored
     * \param type        type of the device's readings (thermal or load data)
     * \param file_name   the file from which the data shall be loaded
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    virtual std::optional<std::string> get_devices(std::vector<thermos::device>& data, const thermos::reading_type type, const std::string& file_name) = 0;


    /** \brief Loads readings of a devices from a file.
     *
     * \param dev         the device for which the readings shall be retrieved
     * \param data        the vector where the readings shall be stored
     * \param file_name   the file from which the data shall be loaded
     * \param time_span   the time span from which the data shall be included;
     *                    Settings this to e. g. two hours will retrieve the
     *                    data from the latest time up to two hours back.
     * \return Returns an empty optional, if the data was read successfully.
     *         Returns an error message otherwise.
     */
    virtual std::optional<std::string> get_device_readings(const thermos::device& dev, std::vector<load::reading>& data, const std::string& file_name, const std::chrono::hours time_span) = 0;
    virtual std::optional<std::string> get_device_readings(const thermos::device& dev, std::vector<thermal::reading>& data, const std::string& file_name, const std::chrono::hours time_span) = 0;
};

} // namespace

#endif // THERMOS_STORAGE_RETRIEVE_HPP
