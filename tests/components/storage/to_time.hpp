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

#ifndef THERMOS_TEST_TO_TIME_HPP
#define THERMOS_TEST_TO_TIME_HPP

#include "../../../lib/thermal/reading.hpp"

thermos::thermal::reading::reading_time_t to_time(const int year, const unsigned short int month, const unsigned short int day,
                                                const unsigned short int hours, const unsigned short int minutes, const unsigned short int seconds);

#endif // THERMOS_TEST_TO_TIME_HPP
