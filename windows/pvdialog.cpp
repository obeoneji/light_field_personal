/****************************************************************************
 *                pvdialog.cpp
 *
 * This module implements dialog-box routines for the Windows build of POV.
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
 * $File: //depot/povray/3.5/windows/pvdialog.cpp $
 * $Revision: #6 $
 * $Change: 3212 $
 * $DateTime: 2004/10/25 01:50:23 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN64
#pragma pack(16)
#include <windows.h>
#pragma pack()
#else
#include <windows.h>
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
#include "pvdialog.h"

extern char                                 ourPath [_MAX_PATH] ;
extern char                                 home [_MAX_PATH] ;
extern char                                 helpPath [_MAX_PATH] ;
extern char                                 home [_MAX_PATH] ;
extern char                                 EngineIniFileName [_MAX_PATH] ;
extern char                                 *tips [] ;
extern unsigned                             screen_width ;
extern unsigned                             screen_height ;
extern unsigned                             renderwin_8bits ;
extern unsigned                             auto_render ;
extern unsigned                             queued_file_count ;
extern bool                                 IsW95UserInterface ;
extern bool                                 tips_enabled ;
extern HWND                                 render_window ;
extern HMENU                                hMainMenu ;
extern HINSTANCE                            hInstance ;

INT_PTR CALLBACK PovTipDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char                  **s ;
  HDC                   hdc ;
  HWND                  hItem ;
  RECT                  rect ;
  HPEN                  hpen ;
  HBRUSH                hbrush ;
  PAINTSTRUCT           ps ;
  WINDOWPLACEMENT       p ;
  static int            tipCount = 0 ;
  static int            tipID ;

  switch (message)
  {
    case WM_INITDIALOG :
         CenterWindowRelative ((HWND) lParam, hDlg, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         tipID = GetPrivateProfileInt ("TipOfTheDay", "NextTip", 0, EngineIniFileName) ;
         if (tipCount == 0)
           for (s = tips ; *s ; s++)
             tipCount++ ;
         if (tipID >= tipCount)
           tipID = 0 ;
         PutPrivateProfileInt ("TipOfTheDay", "NextTip", tipID + 1, EngineIniFileName) ;
         CheckDlgButton (hDlg, IDC_SHOWTIPS, tips_enabled) ;
         return (true) ;

    case WM_PAINT :
         hdc = BeginPaint (hDlg, &ps) ;
         hpen = (HPEN) GetStockObject (BLACK_PEN) ;
//       hbrush = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
//       hbrush = CreateSolidBrush (RGB (255, 255, 128)) ;
         hbrush = CreateSolidBrush (RGB (255, 255, 224)) ;
         hpen = (HPEN) SelectObject (hdc, hpen) ;
         hbrush = (HBRUSH) SelectObject (hdc, hbrush) ;
         hItem = GetDlgItem (hDlg, IDC_TIPFRAME) ;
         p.length = sizeof (WINDOWPLACEMENT) ;
         GetWindowPlacement (hItem, &p) ;
         rect = p.rcNormalPosition ;
         InflateRect (&rect, -5, -5) ;
         Rectangle (hdc, rect.left, rect.top, rect.right, rect.bottom) ;
         InflateRect (&rect, -5, -5) ;
         tip_of_the_day (hdc, &rect, tips [tipID]) ;
         SelectObject (hdc, hpen) ;
         SelectObject (hdc, hbrush) ;
         DeleteObject (hbrush) ;
         EndPaint (hDlg, &ps) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
           case IDCANCEL :
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDC_SHOWTIPS :
                tips_enabled = (IsDlgButtonChecked (hDlg, IDC_SHOWTIPS) != 0);
                PVCheckMenuItem (CM_TIPOFTHEDAY, tips_enabled ? MF_CHECKED : MF_UNCHECKED) ;
                return (true) ;

           case IDC_NEXTTIP :
                if (++tipID >= tipCount)
                  tipID = 0 ;
                PutPrivateProfileInt ("TipOfTheDay", "NextTip", tipID + 1, EngineIniFileName) ;
                PovInvalidateRect (hDlg, NULL, false) ;
                return (true) ;

           case IDC_PREVIOUSTIP :
                if (tipID-- == 0)
                  tipID = tipCount - 1 ;
                PovInvalidateRect (hDlg, NULL, false) ;
                return (true) ;

           default :
                return (true) ;
         }
  }
  return (false) ;
}

