/*******************************************************************************
 * eventhandlers.cpp
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/eventhandlers.cpp $
 * $Revision: #4 $
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
#include "dialogs.h"
#include "..\..\pvedit.h"

HGLOBAL                 hPrinterDevMode ;
HGLOBAL                 hPrinterDevName ;

extern int              AutoReload ;
extern int              EditorCount ;
extern int              AutoSaveDelay ;
extern int              NotifyBase ;
extern int              PrintPointSize ;
extern int              PrintUseBorder ;
extern int              PrintTopMargin ;
extern int              PrintLeftMargin ;
extern int              PrintBottomMargin ;
extern int              PrintRightMargin ;
extern bool             MessagePaneVisible ;
extern bool             CreateBackups ;
extern bool             LastOverwrite ;
extern bool             UndoAfterSave ;
extern bool             PrintUseColor ;
extern bool             PrintUseFancyText ;
extern bool             PrintUsePageNumbers ;
extern bool             PrintUseDateTime ;
extern bool             PrintUseFileName ;
extern bool             PrintMarginUnitsMetric ;
extern char             MessageWinTitle[];
extern HWND             hMainWindow ;
extern HWND             hStatusWindow ;
extern HWND             hTabWindow ;
extern HWND             hNotifyWindow ;
extern CCodeMax         *Editors [MAX_EDITORS] ;
extern CCodeMax         *Editor ;
extern CM_COLORS        CJCColours ;
extern CM_COLORS        DefaultColours ;
extern HINSTANCE        hInstance ;
extern const char       *WindowList[MAX_EDITORS + 1];
extern CStdString       InsertPath ;
extern CStdString       IncludeFilename ;
extern CStdString       CommandLine ;
extern CStdString       POVRayIniPath ;
extern CStdString       DocumentsPath ;
extern CStdStringList   RecentFiles ;
extern CStdStringList   OlderFiles ;
extern CStdStringList   InsertMenuItems ;
extern EditConfigStruct EditConfig ;

void LocateRecentFile (CStdString FileName, CCodeMax *e)
{
  int                   count ;
  CStdString            str ;

  count = RecentFiles.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = RecentFiles [i] ;
    if (FileName.CompareNoCase (GetNextField (str)) == 0)
    {
      e->SetLineNo (atoi (GetNextField (str))) ;
      e->SetColNo (atoi (GetNextField (str))) ;
      e->SetTopLine (atoi (GetNextField (str))) ;
      e->SetLanguage ((TLanguage) atoi (GetNextField (str))) ;
      e->SetTabSize (atoi (GetNextField (str))) ;
      e->SetAutoIndent ((TAutoIndent) atoi (GetNextField (str))) ;
      return ;
    }
  }
  count = OlderFiles.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = OlderFiles [i] ;
    if (FileName.CompareNoCase (GetNextField (str)) == 0)
    {
      e->SetLineNo (atoi (GetNextField (str))) ;
      e->SetColNo (atoi (GetNextField (str))) ;
      e->SetTopLine (atoi (GetNextField (str))) ;
      e->SetLanguage ((TLanguage) atoi (GetNextField (str))) ;
      e->SetTabSize (atoi (GetNextField (str))) ;
      e->SetAutoIndent ((TAutoIndent) atoi (GetNextField (str))) ;
    }
  }
}

void AutoSaveClick (WPARAM wParam, LPARAM lParam)
{
  int result = ShowEnterValueDialog ("Time in min (0=off)", 0, 60, AutoSaveDelay / 60) ;
  if (result >= 0)
    AutoSaveDelay = min (result, 60) * 60 ;
}

void ConstrainCaretClick (WPARAM wParam, LPARAM lParam)
{
  EditConfig.SelBoundsEnabled = ToggleMenuItem (CM_CONSTRAINCARET) ;
  for (int i = 0 ; i < EditorCount ; i++)
    Editors [i]->EnableSelBounds (EditConfig.SelBoundsEnabled) ;
  if (IsMenuItemChecked (CM_CONSTRAINCARET))
  {
    CheckMenuItem (CM_CURSORBEYONDEOL, false) ;
    EnableMenuItem (CM_CURSORBEYONDEOL, false) ;
  }
  else
    EnableMenuItem (CM_CURSORBEYONDEOL, true) ;
}

void FindClick ()
{
  Editor->Find () ;
  EditConfig.WholeWordEnabled = Editor->IsWholeWordEnabled () ;
  EditConfig.PreserveCase = Editor->IsPreserveCaseEnabled () ;
  EditConfig.CaseSensitive = Editor->IsCaseSensitiveEnabled () ;
  EditConfig.RegexpEnabled = Editor->IsRegExpEnabled () ;
}

void ReplaceClick ()
{
  Editor->Replace () ;
  EditConfig.WholeWordEnabled = Editor->IsWholeWordEnabled () ;
  EditConfig.PreserveCase = Editor->IsPreserveCaseEnabled () ;
  EditConfig.CaseSensitive = Editor->IsCaseSensitiveEnabled () ;
  EditConfig.RegexpEnabled = Editor->IsRegExpEnabled () ;
}

void SaveAsClick (WPARAM wParam, LPARAM lParam)
{
  char                  oldName [_MAX_PATH] ;
  TCITEM                item ;
  EditTagStruct         tag ;

  tag = Editor->m_Tag ;
  strcpy (oldName, tag.LongName) ;
  if (!Editor->AskFileName (&tag))
    return ;
  if (Editor->SaveFile (tag.LongName, !UndoAfterSave) == CME_SUCCESS)
  {
    Editor->m_Tag = tag ;
    Editor->UpdateFileTime () ;
    if (strcmp (tag.LongName, oldName) != 0)
    {
      item.mask = TCIF_TEXT ;
      item.dwState = item.dwStateMask = 0 ;
      item.pszText = tag.ShortName ;
      TabCtrl_SetItem (hTabWindow, Editor->m_Index + 1, &item) ;
      UpdateRecent () ;
      Editor->SetLanguageBasedOnFileType () ;
      AddToRecent (tag.LongName) ;
    }
    // pretend the tab has changed so the window caption is updated
    SendMessage (hNotifyWindow, WM_COMMAND, NotifyBase + NotifyTabChange, GetFlags ()) ;
  }
  else
    ShowErrorMessage (tag.ShortName, "Failed to save file") ;
}

void PrintClick (void)
{
  CM_RANGE    range ;
  PRINTDLG    pd ;
  CM_PRINTEX  cmpex ;

  memset (&range, 0, sizeof (CM_RANGE)) ;
  Editor->GetSel (&range) ;

  ZeroMemory (&pd, sizeof (pd)) ;
  pd.lStructSize = sizeof (pd) ;
  pd.hwndOwner = hMainWindow ;
  pd.hDevMode = hPrinterDevMode ;
  pd.hDevNames = hPrinterDevName ;
  pd.nMinPage = 1 ;
  pd.nMaxPage = 9999 ;
  pd.lpfnPrintHook = PrintHook ;
  pd.lpPrintTemplateName = MAKEINTRESOURCE (IDD_PRINT) ;
  pd.Flags = PD_RETURNDC | PD_USEDEVMODECOPIESANDCOLLATE | PD_ENABLEPRINTHOOK | PD_ENABLEPRINTTEMPLATE ;
  if (memcmp (&range.posEnd, &range.posStart, sizeof (CM_POSITION)) == 0)
    pd.Flags |= PD_NOSELECTION ;
  else
    pd.Flags |= PD_SELECTION ;
  pd.hInstance = hInstance ;

  if (PrintDlg (&pd))
  {
    hPrinterDevMode = pd.hDevMode ;
    hPrinterDevName = pd.hDevNames ;

    ZeroMemory (&cmpex, sizeof (cmpex)) ;

    // Editor->Print() will delete the DC for us
    cmpex.hDC = pd.hDC ;
    cmpex.dwFlags = CM_PRINT_HDC ;

    if (PrintUseColor)
      cmpex.dwFlags |= CM_PRINT_COLOR ;
    if (PrintUseFancyText)
      cmpex.dwFlags |= CM_PRINT_RICHFONTS ;
    if (PrintUseBorder)
      cmpex.dwFlags |= CM_PRINT_BORDERTHIN << (PrintUseBorder - 1) ;
    if (PrintUseDateTime)
      cmpex.dwFlags |= CM_PRINT_DATETIME ;
    if (PrintUsePageNumbers)
      cmpex.dwFlags |= CM_PRINT_PAGENUMS ;

    if (PrintUseFileName)
    {
      cmpex.dwFlags |= CM_PRINT_FILENAME ;
      cmpex.pszFileName = Editor->m_Tag.ShortName ;
    }

    if ((pd.Flags & PD_SELECTION) != 0)
      cmpex.dwFlags |= CM_PRINT_SELECTION ;

    if (PrintPointSize != 0)
      cmpex.dwPointSize = PrintPointSize ;

    if ((pd.Flags & PD_PAGENUMS) != 0)
    {
      cmpex.dwFlags |= CM_PRINT_PAGERANGE ;
      cmpex.dwFirstPage = pd.nFromPage ;
      cmpex.dwLastPage = pd.nToPage ;
    }

    if (PrintTopMargin != -1 && PrintBottomMargin != -1 && PrintLeftMargin != -1 && PrintRightMargin != -1)
    {
      if (PrintMarginUnitsMetric)
      {
        // convert hundredths of a millimeter to thousandths of an inch
        cmpex.rcMargin.left = PrintLeftMargin * 1000 / 2630 ;
        cmpex.rcMargin.right = PrintRightMargin * 1000 / 2630 ;
        cmpex.rcMargin.top = PrintTopMargin * 1000 / 2630 ;
        cmpex.rcMargin.bottom = PrintBottomMargin * 1000 / 2630 ;
      }
      else
      {
        cmpex.rcMargin.left = PrintLeftMargin ;
        cmpex.rcMargin.right = PrintRightMargin ;
        cmpex.rcMargin.top = PrintTopMargin ;
        cmpex.rcMargin.bottom = PrintBottomMargin ;
      }
    }

    Editor->Print (&cmpex) ;
  }
}

void PageSetupClick (void)
{
  PAGESETUPDLG          psd ;

  ZeroMemory (&psd, sizeof (psd)) ;
  psd.lStructSize = sizeof (psd) ;
  psd.hwndOwner = hMainWindow ;
  psd.hDevMode = hPrinterDevMode ;
  psd.hDevNames = hPrinterDevName ;
  psd.Flags = PSD_ENABLEPAGESETUPHOOK | PSD_ENABLEPAGESETUPTEMPLATE ;
  if (PrintBottomMargin != -1 && PrintTopMargin != -1 && PrintLeftMargin != -1 && PrintRightMargin != -1)
    psd.Flags |= PSD_MARGINS ;
  psd.hInstance = hInstance ;
  psd.lpfnPageSetupHook = PageSetupHook ;
  psd.lpPageSetupTemplateName = MAKEINTRESOURCE (IDD_PAGESETUP) ;
  psd.rtMargin.bottom = PrintBottomMargin ;
  psd.rtMargin.top = PrintTopMargin ;
  psd.rtMargin.left = PrintLeftMargin ;
  psd.rtMargin.right = PrintRightMargin ;

  if (PageSetupDlg (&psd))
  {
    hPrinterDevMode = psd.hDevMode ;
    hPrinterDevName = psd.hDevNames ;
    PrintLeftMargin = psd.rtMargin.left ;
    PrintTopMargin = psd.rtMargin.top ;
    PrintRightMargin = psd.rtMargin.right ;
    PrintBottomMargin = psd.rtMargin.bottom ;
    PrintMarginUnitsMetric = (psd.Flags & PSD_INHUNDREDTHSOFMILLIMETERS) != 0 ;
  }
}

void ScrollBarsClick (WPARAM wParam, LPARAM lParam)
{
  int                   i ;
  CCodeMax              **e ;

  CheckMenuRadioItem (LOWORD (wParam), CM_SCROLLFIRST, CM_SCROLLLAST) ;
  EditConfig.ScrollBars = (TScrollStyle) (LOWORD (wParam) - CM_SCROLLFIRST) ;
  for (i = 0, e = Editors ; i < EditorCount ; i++, e++)
  {
    (*e)->SetScrollBars (EditConfig.ScrollBars) ;
    (*e)->EnableVSplitter (true) ;
    (*e)->EnableHSplitter (true) ;
  }
}

void SetTabSizeClick (WPARAM wParam, LPARAM lParam)
{
  if (Editor != NULL)
  {
    int result = ShowEnterValueDialog ("Enter New Tab Size", 1, 16, Editor->GetTabSize ()) ;
    if (result >= 1 && result <= 16)
      Editor->SetTabSize (EditConfig.TabSize = result) ;
  }
}

void OpenFileUnderCursorClick (WPARAM wParam, LPARAM lParam)
{
  int                   count ;
  CCodeMax              *e ;
  CStdString            str ;
  CStdStringList        sl ;

  if (Editor == NULL)
    return ;
  if (IncludeFilename == "")
  {
    SendMessage (hMainWindow, COPY_COMMANDLINE_MESSAGE, 0, (LPARAM) (LPCSTR) CommandLine) ;
    return ;
  }
  str = IncludeFilename ;
  // append dir if it isn't an absolute path
  if (str [0] != '\\' && !(str.length () > 1 && isalpha (str [0]) && str [1] == ':'))
    str = GetFilePath (Editor->m_Tag.LongName) + "\\" + IncludeFilename ;
  if (FileExists (str))
  {
    if ((e = CreateNewEditor (str, false, true)) != NULL)
    {
      LocateRecentFile (str, e) ;
      str.Format ("%s,%d,%d,%d,%d,%d,%d",
                  e->m_Tag.LongName,
                  e->GetLineNo (),
                  e->GetColNo (),
                  e->GetTopLine (),
                  (int) e->GetLanguage (),
                  e->GetTabSize (),
                  (int) e->GetAutoIndent ()) ;
      AddToRecent (str) ;
    }
    return ;
  }
  sl.LoadFromFile (POVRayIniPath) ;
  str.Format ("Library_Path=%sinclude", DocumentsPath) ;
  sl.AppendItem (str) ;
  count = sl.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = sl [i].Trim () ;
    if (str.Left (12).CompareNoCase ("Library_Path") == 0)
    {
      str = str.Mid (12).TrimLeft () ;
      if (str [0] != '=')
        continue ;
      str.Delete (0) ;
      str = FixPath (UnquotePath (str) + "\\" + IncludeFilename) ;
      // append dir if it isn't an absolute path
      if (str [0] != '\\' && !(str.length () > 1 && isalpha (str [0]) && str [1] == ':'))
        str = GetFilePath (Editor->m_Tag.LongName) + "\\" + str ;
      if (FileExists (str))
      {
        if ((e = CreateNewEditor (str, false, true)) != NULL)
        {
          LocateRecentFile (str, e) ;
          str.Format ("%s,%d,%d,%d,%d,%d,%d",
                      e->m_Tag.LongName,
                      e->GetLineNo (),
                      e->GetColNo (),
                      e->GetTopLine (),
                      (int) e->GetLanguage (),
                      e->GetTabSize (),
                      (int) e->GetAutoIndent ()) ;
          AddToRecent (str) ;
        }
        return ;
      }
    }
  }
  PutStatusMessage ("Could not locate file") ;
}

void InsertMenuClick (int id)
{
  CStdString  str = InsertMenuItems [id] + ".txt" ;
  CM_POSITION position ;

  Editor->GetPosition (&position) ;
  if (Editor->InsertFile (str, &position) == CME_FAILURE)
    ShowErrorMessage ("", "Failed to insert file") ;
}

void RecentFilesClick (int id)
{
  CCodeMax    *e ;
  CStdString  str ;

  UpdateRecent () ;
  if ((str = RecentFiles [id]) == "")
    return ;
  AddToRecent (str) ;
  if (SelectFile (str))
    return ;
  if ((e = CreateNewEditor (GetNextField (str), false, true)) != NULL)
  {
    e->SetLineNo (atoi (GetNextField (str))) ;
    e->SetColNo (atoi (GetNextField (str))) ;
    e->SetTopLine (atoi (GetNextField (str))) ;
    e->SetLanguage ((TLanguage) atoi (GetNextField (str))) ;
    e->SetTabSize (atoi (GetNextField (str))) ;
    e->SetAutoIndent ((TAutoIndent) atoi (GetNextField (str))) ;
  }
}

void OlderFilesClick (int id)
{
  CCodeMax    *e ;
  CStdString  str ;

  UpdateRecent () ;
  if ((str = OlderFiles [id]) == "")
    return ;
  AddToRecent (str) ;
  if (SelectFile (str))
    return ;
  if ((e = CreateNewEditor (GetNextField (str), false, true)) != NULL)
  {
    e->SetLineNo (atoi (GetNextField (str))) ;
    e->SetColNo (atoi (GetNextField (str))) ;
    e->SetTopLine (atoi (GetNextField (str))) ;
    e->SetLanguage ((TLanguage) atoi (GetNextField (str))) ;
    e->SetTabSize (atoi (GetNextField (str))) ;
    e->SetAutoIndent ((TAutoIndent) atoi (GetNextField (str))) ;
  }
}

bool HandleCommand (WPARAM wParam, LPARAM lParam)
{
  PutStatusMessage ("") ;

  if (LOWORD (wParam) >= CM_FIRSTRECENTFILE && LOWORD (wParam) <= CM_LASTRECENTFILE)
  {
    RecentFilesClick (LOWORD (wParam) - CM_FIRSTRECENTFILE) ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_FIRSTOLDERFILE && LOWORD (wParam) <= CM_LASTOLDERFILE)
  {
    OlderFilesClick (LOWORD (wParam) - CM_FIRSTOLDERFILE) ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_FIRSTWINDOW && LOWORD (wParam) <= CM_LASTWINDOW)
  {
    int i = LOWORD(wParam) - CM_FIRSTWINDOW;
    if (i > MAX_EDITORS) // NB > is correct, we don't want >= in this case
      return (true);
    if (WindowList[i] == NULL)
      return (true);
    if (WindowList[i] == MessageWinTitle)
    {
      TabCtrl_SetCurSel (hTabWindow, 0) ;
      TabIndexChanged();
    }
    else
      SelectFile (WindowList[i]) ;
    return (true) ;
  }

  // handle those messages that can be handled even if Editor == NULL
  switch (LOWORD (wParam))
  {
    case CM_SAVEALL:
         SaveAllFiles (true) ;
         return (true) ;

    case CM_OPENFILE:
         BrowseFile (true) ;
         return (true) ;

    case CM_NEWFILE:
         LoadFile (NULL) ;
         return (true) ;

    case CM_CLOSEALLFILES:
         CloseAll () ;
         return (true) ;

    case CM_PAGESETUP:
         PageSetupClick () ;
         return (true) ;

    case CM_EXIT:
         SendMessage (hNotifyWindow, WM_COMMAND, NotifyBase + NotifyExitRequest, 0) ;
         return (true) ;

    case CM_FILEMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "File Menu") ;
         return (true) ;

    case CM_INSERTMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "Insert Menu") ;
         return (true) ;

    case CM_SEARCHMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "Search Menu") ;
         return (true) ;

    case CM_TEXTMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "Text Menu") ;
         return (true) ;

    case CM_EDITMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "Edit Menu") ;
         return (true) ;

    case CM_EDITORMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "Editor Menu") ;
         return (true) ;

    case CM_WINDOWMENUHELP:
         SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "Window Menu") ;
         return (true) ;
  }

  if (Editor == NULL)
    return (false) ;

  if (LOWORD (wParam) >= CM_FIRSTINSERTMENUITEM && LOWORD (wParam) <= CM_LASTINSERTMENUITEM)
  {
    InsertMenuClick (LOWORD (wParam) - CM_FIRSTINSERTMENUITEM) ;
    return (true) ;
  }

  switch (LOWORD (wParam))
  {
    case CM_SHOWPARSEMESSAGES :
    case CM_CURSORBEYONDEOL :
    case CM_AUTOLOADERRORFILE :
         ToggleMenuItem (LOWORD (wParam)) ;
         return (true) ;

    case CM_CUT:
         Editor->Cut () ;
         return (true) ;

    case CM_COPY:
         Editor->Copy () ;
         return (true) ;

    case CM_PASTE:
         Editor->Paste () ;
         return (true) ;

    case CM_UNDO:
         Editor->Undo () ;
         return (true) ;

    case CM_REDO:
         Editor->Redo () ;
         return (true) ;

    case CM_TOGGLEBOOKMARK:
         Editor->ExecuteCommand (CMD_BOOKMARKTOGGLE, 0) ;
         return (true) ;

    case CM_FIRSTBOOKMARK:
         Editor->ExecuteCommand (CMD_BOOKMARKJUMPTOFIRST, 0) ;
         return (true) ;

    case CM_LASTBOOKMARK:
         Editor->ExecuteCommand (CMD_BOOKMARKJUMPTOLAST, 0) ;
         return (true) ;

    case CM_NEXTBOOKMARK:
         Editor->ExecuteCommand (CMD_BOOKMARKNEXT, 0) ;
         return (true) ;

    case CM_PREVIOUSBOOKMARK:
         Editor->ExecuteCommand (CMD_BOOKMARKPREV, 0) ;
         return (true) ;

    case CM_CLEARALLBOOKMARKS:
         Editor->ExecuteCommand (CMD_BOOKMARKCLEARALL, 0) ;
         return (true) ;

    case CM_SAVE:
         SaveFile (NULL) ;
         return (true) ;

    case CM_SAVEAS:
         SaveAsClick (wParam, lParam) ;
         return (true) ;

    case CM_CLOSEFILE:
    case CM_CLOSECURRENTFILE:
         CloseFile (NULL) ;
         return (true) ;

    case CM_PRINT:
         PrintClick () ;
         return (true) ;

    case CM_FIND:
         FindClick () ;
         return (true) ;

    case CM_REPLACE:
         ReplaceClick () ;
         return (true) ;

    case CM_FINDNEXT:
         Editor->FindNext () ;
         return (true) ;

    case CM_MATCHBRACE:
         Editor->ExecuteCommand (CMD_GOTOMATCHBRACE, 0) ;
         return (true) ;

    case CM_GOTOLINE:
         Editor->GoToLine (-1) ;
         return (true) ;

    case CM_INDENTSELECTION:
         Editor->ExecuteCommand (CMD_INDENTSELECTION, 0) ;
         return (true) ;

    case CM_INDENTSELECTIONPREVIOUS:
         Editor->ExecuteCommand (CMD_INDENTTOPREV, 0) ;
         return (true) ;

    case CM_UNDENTSELECTION:
         Editor->ExecuteCommand (CMD_UNINDENTSELECTION, 0) ;
         return (true) ;

    case CM_UPPERCASESELECTION:
         Editor->ExecuteCommand (CMD_UPPERCASESELECTION, 0) ;
         return (true) ;

    case CM_LOWERCASESELECTION:
         Editor->ExecuteCommand (CMD_LOWERCASESELECTION, 0) ;
         return (true) ;

    case CM_SPACESTOTABS:
         Editor->ExecuteCommand (CMD_TABIFYSELECTION, 0) ;
         return (true) ;

    case CM_TABSTOSPACES:
         Editor->ExecuteCommand (CMD_UNTABIFYSELECTION, 0) ;
         return (true) ;

    case CM_OPENFILEUNDERCURSOR:
         OpenFileUnderCursorClick (wParam, lParam) ;
         return (true) ;

    case CM_CONTEXTHELP:
         GetContextHelp () ;
         return (true) ;

    case CM_SHOWMESSAGES:
         MessagePaneVisible = !MessagePaneVisible ;
         ShowMessagePane () ;
         return (true) ;

    case CM_PROPERTIES:
    case CM_CUSTOMCOLOURS:
         Editor->ShowProperties () ;
         return (true) ;

    case CM_OLDERFILES:
         return (true) ;

    case CM_DELETE:
         Editor->ExecuteCommand (CMD_DELETE, 0) ;
         return (true) ;

    case CM_SELECTALL:
         Editor->ExecuteCommand (CMD_SELECTALL, 0) ;    
         return (true) ;

    case CM_SHOWWHITESPACE:
         Editor->EnableWhitespaceDisplay (ToggleMenuItem (CM_SHOWWHITESPACE)) ;
         return (true) ;

    case CM_SETREPEATCOUNT:
         Editor->ExecuteCommand (CMD_SETREPEATCOUNT, 0) ;
         return (true) ;

    case CM_SETTABSIZE:
         SetTabSizeClick (wParam, lParam) ;
         return (true) ;

    case CM_INDENTSTYLENONE:
    case CM_INDENTSTYLELANGUAGE:
    case CM_INDENTSTYLECOPY:
         CheckMenuRadioItem (LOWORD (wParam), CM_INDENTSTYLEFIRST, CM_INDENTSTYLELAST) ;
         EditConfig.AutoIndent = (TAutoIndent) (LOWORD (wParam) - CM_INDENTSTYLEFIRST) ;
         Editor->SetAutoIndent (EditConfig.AutoIndent) ;
         return (true) ;

    case CM_AUTOSAVE:
         AutoSaveClick (wParam, lParam) ;
         return (true) ;

    case CM_CREATEBACKUPS:
         CreateBackups = ToggleMenuItem (CM_CREATEBACKUPS) ;
         return (true) ;

    case CM_UNDOAFTERSAVE:
         UndoAfterSave = ToggleMenuItem (CM_UNDOAFTERSAVE) ;
         return (true) ;

    case CM_CONSTRAINCARET:
         ConstrainCaretClick (wParam, lParam) ;
         return (true) ;

    case CM_OVERLAYKEYWORDEXPANSION:
         EditConfig.TabKeywordExpansion = ToggleMenuItem (CM_OVERLAYKEYWORDEXPANSION) ;
         return (true) ;

    case CM_AUTORELOADNEVER:
    case CM_AUTORELOADASK:
    case CM_AUTORELOADALWAYS:
         AutoReload = LOWORD (wParam) - CM_AUTORELOADFIRST ;
         return (true) ;

    case CM_SCROLLNONE:
    case CM_SCROLLHORIZONTAL:
    case CM_SCROLLVERTICAL:
    case CM_SCROLLBOTH:
         ScrollBarsClick (wParam, lParam) ;
         return (true) ;

    case CM_BLACKONWHITE:
         memcpy (&EditConfig.Colours, &DefaultColours, sizeof (CM_COLORS)) ;
         for (int i = 0 ; i < EditorCount ; i++)
           Editors [i]->SetColors (&EditConfig.Colours) ;
         return (true) ;

    case CM_WHITEONBLACK:
         memcpy (&EditConfig.Colours, &CJCColours, sizeof (CM_COLORS)) ;
         for (int i = 0 ; i < EditorCount ; i++)
           Editors [i]->SetColors (&EditConfig.Colours) ;
         return (true) ;

    case CM_RECORDMACRO:
         Editor->ExecuteCommand (CMD_RECORDMACRO, 0) ;
         return (true) ;

    case CM_PLAYMACRO1:
    case CM_PLAYMACRO2:
    case CM_PLAYMACRO3:
    case CM_PLAYMACRO4:
    case CM_PLAYMACRO5:
    case CM_PLAYMACRO6:
    case CM_PLAYMACRO7:
    case CM_PLAYMACRO8:
    case CM_PLAYMACRO9:
    case CM_PLAYMACRO10:
         Editor->ExecuteCommand ((WORD) (CMD_PLAYMACRO1 + LOWORD (wParam) - CM_PLAYMACRO1), 0) ;
         return (true) ;

    case CM_EDITINSERTMENU:
         ShellExecute (hMainWindow, "open", InsertPath, NULL, NULL, SW_SHOWNORMAL) ;
         return (true) ;

    case CM_SPLITHORIZONTALLY:
         if (Editor->IsHSplitterEnabled() && Editor->GetHSplitterPos() == 0)
         {
           RECT rect;
           GetClientRect (hTabWindow, &rect) ;
           Editor->SetHSplitterPos(rect.right / 2);
         }
         return (true);

    case CM_SPLITVERTICALLY:
         if (Editor->IsVSplitterEnabled() && Editor->GetVSplitterPos() == 0)
         {
           RECT rect;
           GetClientRect (hTabWindow, &rect) ;
           Editor->SetVSplitterPos(rect.bottom / 2);
         }
         return (true);
  }
  return (false) ;
}
