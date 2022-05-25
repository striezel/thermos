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

#ifndef THERMOS_LOAD_CALCULATOR_HPP
#define THERMOS_LOAD_CALCULATOR_HPP

#if defined(_WIN32) || defined(_WIN64)
#include <chrono>
#include <cstdint>
#include <string>
#include "../../third-party/nonstd/expected.hpp"
#include "reading.hpp"

namespace thermos::load
{

class calculator
{
  public:
    calculator();


    /** \brief Determines whether this is a fresh instance, i. e. whether current() has not been called on it yet.
     *
     * \return Returns true, if this is a fresh instance.
     */
    bool fresh() const;


    /** \brief Gets the average load since the last time this method was called.
     *
     * \return Returns a load readings, if successful.
     *         Returns an error message, if no reading was available.
     */
    nonstd::expected<thermos::load::device_reading, std::string> current();


    /** \brief Gets the average load over a given time span, blocking for that time.
     *
     * \param ms   the minimum amount of milliseconds to wait; actual wait can
     *             be longer, because system timers may not be precise enough or
     *             because the system is busy with other threads
     * \return Returns a load readings, if successful.
     *         Returns an error message, if no reading was available.
     */
    nonstd::expected<thermos::load::device_reading, std::string> current(const std::chrono::milliseconds ms);
  private:
    std::uint64_t previous_idle;
    std::uint64_t previous_total;
}; // class

} // namespace
#endif // Windows

#endif // THERMOS_LOAD_CALCULATOR_HPP
