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

#ifndef THERMOS_RETURNCODES_HPP
#define THERMOS_RETURNCODES_HPP

namespace thermos
{

/** \brief exit code for invalid command line parameters */
constexpr int rcInvalidParameter = 1;

/** \brief exit code for failed or aborted logging */
constexpr int rcLoggingFailure = 2;

/** \brief exit code for failed I/O */
constexpr int rcInputOutputFailure = 3;

} // namespace

#endif // THERMOS_RETURNCODES_HPP
