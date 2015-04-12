/****************************************************************************
 *                pvupdate.cpp
 *
 * This module implements update checking routines (as a DLL).
 *
 * Author: Christopher J. Cason.
 *
 * from Persistence of Vision(tm) Ray Tracer version 3.6.
 * Copyright 1991-2003 Persistence of Vision Team
 * Copyright 2003-2004 Persistence of Vision Raytracer Pty. Ltd.
 *---------------------------------------------------------------------------
 * NOTICE: This source code file is provided so that users may experiment
 * with enhancements to POV-Ray and to port the software to platforms other
 * than those supported by the POV-Ray developers. There are strict rules
 * regarding how you are permitted to use this file. These rules are contained
 * in the distribution and derivative versions licenses which should have been
 * provided with this file.
 *
 * These licences may be found online, linked from the end-user license
 * agreement that is located at http://www.povray.org/povlegal.html
 *---------------------------------------------------------------------------
 * This program is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 *---------------------------------------------------------------------------
 * $File: //depot/povray/3.5/windows/pvupdate.cpp $
 * $Revision: #4 $
 * $Change: 3212 $
 * $DateTime: 2004/10/25 01:50:23 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#pragma hdrstop

#pragma comment(lib, "wininet")

#define HTTPFLAGS       INTERNET_FLAG_NO_UI | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_PRAGMA_NOCACHE

#ifndef INTERNET_CONNECTION_CONFIGURED
#define INTERNET_CONNECTION_CONFIGURED      0x40
#define INTERNET_CONNECTION_OFFLINE         0x20
#define INTERNET_RAS_INSTALLED              0x10
#endif

#ifndef SM_CMONITORS
#define SM_CMONITORS 80
#endif

#ifndef SM_REMOTESESSION
#define SM_REMOTESESSION 0x1000
#endif

#ifdef _WIN64
#define SCRIPTPATH      "/cgi-bin/winupdate36-64"
#else
#define SCRIPTPATH      "/cgi-bin/winupdate36"
#endif

extern "C" __declspec (dllexport) int IsUpdateAvailable (bool, char *, char *, char *) ;
extern "C" __declspec (dllexport) bool InternetConnected (void) ;

#if 0
#pragma warning (disable: 4035)
inline unsigned __int64 GetCycleCount (void)
{
  _asm
  {
    _emit 0x0F
    _emit 0x31
  }
}
#pragma warning (default: 4035)

DWORD GetCPUSpeed (void)
{
	const unsigned __int64 ui64StartCycle = GetCycleCount () ;
	Sleep (1000) ;
	return static_cast<unsigned int> ((GetCycleCount () - ui64StartCycle) / 1000000) ;
}
#endif

bool reg_printf (char *keyName, char *valName, char *format, ...)
{
  char                  str [2048] ;
  HKEY                  hKey ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return (false) ;
  if (RegCreateKeyEx (HKEY_CURRENT_USER, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    va_start (arg_ptr, format) ;
    vsprintf (str, format, arg_ptr) ;
    va_end (arg_ptr) ;
    RegSetValueEx (hKey, valName, 0, REG_SZ, (BYTE *) str, (DWORD) strlen (str) + 1) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

static bool reg_dword (char *keyName, char *valName, DWORD value)
{
  HKEY                  hKey ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    RegSetValueEx (hKey, valName, 0, REG_DWORD, (BYTE *) &value, 4) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

char *GetInstallTime (void)
{
  HKEY        key ;
  DWORD       len ;
  static char str [64] ;

  len = sizeof (str) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\POV-Ray", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, "InstallTime", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
    {
      RegCloseKey (key) ;
      return (str) ;
    }
    RegCloseKey (key) ;
  }
  return (NULL) ;
}

bool InternetConnected (void)
{
  DWORD       flags ;

  BOOL result = InternetGetConnectedState (&flags, 0) ;
  if ((flags & INTERNET_CONNECTION_OFFLINE) != 0)
    return (false) ;
  return (result != 0) ;
}

// -1 == error, 0 == no update, 1 == update
int IsUpdateAvailable (bool SendSysinfo, char *CurrentVersion, char *TestVersion, char *NewVersion)
{
  int                   result = -1 ;
  char                  data [256] ;
  char                  poststr [2048] ;
  char                  *s = poststr ;
  char                  *InstalledOn ;
  __int64               file_time ;
  HDC                   hdc ;
  HKEY                  key ;
  DWORD                 len = sizeof (data) ;
  DWORD                 header = 0 ;
  DWORD                 n ;
  SYSTEMTIME            system_time ;
  SYSTEM_INFO           sysinfo ;
  MEMORYSTATUS          mem_status ;
  OSVERSIONINFO         version_info ;

  if (!InternetConnected ())
    return (-1) ;
  if ((InstalledOn = GetInstallTime ()) == NULL)
  {
    GetSystemTime (&system_time) ;
    SystemTimeToFileTime (&system_time, (FILETIME *) &file_time) ;
    reg_printf ("Software\\POV-Ray", "InstallTime", "%I64u", file_time) ;
    if ((InstalledOn = GetInstallTime ()) == NULL)
      InstalledOn = "Unknown" ;
  }
  sprintf (data, "POVWIN %s", CurrentVersion) ;
  HINTERNET iHandle = InternetOpen (data, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0) ;
  if (iHandle == NULL)
    return (-1) ;
  HINTERNET cHandle = InternetConnect (iHandle, "winupdate.povray.org", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0) ;
  if (cHandle == NULL)
  {
    InternetCloseHandle (iHandle) ;
    return (-1) ;
  }
  HINTERNET hHandle = HttpOpenRequest (cHandle, "POST", SCRIPTPATH, NULL, NULL, NULL, HTTPFLAGS, 0) ;
  if (hHandle == NULL)
  {
    InternetCloseHandle (iHandle) ;
    InternetCloseHandle (cHandle) ;
    return (-1) ;
  }
  if (InstalledOn == NULL)
    InstalledOn = "Unknown" ;
  s += sprintf (s, "TestVersion=%s\n", TestVersion) ;
  s += sprintf (s, "CurrentVersion=%s\n", CurrentVersion) ;
  s += sprintf (s, "InstallDate=%s\n", InstalledOn) ;
  if (SendSysinfo)
  {
    strcpy (s, "&NoInfo=false\n") ;
    GetSystemInfo (&sysinfo) ;
    s += sprintf (s, "CPUArchitecture=0x%04x\n", (DWORD) sysinfo.wProcessorArchitecture) ;
    s += sprintf (s, "NumberOfCPUs=0x%04x\n", sysinfo.dwNumberOfProcessors) ;
    s += sprintf (s, "ProcessorType=0x%04x\n", sysinfo.dwProcessorType) ;
    s += sprintf (s, "ProcessorLevel=0x%04x\n", (DWORD) sysinfo.wProcessorLevel) ;
    s += sprintf (s, "ProcessorRevision=0x%04x\n", (DWORD) sysinfo.wProcessorRevision) ;

    version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
    GetVersionEx (&version_info) ;
    
    s += sprintf (s, "OSVersion=%u.%u\n", version_info.dwMajorVersion, version_info.dwMinorVersion) ;
    s += sprintf (s, "OSBuild=0x%08x\n", version_info.dwBuildNumber) ;
    s += sprintf (s, "CSDVersion=%s\n", version_info.szCSDVersion) ;

    hdc = GetDC (NULL) ;
    s += sprintf (s, "BitsPerPixel=%u\n", GetDeviceCaps (hdc, BITSPIXEL)) ;
    s += sprintf (s, "HorzRes=%u\n", GetDeviceCaps (hdc, HORZRES)) ;
    s += sprintf (s, "VertRes=%u\n", GetDeviceCaps (hdc, VERTRES)) ;
    ReleaseDC (NULL, hdc) ;

    s += sprintf (s, "NumberOfMonitors=%u\n", GetSystemMetrics (SM_CMONITORS)) ;
    s += sprintf (s, "HasMouseWheel=%u\n", GetSystemMetrics (SM_MOUSEWHEELPRESENT)) ;
    s += sprintf (s, "Remote=%u\n", GetSystemMetrics (SM_REMOTESESSION)) ;

    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System\\CentralProcessor\\0", 0, KEY_READ, &key) == ERROR_SUCCESS)
    {
      len = sizeof (n) ;
      if (RegQueryValueEx (key, "~MHZ", 0, NULL, (BYTE *) &n, &len) == ERROR_SUCCESS)
        s += sprintf (s, "CPUFrequency=%u\n", n) ;

      len = sizeof (n) ;
      if (RegQueryValueEx (key, "FeatureSet", 0, NULL, (BYTE *) &n, &len) == ERROR_SUCCESS)
        s += sprintf (s, "FeatureSet=0x%08x\n", n) ;

      len = sizeof (data) ;
      if (RegQueryValueEx (key, "ProcessorNameString", 0, NULL, (BYTE *) data, &len) == ERROR_SUCCESS)
        s += sprintf (s, "CPUName=%s\n", data) ;

      len = sizeof (data) ;
      if (RegQueryValueEx (key, "Identifier", 0, NULL, (BYTE *) data, &len) == ERROR_SUCCESS)
        s += sprintf (s, "CPUIdentifier=%s\n", data) ;

      len = sizeof (data) ;
      if (RegQueryValueEx (key, "VendorIdentifier", 0, NULL, (BYTE *) data, &len) == ERROR_SUCCESS)
        s += sprintf (s, "VendorIdentifier=%s\n", data) ;

      RegCloseKey (key) ;
    }

    mem_status.dwLength = sizeof (MEMORYSTATUS) ;
    GlobalMemoryStatus (&mem_status) ;
    s += sprintf (s, "PhysicalMemory=%u\n", mem_status.dwTotalPhys) ;

    if (GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SLANGUAGE, data, sizeof (data)))
      s += sprintf (s, "DefaultLanguage=%s\n", data) ;
  }
  else
    strcpy (s, "NoInfo=true\n ") ;
  if (HttpSendRequest (hHandle, NULL, 0, poststr, (DWORD) strlen (poststr)))
  {
    if (HttpQueryInfo (hHandle, HTTP_QUERY_STATUS_CODE, data, &len, &header))
    {
      if (len == 3 && memcmp (data, "200", 3) == 0)
      {
        if (InternetReadFile (hHandle, data, sizeof (data) - 1, &len))
        {
          data [len] = '\0' ;
          result = 0 ;
          if (memcmp (data, "YES ", 4) == 0)
          {
            result = 1 ;
            strncpy (NewVersion, data + 4, 64) ;
          }
          else if (memcmp (data, "BADVER", 6) == 0)
            result = -2 ;
        }
      }
    }
  }
  InternetCloseHandle (hHandle) ;
  InternetCloseHandle (cHandle) ;
  InternetCloseHandle (iHandle) ;
  return (result) ;
}

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
  return (true) ;
}

