/****************************************************************************
 *                pvbmp.cpp
 *
 * This module contains the code to read and write the BMP output file format.
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
 * $File: //depot/povray/3.5/windows/pvbmp.cpp $
 * $Revision: #12 $
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

#include "frame.h"
#include "povray.h"
#include "pov_util.h"
#include "pvbmp.h"

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define WIN_NEW     40
#define WIN_OS2_OLD 12
#define MAGIC1 0x15
#define MAGIC2 0x05
#define MAGIC3 0x75

/*****************************************************************************
* Local typedefs
******************************************************************************/

/*****************************************************************************
* Local variables
******************************************************************************/

/*****************************************************************************
* Static functions
******************************************************************************/

/*****************************************************************************
*
* FUNCTION
*
*   Read_Safe_Char
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*   Jan 2004 : Added exception to allow cleanup on error [CJC]
*
******************************************************************************/

static inline unsigned char Read_Safe_Char (IStream& in)
{
  unsigned char ch;

  in >> ch;
  if (!in)
    throw "Error reading data from BMP image." ;

  return (ch) ;
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_1b
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_1b(unsigned char **lines, IStream& in, unsigned width, unsigned height)
{
  int c = 0;
  unsigned pwidth = ((width+31)>>5)<<5; /* clear bits to get 4 byte boundary */

  for (unsigned y=height; (--y)<height;)
    for (unsigned i=0; i<pwidth; i++)
    {
      if ((i&7) == 0)
      {
        c = Read_Safe_Char (in);
      }
      if (i<width)
      {
        lines[y][i] = (c & 0x80) ? 1 : 0;
        c <<= 1;
      }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_4b_RGB
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_4b_RGB(unsigned char **lines, IStream& in, unsigned width, unsigned height)
{
  int c = 0;
  unsigned pwidth = ((width+7)>>3)<<3;

  for (unsigned y=height; (--y)<height;)
    for (unsigned i=0; i<pwidth; i++)
    {
      if ((i&1)==0)
      {
        c = Read_Safe_Char(in);
      }
      if (i<width)
      {
        lines[y][i] = (c&0xf0)>>4;
        c <<= 4;
      }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_4b_RLE
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_4b_RLE(unsigned char **lines, IStream& in, unsigned width, unsigned height)
{
  int c, cc = 0;
  unsigned x = 0;
  unsigned y = height-1;

  while (1)
  {
    c = Read_Safe_Char (in);
    if (c)
    {
      cc = Read_Safe_Char (in);
      for (int i=0; i<c; i++, x++)
        if ((y<height) && (x<width))
          lines[y][x] = (i&1) ? (cc &0x0f) : ((cc>>4)&0x0f);
    }
    else
    {
      c = Read_Safe_Char (in);
      if (c==0)
      {
        x=0;
        y--;
      }
      else if (c==1)
        return;
      else if (c==2)
      {
        x += Read_Safe_Char (in);
        y -= Read_Safe_Char (in);
      }
      else
      {
        for (int i=0; i<c; i++, x++)
        {
          if ((i&1)==0)
            cc = Read_Safe_Char (in);
          if ((y<height) && (x<width))
            lines[y][x] = ((i&1)?cc:(cc>>4))&0x0f;
        }
        if (((c&3)==1) || ((c&3)==2))
          Read_Safe_Char (in);
      }
    }
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_8b_RGB
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_8b_RGB(unsigned char **lines, IStream& in, unsigned width, unsigned height)
{
  int c;
  unsigned pwidth = ((width+3)>>2)<<2;

  for (unsigned y=height; (--y)<height;)
    for (unsigned i=0; i<pwidth; i++)
    {
      c = Read_Safe_Char (in);
      if (i<width)
        lines[y][i] = c;
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_8b_RLE
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_8b_RLE(unsigned char **lines, IStream& in, unsigned width, unsigned height)
{
  int c, cc;
  unsigned x = 0;
  unsigned y = height-1;

  while (1)
  {
    c = Read_Safe_Char (in);
    if (c)
    {
      cc = Read_Safe_Char (in);
      for (int i=0; i<c; i++, x++)
        if ((y<height) && (x<width))
          lines[y][x] = cc;
    }
    else
    {
      c = Read_Safe_Char (in);
      switch(c)
      {
        case 0:
          x = 0;
          y--;
          break;
        case 1:
          return;
          break;
        case 2:
          x += Read_Safe_Char (in);
          y -= Read_Safe_Char (in);
          break;
        default:
          for (int i=0; i<c; i++, x++)
            if ((y<height) && (x<width))
              lines[y][x] = Read_Safe_Char (in);
          if (c & 1)
            Read_Safe_Char (in); /* "absolute mode" runs are word-aligned */
      }
    }
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_24b
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_24b(IMAGE8_LINE *lines, IStream& in, unsigned width, unsigned height)
{
  int pad = (4-((width*3)%4)) &0x03;

  for (unsigned y=height; (--y)<height;)
  {
    for (unsigned i=0; i<width; i++)
    {
      lines[y].blue[i] = Read_Safe_Char (in);
      lines[y].green[i] = Read_Safe_Char (in);
      lines[y].red[i] = Read_Safe_Char (in);
    }
    if (pad && !in.seekg (pad, POV_SEEK_CUR))
      throw "Error reading data from BMP image." ;
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_32b
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Stefan Maierhofer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Nov 2003 : Creation.
*   Jan 2004 : added support for alpha channel [CJC]
*
******************************************************************************/

static void Read_BMP_32b(IMAGE8_LINE *lines, IStream& in, unsigned width, unsigned height)
{
  for (unsigned y=height; (--y)<height;)
  {
    for (unsigned i=0; i<width; i++)
    {
      lines[y].blue[i] = Read_Safe_Char (in);
      lines[y].green[i] = Read_Safe_Char (in);
      lines[y].red[i] = Read_Safe_Char (in);
      lines[y].transm[i] = Read_Safe_Char (in);
    }
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   Open_BMP_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*   Jan 2004 : Added exception handling to allow cleanup on error [CJC]
*
******************************************************************************/

BMP_Image::BMP_Image (char *file_name, int image_width, int image_height, int file_mode, int line /* = 0 */)
{
  unsigned data_location, file_depth, compression, planes;
  unsigned char *magic;

  mode = file_mode ;
  filename = file_name;
  width = image_width ;
  height = image_height ;
  line_number = line ;
  in_file = NULL ;
  out_file = NULL ;

  switch (mode)
  {
    case READ_MODE:
      if ((in_file = New_Checked_IStream (filename, POV_File_Image_System)) != NULL)
      {
        IStream& in = *in_file ;

        if ((in.Read_Byte () != 'B') || (in.Read_Byte () !='M'))
        {
          delete in_file ;
          in_file = NULL ;
          Error ("Error reading magic number of BMP image.");
        }

        in.ignore (8) ; // skip file size and reserved fields.
        data_location = in.Read_Long () ;

        if (in.Read_Long () != WIN_NEW)
        {
          delete in_file ;
          in_file = NULL ;
          Error ("Wrong BMP image format.");
        }

        width = in.Read_Long () ;
        height = in.Read_Long () ;
        planes = in.Read_Short () ;
        file_depth = in.Read_Short () ;
        compression = in.Read_Long () ;
        in.ignore (20) ; // skip image size in bytes, H&V pixels per meter, colors, needed colors

        if ((file_depth != 24) || (compression != BI_RGB) || (planes != 1))
        {
          delete in_file ;
          in_file = NULL ;
          Error ("Wrong BMP image format.");
        }

        if (image_width != width || image_height != height)
        {
          delete in_file ;
          in_file = NULL ;
          Warning (0, "BMP file dimensions do not match render resolution - creating new file.");
          return ;
        }

        if (in.eof () || !in.seekg (data_location))
        {
          delete in_file ;
          in_file = NULL ;
          Warning (0, "Input BMP file ended prematurely - creating new file.");
          return ;
        }
      }
      else
        return ;
      break;

  case WRITE_MODE:
      if (opts.Options & TO_STDOUT)
      {
        // this can't work with BMP's ... we need to seek !
        Error ("BMP files cannot be written to stdout (cannot seek).");
      }
      if ((out_file = New_Checked_OStream (filename, POV_File_Image_System, false)) != NULL)
      {

        OStream& out = *out_file ;

        out << 'B' << 'M' ;
        out.Write_Long (14 + 40 + ((width * 24 + 31) / 32) * 4 * height) ;
        out.Write_Short (0) ;
        out.Write_Short (0) ;
        out.Write_Long (14 + 40) ;
        out.Write_Long (40) ;
        out.Write_Long (width) ;
        out.Write_Long (height) ;
        out.Write_Short (1) ;
        out.Write_Short (24) ;
        out.Write_Long (BI_RGB) ;
        out.Write_Long ((width * 24 + 31) / 32 * 4 * height) ;
        out.Write_Long (0) ;
        out.Write_Long (0) ;
        out.Write_Long (0) ;
        out.Write_Long (0) ;

        width = width;
        height = height;

        magic = (unsigned char *) POV_MALLOC (((width * 24 + 31) / 32) * 4 , "BMP magic") ;
        memset (magic, 0, ((width * 24 + 31) / 32) * 4) ;
        magic [0] = MAGIC1 ;
        magic [1] = MAGIC2 ;
        magic [2] = MAGIC3 ;
        for (int i = 0 ; i < height ; i++)
          out.write ((char *) magic, (width * 24 + 31) / 32 * 4) ;
        POV_FREE (magic) ;
      }
      else
        return ;

      break;

  case APPEND_MODE:
      if (opts.Options & TO_STDOUT)
      {
        // this can't work with BMP's ... we need to read !
        Error ("BMP files cannot be appended to stdout (cannot read).");
      }

      if ((out_file = New_Checked_OStream (filename, POV_File_Image_System, true)) == NULL)
        return ;

      break ;
    }

  valid = true;
}

/*****************************************************************************
*
* FUNCTION
*
*   Close_BMP_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

BMP_Image::~BMP_Image ()
{
  if(in_file != NULL)
  {
    delete in_file ;
    in_file = NULL ;
  }

  /* Close the output file (if open) */
  if (out_file != NULL)
  {
    out_file->flush () ;
    delete out_file ;
    out_file = NULL ;
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   Write_BMP_Line
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

void BMP_Image::Write_Line (COLOUR *line_data)
{
  int pad = (4 - ((width * 3) % 4)) & 0x03 ;
  int i;
  OStream& out = *out_file ;

  if (!out.seekg (14 + 40 + (height - 1 - line_number) * (3 * width + pad)))
    Error ("Error seeking in BMP image.") ;

  for (i = 0 ; i < width ; i++)
  {
    out << (unsigned char) floor (line_data [i] [pBLUE] * 255.0) ;
    out << (unsigned char) floor (line_data [i] [pGREEN] * 255.0) ;
    out << (unsigned char) floor (line_data [i] [pRED] * 255.0) ;
  }
  for (i = 0 ; i < pad; i++)
    out << (unsigned char) 0 ;

  if (!out)
    Error ("Error writing to BMP image.") ;

  line_number++;

  out.flush () ;
}


/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_Line
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

int BMP_Image::Read_Line (COLOUR *line_data)
{
  int info = 0;
  int pad = (4 - ((width * 3) % 4)) & 0x03 ;

  IStream &in = *in_file ;
  if (!in.seekg (-(line_number + 1) * (3 * width + pad), POV_SEEK_END))
    return (-1) ;

  for (int i = 0 ; i < width ; i++)
  {
    unsigned char ch ;

    if (!in.Read_Byte (ch))
      return (-1) ;
    line_data [i] [pBLUE] = (DBL) ch / 255.0 ;
    if ((!i && (ch != MAGIC1)) || (i && ch))
      info = 1 ;

    if (!in.Read_Byte (ch))
      return (-1) ;
    line_data [i] [pGREEN] = (DBL) ch /255.0 ;
    if ((!i && (ch != MAGIC2)) || (i && ch))
      info = 1 ;

    if (!in.Read_Byte (ch))
      return (-1) ;
    line_data [i] [pRED] = (DBL) ch / 255.0 ;
    if ((!i && (ch != MAGIC3)) || (i && ch))
      info = 1 ;
  }

  if (info)
    line_number++ ;
  return (info) ;
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

BEGIN_POV_NAMESPACE

/*****************************************************************************
*
* FUNCTION
*
*   BMP_Is_Continuable
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Cason
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 2001 : Creation.
*
******************************************************************************/

bool BMP_Is_Continuable (const char *filename, int width, int height)
{
  IStream *in ;

  if ((in = New_Checked_IStream ((char *) filename, POV_File_Image_System)) == NULL)
    return (false) ;

  if ((in->Read_Byte () != 'B') || (in->Read_Byte () !='M'))
  {
    delete in ;
    return (false) ;
  }

  in->ignore (12) ;
  if (in->Read_Long () != WIN_NEW)
  {
    delete in ;
    return (false) ;
  }

  if (width != -1 && height != -1)
  {
    if (width != in->Read_Long () || height != in->Read_Long ())
    {
      delete in ;
      return (false) ;
    }
  }
  else
    in->ignore (8) ;

  if (in->Read_Short () != 1 || in->Read_Short () != 24 || in->Read_Long () != BI_RGB)
  {
    delete in ;
    return (false) ;
  }

  delete in ;
  return (true) ;
}

void Read_BMP_Image(IMAGE *Image, char *name)
{
  unsigned file_width, file_height;
  unsigned file_depth, file_colors;
  unsigned data_location, planes, compression;
  unsigned info;
  IMAGE8_LINE *line_data;

  IStream *f ;
  if ((f = Locate_File (name, POV_File_Image_System, NULL, 0)) == NULL)
    Error ("Error opening BMP image '%s'.", name);

  if ((f->Read_Byte () != 'B') || (f->Read_Byte () !='M'))
  {
    delete f ;
    Error ("Error reading magic number of BMP image.");
  }

  // skip file size and reserved fields
  f->ignore (8) ;
  data_location = f->Read_Long () ;

  // read properties
  if ((info = f->Read_Long ()) != WIN_OS2_OLD)
  {
    file_width = f->Read_Long () ;
    file_height = f->Read_Long () ;
    planes = f->Read_Short () ;
    file_depth = f->Read_Short () ;
    compression = f->Read_Long () ;
    f->ignore (12) ; // skip image size in bytes, H&V pixels per meter
    file_colors = f->Read_Long () ;
    f->ignore (4) ; // skip needed colors
  }
  else  /* info == WIN_OS2_OLD */
  {
    file_width = f->Read_Short () ;
    file_height = f->Read_Short () ;
    planes = f->Read_Short () ;
    file_depth = f->Read_Short () ;
    compression = BI_RGB ;
    file_colors = 0 ;
  }

  if (f->eof ())
  {
    delete f ;
    Error ("Error reading data from BMP image.") ;
  }

  /* do not allow other subtypes */
  if (((file_depth!=1) && (file_depth!=4) && (file_depth!=8) && (file_depth!=24) && (file_depth!=32)) ||
      (planes!=1) || (compression>BI_RLE4) ||
      (((file_depth==1) || (file_depth==24) || (file_depth==32)) && (compression!=BI_RGB)) ||
      ((file_depth==4) && (compression==BI_RLE8)) ||
      ((file_depth==8) && (compression==BI_RLE4))) 
  {
    delete f ;
    Error ("Invalid BMP image (depth=%d planes=%d compression=%d).", file_depth, planes, compression);
  }

  Image->iwidth = file_width;
  Image->iheight = file_height;
  Image->width = (SNGL)file_width;
  Image->height = (SNGL)file_height;
  Image->Colour_Map = NULL;

  /* seek to colormap */
  if (info != WIN_OS2_OLD)
    f->ignore (info - 40) ;

  /* load color_map */
  if (file_depth<24)
  {
    int color_map_length = file_colors ? file_colors : 1<<file_depth ;

    Image->Colour_Map = (IMAGE_COLOUR *)POV_CALLOC((size_t)color_map_length, sizeof(IMAGE_COLOUR), "BMP color map");

    for (int i=0; i<color_map_length; i++)
    {
      Image->Colour_Map[i].Blue = f->Read_Byte () ;
      Image->Colour_Map[i].Green = f->Read_Byte () ;
      Image->Colour_Map[i].Red = f->Read_Byte () ;
      if (info != WIN_OS2_OLD)
        f->ignore (1) ;
    }

    if (f->eof ())
    {
      delete f ;
      Error("Error reading data from BMP image.");
    }

    Image->Colour_Map_Size = color_map_length;
    Image->data.map_lines =
      (unsigned char **)POV_MALLOC(file_height*sizeof(unsigned char **), "BMP image");
  }
  else
    Image->data.rgb8_lines = (IMAGE8_LINE *)POV_MALLOC(file_height*sizeof(IMAGE8_LINE), "BMP image");
  f->seekg (data_location) ;

  try
  {
    if (file_depth < 24)
    {
      for (int i=file_height-1; i>=0; i--)
      {
        Image->data.map_lines[i] = (unsigned char *)POV_MALLOC(file_width*sizeof(unsigned char), "BMP image line");
      }
      switch (file_depth)
      {
        case 1:
          Read_BMP_1b(Image->data.map_lines, *f, file_width, file_height);
          break;
        case 4:
          switch(compression)
          {
            case BI_RGB:
              Read_BMP_4b_RGB(Image->data.map_lines, *f, file_width, file_height);
              break;
            case BI_RLE4:
              Read_BMP_4b_RLE(Image->data.map_lines, *f, file_width, file_height);
              break;
            default:
              delete f ;
              Error ("Unknown compression scheme in BMP image.");
          }
          break;
        case 8:
          switch(compression)
          {
            case BI_RGB:
              Read_BMP_8b_RGB(Image->data.map_lines, *f, file_width, file_height);
              break;
            case BI_RLE8:
              Read_BMP_8b_RLE(Image->data.map_lines, *f, file_width, file_height);
              break;
            default:
              delete f ;
              Error ("Unknown compression scheme in BMP image.");
          }
          break;
        default:
          delete f ;
          Error ("Unknown depth in BMP image.");
      }
    }
    else
    {
      for (int i=file_height-1; i>=0; i--)
      {
        line_data = &Image->data.rgb8_lines[i];
        line_data->red = (unsigned char *)POV_MALLOC(file_width*sizeof(unsigned char), "BMP image line");
        line_data->green = (unsigned char *)POV_MALLOC(file_width*sizeof(unsigned char), "BMP image line");
        line_data->blue = (unsigned char *)POV_MALLOC(file_width*sizeof(unsigned char), "BMP image line");
        line_data->transm = NULL;
        if (file_depth == 32)
          line_data->transm = (unsigned char *)POV_MALLOC(file_width*sizeof(unsigned char), "BMP image line");
        else
          line_data->transm = NULL;
      }

      /* update stefan maierhofer, 32bit */
      if (file_depth == 24)
        Read_BMP_24b (Image->data.rgb8_lines, *f, file_width, file_height) ;
      else
        Read_BMP_32b (Image->data.rgb8_lines, *f, file_width, file_height) ;
    }
  }
  catch (char *msg)
  {
    delete f ;
    Error (msg) ;
  }
 
  delete f ;
}

END_POV_NAMESPACE
