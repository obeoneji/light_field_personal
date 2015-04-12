/****************************************************************************
 *                pvfiles.c
 *
 * This module contains ASCII file related code.
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
 * $File: //depot/povray/3.5/windows/pvfiles.cpp $
 * $Revision: #8 $
 * $Change: 4793 $
 * $DateTime: 2009/05/26 07:13:51 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION
#define WIN32_LEAN_AND_MEAN

#include <math.h>
#include <setjmp.h>
#include <string.h>

#ifdef _WIN64
#pragma pack(16)
#include <windows.h>
#pragma pack()
#else
#include <windows.h>
#endif

#include <io.h>
#include "frame.h"
#include "colour.h"
#include "povray.h"
#include "optout.h"
#include "pvengine.h"
#include "resource.h"
#include "pvlegal.h"
#include "pvdemo.h"
#include "pvtips.h"

extern int              message_ychar ;
extern char             ourPath [] ;
extern char             DocumentsPath [] ;

void fill_listbox (HWND hwnd)
{
  char        *p = povlegal_text ;
  char        str [1024] = "" ;
  char        *s = str ;

  while (*p)
  {
    if (*p == '\n')
    {
      *s = '\0' ;
      SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM) str) ;
      s = str ;
      p++ ;
      continue ;
    }
    *s++ = *p++ ;
  }
  *s = '\0' ;
  SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM) str) ;
}

void save_povlegal (void)
{
  char        filename [_MAX_PATH + 64] ;
  FILE        *outF ;

  sprintf (filename, "%sPOVLEGAL.DOC.TXT", DocumentsPath) ;
  if ((outF = fopen (filename, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create POVLEGAL.DOC.TXT", "Cannot save document") ;
    return ;
  }
  if (fwrite (povlegal_text, 1, sizeof (povlegal_text) - 1, outF) != sizeof (povlegal_text) - 1)
  {
    PovMessageBox ("Cannot write to POVLEGAL.DOC.TXT", "Cannot save document") ;
    fclose (outF) ;
    return ;
  }
  fclose (outF) ;
  sprintf (filename, "The POV-Ray license was written to the file %sPOVLEGAL.DOC.TXT\n\n"
                     "This file is in plain text format and may be viewed and printed "
                     "using notepad or any other text editor. Microsoft Word is not "
                     "required.", DocumentsPath) ;
  PovMessageBox (filename, "POVLEGAL.DOC.TXT saved") ;
}

INT_PTR CALLBACK PovLegalDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_INITDIALOG :
         resize_listbox_dialog (hDlg, IDC_LISTBOX, 79) ;
         CenterWindowRelative ((HWND) lParam, hDlg, true) ;
         SetWindowText (hDlg, "POV-Ray Legal Statement [POVLEGAL.DOC]") ;
//       hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         fill_listbox (GetDlgItem (hDlg, IDC_LISTBOX)) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam)) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
//              DeleteObject (hbr) ;
                EndDialog (hDlg, true) ;
                return (true) ;

           default :
                return (true) ;
         }

    case WM_MEASUREITEM :
         if (wParam == IDC_LISTBOX)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = message_ychar ;
           return (true) ;
         }
         else
           return (false) ;

    case WM_DRAWITEM :
         if (wParam == IDC_LISTBOX)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           d->itemState &= ~ODS_SELECTED ;
           draw_ordinary_listbox (d, false) ;
           return (true) ;
         }
         else
           return (false) ;
  }
  return (false) ;
}

char *save_demo_file (char *s1, char *s2)
{
  char        **p = povdemo_scene ;
  FILE        *outF ;

  GetTempPath (_MAX_PATH - 16, s1) ;
  if (s1 [strlen (s1) - 1] != '\\')
    strcat (s1, "\\") ;
  strcpy (s2, s1) ;
  strcat (s1, "POVDEMO.$$1") ;
  strcat (s2, "POVDEMO.$$2") ;
  if ((outF = fopen (s1, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    return (NULL) ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to temporary file", "Cannot run demo") ;
      fclose (outF) ;
      DELETE_FILE (s1) ;
      return (NULL) ;
    }
  }
  fclose (outF) ;
  p = povdemo_ini ;
  if ((outF = fopen (s2, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    DELETE_FILE (s1) ;
    return (NULL) ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to temporary file", "Cannot run demo") ;
      fclose (outF) ;
      DELETE_FILE (s1) ;
      DELETE_FILE (s2) ;
      return (NULL) ;
    }
  }
  fclose (outF) ;
  return (s1) ;
}


