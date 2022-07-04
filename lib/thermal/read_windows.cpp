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

#include "read_windows.hpp"
#if defined(_WIN32) || defined(_WIN64)
#define _WIN32_DCOM
#include <comdef.h>
#include <wbemidl.h>
#if defined(_MSC_VER)
#pragma comment(lib, "wbemuuid.lib")
#endif
#if defined(__STDC_LIB_EXT1__)
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <cwchar>
#include <vector>

namespace thermos::windows::thermal
{

std::string better_narrowing(const wchar_t* wc_str)
{
    if (nullptr == wc_str)
        return std::string();

    std::mbstate_t state = std::mbstate_t();
    std::size_t len = static_cast<std::size_t>(-1);
    const auto error = wcsrtombs_s(&len, nullptr, 0, &wc_str, 0, &state);
    if (error != 0)
    {
        throw std::invalid_argument("UTF-16 string cannot be narrowed to UTF-8!");
    }
    std::vector<char> vec_string(len);
    wcsrtombs_s(&len, vec_string.data(), vec_string.size(), &wc_str, vec_string.size(), &state);
    return std::string(vec_string.data(), vec_string.size() - 1);
}

nonstd::expected<std::vector<thermos::thermal::device_reading>, std::string> read_wmi()
{
  // Initialize COM. (Yes, WMI uses COM.)
  HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hr))
  {
    return nonstd::make_unexpected("Failed to initialize COM library. "
      + std::string("Error code is ") + std::to_string(hr) + ".");
  }

  // Initialize COM security level.
  hr = CoInitializeSecurity(
         nullptr,  // no security descriptor
         -1,       // let COM to choose which authentication services to register
         nullptr,  // no predefined authentication services provided
         nullptr, // reserved, must always be null
         RPC_C_AUTHN_LEVEL_DEFAULT, // default: use security blanket negotiation
         RPC_C_IMP_LEVEL_IMPERSONATE, // default impersonation level for proxies
         nullptr, // empty authentication list
         EOAC_NONE, // no additional authentication capabilities requested
         nullptr // reserved, too, must always be null
  );
  if (FAILED(hr))
  {
    CoUninitialize();
    return nonstd::make_unexpected("Failed to initialize security context. "
      + std::string("Error code is ") + std::to_string(hr) + ".");
  }

  // Create locator for WMI.
  IWbemLocator* pLocator = nullptr;
  hr = CoCreateInstance(
         CLSID_WbemLocator, // CLSID for data and code that will be used to
                            // create the object
         nullptr, // pointer to aggregate object when used as part of an
                  // aggregate, which is not the case here
         CLSCTX_INPROC_SERVER, // context in which the code will run;
                               // CLSCTX_INPROC_SERVER = same process as the
                               // caller of the function
         IID_IWbemLocator, // reference to the identifier of the interface to
                           // be used to communicate with the object
         (LPVOID*) &pLocator // pointer to object (if call is successful)
  );
  if (FAILED(hr))
  {
    CoUninitialize();
    return nonstd::make_unexpected("Failed to create IWbemLocator. "
      + std::string("Error code is ") + std::to_string(hr) + ".");
  }

  // Connect to WMI through the IWbemLocator::ConnectServer method.
  IWbemServices* pService = nullptr;
  // Connect to the root\cimv2 namespace with the current user and obtain a
  // pointer to make IWbemServices calls.
  hr = pLocator->ConnectServer(
         _bstr_t(L"ROOT\\WMI"), // path of WMI namespace
         nullptr, // user name; nullptr = current user
         nullptr, // user password; nullptr = current security context
         nullptr, // locale; nullptr = current locale
         WBEM_FLAG_CONNECT_USE_MAX_WAIT, // flags;
           // WBEM_FLAG_CONNECT_USE_MAX_WAIT = wait up to two minutes. Without
           // this flag the application may hang indefinitely, if the server
           // is not responding / broken.
         nullptr, // authority (e. g. Kerberos)
         nullptr, // IWbemContext object
         &pService // pointer to IWbemServices proxy
  );
  if (FAILED(hr))
  {
    pLocator->Release();
    CoUninitialize();
    return nonstd::make_unexpected("Could not connect to WMI namespace. "
      + std::string("Error code is ") + std::to_string(hr) + ".");
  }

  // Set security levels on proxy.
  hr = CoSetProxyBlanket(
         pService, // the proxy
         RPC_C_AUTHN_WINNT, // authentication service: NTLMSSP
         RPC_C_AUTHZ_NONE,  // authorization service: none is the only one which
                            // is available for NTLMSSP authentication service
         nullptr, // server principal name
         RPC_C_AUTHN_LEVEL_CALL, // authentication level; RPC_C_AUTHN_LEVEL_CALL
                                 // means only authenticate at the beginning of
                                 // an RPC call
         RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation level;
           // RPC_C_IMP_LEVEL_IMPERSONATE = server process can impersonate the
           // client's security context while acting on behalf of the client
         nullptr, // client identity; nullptr = current proxy identity
         EOAC_NONE // proxy capabilities; EOAC_NONE = none
  );
  if (FAILED(hr))
  {
    pService->Release();
    pLocator->Release();
    CoUninitialize();
    return nonstd::make_unexpected("Could not set proxy blanket. "
      + std::string("Error code is ") + std::to_string(hr) + ".");
  }

  // Use the IWbemServices pointer to make requests to WMI.
  IEnumWbemClassObject* pEnumerator = nullptr;
  hr = pService->ExecQuery(
         _bstr_t(L"WQL"), // query language; must be "WQL" for WMI Query Language
         _bstr_t(L"SELECT * FROM MSAcpi_ThermalZoneTemperature"),
         // flags for query behaviour:
         //   WBEM_FLAG_FORWARD_ONLY = return forward-only enumerator, which is
         //                            faster than other enumerators;
         //   WBEM_FLAG_RETURN_IMMEDIATELY = semi-synchronous call
         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
         nullptr, // context; documentation says "typically NULL"
         &pEnumerator // enumerator
  );
  if (FAILED(hr))
  {
    pService->Release();
    pLocator->Release();
    CoUninitialize();
    return nonstd::make_unexpected("WMI query failed. "
      + std::string("Error code is ") + std::to_string(hr) + ".");
  }

  // Get the data from the query.
  IWbemClassObject* pObject = nullptr;
  ULONG uReturn = 0;
  std::vector<thermos::thermal::device_reading> result;
  while (pEnumerator)
  {
    pObject = nullptr;
    hr = pEnumerator->Next(
           WBEM_INFINITE, // timeout: wait until data is available
           1, // number of requested objects
           &pObject, // storage
           &uReturn // number of objects actually returned
    );
    if(0 == uReturn)
    {
      break;
    }

    VARIANT property;
    VariantInit(&property);
    // Get the value of the InstanceName property
    hr = pObject->Get(
           L"InstanceName", // name of the property
           0, // reserved, must be zero
           &property, // variant to store the data
           nullptr, // pointer to type information
           nullptr // pointer to origin information
    );
    if (FAILED(hr))
    {
      pObject->Release();
      pEnumerator->Release();
      pService->Release();
      pLocator->Release();
      CoUninitialize();
      return nonstd::make_unexpected("Failed to get instance name from WMI result. "
        + std::string("Error code is ") + std::to_string(hr) + ".");
    }

    thermos::thermal::device_reading d_reading;
    d_reading.dev.name = better_narrowing(property.bstrVal);
    VariantClear(&property);

    hr = pObject->Get(
           L"CurrentTemperature", // name of the property
           0, // reserved, must be zero
           &property, // variant to store the data
           nullptr, // pointer to type information
           nullptr // pointer to origin information
    );
    if (FAILED(hr))
    {
      pObject->Release();
      pEnumerator->Release();
      pService->Release();
      pLocator->Release();
      CoUninitialize();
      return nonstd::make_unexpected("Failed to get current temperature from WMI result. "
        + std::string("Error code is ") + std::to_string(hr) + ".");
    }
    // Temperature is given in tenths of Kelvins as VT_I4 in lVal, but reading
    // value is expected to be in thousands of degrees Celsius (millicelsius).
    d_reading.reading.value = static_cast<int64_t>(property.lVal) * 100 - 273200;
    VariantClear(&property);
    pObject->Release();
    d_reading.dev.origin = std::string("ROOT\\WMI:MSAcpi_ThermalZoneTemperature:").append(d_reading.dev.name);
    d_reading.reading.time = std::chrono::system_clock::now();
    result.emplace_back(d_reading);
  }

  pEnumerator->Release();
  pService->Release();
  pLocator->Release();
  CoUninitialize();

  return result;
}

nonstd::expected<std::vector<thermos::thermal::device_reading>, std::string> read_all()
{
  return read_wmi();
}

} // namespace

#endif // Windows
