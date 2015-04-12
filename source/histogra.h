/****************************************************************************
 *                  histogra.h
 *
 * This module contains all defines, typedefs, and prototypes for histogra.cpp.
 *
 * from Persistence of Vision(tm) Ray Tracer version 3.6.
 * Copyright 1991-2003 Persistence of Vision Team
 * Copyright 2003-2009 Persistence of Vision Raytracer Pty. Ltd.
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
 * $File: //depot/povray/3.5/source/histogra.h $
 * $Revision: #13 $
 * $Change: 4808 $
 * $DateTime: 2009/06/03 08:16:08 $
 * $Author: calimet $
 * $Log$
 *****************************************************************************/


#ifndef HISTOGRA_H
#define HISTOGRA_H

BEGIN_POV_NAMESPACE

void initialise_histogram();
void destroy_histogram();
void accumulate_histogram(int x, int  y, int  on);
void write_histogram(char *filename);

END_POV_NAMESPACE

#endif
