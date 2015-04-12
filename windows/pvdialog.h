/****************************************************************************
 *                pvdialog.h
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
 * $File: //depot/povray/3.5/windows/pvdialog.h $
 * $Revision: #9 $
 * $Change: 2933 $
 * $DateTime: 2004/07/03 13:06:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef PVDIALOG_H_INCLUDED
#define PVDIALOG_H_INCLUDED

INT_PTR CALLBACK PovTipDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;

#endif // PVDIALOG_H_INCLUDED
