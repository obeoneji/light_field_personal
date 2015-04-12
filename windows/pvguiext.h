/****************************************************************************
 *                pvguiext.h
 *
 * This file contains POV-Ray for Windows GUI Extension specific defines.
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
 * $File: //depot/povray/3.5/windows/pvguiext.h $
 * $Revision: #16 $
 * $Change: 2936 $
 * $DateTime: 2004/07/03 16:21:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef PVGUIEXT_H_INCLUDED
#define PVGUIEXT_H_INCLUDED

#define MAX_GUI_EXT               32
#define GUI_INTERFACE_VERSION     101

#define CONTACT "Do not contact the POV-Team about this error. Contact the author of this extension - \n\n\t" AUTHOR " at " EMAIL

#ifdef POVWIN_FILE
USING_POV_NAMESPACE
#else
// WARNING: also declared in pvengine.h (for a reason)
typedef enum
{
  mUnknown = 0,
  mAll = 1,
  All = 1,
  mIDE,
  mBanner,
  mWarning,
  mRender,
  mStatus,
  mDebug,
  mFatal,
  mStatistics,
  mDivider,
  mHorzLine,
} msgtype ;
#endif

#define MAX_QUEUE       512
#define OLD_MAX_QUEUE   128

typedef struct
{
  int                   RecSize ;
  char                  command_line [_MAX_PATH * 3] ;
  char                  source_file_name [_MAX_PATH] ;
  char                  lastRenderName [_MAX_PATH] ;
  char                  lastRenderPath [_MAX_PATH] ;
  char                  lastQueuePath [_MAX_PATH] ;
  char                  lastSecondaryIniFilePath [_MAX_PATH] ;
  char                  DefaultRenderIniFileName [_MAX_PATH] ;
  char                  SecondaryRenderIniFileName [_MAX_PATH] ;
  char                  SecondaryRenderIniFileSection [64] ;
  char                  ourPath [_MAX_PATH] ;
  char                  engineHelpPath [_MAX_PATH] ;
  char                  rendererHelpPath [_MAX_PATH] ;
  char                  HomePath [_MAX_PATH] ;
  char                  EngineIniFileName [_MAX_PATH] ;
  char                  ToolIniFileName [_MAX_PATH] ;
  unsigned              loadRerun ;
  unsigned              continueRerun ;
  unsigned              povray_return_code ;
  BOOL                  rendering ;
  BOOL                  IsWin32 ;
  BOOL                  IsW95UserInterface ;
  BOOL                  running_demo ;
  BOOL                  debugging ;
  BOOL                  isMaxiMinimized ;
  BOOL                  newVersion ;
  BOOL                  use_threads ;
  BOOL                  use_toolbar ;
  BOOL                  use_tooltips ;
  BOOL                  use_editors ;
  BOOL                  drop_to_editor ;
  BOOL                  rendersleep ;
  BOOL                  ExtensionsEnabled ;
  char                  queued_files [OLD_MAX_QUEUE] [_MAX_PATH] ;
  unsigned              queued_file_count ;
  unsigned              auto_render ;
  DWORD                 Reserved [32] ;
} ExternalVarStruct ;

typedef enum
{
  EventFirst,
  EventStartRendering,
  EventStopRendering,
  EventDisplayInit,
  EventDisplayFinished,
  EventDisplayClose,
  EventWinStartup,
  EventWinFinish,
  EventWinCooperate,
  EventLoadToolMenu,
  EventTimer,
  EventSize,
  EventMove,
  EventClose,
  EventDestroy,
  EventLast
} ExternalEvents ;

typedef enum
{
  RequestFirst,
  RequestGetVars,
  RequestSetVars,
  RequestStartRendering,
  RequestStopRendering,
  RequestExit,
  RequestSetCooperateLevel,
  RequestLast
} ExternalRequests ;

typedef enum
{
  ExRequestDisabled,
  ExRequestOK,
  ExRequestFailed,
  ExRequestFailedRendering,
  ExRequestFailedNotRendering,
  ExRequestBadRecSize,
  ExRequestUnknown
} ExternalRequestResult ;

typedef enum
{
  dfRealDrop,
  dfRenderEditor,
  dfRenderMessage,
  dfRenderCommandLine,
  dfRenderSourceFile,
  dfRenderFileQueue
} ExternalDropType ;

typedef struct
{
  // data POV passes to the DLL
  LPSTR                 PovVersion ;
  LPSTR                 GuiVersion ;
  DWORD                 GuiInterfaceVersion ;
  WPARAM                FirstMenuItem ;
  HINSTANCE             hInst ;
  HWND                  MainWindow ;
  ExternalRequestResult (WINAPI *ExternalRequest) (ExternalRequests Request, void *RequestBlock) ;

  // data the DLL passes to POV
  LPSTR                 Name ;
  LPSTR                 Author ;
  LPSTR                 AuthorEmail ;
  HMENU                 hMenu ;
  DWORD                 DLLInterfaceVersion ;
  char                  Agreement [1024] ;
} GuiExtInitStruct ;

typedef struct
{
  DWORD                 InstanceID ;
  HMENU                 hMenu ;
  WPARAM                FirstMenuItem ;
} IDataStruct ;

typedef struct
{
  DWORD                 Signature ;
  DWORD                 InstanceID ;
  BOOL                  (WINAPI *Init) (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct) ;
  void                  (WINAPI *Destroy) (IDataStruct *InstanceData) ;
  DWORD                 (WINAPI *MenuSelect) (IDataStruct *InstanceData, WPARAM Code) ;
  LPSTR                 (WINAPI *MenuTip) (IDataStruct *InstanceData, WPARAM Code) ;
  DWORD                 (WINAPI *Event) (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal) ;
  void                  (WINAPI *DisplayPlot) (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *DisplayPlotRect) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *WinPrePixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *WinPostPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  BOOL                  (WINAPI *WinSystem) (IDataStruct *InstanceData, LPSTR command, int *returnval) ;
  void                  (WINAPI *CleanupAll) (IDataStruct *InstanceData) ;
  void                  (WINAPI *BufferMessage) (IDataStruct *InstanceData, msgtype message_type, LPSTR message) ;
  LPSTR                 (WINAPI *ParseToolCommand) (IDataStruct *InstanceData, char command [512]) ;
  BOOL                  (WINAPI *DragFunction) (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType) ;
  DWORD                 Reserved [128] ;
} GuiPointerBlock_Version_100 ;

typedef struct
{
  DWORD                 Signature ;
  DWORD                 InstanceID ;
  BOOL                  (WINAPI *Init) (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct) ;
  void                  (WINAPI *Destroy) (IDataStruct *InstanceData) ;
  DWORD                 (WINAPI *MenuSelect) (IDataStruct *InstanceData, WPARAM Code) ;
  LPSTR                 (WINAPI *MenuTip) (IDataStruct *InstanceData, WPARAM Code) ;
  DWORD                 (WINAPI *Event) (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal) ;
  void                  (WINAPI *DisplayPlot) (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *DisplayPlotRect) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *WinPrePixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *WinPostPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  BOOL                  (WINAPI *WinSystem) (IDataStruct *InstanceData, LPSTR command, int *returnval) ;
  void                  (WINAPI *CleanupAll) (IDataStruct *InstanceData) ;
  void                  (WINAPI *BufferMessage) (IDataStruct *InstanceData, msgtype message_type, LPSTR message) ;
  LPSTR                 (WINAPI *ParseToolCommand) (IDataStruct *InstanceData, char command [512]) ;
  BOOL                  (WINAPI *DragFunction) (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType) ;
  void                  (WINAPI *AssignPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  DWORD                 Reserved [127] ;
} GuiPointerBlock ;

DWORD ExternalEvent (ExternalEvents Event, DWORD EventVal) ;
void ExternalDisplayPlot (int x, int y, int Red, int Green, int Blue, int Alpha) ;
void ExternalDisplayPlotRect (int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
void ExternalWinPrePixel (int x, int y, COLOUR colour) ;
void ExternalWinPostPixel (int x, int y, COLOUR colour) ;
void ExternalAssignPixel (int x, int y, COLOUR colour) ;
BOOL ExternalWinSystem (LPSTR command, int *returnval) ;
void ExternalCleanupAll (void) ;
void ExternalBufferMessage (msgtype message_type, LPSTR message) ;
void ExternalParseToolCommand (char command [512]) ;
BOOL ExternalDragFunction (LPSTR szFile, ExternalDropType DropType) ;
void LoadGUIExtensions (void) ;
DWORD ExternalMenuSelect (WPARAM Code) ;
char *ExternalMenuTip (WPARAM wParam) ;
BOOL CheckGUIExtLoaded (const char *Name) ;

#endif // PVGUIEXT_H_INCLUDED

