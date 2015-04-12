/*******************************************************************************
 * editorinterface.h
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/editorinterface.h $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __EDITORINTERFACE_H__
#define __EDITORINTERFACE_H__

extern "C" __declspec (dllexport) HWND CreateTabWindow (HWND mainWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath) ;
extern "C" __declspec (dllexport) DWORD GetDLLVersion (void) ;
extern "C" __declspec (dllexport) void SetWindowPosition (int x, int y, int w, int h) ;
extern "C" __declspec (dllexport) void SetMessageWindow (HWND MsgWindow) ;
extern "C" __declspec (dllexport) void RestoreState (int RestoreFiles) ;
extern "C" __declspec (dllexport) void SaveState (void) ;
extern "C" __declspec (dllexport) bool SelectFile (const char *FileName) ;
extern "C" __declspec (dllexport) bool ShowParseError (char *FileName, char *Message, int Line, int Col) ;
extern "C" __declspec (dllexport) bool BrowseFile (bool CreateNewWindow) ;
extern "C" __declspec (dllexport) bool LoadFile (char *FileName) ;
extern "C" __declspec (dllexport) bool ExternalLoadFile (char *ParamString) ;
extern "C" __declspec (dllexport) bool CloseFile (char *FileName) ;
extern "C" __declspec (dllexport) bool SaveFile (char *FileName) ;
extern "C" __declspec (dllexport) DWORD GetTab (void) ;
extern "C" __declspec (dllexport) DWORD GetFlags (void) ;
extern "C" __declspec (dllexport) char *GetFilename (void) ;
extern "C" __declspec (dllexport) void NextTab (bool Forward) ;
extern "C" __declspec (dllexport) bool CanClose (bool AllFiles) ;
extern "C" __declspec (dllexport) bool SaveModified (char *FileName) ;
extern "C" __declspec (dllexport) bool ShowMessages (bool on) ;
extern "C" __declspec (dllexport) void DispatchMenuId (DWORD id) ;
extern "C" __declspec (dllexport) HMENU GetMenuHandle (int which) ;
extern "C" __declspec (dllexport) void SetNotifyBase (HWND WindowHandle, int MessageBase) ;
extern "C" __declspec (dllexport) void UpdateMenus (HMENU MenuHandle) ;
extern "C" __declspec (dllexport) void GetContextHelp (void) ;
extern "C" __declspec (dllexport) void SetTabFocus (void) ;
extern "C" __declspec (dllexport) bool PassOnMessage (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal) ;
extern "C" __declspec (dllexport) void SetKeywords (LPCSTR Keywords) ;
extern "C" __declspec (dllexport) const char **GetWindowList (void);

void PutStatusMessage (LPCSTR Message) ;
void debug (char *format, ...) ;
void TabIndexChanged (void) ;
void ShowMessage (LPCSTR str, int type = 0) ;
void ShowErrorMessage (CStdString Title, const char *Msg, int ErrorCode = 0) ;
void GetFileTimeFromDisk (LPCSTR Filename, FILETIME& time) ;
bool FileExists (LPCSTR FileName) ;
void MakeFileNames (EditTagStruct *t, LPCSTR str) ;
CStdString GetFilePath (LPCSTR str) ;
CStdString GetFileExt (LPCSTR str) ;
CStdString GetFullPath (LPCSTR str) ;
CStdString GetBaseName (LPCSTR str) ;
CStdString GetFileNameNoExt (LPCSTR str) ;
CStdString FixPath (CStdString Name) ;
CStdString UnquotePath (CStdString Name) ;
int GetFileLength (LPCSTR FileName) ;
void AddToRecent (LPCSTR FileName) ;
void UpdateRecent (void) ;
CCodeMax *CreateNewEditor (const char *FileName, bool ReadOnly, bool Show) ;
bool CloseAll (void) ;
bool CloseFile (char *FileName) ;
bool SaveAllFiles (bool IncludeUntitled) ;
void ShowMessagePane (void) ;
bool HaveWin98OrLater (void) ;
bool HaveWin2kOrLater (void) ;
bool HaveWinXPOrLater (void) ;
CCodeMax *FindEditor (LPCSTR FileName) ;
void InsertTab (LPCSTR title) ;
void DeleteTab (int index) ;
void SetWindowPosition (void) ;

#endif
