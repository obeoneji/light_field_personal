/*******************************************************************************
 * pvedit.h
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
 * $File: //depot/povray/3.5/windows/pvedit.h $
 * $Revision: #21 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef PVEDIT_H_INCLUDED
#define PVEDIT_H_INCLUDED

#define EDITDLLVERSION            0x0301

#define MAX_EDITORS               32

#define EDIT_CAN_OPEN             0x01
#define EDIT_CAN_CLOSE            0x02
#define EDIT_CAN_WRITE            0x04
#define EDIT_CAN_UNDO             0x08
#define EDIT_CAN_REDO             0x10
#define EDIT_MSG_SELECTED         0x20
#define EDIT_CURRENT_MODIFIED     0x40
#define EDIT_ANY_MODIFIED         0x80
#define EDIT_DRAG_ACTIVE          0x100

#define RENDER_MESSAGE            WM_USER + 1000
#define CREATE_RENDERWIN_MESSAGE  WM_USER + 1005
#define CLOSE_EDITOR_MESSAGE      WM_USER + 1010
#define CREATE_EDITOR_MESSAGE     WM_USER + 1015
#define EDITOR_RENDER_MESSAGE     WM_USER + 1020
#define SHOW_MESSAGES_MESSAGE     WM_USER + 1025
#define KEYWORD_LOOKUP_MESSAGE    WM_USER + 1030
#define RENDERWIN_CLOSE_MESSAGE   WM_USER + 1035
#define HIDE_NEWUSER_HELP_MESSAGE WM_USER + 1040
#define COPY_COMMANDLINE_MESSAGE  WM_USER + 1045
#define POVMS_QUEUE_MESSAGE       WM_USER + 1050
#define TASKBAR_NOTIFY_MESSAGE    WM_USER + 1100 // reserved 1100-1199
#define GUIEXT_CREATE_EDITOR      WM_USER + 1200 // reserved 1200-1299

#define REGKEY                    "POV-Ray"
#define REGVERKEY                 "v3.6"

enum
{
  GetFileMenu,
  GetEditMenu,
  GetSearchMenu,
  GetTextMenu,
  GetInsertMenu,
  GetOptionsMenu,
  GetAppearanceMenu,
  GetWindowMenu
} ;

enum
{
  NotifyTabChange,
  NotifyEditStateChange,
  NotifyModifiedChange,
  NotifyExitRequest,
  NotifyFocusSaveModified
} ;

typedef enum
{
  StatusMessage,
  StatusLine,
  StatusCol,
  StatusIns,
  StatusModified,
  StatusPPS,
  StatusRendertime,
  StatusLast
} StatusBarSection ;

bool LoadEditorDLL (char *path, bool missingOK) ;
HWND InitialiseEditor (HWND ParentWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath) ;
void SetEditorPosition (int x, int y, int w, int h) ;
void EditSetMessageWindow (HWND MessageWindow) ;
void EditRestoreState (int RestoreFiles) ;
void EditSaveState (void) ;
bool EditSelectFile (char *FileName) ;
bool EditBrowseFile (bool CreateNewWindow) ;
bool EditOpenFile (char *FileName) ;
bool EditCloseFile (char *FileName) ;
bool EditSaveFile (char *FileName) ;
bool EditExternalOpenFile (char *ParamString) ;
DWORD EditGetTab (void) ;
DWORD EditGetFlags (void) ;
char *EditGetFilename (bool IncludeModifiedIndicator) ;
void EditNextTab (bool Forward) ;
bool EditCanClose (bool AllFiles) ;
bool EditSaveModified (char *FileName) ;
bool EditEnabled (void) ;
void EditSetState (bool on) ;
bool EditShowMessages (bool on) ;
void EditDispatchMenuId (DWORD id) ;
void EditSetVisible (bool visible) ;
HMENU EditGetMenu (int which) ;
void EditUnload (void) ;
void EditSetNotifyBase (HWND WindowHandle, int MessageBase) ;
void EditUpdateMenus (HMENU MenuHandle) ;
void EditContextHelp (void) ;
void EditSetFocus (void) ;
bool EditShowParseError (const char *FileName, const char *Message, int Line, int Col) ;
bool EditPassOnMessage (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal) ;
void EditSetKeywords (LPCSTR Keywords) ;
const char **EditGetWindowList(void);

#endif // PVEDIT_H_INCLUDED

