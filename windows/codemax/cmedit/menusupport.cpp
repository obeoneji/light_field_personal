/*******************************************************************************
 * menusupport.cpp
 *
 * This file is part of the CodeMax editor support code.
 *
 * Author: Christopher J. Cason.
 *
 * from Persistence of Vision Ray Tracer ('POV-Ray') version 3.6.
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/menusupport.cpp $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#include "cmedit.h"
#include "ccodemax.h"
#include "settings.h"
#include "menusupport.h"
#include "eventhandlers.h"
#include "editorinterface.h"
#include "..\..\pvedit.h"

extern int              EditorCount ;
extern int              EditDragOffset ;
extern int              EditStartDragOffset ;
extern int              AutoReload ;
extern bool             CreateBackups ;
extern bool             LastOverwrite ;
extern bool             UndoAfterSave ;
extern bool             MessagePaneVisible ;
extern HWND             hMainWindow ;
extern HMENU            hMainMenu ;
extern HMENU            hPopupMenu ;
extern HMENU            hFileMenu ;
extern HMENU            hEditMenu ;
extern HMENU            hSearchMenu ;
extern HMENU            hTextMenu ;
extern HMENU            hEditorMenu ;
extern HMENU            hInsertMenu ;
extern HMENU            hOlderFilesMenu ;
extern HMENU            hWindowMenu ;
extern CCodeMax         *Editor ;
extern const char       *WindowList[MAX_EDITORS + 1];
extern CStdStringList   RecentFiles ;
extern CStdStringList   OlderFiles ;

//------------------------------------------------------------------------------------------------------------------------

void MakeRecentMenus (void)
{
  int                count = RecentFiles.ItemCount () ;
  int                index ;
  CStdString         str ;

  while ((index = GetMenuItemCount (hFileMenu) - 1) > 0 && GetSubMenu (hFileMenu, index) != hOlderFilesMenu)
    if (!DeleteMenu (hFileMenu, index, MF_BYPOSITION))
      break ;
  if (GetSubMenu (hFileMenu, index) == hOlderFilesMenu)
    EnableMenuItem (hFileMenu, index, MF_BYPOSITION | (OlderFiles.ItemCount () ? MF_ENABLED : MF_GRAYED)) ;
  if (count > 0)
  {
    AppendMenu (hFileMenu, MF_SEPARATOR, 0, 0) ;
    for (int i = 0 ; i < count ; i++)
    {
      str.Format ("&%d. %s", i + 1, (LPCSTR) GetBaseName (GetField (RecentFiles [i], 0))) ;
      AppendMenu (hFileMenu, MF_STRING, CM_FIRSTRECENTFILE + i, str) ;
    }
  }
}

void MakeOlderMenus (void)
{
  int                count = OlderFiles.ItemCount () ;
  CStdString         str ;

  while (GetMenuItemCount (hOlderFilesMenu) > 0)
    if (!DeleteMenu (hOlderFilesMenu, 0, MF_BYPOSITION))
      break ;
  for (int i = 0 ; i < count ; i++)
  {
    str = GetBaseName (GetField (OlderFiles [i], 0)) ;
    AppendMenu (hOlderFilesMenu, MF_STRING, CM_FIRSTOLDERFILE + i, str) ;
  }
}

void AddToRecent (LPCTSTR FileName)
{
  int         count ;
  CStdString  str = GetField (FileName, 0) ;

  if (FileName == NULL || FileName [0] == '\0')
    return ;
  count = RecentFiles.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
    if (GetField (RecentFiles [i], 0).CompareNoCase (str) == 0)
      RecentFiles.DeleteItem (i) ;
  count = OlderFiles.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
    if (GetField (OlderFiles [i], 0).CompareNoCase (str) == 0)
      OlderFiles.DeleteItem (i) ;
  RecentFiles.InsertItem (FileName) ;
  if (RecentFiles.ItemCount () > 9)
  {
    OlderFiles.InsertItem (0, RecentFiles [9]) ;
    if (OlderFiles.ItemCount () >= MAX_OLDER_FILES)
      OlderFiles.DeleteItem (MAX_OLDER_FILES - 1) ;
    RecentFiles.DeleteItem (9) ;
  }
  MakeRecentMenus () ;
  MakeOlderMenus () ;
}

void UpdateRecent (void)
{
  int                   count ;
  CCodeMax              *e ;
  CStdString            str ;

  count = RecentFiles.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = GetField (RecentFiles [i], 0) ;
    if ((e = FindEditor (str)) == NULL)
      continue ;
    str.Format ("%s,%d,%d,%d,%d,%d,%d",
                e->m_Tag.LongName,
                e->GetLineNo (),
                e->GetColNo (),
                e->GetTopLine (),
                (int) e->GetLanguage (),
                e->GetTabSize (),
                (int) e->GetAutoIndent ()) ;
    RecentFiles [i] = str ;
  }
  count = OlderFiles.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = GetField (OlderFiles [i], 0) ;
    if ((e = FindEditor (str)) == NULL)
      continue ;
    str.Format ("%s,%d,%d,%d,%d,%d,%d",
                e->m_Tag.LongName,
                e->GetLineNo (),
                e->GetColNo (),
                e->GetTopLine (),
                (int) e->GetLanguage (),
                e->GetTabSize (),
                (int) e->GetAutoIndent ()) ;
    OlderFiles [i] = str ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void EnableMenuItem (DWORD id, bool state)
{
  EnableMenuItem (hMainMenu, id, state ? MF_ENABLED : MF_GRAYED) ;
  EnableMenuItem (hPopupMenu, id, state ? MF_ENABLED : MF_GRAYED) ;
  EnableMenuItem (hWindowMenu, id, state ? MF_ENABLED : MF_GRAYED) ;
}

void CheckMenuItem (DWORD id, bool state)
{
  CheckMenuItem (hMainMenu, id, state ? MF_CHECKED : MF_UNCHECKED) ;
  CheckMenuItem (hPopupMenu, id, state ? MF_CHECKED : MF_UNCHECKED) ;
}

bool IsMenuItemChecked (DWORD id)
{
  int val = GetMenuState (hMainMenu, id, MF_BYCOMMAND) ;
  if (val != -1)
    return ((val & MF_CHECKED) != 0) ;
  val = GetMenuState (hPopupMenu, id, MF_BYCOMMAND) ;
  if (val != -1)
    return ((val & MF_CHECKED) != 0) ;
  return (false) ;
}

bool ToggleMenuItem (DWORD id)
{
  bool state = !IsMenuItemChecked (id) ;
  CheckMenuItem (hMainMenu, id, state ? MF_CHECKED : MF_UNCHECKED) ;
  CheckMenuItem (hPopupMenu, id, state ? MF_CHECKED : MF_UNCHECKED) ;
  return (state) ;
}

void CheckMenuRadioItem (DWORD idItem, DWORD idFirst, DWORD idLast)
{
  CheckMenuRadioItem (hMainMenu, idFirst, idLast, idItem, MF_BYCOMMAND) ;
  CheckMenuRadioItem (hPopupMenu, idFirst, idLast, idItem, MF_BYCOMMAND) ;
}

void SetMenuItemText (DWORD idItem, LPCSTR str)
{
  ModifyMenu (hMainMenu, idItem, MF_STRING, idItem, str) ;
  ModifyMenu (hPopupMenu, idItem, MF_STRING, idItem, str) ;
}

CStdString GetMenuItemText (DWORD idItem)
{
  char                  str [256] ;
  MENUITEMINFO          info ;

  info.cbSize = sizeof (MENUITEMINFO) ;
  info.fMask = MIIM_TYPE ;
  info.dwTypeData = str ;
  info.cch = sizeof (str) ;

  if (GetMenuItemInfo (hMainMenu, idItem, false, &info))
    return (str) ;
  if (GetMenuItemInfo (hPopupMenu, idItem, false, &info))
    return (str) ;
  return ("") ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString HotKeyString (WORD Command)
{
  int                   count ;
  CM_HOTKEY             *hotkeys ;
  CM_HOTKEY             lastKey ;
  CM_HOTKEY             key ;
  CStdString            str ;

  if ((count = CCodeMax::GetHotKeysForCmd (Command, NULL)) == 0)
    return (str) ;
  hotkeys = new CM_HOTKEY [count] ;
  CCodeMax::GetHotKeysForCmd (Command, hotkeys) ;
  while (count--)
  {
    key = hotkeys [count] ;
    if (str != "")
    {
      bool lastIsChar = lastKey.nVirtKey1 >= '0' && lastKey.nVirtKey1 <= 'Z' ;
      bool thisIsChar = key.nVirtKey1 >= '0' && key.nVirtKey1 <= 'Z' ;
      if (lastIsChar && !thisIsChar)
        continue ;
      if (lastIsChar && thisIsChar)
        if (lastKey.nVirtKey1 < key.nVirtKey1)
          continue ;
    }
    str = CCodeMax::GetHotKeyString (key) ;
    lastKey = key ;
  }
  delete [] hotkeys ;
  return (str) ;
}

//------------------------------------------------------------------------------------------------------------------------

void SetMenuShortcut (DWORD idItem, WORD Command)
{
  int         pos ;
  CStdString  text = GetMenuItemText (idItem) ;
  CStdString  hks = HotKeyString (Command) ;

  if ((pos = text.Find ("\t")) >= 0)
    text = text.Left (pos) ;
  if (hks != "")
    text += "\t" + hks ;
  SetMenuItemText (idItem, text) ;
}

void SetMenuShortcuts (void)
{
  SetMenuShortcut (CM_UNDO, CMD_UNDO) ;
  SetMenuShortcut (CM_REDO, CMD_REDO) ;
  SetMenuShortcut (CM_CUT, CMD_CUT) ;
  SetMenuShortcut (CM_COPY, CMD_COPY) ;
  SetMenuShortcut (CM_PASTE, CMD_PASTE) ;
  SetMenuShortcut (CM_DELETE, CMD_DELETE) ;
  SetMenuShortcut (CM_SELECTALL, CMD_SELECTALL) ;
  SetMenuShortcut (CM_FIND, CMD_FIND) ;
  SetMenuShortcut (CM_REPLACE, CMD_FINDREPLACE) ;
  SetMenuShortcut (CM_FINDNEXT, CMD_FINDNEXT) ;
  SetMenuShortcut (CM_MATCHBRACE, CMD_GOTOMATCHBRACE) ;
  SetMenuShortcut (CM_GOTOLINE, CMD_GOTOLINE) ;
  SetMenuShortcut (CM_PROPERTIES, CMD_PROPERTIES) ;

  SetMenuShortcut (CM_INDENTSELECTION, CMD_INDENTSELECTION) ;
  SetMenuShortcut (CM_INDENTSELECTIONPREVIOUS, CMD_INDENTTOPREV) ;
  SetMenuShortcut (CM_UNDENTSELECTION, CMD_UNINDENTSELECTION) ;
  SetMenuShortcut (CM_UPPERCASESELECTION, CMD_UPPERCASESELECTION) ;
  SetMenuShortcut (CM_LOWERCASESELECTION, CMD_LOWERCASESELECTION) ;
  SetMenuShortcut (CM_SPACESTOTABS, CMD_TABIFYSELECTION) ;
  SetMenuShortcut (CM_TABSTOSPACES, CMD_UNTABIFYSELECTION) ;

  SetMenuShortcut (CM_SHOWWHITESPACE, CMD_TOGGLEWHITESPACEDISPLAY) ;
  SetMenuShortcut (CM_SETREPEATCOUNT, CMD_SETREPEATCOUNT) ;
  SetMenuShortcut (CM_RECORDMACRO, CMD_RECORDMACRO) ;
  SetMenuShortcut (CM_PLAYMACRO1, CMD_PLAYMACRO1) ;
  SetMenuShortcut (CM_PLAYMACRO2, CMD_PLAYMACRO2) ;
  SetMenuShortcut (CM_PLAYMACRO3, CMD_PLAYMACRO3) ;
  SetMenuShortcut (CM_PLAYMACRO4, CMD_PLAYMACRO4) ;
  SetMenuShortcut (CM_PLAYMACRO5, CMD_PLAYMACRO5) ;
  SetMenuShortcut (CM_PLAYMACRO6, CMD_PLAYMACRO6) ;
  SetMenuShortcut (CM_PLAYMACRO7, CMD_PLAYMACRO7) ;
  SetMenuShortcut (CM_PLAYMACRO8, CMD_PLAYMACRO8) ;
  SetMenuShortcut (CM_PLAYMACRO9, CMD_PLAYMACRO9) ;
  SetMenuShortcut (CM_PLAYMACRO10, CMD_PLAYMACRO10) ;
  SetMenuShortcut (CM_CLEARALLBOOKMARKS, CMD_BOOKMARKCLEARALL) ;
  SetMenuShortcut (CM_FIRSTBOOKMARK, CMD_BOOKMARKJUMPTOFIRST) ;
  SetMenuShortcut (CM_LASTBOOKMARK, CMD_BOOKMARKJUMPTOLAST) ;
  SetMenuShortcut (CM_NEXTBOOKMARK, CMD_BOOKMARKNEXT) ;
  SetMenuShortcut (CM_PREVIOUSBOOKMARK, CMD_BOOKMARKPREV) ;
  SetMenuShortcut (CM_TOGGLEBOOKMARK, CMD_BOOKMARKTOGGLE) ;

  SetMenuShortcut (CM_SAVE, CM_SAVE) ;
  SetMenuShortcut (CM_SAVEAS, CM_SAVEAS) ;
  SetMenuShortcut (CM_SAVEALL, CM_SAVEALL) ;
  SetMenuShortcut (CM_EXIT, CM_EXIT) ;
  SetMenuShortcut (CM_SHOWMESSAGES, CM_SHOWMESSAGES) ;
  SetMenuShortcut (CM_NEWFILE, CM_NEWFILE) ;
  SetMenuShortcut (CM_OPENFILE, CM_OPENFILE) ;
  SetMenuShortcut (CM_PRINT, CM_PRINT) ;
  SetMenuShortcut (CM_PAGESETUP, CM_PAGESETUP) ;

  SetMenuShortcut (CM_CLOSECURRENTFILE, CM_CLOSECURRENTFILE) ;
  SetMenuShortcut (CM_CLOSEALLFILES, CM_CLOSEALLFILES) ;
}

//------------------------------------------------------------------------------------------------------------------------

void SetMenuState (void)
{
  bool        canCut = Editor != NULL && Editor->CanCut () ;

  debug ("SetMenuState\n") ;
  EnableMenuItem (CM_OPENFILE, EditorCount < MAX_EDITORS) ;
  EnableMenuItem (CM_UNDO, Editor != NULL && Editor->CanUndo ()) ;
  EnableMenuItem (CM_REDO, Editor != NULL && Editor->CanRedo ()) ;
  EnableMenuItem (CM_CUT, canCut) ;
  EnableMenuItem (CM_COPY, Editor != NULL && Editor->CanCopy ()) ;
  EnableMenuItem (CM_PASTE, Editor != NULL && Editor->CanPaste ()) ;
  EnableMenuItem (CM_NEWFILE, EditorCount < MAX_EDITORS) ;
  EnableMenuItem (CM_OPENFILE, EditorCount < MAX_EDITORS) ;
  EnableMenuItem (CM_CLOSEFILE, Editor != NULL) ;
  EnableMenuItem (CM_CLOSEALLFILES, EditorCount > 0) ;
  EnableMenuItem (CM_SAVE, Editor != NULL) ;
  EnableMenuItem (CM_SAVEAS, Editor != NULL) ;
  EnableMenuItem (CM_SAVEALL, EditorCount > 0) ;
  EnableMenuItem (CM_PRINT, Editor != NULL) ;
  EnableMenuItem (CM_UNDO, Editor != NULL && Editor->CanUndo ()) ;
  EnableMenuItem (CM_REDO, Editor != NULL && Editor->CanRedo ()) ;
  EnableMenuItem (CM_CUT, canCut) ;
  EnableMenuItem (CM_DELETE, canCut) ;
  EnableMenuItem (CM_COPY, Editor != NULL && Editor->CanCopy ()) ;
  EnableMenuItem (CM_PASTE, Editor != NULL && Editor->CanPaste ()) ;
  EnableMenuItem (CM_PLAYMACRO1, CCodeMax::GetMacro (0, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO2, CCodeMax::GetMacro (1, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO3, CCodeMax::GetMacro (2, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO4, CCodeMax::GetMacro (3, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO5, CCodeMax::GetMacro (4, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO6, CCodeMax::GetMacro (5, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO7, CCodeMax::GetMacro (6, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO8, CCodeMax::GetMacro (7, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO9, CCodeMax::GetMacro (8, NULL) > 0) ;
  EnableMenuItem (CM_PLAYMACRO10, CCodeMax::GetMacro (9, NULL) > 0) ;

  EnableMenuItem (CM_SPLITHORIZONTALLY, Editor != NULL && Editor->IsHSplitterEnabled() && Editor->GetHSplitterPos() == 0) ;
  EnableMenuItem (CM_SPLITVERTICALLY, Editor != NULL && Editor->IsVSplitterEnabled() && Editor->GetVSplitterPos() == 0) ;

  EnableMenuItem (CM_INDENTSELECTION, canCut) ;
  EnableMenuItem (CM_INDENTSELECTIONPREVIOUS, canCut) ;
  EnableMenuItem (CM_UNDENTSELECTION, canCut) ;
  EnableMenuItem (CM_UPPERCASESELECTION, canCut) ;
  EnableMenuItem (CM_LOWERCASESELECTION, canCut) ;
  EnableMenuItem (CM_SPACESTOTABS, canCut) ;
  EnableMenuItem (CM_TABSTOSPACES, canCut) ;

  CheckMenuItem (CM_SHOWWHITESPACE, Editor != NULL && Editor->IsWhitespaceDisplayEnabled ()) ;
  CheckMenuItem (CM_UNDOAFTERSAVE, UndoAfterSave) ;
  CheckMenuItem (CM_CREATEBACKUPS, CreateBackups) ;

  SetMenuItemText (CM_SHOWMESSAGES, MessagePaneVisible ? "Hide Message &Window" : "Show Message &Window") ;
  SetMenuShortcut (CM_SHOWMESSAGES, CM_SHOWMESSAGES) ;

  if (Editor != NULL)
  {
    CheckMenuRadioItem (CM_INDENTSTYLEFIRST + (int) Editor->GetAutoIndent (), CM_INDENTSTYLEFIRST, CM_INDENTSTYLELAST) ;
    CheckMenuRadioItem (CM_SCROLLFIRST + (int) Editor->GetScrollBars (), CM_SCROLLFIRST, CM_SCROLLLAST) ;
  }

  CheckMenuRadioItem (CM_AUTORELOADFIRST + AutoReload, CM_AUTORELOADFIRST, CM_AUTORELOADLAST) ;
}

void UpdateWindowMenu (void)
{
  int             i;
  char            str[_MAX_PATH];
  const char      *s;

  for (i = 0; i < MAX_EDITORS + 1; i++)
    DeleteMenu (hWindowMenu, i + CM_FIRSTWINDOW, MF_BYCOMMAND) ;
  for (i = 0; i < MAX_EDITORS + 1; i++)
  {
    if (WindowList[i] == NULL)
      break;
    if ((s = strrchr(WindowList[i], '\\')) == NULL)
      s = WindowList[i];
    else
      s++;
    if (i < 9)
      sprintf(str, "&%d %s", i + 1, s);
    else if (i == 9)
      sprintf(str, "1&0 %s", s);
    else
      sprintf(str, "%d %s", i + 1, s);
    AppendMenu(hWindowMenu, MF_STRING, i + CM_FIRSTWINDOW, str) ;
  }
  DrawMenuBar (hMainWindow) ;
}

//------------------------------------------------------------------------------------------------------------------------

