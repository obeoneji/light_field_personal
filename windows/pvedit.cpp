/*******************************************************************************
 * pvedit.cpp
 *
 * This file contains editor support code.
 *
 * Author: Christopher J. Cason.
 *
 * from Persistence of Vision(tm) Ray Tracer version 3.6.
 * Copyright 1991-2003 Persistence of Vision Team
 * Copyright 2003-2009 Persistence of Vision Raytracer Pty. Ltd.
 * ---------------------------------------------------------------------------
 * NOTICE: This source code file is provided so that users may experiment
 * with enhancements to POV-Ray and to port the software to platforms other
 * than those supported by the POV-Ray developers. There are strict rules
 * regarding how you are permitted to use this file. These rules are contained
 * in the distribution and derivative versions licenses which should have been
 * provided with this file.
 *
 * These licences may be found online, linked from the end-user license
 * agreement that is located at http://www.povray.org/povlegal.html
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/povray/3.5/windows/pvedit.cpp $
 * $Revision: #11 $
 * $Change: 4793 $
 * $DateTime: 2009/05/26 07:13:51 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#ifdef _WIN64
#pragma pack(16)
#include <windows.h>
#pragma pack()
#else
#include <windows.h>
#endif

#include <string.h>
#include <sys/stat.h>
#include "pvengine.h"
#include "pvedit.h"
#include "reswords.h"

using namespace pov;
  
HINSTANCE               hLibPovEdit ;

extern bool             debugging ;
extern HWND             main_window ;
extern bool             use_editors ;

static HWND (*CreateTabWindow) (HWND ParentWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath) ;
static DWORD (*GetDLLVersion) (void) ;
static void (*SetWindowPosition) (int x, int y, int w, int h) ;
static void (*SetMessageWindow) (HWND MessageWindow) ;
static void (*RestoreState) (int RestoreFiles) ;
static void (*SaveState) (void) ;
static bool (*SelectFile) (char *FileName) ;
static bool (*BrowseFile) (bool CreateNewWindow) ;
static bool (*LoadFile) (char *FileName) ;
static bool (*CloseFile) (char *FileName) ;
static bool (*SaveFile) (char *FileName) ;
static bool (*ExternalLoadFile) (char *ParamString) ;
static DWORD (*GetTab) (void) ;
static DWORD (*GetFlags) (void) ;
static char *(*GetFilename) (void) ;
static void (*NextTab) (bool Forward) ;
static bool (*CanClose) (bool AllFiles) ;
static bool (*SaveModified) (char *FileName) ;
static bool (*ShowMessages) (bool on) ;
static void (*DispatchMenuId) (DWORD id) ;
static HMENU (*GetMenuHandle) (int which) ;
static void (*SetNotifyBase) (HWND WindowHandle, int MessageBase) ;
static void (*UpdateMenus) (HMENU MenuHandle) ;
static void (*GetContextHelp) (void) ;
static void (*SetTabFocus) (void) ;
static bool (*ShowParseError) (const char *FileName, const char *Message, int Line, int Col) ;
static bool (*PassOnMessage) (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal) ;
static void (*SetKeywords) (LPCSTR Keywords) ;
static const char **(*GetWindowList) (void);

void                    **FunctionPtrs [] =
                        {
                         (void **) &GetWindowList,
                         (void **) &SetWindowPosition,
                         (void **) &SetMessageWindow,
                         (void **) &RestoreState,
                         (void **) &SaveState,
                         (void **) &SelectFile,
                         (void **) &BrowseFile,
                         (void **) &LoadFile,
                         (void **) &CloseFile,
                         (void **) &SaveFile,
                         (void **) &ExternalLoadFile,
                         (void **) &GetTab,
                         (void **) &GetFlags,
                         (void **) &GetFilename,
                         (void **) &NextTab,
                         (void **) &CanClose,
                         (void **) &SaveModified,
                         (void **) &ShowMessages,
                         (void **) &DispatchMenuId,
                         (void **) &GetMenuHandle,
                         (void **) &SetNotifyBase,
                         (void **) &UpdateMenus,
                         (void **) &GetContextHelp,
                         (void **) &SetTabFocus,
                         (void **) &ShowParseError,
                         (void **) &PassOnMessage,
                         (void **) &SetKeywords,
                         (void **) &CreateTabWindow,
                         NULL
                        } ;

char                    *FunctionNames [] =
                        {
                         "GetWindowList",
                         "SetWindowPosition",
                         "SetMessageWindow",
                         "RestoreState",
                         "SaveState",
                         "SelectFile",
                         "BrowseFile",
                         "LoadFile",
                         "CloseFile",
                         "SaveFile",
                         "ExternalLoadFile",
                         "GetTab",
                         "GetFlags",
                         "GetFilename",
                         "NextTab",
                         "CanClose",
                         "SaveModified",
                         "ShowMessages",
                         "DispatchMenuId",
                         "GetMenuHandle",
                         "SetNotifyBase",
                         "UpdateMenus",
                         "GetContextHelp",
                         "SetTabFocus",
                         "ShowParseError",
                         "PassOnMessage",
                         "SetKeywords",
                         "CreateTabWindow",
                         NULL
                        } ;

bool LoadEditorDLL (char *path, bool errorOK)
{
  int                   DllVersion ;
  int                   err ;
  char                  str [_MAX_PATH] ;
  char                  **s ;
  void                  ***f ;
  struct stat           statbuf ;

   if (!use_editors)
    return (false) ;
  if (debugging)
    debug_output ("Trying to load editor DLL from '%s' [%s]\n", path, stat (path, &statbuf) != 0 ? "missing" : "found") ;
  if (hLibPovEdit != NULL)
    FreeLibrary(hLibPovEdit);
  if ((hLibPovEdit = LoadLibraryEx (path, 0, LOAD_WITH_ALTERED_SEARCH_PATH)) == NULL)
  {
    err = GetLastError () ;
    if (debugging)
      debug_output ("Could not load editor DLL '%s', error code is %08lx\n", path, err) ;
    if (!errorOK)
    {
      sprintf (str, "Editor DLL initialisation failed [LoadLibrary failed, code is %08lx]", err) ;
      PovMessageBox (str, "POV-Ray Editor error") ;
      PovMessageBox ("See the 'Internal Editor Reference' section in the help file for\n"
                     "instructions on how to correct this or turn editor loading off.", "Important!") ;
    }
    return (false) ;
  }

  GetDLLVersion = (DWORD (*) (void)) GetProcAddress (hLibPovEdit, "GetDLLVersion") ;
  if (GetDLLVersion == NULL)
  {
    if (debugging)
      debug_output ("Could not resolve GetDLLVersion, error code is %08lx\n", GetLastError ()) ;
    if (!errorOK)
      PovMessageBox ("Editor DLL initialization failed [could not resolve GetDLLVersion]\n\n"
                     "See the 'Built-In Editors' section in the help file",
                     "POV-Ray Editor Error") ;
    return (false) ;
  }
  if ((DllVersion = GetDLLVersion ()) != EDITDLLVERSION)
  {
    sprintf (str, "ERROR : Wrong editor DLL version [expected %04x, got %04x].", EDITDLLVERSION, DllVersion) ;
    if (debugging)
      debug_output ("%s\n", str) ;
    if (!errorOK)
    {
      MessageBox (NULL, str, "POV-Ray Editor Error", MB_ICONSTOP) ;
      PovMessageBox ("Editor switched off.\n\nSee the 'Built-In Editors' section in the help file", "Important!") ;
    }
    return (false) ;
  }
  for (s = FunctionNames, f = FunctionPtrs ; *s ; s++, f++)
  {
    **f = (void *) GetProcAddress (hLibPovEdit, *s) ;
    if (**f == NULL)
    {
      if (debugging)
        debug_output ("Could not get address of '%s', error code is %08lx\n", *s, GetLastError ()) ;
      if (!errorOK)
      {
        sprintf (str, "Editor DLL initialization failed:\nCould not resolve %s.", *s) ;
        PovMessageBox (str, "Editor Error") ;
        PovMessageBox ("See the 'Built-In Editors' section in the help file", "Important!") ;
      }
      return (false) ;
    }
  }
  if (debugging)
    debug_output ("Loaded editor DLL '%s'\n", path) ;
  return (true) ;
}

char *Get_Reserved_Words (const char *additional_words)
{
  int length = 0 ;
  int i ;

  for (i = 0; i < LAST_TOKEN; i++)
  {
    if (!isalpha (Reserved_Words [i].Token_Name [0]))
      continue ;
    if (strchr (Reserved_Words [i].Token_Name, ' ') != NULL)
      continue ;
    length += (int)strlen (Reserved_Words[i].Token_Name) + 1 ;
  }

  length += (int)strlen (additional_words) ;

  char *result = (char *) malloc (++length) ;
  strcpy (result, additional_words) ;
  char *s = result + strlen (additional_words) ;

  for (i = 0 ; i < LAST_TOKEN ; i++)
  {
    if (!isalpha (Reserved_Words [i].Token_Name [0]))
      continue ;
    if (strchr (Reserved_Words [i].Token_Name, ' ') != NULL)
      continue ;
    s += sprintf (s, "%s\n", Reserved_Words[i].Token_Name) ;
  }
  *--s = '\0' ;

  return (result) ;
}

HWND InitialiseEditor (HWND ParentWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath)
{
  char        *s ;
  HWND        hwnd ;

  if (!use_editors)
    return (NULL) ;
  s = Get_Reserved_Words ("#\n") ;
  EditSetKeywords (s) ;
  free (s) ;
  if ((hwnd = CreateTabWindow (ParentWindow, StatusWindow, BinariesPath, DocumentsPath)) == NULL)
  {
    PovMessageBox ("TabWindow error: see the 'Built-In Editors' section in the help file", "Important!") ;
    return (NULL) ;
  }
  return (hwnd) ;
}

void SetEditorPosition (int x, int y, int w, int h)
{
  if (!use_editors)
    return ;
  SetWindowPosition (x, y, w, h) ;
}

void EditSetMessageWindow (HWND MessageWindow)
{
  if (!use_editors)
    return ;
  SetMessageWindow (MessageWindow) ;
}

void EditRestoreState (int RestoreFiles)
{
  if (!use_editors)
    return ;
  RestoreState (RestoreFiles) ;
}

void EditSaveState (void)
{
  if (!use_editors)
    return ;
  SaveState () ;
}

bool EditSelectFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (SelectFile (FileName)) ;
}

bool EditBrowseFile (bool CreateNewWindow)
{
  if (!use_editors)
    return (false) ;
  return (BrowseFile (CreateNewWindow)) ;
}

// NULL means create a new, untitled, editor window.
bool EditOpenFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (LoadFile (FileName)) ;
}

// NULL means currently selected file
bool EditCloseFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (CloseFile (FileName)) ;
}

// NULL means currently selected file
bool EditSaveFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (SaveFile (FileName)) ;
}

bool EditExternalOpenFile (char *ParamString)
{
  if (!use_editors)
    return (false) ;
  return (ExternalLoadFile (ParamString)) ;
}

DWORD EditGetTab (void)
{
  if (!use_editors)
    return (0) ;
  return (GetTab ()) ;
}

DWORD EditGetFlags (void)
{
  if (!use_editors)
    return (EDIT_MSG_SELECTED) ;
  return (GetFlags ()) ;
}

char *EditGetFilename (bool IncludeModifiedIndicator)
{
  char                  *s;
  static char           str [_MAX_PATH + 2] ;

  if (!use_editors)
    return (NULL) ;
  s = GetFilename();
  if (IncludeModifiedIndicator == false || s == NULL || *s == '\0')
    return s;
  if ((GetFlags() & EDIT_CURRENT_MODIFIED) == 0)
    return s;
  strcpy(str, s);
  strcat(str, "*");
  return str;
}

void EditNextTab (bool Forward)
{
  if (!use_editors)
    return ;
  NextTab (Forward) ;
}

bool EditCanClose (bool AllFiles)
{
  if (!use_editors)
    return (true) ;
  return (CanClose (AllFiles)) ;
}

bool EditSaveModified (char *FileName)
{
  if (!use_editors)
    return (true) ;
  return (SaveModified (FileName)) ;
}

bool EditShowMessages (bool on)
{
  // return true if the caller should show the message window themselves
  if (!use_editors)
    return (true) ;
  return (ShowMessages (on)) ;
}

void EditDispatchMenuId (DWORD id)
{
  if (!use_editors)
    return ;
  DispatchMenuId (id) ;
}

void EditUpdateMenus (HMENU MenuHandle)
{
  if (!use_editors)
    return ;
  UpdateMenus (MenuHandle) ;
}

void EditSetFocus (void)
{
  if (!use_editors)
    return ;
  SetTabFocus () ;
}

HMENU EditGetMenu (int which)
{
  if (!use_editors)
    return (NULL) ;
  return (GetMenuHandle (which)) ;
}

void EditSetNotifyBase (HWND WindowHandle, int MessageBase)
{
  if (!use_editors)
    return ;
  SetNotifyBase (WindowHandle, MessageBase) ;
}

void EditContextHelp (void)
{
  if (!use_editors)
    return ;
  GetContextHelp () ;
}

bool EditShowParseError (const char *FileName, const char *Message, int Line, int Col)
{
  const char  *s ;

  if (!use_editors)
    return (false) ;
  s = strstr (Message, "<----ERROR") ;
  if (s != NULL)
    Message = s + 10 ;
  while (*Message == ' ' || *Message == '\n')
    Message++ ;
  return (ShowParseError (FileName, Message, Line, Col)) ;
}

bool EditPassOnMessage (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal)
{
  if (!use_editors)
    return (false) ;
  return (PassOnMessage (hwnd, message, wParam, lParam, rVal)) ;
}

void EditUnload (void)
{
}

bool EditEnabled (void)
{
  return (use_editors) ;
}

void EditSetState (bool on)
{
  use_editors = on ;
}

void EditSetKeywords (LPCSTR Keywords)
{
  SetKeywords (Keywords) ;
}

const char **EditGetWindowList (void)
{
  if (!use_editors)
    return NULL;
  return GetWindowList();
}
