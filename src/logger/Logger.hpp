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

#include <optional>
#include <string>
#include "../../lib/storage/type.hpp"

namespace thermos
{

/** \brief Handles the thermal data logging process.
 */
class Logger
{
  public:
    /** \brief Creates a new instance.
     *
     * \param fileName   path of the file where the data shall be logged
     * \param fileType   the file type to use (CSV or SQLite 3 database)
     */
    Logger(const std::string& fileName, const storage::type fileType);

    /** \brief Starts data logging.
     *
     * \return Returns an empty optional in case of success.
     *         Returns an error message, if an error occurred.
     * \remarks Note that this method currently only returns in the failure
     *          case. If it is successful, it keeps running indefinitely.
     */
    std::optional<std::string> log();
  private:
    std::string file_name;
    storage::type file_type;
}; // class

} // namespace
