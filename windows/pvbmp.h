/****************************************************************************
 *                bmp.h
 *
 * This module contains all defines, typedefs, and prototypes for BMP.CPP.
 *
 * Author: Wlodzimierz ABX Skiba (abx@abx.art.pl)
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
 * $File: //depot/povray/3.5/windows/pvbmp.h $
 * $Revision: #14 $
 * $Change: 2933 $
 * $DateTime: 2004/07/03 13:06:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef BMP_H
#define BMP_H

USING_POV_NAMESPACE

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/*****************************************************************************
* Global typedefs
******************************************************************************/

class BMP_Image : public Image_File_Class
{
	public:
		BMP_Image(char *filename, int width, int height, int mode, int line = 0);
		~BMP_Image();

		void Write_Line(COLOUR *line_data);
		int Read_Line(COLOUR *line_data);

		int Line() { return line_number; };
		int Width() { return width; };
		int Height() { return height; };
	private:
		char *filename;
		int mode;
		int width, height;
		int line_number;
    IStream *in_file;
    OStream *out_file;
};

/*****************************************************************************
* Global variables
******************************************************************************/

/*****************************************************************************
* Global functions
******************************************************************************/

BEGIN_POV_NAMESPACE
void Read_BMP_Image (IMAGE *Image, char *filename);
bool BMP_Is_Continuable (const char *filename, int width, int height) ;
END_POV_NAMESPACE

#endif
