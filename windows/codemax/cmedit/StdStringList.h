/*******************************************************************************
 * StdStringList.h
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/StdStringList.h $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __STDSTRINGLIST_H__
#define __STDSTRINGLIST_H__

typedef std::deque<CStdString> StdStringList ;
typedef StdStringList::iterator StdStringListIterator ;

class CStdStringList
{
public:
  CStdStringList(void);
  ~CStdStringList(void);
  bool LoadFromFile (LPCSTR FileName) ;
  int ItemCount (void) ;
  bool DeleteItem (int index) ;
  CStdString& InsertItem (LPCSTR text) ;
  CStdString& InsertItem (int index, LPCSTR text) ;
  CStdString& AppendItem (LPCSTR text) ;
  CStdString& operator [] (int index) ;
  void Clear (void) ;
private:
  CStdString m_EmptyStr ;
  StdStringList m_Items ;
};

#endif
