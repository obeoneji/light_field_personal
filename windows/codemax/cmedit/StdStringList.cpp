/*******************************************************************************
 * stdstringlist.cpp
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/StdStringList.cpp $
 * $Revision: #4 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#include "cmedit.h"
#include "ccodemax.h"
#include "settings.h"
#include "menusupport.h"
#include "eventhandlers.h"
#include "editorinterface.h"
#include "dialogs.h"

CStdStringList::CStdStringList(void)
{
}

CStdStringList::~CStdStringList(void)
{
}

bool CStdStringList::LoadFromFile (LPCSTR FileName)
{
  FILE        *f ;

  m_Items.clear () ;
  int size = GetFileLength (FileName) ;
  if (size == -1 || size > 1024 * 1024)
    return (false) ;
  if ((f = fopen (FileName, "rb")) == NULL)
    return (false) ;
  char *buffer = new char [size + 1] ;
  int nread = (int) fread (buffer, size, 1, f) ;
  fclose (f) ;
  if (nread != 1)
  {
    delete [] buffer ;
    return (false) ;
  }
  char *s1 = buffer ;
  char *s2 = buffer + size ;
  char *s3 = buffer ;
  while (s1 < s2)
  {
    if (*s1 == '\r' || *s1 == '\n')
    {
      *s1++ = '\0' ;
      AppendItem (s3) ;
      if (s1 < s2 && *s1 == '\n')
        *s1++ ;
      s3 = s1 ;
    }
    else
      s1++ ;
  }
  if (s3 < s2 && s1 > s3)
  {
    *s2 = '\0' ;
    AppendItem (s3) ;
  }
  delete [] buffer ;
  return (true) ;
}

int CStdStringList::ItemCount (void)
{
  return ((int) m_Items.size ()) ;
}

CStdString& CStdStringList::operator [] (int index)
{
  if (index >= (int) m_Items.size ())
    return (m_EmptyStr) ;
  return (m_Items.at (index)) ;
}

bool CStdStringList::DeleteItem (int index)
{
  if (index == 0)
    m_Items.pop_front () ;
  else if (index == m_Items.size () - 1)
    m_Items.pop_back () ;
  else if (index < (int) m_Items.size ())
    m_Items.erase (m_Items.begin () + index) ;
  else
    return (false) ;
  return (true) ;
}

CStdString& CStdStringList::InsertItem (LPCSTR text)
{
  m_Items.push_front (text) ;
  return (m_Items.front ()) ;
}

CStdString& CStdStringList::InsertItem (int index, LPCSTR text)
{
  if (index == 0)
    return (InsertItem (text)) ;
  m_Items.insert (m_Items.begin () + index, text) ;
  return (m_Items [index]) ;
}

CStdString& CStdStringList::AppendItem (LPCSTR text)
{
  m_Items.push_back (text) ;
  return (m_Items.back ()) ;
}

void CStdStringList::Clear (void)
{
  m_Items.clear () ;
}
