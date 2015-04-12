/*******************************************************************************
 * cmedit.cpp
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/cmedit.cpp $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#include "cmedit.h"

#ifdef _WINDLL
HINSTANCE     hInstance ;

void CleanUp (void) ;

BOOL APIENTRY DllMain (HINSTANCE hInstDLL, DWORD reason, LPVOID reserved)
{
  switch (reason)
  {
    case DLL_PROCESS_ATTACH :
         hInstance = hInstDLL ;
         break ;

    case DLL_THREAD_ATTACH :
         break ;

    case DLL_THREAD_DETACH :
         break ;

    case DLL_PROCESS_DETACH :
         CleanUp () ;
         break ;
  }
  return (true) ;
}
#endif
