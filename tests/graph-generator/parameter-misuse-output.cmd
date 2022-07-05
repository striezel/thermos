:: Script to test wrong values of parameter `--output`.
::
::  Copyright (C) 2022  Dirk Stolle
::
::  This program is free software: you can redistribute it and/or modify
::  it under the terms of the GNU General Public License as published by
::  the Free Software Foundation, either version 3 of the License, or
::  (at your option) any later version.
::
::  This program is distributed in the hope that it will be useful,
::  but WITHOUT ANY WARRANTY; without even the implied warranty of
::  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
::  GNU General Public License for more details.
::
::  You should have received a copy of the GNU General Public License
::  along with this program.  If not, see <http://www.gnu.org/licenses/>.

@echo off

:: 1st parameter = executable path
if "%1" EQU "" (
  echo First parameter must be executable file!
  exit /B 1
)
SET EXECUTABLE=%1
:: 2nd parameter = HTML file path
if "%2" EQU "" (
  echo Second parameter must be HTML file!
  exit /B 1
)
SET HTML_FILE=%2
if NOT EXIST "%HTML_FILE%" (
  echo HTML file does not exist!
  exit /B 1
)

:: multiple occurrences of parameter
"%EXECUTABLE%" --output foo.html --output foo.html --template foo.tpl --file foo.db
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1.
  exit /B 1
)

:: missing file name after `--output`
"%EXECUTABLE%" --file foo.db --template foo.tpl --output
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1.
  exit /B 1
)

:: `--output` not specified
"%EXECUTABLE%" --file foo.db --template foo.tpl
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1.
  exit /B 1
)

:: output file already exists
"%EXECUTABLE%" --output "%HTML_FILE%" --template bar.tpl --file foo.db
if %ERRORLEVEL% NEQ 3 (
  echo Executable did not exit with code 3.
  exit /B 1
)

exit /B 0
