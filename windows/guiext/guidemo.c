/****************************************************************************
 *                guidemo.c
 *
 * This file contains POV-Ray for Windows GUI Extension demonstration code.
 *
 * Primary author: Christopher J. Cason.
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
 * EXCLUSION FROM STANDARD LICENCE PROVISIONS
 *
 * Software authors may use this source code in derived programs PROVIDED
 * thay they comply with the terms of POVEXT.TXT.
 *---------------------------------------------------------------------------
 *
 * $File: //depot/povray/3.5/windows/guiext/guidemo.c $
 * $Revision: #4 $
 * $Change: 2933 $
 * $DateTime: 2004/07/03 13:06:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

// Template file that both demonstrates a POV-Ray for Windows GUI Extension
// DLL, and provides you with source to implement your own.

// REQUIREMENTS. This code must be compiled into a 32-bit Windows DLL with
// the extension .pge (for POV-Ray Gui Extension).

// you must modify the code wherever there is a #error directive to make it
// comply with our conditions. you must comply with POVEXT.TXT. here's a
// quick summary -

//  o extensions may be based on the 'extension toolkit' source code we provide.
//    that is, we explicitly permit you to re-use our demonstration source code
//    (and in fact encourage it, since it helps ensure that you comply with our
//    standards.)
//  o extensions may be written in any programming language compatible with the
//    requirements for the interface.
//  o extensions are not permitted to display splash screens or any other
//    notification that they are loaded other than functionally essential ones,
//    unless the user explicitly activates such by selecting them from the GUI
//    Extensions menu.
//  o extensions are not permitted to take any action that could jeopardize the
//    correct operation of POV-Ray.
//  o they may not show or do anything that may suggest that they are endorsed
//    by the POV team, or that they are part of POV-Ray.
//  o except by written agreement, extensions and applications that use or
//    communicate with them must be freeware and provide full source.
//  o extension authors are required to build their real name and a valid email
//    address into the DLL.

// the above text is not meant to be definitive. if any of the above differs
// from the enclosed POVEXT.TXT, or from the current one, then the enclosed or
// current one takes precedence. the current one may be obtained by downloading
// the latest version of the POV-Ray for Windows source code from our official
// distribution sites.

// An extension is installed by modifying PVENGINE.INI in the following manner -
//
// [GUIExtensions]
// ExtDll16=c:\povsrc\3.5\windows\guiext\debug\guidemo.pge
//
// in the above example, the DLL is installed as number 16. there can be up to
// 32. numbers 00-15 are RESERVED for the POV-Team. the number determines the
// order in which DLL's are called with information, such as drag'n'drop
// filenames. REMINDER : do NOT use numbers 00-15.

// this typedef avoids our needing to include povray's frame.h (which is C++ only).
typedef float COLOUR [5] ;

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "pvguiext.h"
#include "guidemo.rh"

#error                  You need to customize the AUTHOR, EMAIL and EXTNAME macros first.
#define AUTHOR          "Place Author Name Here"                 // e.g. "Fu Bar"
#define EMAIL           "Place Author Email Address Here"        // e.g. fubar@somewhere.com. don't use '<' or '('.
#define EXTNAME         "Place the name of your extension Here"  // this is what's displayed to the users.

// this is a demonstration of a context structure. put whatever you like in here.
typedef struct
{
  BOOL                            Enabled ;
  HINSTANCE                       hInstance ;
  DWORD                           PixelsTraced ;
  HWND                            MainWindow ;
  ExternalRequestResult           (WINAPI *Request) (ExternalRequests Request, void *RequestBlock) ;
} InstanceStruct ;

HINSTANCE               hInstance ;

void GetRelativeClientRect (HWND hParent, HWND hChild, RECT *rect)
{
  POINT       *points = (POINT *) rect ;

  GetWindowRect (hChild, rect) ;
  ScreenToClient (hParent, points++) ;
  ScreenToClient (hParent, points) ;
}

void CenterWindowRelative (HWND hRelativeTo, HWND hTarget)
{
  int         difference ;
  int         width ;
  int         height ;
  int         x ;
  int         y ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  if (hRelativeTo == NULL)
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;
  GetWindowRect (hTarget, &targetRect) ;
  width = targetRect.right - targetRect.left ;
  height = targetRect.bottom - targetRect.top ;
  difference = relativeToRect.right - relativeToRect.left - width ;
  x = relativeToRect.left + difference / 2 ;
  difference = relativeToRect.bottom - relativeToRect.top - height ;
  y = relativeToRect.top + difference / 2 ;
  MoveWindow (hTarget, x, y, width, height, TRUE) ;
}

void FitWindowInWindow (HWND hRelativeTo, HWND hTarget)
{
  int         rwidth ;
  int         rheight ;
  int         twidth ;
  int         theight ;
  int         x ;
  int         y ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  if (hRelativeTo == NULL)
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;
  GetWindowRect (hTarget, &targetRect) ;

  twidth = targetRect.right - targetRect.left ;
  theight = targetRect.bottom - targetRect.top ;
  rwidth = relativeToRect.right - relativeToRect.left ;
  rheight = relativeToRect.bottom - relativeToRect.top ;
  x = targetRect.left ;
  y = targetRect.top ;

  if (twidth > rwidth)
    twidth = rwidth ;
  if (theight > rheight)
    theight = rheight ;
  if (x < relativeToRect.left)
    x = relativeToRect.left ;
  if (y < relativeToRect.top)
    y = relativeToRect.top ;
  if (x + twidth > relativeToRect.right)
    x = relativeToRect.right - twidth ;
  if (y + theight > relativeToRect.bottom)
    y = relativeToRect.bottom - theight ;

  MoveWindow (hTarget, x, y, twidth, theight, TRUE) ;
}

// an about dialog is REQUIRED. modify this one to suit your needs !
// remove the pixels traced and home path stuff ; that's just there for demonstration.
BOOL FAR PASCAL AboutDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char                  str [256] ;
  InstanceStruct        *Instance = (InstanceStruct *) lParam ;
  ExternalVarStruct     ExternalVars ;

  switch (message)
  {
    case WM_INITDIALOG :
         sprintf (str, "%d pixels traced", Instance->PixelsTraced) ;
         SetDlgItemText (hDlg, ID_PIXELS, str) ;
         ExternalVars.RecSize = sizeof (ExternalVarStruct) ;
         (*Instance->Request) (RequestGetVars, &ExternalVars) ;
         sprintf (str, "POV-Ray for Windows home path is '%s'", ExternalVars.HomePath) ;
         SetDlgItemText (hDlg, ID_HOMEPATH, str) ;
         CenterWindowRelative (Instance->MainWindow, hDlg) ;
         FitWindowInWindow (NULL, hDlg) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
           case IDCANCEL :
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;
         }
         break ;
  }
  return (FALSE) ;
}

/*
** Possible events -
**
**  EventStartRendering             Guess ?
**  EventStopRendering              uhhh ...
**  EventDisplayInit                see POV-Ray source code (Dos, Win, Unix, whatever) for info on this.
**  EventDisplayFinished            ditto
**  EventDisplayClose               ditto
**  EventWinStartup                 ditto
**  EventWinFinish                  ditto
**  EventWinCooperate               ditto (see also RequestSetCooperateLevel)
**  EventLoadToolMenu               called BEFORE tool menu is loaded/reloaded.
**  EventTimer                      called once per second, or so. use this to avoid having your own timer/message loop.
**  EventClose                      called when the main window is closed.
**  EventDestroy                    called when the main window is destroyed.
**
*/

