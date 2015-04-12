/*******************************************************************************
 * ccodemax.h
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/ccodemax.h $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __CCODEMAX_H__
#define __CCODEMAX_H__

#include "..\source\codemax.h"

enum TLanguage {cmlNone, cmlCCpp, cmlBasic, cmlJava, cmlPascal, cmlSQL, cmlPOVRay, cmlHTML, cmlSGML, cmlINI} ;
enum TAutoIndent {cmiNone=CM_INDENT_OFF, cmiScope=CM_INDENT_SCOPE, cmiPrevLine=CM_INDENT_PREVLINE} ;
enum TScrollStyle {ssNone, ssHorizontal, ssVertical, ssBoth} ;

struct _EditConfigStruct ;

typedef enum
{
  stSave,
  stSaved = stSave,
  stDiscard,
  stDiscarded = stDiscard,
  stSaveAll,
  stDiscardAll,
  stCancel,
  stContinue,
  stRetry,
  stError
} TSaveType ;

typedef struct
{
  char                  ShortName [_MAX_PATH] ;
  char                  LongName [_MAX_PATH] ;
  FILETIME              TimeSaved ;
} EditTagStruct ;

class CCodeMax
{
public:
  int                   m_Index ;
  int                   m_RMBDownX ;
  int                   m_RMBDownY ;
  int                   m_RMBDownLine ;
  int                   m_RMBDownCol ;
  bool                  m_Opened ;
  bool                  m_BackedUp ;
  bool                  m_LButtonDown ;
  HWND                  m_hWnd ;
  char                  m_LanguageName [64] ;
  WNDPROC               m_OldWndProc ;
  TLanguage             m_Language ;
  EditTagStruct         m_Tag ;

  static bool           SaveDialogActive ;

  CCodeMax (HWND parent) ;
  ~CCodeMax () ;

  operator const HWND () const { return (this->m_hWnd) ; }

  void PopupMenuPopup (void) ;
  CStdString LocateIncludeFilename (int line, int col) ;
  CStdString LocateCommandLine (int line) ;
  LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) ;
  bool IsCodeUpToDate (bool Stale) ;
  bool AskFileName (EditTagStruct *t = NULL) ;
  void SetLanguageBasedOnFileType (void) ;
  void SetupEditor (const struct _EditConfigStruct *ec, bool PropsChange, bool InitCM) ;
  TSaveType SaveEditorFile (void) ;
  TSaveType TrySave (bool ContinueOption) ;
  void UpdateFileTime (void) ;
  void SetFileName (LPCTSTR FileName) ;
  void SetOpened (bool IsOpened) ;
  void SetLanguageName (LPCSTR Language) { CM_SetLanguage (m_hWnd, Language) ; }
  void GetConfigFromInstance (struct _EditConfigStruct *ec) ;
  CStdString GetCurrentKeyword (void) ;
  LPCTSTR GetLanguageName (void) ;
  void SetLanguage (TLanguage Language) ;
  TLanguage GetLanguage (void) ;
  void SetAutoIndent (TAutoIndent AutoIndent) { CM_SetAutoIndentMode (m_hWnd, int (AutoIndent)) ; }
  TAutoIndent GetAutoIndent (void) { return (TAutoIndent (CM_GetAutoIndentMode (m_hWnd))) ; }
  bool GetColourSyntax (void) { return CM_IsColorSyntaxEnabled (m_hWnd) != 0 ; }
  void SetColourSyntax (bool enabled) { CM_EnableColorSyntax (m_hWnd, enabled) ; }
  int GetLineLength (int line) { return (CM_GetLineLength (m_hWnd, line - 1, false)) ; }
  TScrollStyle GetScrollBars (void) ;
  void SetScrollBars (TScrollStyle style) ;
  int GetHSplitterPos (void) { return (CM_GetSplitterPos (m_hWnd, true)) ; }
  int GetVSplitterPos (void) { return (CM_GetSplitterPos (m_hWnd, false)) ; }
  void SetHSplitterPos (int pos) { CM_SetSplitterPos (m_hWnd, true, pos) ; }
  void SetVSplitterPos (int pos) { CM_SetSplitterPos (m_hWnd, false, pos) ; }
  bool GetHSplitterEnable (void) { return (CM_IsSplitterEnabled (m_hWnd, 1) != 0) ; }
  void SetHSplitterEnable (bool enable) { CM_EnableSplitter (m_hWnd, 1, enable) ; }
  bool GetVSplitterEnable (void) { return (CM_IsSplitterEnabled (m_hWnd, 0) != 0) ; }
  void SetVSplitterEnable (bool enable) { CM_EnableSplitter (m_hWnd, 0, enable) ; }
  int GetLineNo (void) ;
  void SetLineNo (int LineNo) ;
  int GetColNo (void) ;
  void SetColNo (int ColNo) ;
  void SetPosition (int LineNo, int ColNo) ;
  int GetTopLine (void) { return (CM_GetTopIndex (m_hWnd, 0) + 1) ; }
  void SetTopLine (int TopLine) { CM_SetTopIndex (m_hWnd, 0, TopLine - 1) ; }

  CME_CODE GetLine (int nLine, LPTSTR pszBuff) { return (CM_GetLine (m_hWnd, nLine - 1, pszBuff)) ; }
  CME_CODE InsertText (LPCTSTR Text, const CM_POSITION *pPos = NULL) { return (CM_InsertText (m_hWnd, Text, pPos)) ; }
  CME_CODE Copy (void) { return (CM_Copy (m_hWnd)) ; }
  CME_CODE Cut (void) { return (CM_Cut (m_hWnd)) ; }
  CME_CODE Paste (void) { return (CM_Paste (m_hWnd)) ; }
  CME_CODE ClearUndoBuffer (void) { return (CM_ClearUndoBuffer (m_hWnd)) ; }
  CME_CODE InsertFile (LPCTSTR FileName, const CM_POSITION *pPos = NULL) { return (CM_InsertFile (m_hWnd, FileName, pPos )) ; }
  CME_CODE SetColors (const CM_COLORS *pColors) { return (CM_SetColors (m_hWnd, pColors)) ; }
  CME_CODE GetColors (CM_COLORS *pColors) { return (CM_GetColors (m_hWnd, pColors)) ; }
  CME_CODE SetFontStyles (const CM_FONTSTYLES *pFontStyles) { return (CM_SetFontStyles (m_hWnd, pFontStyles)) ; }
  CME_CODE GetFontStyles (CM_FONTSTYLES *pFontStyles) { return (CM_GetFontStyles (m_hWnd, pFontStyles)) ; }
  CME_CODE ReplaceText (LPCTSTR pszText, const CM_RANGE *pRange = NULL) { return (CM_ReplaceText (m_hWnd, pszText, pRange)) ; }
  CME_CODE SetText (LPCTSTR pszText) { return (CM_SetText (m_hWnd, pszText)) ; }
  CME_CODE GetText (LPTSTR pszBuff, const CM_RANGE *pRange = NULL) { return (CM_GetText (m_hWnd, pszBuff, pRange)) ; }
  int GetTextLength (const CM_RANGE *pRange = NULL) { return (CM_GetTextLength (m_hWnd, pRange, false)) ; }
  CME_CODE SaveFile (LPCTSTR pszFileName, BOOL bClearUndo = TRUE) { return (CM_SaveFile (m_hWnd, pszFileName, bClearUndo)) ; }
  CME_CODE OpenFile (LPCTSTR File) { return (CM_OpenFile (m_hWnd, File)) ; }
  CME_CODE GetWord (LPTSTR pszBuff, CM_POSITION *pPos) { return (CM_GetWord (m_hWnd, pszBuff, pPos)) ; }
  int GetWordLength (CM_POSITION *pPos) { return (CM_GetWordLength (m_hWnd, pPos, false)) ; }
  CStdString GetCurrentWord (void) { return (GetWord (NULL)) ; }
  CME_CODE AddText (LPCTSTR pszText) { return (CM_AddText (m_hWnd, pszText )) ; }
  CME_CODE DeleteLine (int nLine) { return (CM_DeleteLine (m_hWnd, nLine - 1)) ; }
  CME_CODE InsertLine (int nLine, LPCTSTR pszText) { return (CM_InsertLine (m_hWnd, nLine - 1, pszText)) ; }
  CME_CODE GetSel (CM_RANGE *pRange, BOOL bNormalized = TRUE) { return (CM_GetSel (m_hWnd, pRange, bNormalized)) ; }
  CME_CODE SetSel (const CM_RANGE *pRange, BOOL bMakeVisible = TRUE) { return (CM_SetSel (m_hWnd, pRange, bMakeVisible)) ; }
  CME_CODE DeleteSel (void) { return (CM_DeleteSel (m_hWnd)) ; }
  CME_CODE ReplaceSel (LPCTSTR pszText) { return (CM_ReplaceSel (m_hWnd, pszText)) ; }
  CME_CODE ExecuteCmd (WORD wCmd, DWORD dwCmdData = 0) { return (CM_ExecuteCmd (m_hWnd, wCmd, dwCmdData )) ; }
  CME_CODE SetSplitterPos (BOOL bHorz, int nPos) { return (CM_SetSplitterPos (m_hWnd, bHorz, nPos)) ; }
  int GetSplitterPos (BOOL bHorz) { return (CM_GetSplitterPos (m_hWnd, bHorz)) ; }
  CME_CODE SetTopIndex (int nView, int nLine) { return (CM_SetTopIndex (m_hWnd, nView, nLine - 1)) ; }
  int GetTopIndex (int nView) { return (CM_GetTopIndex (m_hWnd, nView)) ; }
  int GetVisibleLineCount (int nView, BOOL bFullyVisible = TRUE) { return (CM_GetVisibleLineCount (m_hWnd, nView, bFullyVisible)) ; }
  CME_CODE SetFontOwnership (BOOL bEnable) { return (CM_SetFontOwnership (m_hWnd, bEnable)) ; }
  bool GetFontOwnership (void) { return (CM_GetFontOwnership (m_hWnd) != 0) ; }
  int GetCurrentView (void) { return (CM_GetCurrentView (m_hWnd)) ; }
  int GetViewCount (void) { return (CM_GetViewCount (m_hWnd)) ; }
  CME_CODE GetSelFromPoint (int xClient, int yClient, CM_POSITION *pPos) { return (CM_GetSelFromPoint (m_hWnd, xClient, yClient, pPos)) ; }
  CME_CODE SelectLine (int nLine, BOOL bMakeVisible = TRUE) { return (CM_SelectLine (m_hWnd, nLine - 1, bMakeVisible)) ; }
  int GetErrorLine (void) { return (CM_GetHighlightedLine (m_hWnd) + 1) ; }
  void SetErrorLine (int LineNo) { CM_SetHighlightedLine (m_hWnd, --LineNo) ; }
  void ClearErrorLine (void) { CM_ClearHighlightedLines (m_hWnd) ; }
  int HitTest (int xClient, int yClient) { return (CM_HitTest (m_hWnd, xClient, yClient)) ; }
  CME_CODE SetDlgParent (HWND parent) { return (CM_SetDlgParent (m_hWnd, parent)) ; }
  HFONT GetFont (void) { return ((HFONT) SendMessage (m_hWnd, WM_GETFONT, 0, 0)) ; }
  void SetFont (HFONT font) { SendMessage (m_hWnd, WM_SETFONT, (WPARAM) font, MAKELPARAM (true, 0)) ; }
  void ShowProperties (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_PROPERTIES, 0) ; }
  void Undo (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_UNDO, 0) ; }
  void Redo (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_REDO, 0) ; }
  void Find (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_FIND, 0) ; }
  void FindNext (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_FINDNEXT, 0) ; }
  void Replace (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_FINDREPLACE, 0) ; }
  void ReplaceNext (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_REPLACE, 0) ; }
  void Record (void) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_RECORDMACRO, 0) ; }
  void GoToLine (int Line) { SendMessage (m_hWnd, CMM_EXECUTECMD, CMD_GOTOLINE, Line - 1)  ; }
  void GetLineNumbering (CM_LINENUMBERING *LineNumbering) { CM_GetLineNumbering (m_hWnd, LineNumbering) ; }
  void SetLineNumbering (const CM_LINENUMBERING *LineNumbering) { CM_SetLineNumbering (m_hWnd, LineNumbering) ; }
  void SetCaretPos (int Col, int Line) { CM_SetCaretPos (m_hWnd, Line - 1, Col - 1) ; }
  DWORD ExecuteCommand (WORD Command, int Param) { return ((DWORD) SendMessage (m_hWnd, CMM_EXECUTECMD, Command, Param)) ; }
  bool IsModified (void) { return (CM_IsModified (m_hWnd) != 0) ; }
  void SetModified (bool State) { CM_SetModified (m_hWnd, State) ; }
  bool IsOvertypeMode (void) { return (CM_IsOvertypeEnabled (m_hWnd) != 0) ; }
  void EnableOvertypeMode (bool State) { CM_EnableOvertype (m_hWnd, State) ; }
  CStdString GetWord (CM_POSITION *pPos = NULL) ;
  CStdString GetText (const CM_RANGE *pRange) ;
  CStdString GetLine (int nLine) ;
  void GetPosition (CM_POSITION *Position) ;
  void SetPosition (const CM_POSITION *Position) ;
  int GetTabSize (void) { return (CM_GetTabSize (m_hWnd)) ; }
  void SetTabSize (int TabSize) { CM_SetTabSize (m_hWnd, TabSize) ; }
  int GetUndoLimit (void) { return (CM_GetUndoLimit (m_hWnd)) ; }
  void SetUndoLimit (int UndoLimit) { CM_SetUndoLimit (m_hWnd, UndoLimit) ; }
  bool CanUndo (void) { return (CM_CanUndo (m_hWnd) != 0) ; }
  bool CanRedo (void) { return (CM_CanRedo (m_hWnd) != 0) ; }
  bool CanCut (void) { return (CM_CanCut (m_hWnd) != 0) ; }
  bool CanCopy (void) { return (CM_CanCopy (m_hWnd) != 0) ; }
  bool CanPaste (void) { return (CM_CanPaste (m_hWnd) != 0) ; }
  CME_CODE Print (DWORD flags) { return (CM_Print (m_hWnd, NULL, flags)) ; }
  CME_CODE Print (CM_PRINTEX *cmpex) { return (CM_PrintEx (m_hWnd, cmpex)) ; }

  bool IsTabExpandEnabled (void) { return (CM_IsTabExpandEnabled (m_hWnd) != 0) ; }
  void EnableTabExpand (bool State) { CM_EnableTabExpand (m_hWnd, State) ; }
  void EnableColorSyntax (bool State) { CM_EnableColorSyntax (m_hWnd, State) ; }
  bool IsColorSyntaxEnabled (void) { return (CM_IsColorSyntaxEnabled (m_hWnd) != 0) ; }
  void EnableWhitespaceDisplay  (bool State) { CM_EnableWhitespaceDisplay  (m_hWnd, State) ; }
  bool IsWhitespaceDisplayEnabled (void) { return (CM_IsWhitespaceDisplayEnabled (m_hWnd) != 0) ; }
  void EnableSmoothScrolling (bool State) { CM_EnableSmoothScrolling (m_hWnd, State) ; }
  bool IsSmoothScrollingEnabled (void) { return (CM_IsSmoothScrollingEnabled (m_hWnd) != 0) ; }
  void EnableLineToolTips (bool State) { CM_EnableLineToolTips (m_hWnd, State) ; }
  bool IsLineToolTipsEnabled (void) { return (CM_IsLineToolTipsEnabled (m_hWnd) != 0) ; }
  void EnableLeftMargin (bool State) { CM_EnableLeftMargin (m_hWnd, State) ; }
  bool IsLeftMarginEnabled (void) { return (CM_IsLeftMarginEnabled (m_hWnd) != 0) ; }
  void EnableOvertype (bool State) { CM_EnableOvertype (m_hWnd, State) ; }
  bool IsOvertypeEnabled (void) { return (CM_IsOvertypeEnabled (m_hWnd) != 0) ; }
  void EnableCaseSensitive (bool State) { CM_EnableCaseSensitive (m_hWnd, State) ; }
  bool IsCaseSensitiveEnabled (void) { return (CM_IsCaseSensitiveEnabled (m_hWnd) != 0) ; }
  void EnablePreserveCase (bool State) { CM_EnablePreserveCase (m_hWnd, State) ; }
  bool IsPreserveCaseEnabled (void) { return (CM_IsPreserveCaseEnabled (m_hWnd) != 0) ; }
  void EnableWholeWord (bool State) { CM_EnableWholeWord (m_hWnd, State) ; }
  bool IsWholeWordEnabled (void) { return (CM_IsWholeWordEnabled (m_hWnd) != 0) ; }
  void EnableRegExp (bool State) { CM_EnableRegExp (m_hWnd, State) ; }
  bool IsRegExpEnabled (void) { return (CM_IsRegExpEnabled (m_hWnd) != 0) ; }
  void EnableCRLF (bool State) { CM_EnableCRLF (m_hWnd, State) ; }
  bool IsCRLFEnabled (void) { return (CM_IsCRLFEnabled (m_hWnd) != 0) ; }
  void EnableDragDrop (bool State) { CM_EnableDragDrop (m_hWnd, State) ; }
  bool IsDragDropEnabled (void) { return (CM_IsDragDropEnabled (m_hWnd) != 0) ; }
  void EnableColumnSel (bool State) { CM_EnableColumnSel (m_hWnd, State) ; }
  bool IsColumnSelEnabled (void) { return (CM_IsColumnSelEnabled (m_hWnd) != 0) ; }
  void EnableGlobalProps (bool State) { CM_EnableGlobalProps (m_hWnd, State) ; }
  bool IsGlobalPropsEnabled (void) { return (CM_IsGlobalPropsEnabled (m_hWnd) != 0) ; }
  void EnableSelBounds (bool State) { CM_EnableSelBounds (m_hWnd, State) ; }
  bool IsSelBoundsEnabled (void) { return (CM_IsSelBoundsEnabled (m_hWnd) != 0) ; }
  void EnableHideSel (bool State) { CM_EnableHideSel (m_hWnd, State) ; }
  bool IsHideSelEnabled (void) { return (CM_IsHideSelEnabled (m_hWnd) != 0) ; }
  void EnableOvertypeCaret (bool State) { CM_EnableOvertypeCaret (m_hWnd, State) ; }
  bool IsOvertypeCaretEnabled (void) { return (CM_IsOvertypeCaretEnabled (m_hWnd) != 0) ; }
  void SetReadOnly (bool State) { CM_SetReadOnly (m_hWnd, State) ; }
  bool IsReadOnly (void) { return (CM_IsReadOnly (m_hWnd) != 0) ; }
  bool IsHSplitterEnabled (void) { return (CM_IsSplitterEnabled (m_hWnd, 1) != 0) ; }
  void EnableHSplitter (bool State) { CM_EnableSplitter (m_hWnd, 1, State) ; }
  bool IsVSplitterEnabled (void) { return (CM_IsSplitterEnabled (m_hWnd, 0) != 0) ; }
  void EnableVSplitter (bool State) { CM_EnableSplitter (m_hWnd, 0, State) ; }

  static int LookupHotKey (CM_HOTKEY *hotkey) { return (CMLookupHotKey (hotkey)) ; }
  static CME_CODE RegisterCommand (WORD wCmd, LPCTSTR pszName, LPCTSTR pszDesc) { return (CMRegisterCommand (wCmd, pszName, pszDesc)) ; }
  static CME_CODE UnregisterCommand (WORD wCmd) { return (CMUnregisterCommand (wCmd)) ; }
  static CME_CODE RegisterLanguage (char *LangName, CM_LANGUAGE *LangDef) { return (CMRegisterLanguage (LangName, LangDef)) ; }
  static int GetHotKeysForCmd (WORD Command, CM_HOTKEY *HotKeys) { return (CMGetHotKeysForCmd (Command, HotKeys)) ; }
  static CME_CODE RegisterHotKey (CM_HOTKEY *HotKey, WORD Command) { return (CMRegisterHotKey (HotKey, Command)) ; }
  static CME_CODE SetHotKeys (char *HotKeys) ;
  static void SetDefaultHotKeys (void) ;
  static CME_CODE SetMacro (int Index, char *Macro) ;
  static void SetFindReplaceMRUList (LPCTSTR List, bool IsFind) ;
  static int GetHotKeys (char *HotKeys) ;
  static int GetMacro (int Index, char *Macro) ;
  static CStdString GetFindReplaceMRUList (bool IsFind) ;
  static CStdString GetHotKeyString (CM_HOTKEY &cmHotKey) ;
  static void GetConfigFromCommonSettings (struct _EditConfigStruct *ec) ;
  static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;
} ;

#endif
