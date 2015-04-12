/*******************************************************************************
 * dialogs.h
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/dialogs.h $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __DIALOGS_H__
#define __DIALOGS_H__

#ifndef MB_CANCELTRYCONTINUE
#define MB_CANCELTRYCONTINUE      6
#define IDTRYAGAIN                10
#define IDCONTINUE                11
#endif

#ifndef SM_CMONITORS
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#endif

#define DONTASKAGAINFLAG        0x80000000

int ShowFileChangedDialog (char *FileName, bool HasSaveAll = true) ;
int ShowReloadDialog (char *FileName) ;
int ShowSaveBeforeRenderDialog (char *FileName) ;
int ShowEnterValueDialog (char *caption, unsigned short min, unsigned short max, unsigned int initial) ;
UINT_PTR CALLBACK PageSetupHook (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
UINT_PTR CALLBACK PrintHook (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;

#endif