// return 1 (exactly 1, not just non-zero) if you process an event.
DWORD WINAPI Event (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal)
{
  return (0) ;
}

// see POV-Ray source.
void WINAPI DisplayPlot (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha)
{
}

// see POV-Ray source.
void WINAPI DisplayPlotRect (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha)
{
}

// see POV-Ray source.
void WINAPI WinPrePixel (IDataStruct *InstanceData, int x, int y, COLOUR colour)
{
}

// see POV-Ray source.
void WINAPI WinPostPixel (IDataStruct *InstanceData, int x, int y, COLOUR colour)
{
  // take this out if you don't need it. it's just a demonstration.
  ((InstanceStruct *) InstanceData->InstanceID)->PixelsTraced++ ;
}

// see POV-Ray source.
void WINAPI WinAssignPixel (IDataStruct *InstanceData, int x, int y, COLOUR colour)
{
}

/****************************************************************************************/
/*                                                                                      */
/* This is called before POV-Ray for Windows executes a system call (i.e. runs an       */
/* external program). If you return TRUE, POV will assume that you've handled the call. */
/*                                                                                      */
/* CONVENTION : External program names that start with an '!' are really GUI extensions.*/
/* For example, the line -                                                              */
/*                                                                                      */
/*  Post_Scene_Command=!Peter's FLC Maker %f                                            */
/*                                                                                      */
/* is intended to refer not to a program called 'Peter's FLC maker' but any GUI Ext     */
/* that chooses to respond to that name. This should be either the name of the DLL,     */
/* which should always have a .pge extension, the name of your extension, or an         */
/* extension of it. (e.g. !Peter's FLC Maker (FLH mode) %f.)                            */
/*                                                                                      */
/* WARNING. If you hijack POV-Ray for Windows normal shellout commands (i.e., you       */
/* attempt to execute ALL commands), make SURE it works properly.                       */
/*                                                                                      */
/****************************************************************************************/

