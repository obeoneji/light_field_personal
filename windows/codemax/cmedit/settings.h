/*******************************************************************************
 * settings.h
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
 * $File: //depot/povray/3.5/windows/codemax/cmedit/settings.h $
 * $Revision: #2 $
 * $Change: 4779 $
 * $DateTime: 2009/05/17 10:55:51 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define MAX_OLDER_FILES           32
#define MAX_KEYWORDS              1024
#define CMD_NEXT_KEYWORD          (CMD_USER_BASE)
#define CMD_PREV_KEYWORD          (CMD_USER_BASE + 1)

#define DEF_CLRWINDOW             CLR_INVALID
#define DEF_CLRLEFTMARGIN         RGB (0, 255, 255)
#define DEF_CLRBOOKMARK           CLR_INVALID
#define DEF_CLRBOOKMARKBK         CLR_INVALID
#define DEF_CLRTEXT               RGB (0, 0, 0)
#define DEF_CLRTEXTBK             CLR_INVALID
#define DEF_CLRNUMBER             RGB (0, 128, 128)
#define DEF_CLRNUMBERBK           CLR_INVALID
#define DEF_CLRKEYWORD            RGB (128, 0, 128)
#define DEF_CLRKEYWORDBK          CLR_INVALID
#define DEF_CLROPERATOR           RGB (255, 0, 0)
#define DEF_CLROPERATORBK         CLR_INVALID
#define DEF_CLRSCOPEKEYWORD       RGB (0, 0, 255)
#define DEF_CLRSCOPEKEYWORDBK     CLR_INVALID
#define DEF_CLRCOMMENT            RGB (0, 128, 0)
#define DEF_CLRCOMMENTBK          CLR_INVALID
#define DEF_CLRSTRING             RGB (255, 0, 0)
#define DEF_CLRSTRINGBK           CLR_INVALID
#define DEF_CLRTAGTEXT            RGB (0, 0, 0)
#define DEF_CLRTAGTEXTBK          CLR_INVALID
#define DEF_CLRTAGENT             RGB (255, 0, 0)
#define DEF_CLRTAGENTBK           CLR_INVALID
#define DEF_CLRTAGELEMNAME        RGB (128, 0, 0)
#define DEF_CLRTAGELEMNAMEBK      CLR_INVALID
#define DEF_CLRTAGATTRNAME        RGB (0, 0, 255)
#define DEF_CLRTAGATTRNAMEBK      CLR_INVALID
#define DEF_CLRLINENUMBER         RGB (0, 0, 0)
#define DEF_CLRLINENUMBERBK       RGB (255, 255, 255)
#define DEF_CLRHDIVIDERLINES      CLR_INVALID
#define DEF_CLRVDIVIDERLINES      CLR_INVALID
#define DEF_CLRHIGHLIGHTEDLINE    RGB (255, 255, 0)

#define CJC_CLRWINDOW             RGB (0, 0, 0)
#define CJC_CLRLEFTMARGIN         RGB (128, 0, 0)
#define CJC_CLRBOOKMARK           RGB (255, 255, 0)
#define CJC_CLRBOOKMARKBK         CLR_INVALID
#define CJC_CLRTEXT               RGB (255, 255, 255)
#define CJC_CLRTEXTBK             CLR_INVALID
#define CJC_CLRNUMBER             RGB (0, 255, 0)
#define CJC_CLRNUMBERBK           CLR_INVALID
#define CJC_CLRKEYWORD            RGB (0, 255, 255)
#define CJC_CLRKEYWORDBK          CLR_INVALID
#define CJC_CLROPERATOR           RGB (0, 255, 255)
#define CJC_CLROPERATORBK         CLR_INVALID
#define CJC_CLRSCOPEKEYWORD       RGB (0, 255, 0)
#define CJC_CLRSCOPEKEYWORDBK     CLR_INVALID
#define CJC_CLRCOMMENT            RGB (255, 255, 0)
#define CJC_CLRCOMMENTBK          CLR_INVALID
#define CJC_CLRSTRING             RGB (255, 255, 0)
#define CJC_CLRSTRINGBK           CLR_INVALID
#define CJC_CLRTAGTEXT            RGB (0, 255, 255)
#define CJC_CLRTAGTEXTBK          CLR_INVALID
#define CJC_CLRTAGENT             RGB (255, 0, 0)
#define CJC_CLRTAGENTBK           CLR_INVALID
#define CJC_CLRTAGELEMNAME        RGB (255, 255, 0)
#define CJC_CLRTAGELEMNAMEBK      CLR_INVALID
#define CJC_CLRTAGATTRNAME        RGB (0, 255, 255)
#define CJC_CLRTAGATTRNAMEBK      CLR_INVALID
#define CJC_CLRLINENUMBER         RGB (255, 255, 255)
#define CJC_CLRLINENUMBERBK       RGB (0, 0, 0)
#define CJC_CLRHDIVIDERLINES      CLR_INVALID
#define CJC_CLRVDIVIDERLINES      CLR_INVALID
#define CJC_CLRHIGHLIGHTEDLINE    RGB (0, 0, 255)

#define DEF_FSKEYWORD             CM_FONT_NORMAL
#define DEF_FSCOMMENT             CM_FONT_NORMAL
#define DEF_FSOPERATOR            CM_FONT_NORMAL
#define DEF_FSSCOPEKEYWORD        CM_FONT_NORMAL
#define DEF_FSSTRING              CM_FONT_NORMAL
#define DEF_FSTEXT                CM_FONT_NORMAL
#define DEF_FSNUMBER              CM_FONT_NORMAL
#define DEF_FSTAGTEXT             CM_FONT_NORMAL
#define DEF_FSTAGENT              CM_FONT_NORMAL
#define DEF_FSTAGELEMNAME         CM_FONT_NORMAL
#define DEF_FSTAGATTRNAME         CM_FONT_NORMAL
#define DEF_FSLINENUMBER          CM_FONT_NORMAL

class CRegDef
{
public:
  CStdString m_Path ;

public:
  CRegDef (LPCSTR Path) ;
  ~CRegDef () ;
  int ReadInt (LPCSTR Name, int defval) ;
  bool ReadBool (LPCSTR Name, bool defval) ;
  int ReadBin (LPCSTR Name, char *data, int len, char *defval) ;
  int ReadBin (LPCSTR Name, char **data, int len, char *defval) ;
  LPCTSTR ReadString (LPCSTR Name, LPCTSTR defval) ;
  bool WriteInt (LPCSTR Name, int val) ;
  bool WriteBool (LPCSTR Name, bool val) ;
  bool WriteBin (LPCSTR Name, char *data, int len) ;
  bool WriteString (LPCSTR Name, LPCTSTR data) ;
  bool DeleteValue (LPCSTR Name) ;

  bool IsOK (void) { return m_Result ; }

protected:
  HKEY m_hKey ;
  bool m_Result ;
} ;

typedef struct _EditConfigStruct
{
  TAutoIndent           AutoIndent ;
  bool                  SyntaxHighlighting ;
  bool                  WhiteSpaceDisplay ;
  bool                  TabExpand ;
  bool                  SmoothScrolling ;
  bool                  LineToolTips ;
  bool                  LeftMarginVisible ;
  bool                  CaseSensitive ;
  bool                  PreserveCase ;
  bool                  WholeWordEnabled ;
  bool                  DragDropEnabled ;
  bool                  HSplitterEnabled ;
  bool                  VSplitterEnabled ;
  bool                  ColumnSelEnabled ;
  bool                  RegexpEnabled ;
  bool                  OvertypeCaret ;
  bool                  SelBoundsEnabled ;
  bool                  TabKeywordExpansion ;
  TScrollStyle          ScrollBars ;
  int                   TabSize ;
  int                   UndoLimit ;
  CM_COLORS             Colours ;
  CM_FONTSTYLES         FontStyles ;
  HFONT                 HFont ;
  char                  *HotKeys ;
  int                   HotKeyLen ;
  CStdString            FindMRUList ;
  CStdString            ReplaceMRUList ;
  char                  *Macros [CM_MAX_MACROS] ;
  int                   MacroLen [CM_MAX_MACROS] ;
} EditConfigStruct ;

CStdString GetNextField (CStdString& str) ;
CStdString GetField (CStdString str, int FieldNo = 0) ;
void GetSettings (CRegDef *t, EditConfigStruct *ec, bool RestoreFiles) ;
void PutSettings (CRegDef *t, EditConfigStruct *ec) ;

#endif
