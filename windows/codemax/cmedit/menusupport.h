/*******************************************************************************
 * menusupport.h
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/menusupport.h $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __MENUSUPPORT_H__
#define __MENUSUPPORT_H__

void MakeRecentMenus (void) ;
void MakeOlderMenus (void) ;
void SetMenuShortcuts (void) ;
void EnableMenuItem (DWORD id, bool state) ;
void CheckMenuItem (DWORD id, bool state) ;
bool IsMenuItemChecked (DWORD id) ;
bool ToggleMenuItem (DWORD id) ;
void CheckMenuRadioItem (DWORD idItem, DWORD idFirst, DWORD idLast) ;
void SetMenuItemText (DWORD idItem, LPCSTR str) ;
void SetMenuShortcut (DWORD idItem, WORD Command) ;
void SetMenuState (void) ;
void MakeRecentMenus (void) ;
void MakeOlderMenus (void) ;
void AddToRecent (LPCTSTR FileName) ;
void UpdateRecent (void) ;
void UpdateWindowMenu (void) ;

#endif