BOOL WINAPI WinSystem (IDataStruct *InstanceData, LPSTR command, int *returnval)
{
  return (FALSE) ;
}

// do any per-instance de-initialisation here.
void WINAPI CleanupAll (IDataStruct *InstanceData)
{
}

// messages intended for the message window. you can do things here like send them
// via the network if you've implemented network rendering, or write them to a log file.
void WINAPI BufferMessage (IDataStruct *InstanceData, msgtype message_type, LPSTR message)
{
}

// you can add your own escapes to tool commands by processing them here.
LPSTR WINAPI ParseToolCommand (IDataStruct *InstanceData, char command [512])
{
  return (command) ;
}

// if an unknown file type is dropped onto POV-Ray for Windows, it will be passed
// along to here. if you want to handle it, do so here and return TRUE.
// at a later date, we will also call this function for any render for a file
// extension type it does not recognise. this will allow a user to be editing,
// for example, a file called 'myfile.myextension', and as long as you handle
// 'myextension' here, you can handle the render action. typically, this would be
// to translate the file into POV-Ray source code, then to invoke the renderer on
// it. this can be used by authors of 'shape generator' programs, for example.
// (a GUI extension version of lparser would be a perfect example.)
//
// DropType may be one of -
//
//   dfRealDrop             A file was really dropped on POV-Ray for Windows.
//   dfRenderEditor         Not a drop. A render was started with the editor selected.
//                          [That is, the tabbed control was selecting an editor page.]
//                          szFile will point to the filename that they're rendering.
//   dfRenderMessage        Ditto, but the editor wasn't selected.
//   dfRenderCommandLine    Render was selected from the Render dialog (ALT-C)
//   dfRenderSourceFile     Render was selected due to a source file selection (ALT-S)
//   dfRenderFileQueue      A render was initiated by a file moving out of the file queue
//
// In the case of dfRealDrop, Make sure you check the value of drop_to_editor before
// processing a file. You normally should let it be loaded into the editor, by returning
// false. If, however, you know something about the file (like it's not a text file), then
// you can do what you need to.
//
// Again, if you're writing some sort of LParser-like program, you can intercept render
// requests here and perform any needed processing before firing up the renderer yourself.

BOOL WINAPI DragFunction (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType)
{
  return (FALSE) ;
}

// called whenever a menu selection is made on your menu. the code passed is the original
// menu command ID, before the offset was added to it. make sure you modify the below code
// to suit your application. note that an ABOUT and ENABLED menu entry is mandatory.
// we require that you make your enabled entry persistent by storing it in an INI file.
// the INI file should be stored wherever your DLL is ; preferably, this will be the
// <POV-Ray for Windows>\guiext\ directory. you MAY NOT write to the POV-Ray INI files.
//
// several of these entries are for demonstration. make sure you remove them.
DWORD WINAPI MenuSelect (IDataStruct *InstanceData, WPARAM Code)
{
  InstanceStruct        *Instance = (InstanceStruct *) InstanceData->InstanceID ;

  switch (Code)
  {
    case CM_ENABLED :
         Instance->Enabled = !Instance->Enabled ;
         CheckMenuItem (InstanceData->hMenu, CM_ENABLED + InstanceData->FirstMenuItem, Instance->Enabled ? MF_CHECKED : MF_UNCHECKED) ;
         return (0) ;

    case CM_ABOUT :
         DialogBoxParam (Instance->hInstance, MAKEINTRESOURCE (IDD_ABOUT), NULL, (DLGPROC) AboutDialogProc, (DWORD) Instance) ;
         return (0) ;

    case CM_STARTRENDERING :
         (*Instance->Request) (RequestStartRendering, NULL) ;
         return (0) ;

    case CM_STOPRENDERING :
         (*Instance->Request) (RequestStopRendering, NULL) ;
         return (0) ;

    case CM_EXITPOV :
         (*Instance->Request) (RequestExit, NULL) ;
         return (0) ;

    case CM_HELP :
         MessageBox (Instance->MainWindow, "There is no help available\n\n" CONTACT, EXTNAME, MB_OK | MB_ICONSTOP) ;
         return (0) ;
  }
  return (1) ;
}

