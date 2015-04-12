/****************************************************************************
 *                pvmenu.cpp
 *
 * This module implements menu-related routines for the Windows build of POV.
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
 * $File: //depot/povray/3.5/windows/pvmenu.cpp $
 * $Revision: #8 $
 * $Change: 4782 $
 * $DateTime: 2009/05/18 09:58:23 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

// REMOVAL OF NON-EXPERT MENUS (August 1998) ... the 'non-expert menus' option
// was removed. This leaves some of this code redundant, but here it is anyhow ...

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN64
#pragma pack(16)
#include <windows.h>
#include <commctrl.h>
#pragma pack()
#else
#include <windows.h>
#include <commctrl.h>
#endif

#include <math.h>
#include <setjmp.h>
#include <string.h>

#include "frame.h"
#include "colour.h"
#include "povray.h"
#include "optout.h"

#include "pvengine.h"
#include "resource.h"
#include "pvedit.h"

bool                    MenuBarDraw = false ;
char                    WindowList[MAX_EDITORS + 1][_MAX_PATH];
HMENU                   hMainMenu ;
HMENU                   hMenuBar ;
HMENU                   hPopupMenus ;
HMENU                   hFileMenu ;
HMENU                   hEditMenu ;
HMENU                   hRenderMenu ;
HMENU                   hRerunMenu ;
HMENU                   hOptionsMenu ;
HMENU                   hToolsMenu ;
HMENU                   hPluginsMenu ;
HMENU                   hHelpMenu ;

extern HWND             main_window ;
extern HWND             toolbar_window ;
extern HINSTANCE        hInstance ;
extern HACCEL           hAccelerators ;

bool PVEnableMenuItem (UINT idItem, UINT state)
{
  EnableMenuItem (hPopupMenus, idItem, state) ;
  EnableMenuItem (hMenuBar, idItem, state) ;
  SendMessage (toolbar_window, TB_ENABLEBUTTON, idItem, MAKELONG (state == MF_ENABLED, 0)) ;
  if (idItem == CM_RENDERCLOSE)
  {
    SendMessage (toolbar_window, TB_HIDEBUTTON, CM_RENDERSHOW, MAKELONG (state == MF_ENABLED, 0)) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, CM_RENDERCLOSE, MAKELONG (state != MF_ENABLED, 0)) ;
  }
  return (true) ;
}

bool PVCheckMenuItem (UINT idItem, UINT state)
{
  CheckMenuItem (hPopupMenus, idItem, state) ;
  CheckMenuItem (hMenuBar, idItem, state) ;
  return (true) ;
}

bool PVCheckMenuRadioItem (UINT idFirst, UINT idLast, UINT idItem)
{
  CheckMenuRadioItem (hPopupMenus, idFirst, idLast, idItem, MF_BYCOMMAND) ;
  CheckMenuRadioItem (hMenuBar, idFirst, idLast, idItem, MF_BYCOMMAND) ;
  return (true) ;
}

bool PVModifyMenu (UINT idItem, UINT flags, UINT idNewItem, LPCSTR lpNewItem)
{
  ModifyMenu (hPopupMenus, idItem, flags, idNewItem, lpNewItem) ;
  ModifyMenu (hMenuBar, idItem, flags, idNewItem, lpNewItem) ;
  MenuBarDraw = true ;
  return (true) ;
}

bool PVDeleteMenuItem (UINT idItem)
{
  DeleteMenu (hPopupMenus, idItem, MF_BYCOMMAND) ;
  DeleteMenu (hMenuBar, idItem, MF_BYCOMMAND) ;
  return (true) ;
}

int find_menuitem (HMENU hMenu, LPCSTR title)
{
  int         max = GetMenuItemCount (hMenu) ;
  char        str [256] ;

  if (title[0] == '\0')
    return (-1);
  for (int i = 0 ; i < 64 ; i++)
    if (GetMenuString (hMenu, i, str, sizeof (str) - 1, MF_BYPOSITION) > 0)
      if (strcmp (title, str) == 0)
        return (i) ;
  return (-1) ;
}

void init_menus (void)
{
  hPopupMenus = LoadMenu (hInstance, MAKEINTRESOURCE (POPUP_MENUS32)) ;
  hAccelerators = LoadAccelerators (hInstance, MAKEINTRESOURCE (PVENGINE_MENU)) ;
  hMenuBar = LoadMenu (hInstance, MAKEINTRESOURCE (PVENGINE_MENU32)) ;
  hFileMenu = GetSubMenu (hMenuBar, 0) ;
  hEditMenu = GetSubMenu (hMenuBar, 1) ;
  hRenderMenu = GetSubMenu (hMenuBar, 2) ;
  hRerunMenu = GetSubMenu (hMenuBar, 3) ;
  hOptionsMenu = GetSubMenu (hMenuBar, 4) ;
  hToolsMenu = GetSubMenu (hMenuBar, 5) ;
  hHelpMenu = GetSubMenu (hMenuBar, 6) ;
  AppendMenu (hRenderMenu, MF_POPUP, (UINT_PTR) hRerunMenu, "&Rerun") ;
  int n = find_menuitem(hOptionsMenu, "GU&I-Extensions") ;
  assert(n != -1) ;
  hPluginsMenu = GetSubMenu(hOptionsMenu, n);
}

void set_newuser_menu (HMENU hMenu, UINT ID, bool hide, bool separator)
{
  if (GetMenuItemID (hMenu, 0) != ID)
  {
    if (hide)
      return ;
    if (separator)
      InsertMenu (hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL) ;
    InsertMenu (hMenu, 0, MF_BYPOSITION, ID, "&Help On This Menu") ;
  }
  else
  {
    if (!hide)
      return ;
    DeleteMenu (hMenu, 0, MF_BYPOSITION) ;
    if (separator)
      DeleteMenu (hMenu, 0, MF_BYPOSITION) ;
  }
}

void set_newuser_menus (bool hide)
{
  set_newuser_menu (hFileMenu, CM_FILEMENUHELP, hide, true) ;
  set_newuser_menu (hEditMenu, CM_EDITMENUHELP, hide, true) ;
  set_newuser_menu (hRenderMenu, CM_RENDERMENUHELP, hide, true) ;
  set_newuser_menu (hOptionsMenu, CM_OPTIONSMENUHELP, hide, true) ;
  set_newuser_menu (hToolsMenu, CM_TOOLSMENUHELP, hide, false) ;
  set_newuser_menu (hPluginsMenu, CM_GUIEXTMENUHELP, hide, true) ;
  set_newuser_menu (GetSubMenu (hPopupMenus, 0), CM_MESSAGEWINMENUHELP, hide, true) ;
  set_newuser_menu (GetSubMenu (hPopupMenus, 1), CM_RENDERWINMENUHELP, hide, true) ;
  EditPassOnMessage (NULL, HIDE_NEWUSER_HELP_MESSAGE, hide, 0, NULL) ;
}

void clear_menu (HMENU hMenu)
{
  while (RemoveMenu (hMenu, 0, MF_BYPOSITION)) ;
}

void setup_menus (bool have_editor)
{
  if (have_editor)
    AppendMenu (hOptionsMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetOptionsMenu), "&Editor Window") ;
  DrawMenuBar (main_window) ;
}

// build the menu displayed when the message window is selected
void build_main_menu (HMENU hMenu, bool have_editor)
{
  clear_menu (hMenu) ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) (have_editor ? EditGetMenu (GetFileMenu) : hFileMenu), "&File") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hEditMenu, "&Edit") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hRenderMenu, "&Render") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hOptionsMenu, "&Options") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hToolsMenu, "&Tools") ;
  if (EditGetMenu (GetWindowMenu) != NULL)
    AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetWindowMenu), "&Window") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hHelpMenu, "&Help") ;
  DrawMenuBar (main_window) ;
}

// build the menu displayed when an editor window is selected
void build_editor_menu (HMENU hMenu)
{
  clear_menu (hMenu) ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetFileMenu), "&File") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetEditMenu), "&Edit") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetSearchMenu), "Se&arch") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetTextMenu), "&Text") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetOptionsMenu), "E&ditor") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetInsertMenu), "&Insert") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hRenderMenu, "&Render") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hOptionsMenu, "&Options") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hToolsMenu, "Too&ls") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetWindowMenu), "&Window") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hHelpMenu, "&Help") ;
  DrawMenuBar (main_window) ;
}