// provides the text for ToolTips. MANDATORY.
LPSTR WINAPI MenuTip (IDataStruct *InstanceData, WPARAM Code)
{
  switch (Code)
  {
    case CM_ENABLED :
         return ("Enables/Disables the " EXTNAME) ;

    case CM_ABOUT :
         return ("About the " EXTNAME) ;

    case CM_STARTRENDERING :
         return ("Starts rendering") ;

    case CM_STOPRENDERING :
         return ("Stops rendering") ;

    case CM_EXITPOV :
         return ("Causes POV-Ray to exit") ;

    case CM_HELP :
         return ("Help on the " EXTNAME) ;
  }
  return ("") ;
}

#error You need to agree.

/**********************************************************************************************/
/*                                                                                            */
/* This GUI extension WILL NOT LOAD unless you copy the below text, EXACTLY as it is,         */
/* into the array 'Agreement' below. See the comment.                                         */
/*                                                                                            */
/*        "The author of this POV-Ray(tm) GUI Extension DLL certifies that, at the time of "  */
/*        "its production or distribution, it complied with the POV-Ray Team's then current " */
/*        "requirements for GUI Extensions to POV-Ray for Windows, and acknowledges that it " */
/*        "is a violation of copyright to fail to do so. This text is copyright (c) the "     */
/*        "POV-Team 1996. Used by permission."                                                */
/*                                                                                            */
/* You may only use the above text if you comply with our terms and conditions as stated in   */
/* the file 'POVEXT.TXT' distributed with this toolkit. While this may seem severe, we have   */
/* good reasons for doing it, based on past experience. There are always, it seems, people    */
/* who are willing to try to make a quick buck out of other people's hard work. You will note */
/* that the agreement attempts to ensure the quality of GUI Extensions, and also that we, the */
/* POV-Team, does not get our time wasted by people asking us for support on your extension.  */
/*                                                                                            */
/**********************************************************************************************/

BOOL WINAPI Init (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct)
{
  char                  str [256] ;
  HMENU                 hMenu ;
  HMENU                 hPopup ;
  InstanceStruct        *Instance = (InstanceStruct *) InstanceID ;

  if (RecSize != sizeof (GuiExtInitStruct))
  {
    MessageBox (NULL, "InitStruct has invalid size ! (Probable version error)\n\n" CONTACT, EXTNAME, MB_OK | MB_ICONSTOP) ;
    return (FALSE) ;
  }
  if (InitStruct->GuiInterfaceVersion / 100 != GUI_INTERFACE_VERSION / 100)
  {
    MessageBox (NULL, "Version error - this GUI extension cannot be used with your version of POV-Ray for Windows\n\n" CONTACT, EXTNAME, MB_OK | MB_ICONSTOP) ;
    return (FALSE) ;
  }
  Instance->MainWindow = InitStruct->MainWindow ;
  Instance->Request = InitStruct->ExternalRequest ;
  InitStruct->Name = EXTNAME ;
  InitStruct->DLLInterfaceVersion = GUI_INTERFACE_VERSION ;
  InitStruct->Author = AUTHOR ;
  InitStruct->AuthorEmail = EMAIL ;
  // see above for information as to how to initialize this array.
  strcpy (InitStruct->Agreement, "") ;
  if ((hMenu = CreateMenu ()) == NULL || (hPopup = CreateMenu ()) == NULL)
  {
    sprintf (str, "CreateMenu failed, code = %08lx\n\n" CONTACT, GetLastError ()) ;
    MessageBox (InitStruct->MainWindow, str, EXTNAME, MB_OK | MB_ICONSTOP) ;
    return (FALSE) ;
  }
  // set up our menu. the allowable command ID range is InitStruct->FirstMenuItem to InitStruct->FirstMenuItem + 59.
  AppendMenu (hMenu, MF_STRING, CM_ENABLED + InitStruct->FirstMenuItem, "&Enabled") ;
  CheckMenuItem (hMenu, CM_ENABLED + InitStruct->FirstMenuItem, Instance->Enabled ? MF_CHECKED : MF_UNCHECKED) ;
  AppendMenu (hMenu, MF_STRING, CM_STARTRENDERING + InitStruct->FirstMenuItem, "&Start Rendering") ;
  AppendMenu (hMenu, MF_STRING, CM_STOPRENDERING + InitStruct->FirstMenuItem, "Stop &Rendering") ;
  AppendMenu (hMenu, MF_STRING, CM_EXITPOV + InitStruct->FirstMenuItem, "E&xit POV-Ray") ;
  AppendMenu (hMenu, MF_SEPARATOR, -1, "-") ;
  AppendMenu (hMenu, MF_POPUP, (UINT) hPopup, "&Help") ;
  AppendMenu (hPopup, MF_STRING, CM_HELP + InitStruct->FirstMenuItem, "&Help on this GUI Extension") ;
  AppendMenu (hPopup, MF_STRING, CM_ABOUT + InitStruct->FirstMenuItem, "&About this GUI Extension") ;
  InitStruct->hMenu = hMenu ;
  return (TRUE) ;
}

void WINAPI DestroyInstance (IDataStruct *InstanceData)
{
  InstanceStruct        *Instance = (InstanceStruct *) InstanceData->InstanceID ;

  DestroyMenu (InstanceData->hMenu) ;
  free (Instance) ;
}

DWORD WINAPI PovGuiExtensionGetPointers (int RecSize, GuiPointerBlock *PointerBlock)
{
  InstanceStruct        *Instance ;

  if (RecSize != sizeof (GuiPointerBlock))
  {
    MessageBox (NULL, "PointerBlock has invalid size ! (Probable version error)\n\n" CONTACT, EXTNAME, MB_OK | MB_ICONSTOP) ;
    return (FALSE) ;
  }
  PointerBlock->Init = Init ;
  PointerBlock->Destroy = DestroyInstance ;
  PointerBlock->MenuSelect = MenuSelect ;
  PointerBlock->MenuTip = MenuTip ;
  PointerBlock->Signature = 'CJC!' ;
  PointerBlock->CleanupAll = CleanupAll ;

#error you need to choose which functions to implement.
  // Don't initialize ALL of these ! Only the ones that you need for your application.
//PointerBlock->Event = Event ;
//PointerBlock->DisplayPlot = DisplayPlot ;
//PointerBlock->DisplayPlotRect = DisplayPlotRect ;
//PointerBlock->WinPrePixel = WinPrePixel ;
  PointerBlock->WinPostPixel = WinPostPixel ;
//PointerBlock->WinSystem = WinSystem ;
//PointerBlock->BufferMessage = BufferMessage ;
//PointerBlock->ParseToolCommand = ParseToolCommand ;
//PointerBlock->DragFunction = DragFunction ;
  PointerBlock->AssignPixel = WinAssignPixel ;

  // define whatever you need in the instance structure. POV-Ray for Windows treats is as a black box.
  // BE WARNED that you will need to be wary of multiple thread issues. don't forget that POV-Ray
  // can be loaded into memory more than once, and will link to your DLL more than once if this happens !
  if ((Instance = calloc (sizeof (InstanceStruct), 1)) == NULL)
  {
    MessageBox (NULL, "Cannot allocate instance data\n\n" CONTACT, EXTNAME, MB_OK | MB_ICONSTOP) ;
    return (FALSE) ;
  }
  // READ THIS FROM AN INI FILE ! it should be persistent.
  Instance->Enabled = TRUE ;
  Instance->hInstance = hInstance ;
  // this is really a pointer to our InstanceStruct. don't confuse the term 'Instance' in this context
  // with Microsoft Windows's concept of an 'instance'. they're two different things.
  PointerBlock->InstanceID = (DWORD) Instance ;
  return (TRUE) ;
}

BOOL _declspec (dllexport) WINAPI DllMain (HINSTANCE hInst, DWORD Reason, LPVOID Reserved)
{
  if (Reason == DLL_PROCESS_ATTACH)
    hInstance = hInst ;
  return (TRUE) ;
}

BOOL _declspec (dllexport) WINAPI DllEntryPoint (HINSTANCE hInst, DWORD Reason, LPVOID Reserved)
{
  return (DllMain (hInst, Reason, Reserved)) ;
}

