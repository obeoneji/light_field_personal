/****************************************************************************
 *                pvfrontend.cpp
 *
 * This module contains the default C++ interface for render frontend.
 *
 * Author: Christopher J. Cason and Thorsten Froelich.
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
 * $File: //depot/povray/3.5/windows/pvfrontend.cpp $
 * $Revision: #18 $
 * $Change: 3231 $
 * $DateTime: 2004/12/09 13:03:35 $
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

#include "pvengine.h"
#include "pvedit.h"
#include "povmsgid.h"
#include "pvfrontend.h"

extern int              delay_next_status ;
extern int              seconds_for_last_line ;
extern bool             no_status_output ;
extern bool             stop_rendering ;
extern bool             rendering ;
extern bool             output_to_file ;
extern bool             running_demo ;
extern bool             demo_mode ;
extern char             output_file_name [_MAX_PATH] ;
extern char             EngineIniFileName [_MAX_PATH] ;
extern char             CurrentRerunFileName [_MAX_PATH] ;
extern char             ErrorFilename [_MAX_PATH] ;
extern char             ErrorMessage [4096] ;
extern HWND             main_window ;
extern unsigned         ErrorLine ;
extern unsigned         ErrorCol ;
extern unsigned         render_width ;

void PrintRenderTimes (bool Finished, bool NormalCompletion) ;
void write_rerun_information (POVMSObjectPtr msg, POV_FRONTEND_NAMESPACE::ProcessRenderOptions *options) ;

BEGIN_POV_FRONTEND_NAMESPACE

bool WinRenderFrontend::CollectStats ;
int WinRenderFrontend::StatLine ;

WinRenderFrontend::WinRenderFrontend(POVMSContext ctx, POVMSAddress addr) : RenderFrontend (ctx, addr)
{
  for (int i = 0 ; i < MAX_STREAMS ; i++)
    consoleoutput [i] = TRUE ;
  CollectStats = false ;
  StatLine = 0 ;
  OpenStreams (false) ;

  InstallFront (kPOVMsgClass_RenderOutput, kPOVMsgIdent_RenderStarted, this, &WinRenderFrontend::RenderStarted) ;
  InstallBack (kPOVMsgClass_RenderOutput, kPOVMsgIdent_RenderDone, this, &WinRenderFrontend::RenderFinished) ;
  InstallBack (kPOVMsgClass_RenderOutput, kPOVMsgIdent_Progress, this, &WinRenderFrontend::RenderProgress) ;
  InstallFront (kPOVMsgClass_RenderOutput, kPOVMsgIdent_RenderOptions, this, &WinRenderFrontend::RenderOptions) ;
  InstallFront (kPOVMsgClass_RenderOutput, kPOVMsgIdent_RenderStatistics, this, &WinRenderFrontend::RenderStatistics) ;
  InstallBack (kPOVMsgClass_RenderOutput, kPOVMsgIdent_FatalError, this, &WinRenderFrontend::FatalError) ;
  InstallBack (kPOVMsgClass_Miscellaneous, kPOVMsgIdent_InitInfo, this,  &WinRenderFrontend::InitInfo) ;
}

WinRenderFrontend::~WinRenderFrontend()
{
  rendering = false ;
  CloseStreams () ;
  CollectStats = false ;
}

void WinRenderFrontend::RenderStarted (POVMS_Message& msg, POVMS_Message&, int)
{
  CollectStats = false ;
}

void WinRenderFrontend::RenderFinished (POVMS_Message& msg, POVMS_Message&, int)
{
  FlushStreams () ;
  render_stopped () ;
  CloseStreams () ;
  CollectStats = false ;
  for (int i = 0 ; i < MAX_STREAMS ; i++)
    consoleoutput [i] = TRUE ;
}

void WinRenderFrontend::RenderStatistics (POVMSObjectPtr msg, POVMSObjectPtr, int)
{
  if (!running_demo && !demo_mode)
    CollectStats = true ;
  StatLine = 0 ;
}

void WinRenderFrontend::InitInfo (POVMSObjectPtr msg, POVMSObjectPtr, int)
{
  buffer_message (mDivider, "\n") ;
}

void WinRenderFrontend::RenderProgress (POVMSObjectPtr msg, POVMSObjectPtr, int)
{
  int         ret = kNoErr ;
  int         n = 0 ;
  int         l = 0 ;
  int         s = 0 ;
  char        str [512] ;
  char        *p = str ;
  POVMSLong   ll = 0 ;

  if (no_status_output)
    return ;

  if (delay_next_status)
    return ;

  // animation frame progress
  if (POVMSUtil_GetInt (msg, kPOVAttrib_FrameCount, &l) == kNoErr)
  {
    if (POVMSUtil_GetInt (msg, kPOVAttrib_AbsoluteCurFrame, &s) == kNoErr)
    {
      status_printf (StatusMessage, "Rendering frame %d of %d", s, l) ;
      delay_next_status = 1000 ;
    }
  }
  // parsing progress
  else if ((POVMSUtil_GetLong (msg, kPOVAttrib_CurrentToken, &ll) == kNoErr) && (ll > 0))
  {
    status_printf (StatusMessage, "Parsed %I64u tokens", ll) ;
  }
  // rendering progress
  else if (POVMSUtil_GetInt (msg, kPOVAttrib_CurrentLine, &l) == kNoErr)
  {
    if (POVMSUtil_GetInt (msg, kPOVAttrib_LineCount, &s) == kNoErr)
    {
      if (POVMSUtil_GetInt(msg, kPOVAttrib_MosaicPreviewSize, &n) == kNoErr)
        p += sprintf (str, "Pre-rendering line %d of %d (mosaic %dx%d)", l, s, n, n) ;
      else
        p += sprintf (str, "Rendering line %d of %d", l, s) ;
      if (POVMSUtil_GetInt (msg, kPOVAttrib_SuperSampleCount, &l) == kNoErr)
        p += sprintf (p, " (%d supersamples)", l) ;
      if (POVMSUtil_GetInt (msg, kPOVAttrib_RadGatherCount, &l) == kNoErr)
      {
        if (*(p - 1) == ')')
          p += sprintf (--p, ", %d radiosity samples)", l) ;
        else
          p += sprintf (p, " (%d radiosity samples)", l) ;
      }
      if (seconds_for_last_line > 9)
        sprintf (p, " [last line %d PPS]", render_width / seconds_for_last_line) ;
      say_status_message (StatusMessage, str) ;
    }
  }
  // photon progress
  else if (POVMSUtil_GetInt (msg, kPOVAttrib_TotalPhotonCount, &n) == kNoErr)
  {
    // sorting
    if (POVMSUtil_GetInt (msg, kPOVAttrib_CurrentPhotonCount, &s) == kNoErr)
      status_printf (StatusMessage, "Sorting photon %d of %d", s, n) ;
    // shooting
    else
    {
      l = s = 0 ;
      POVMSUtil_GetInt (msg, kPOVAttrib_PhotonXSamples, &l) ;
      POVMSUtil_GetInt (msg, kPOVAttrib_PhotonYSamples, &s) ;
      status_printf (StatusMessage, "Photons %d (sampling %dx%d)", n, s, l) ;
    }
  }
}

void WinRenderFrontend::RenderOptions (POVMSObjectPtr msg, POVMSObjectPtr, int)
{
  int                   l ;
  char                  dir [_MAX_PATH] ;
  char                  str [_MAX_PATH] ;
  char                  path [_MAX_PATH] ;
  char                  file [_MAX_PATH] ;
  POVMSBool             b ;
  ProcessRenderOptions  options ;

  if (!running_demo && !demo_mode)
  {
    WritePrivateProfileString ("LastRender", "SceneFile", "", EngineIniFileName) ;
    WritePrivateProfileString ("LastRender", "OutputFile", "", EngineIniFileName) ;
    WritePrivateProfileString ("LastRender", "HistogramFile", "", EngineIniFileName) ;
    WritePrivateProfileString ("LastRender", "IniOutputFile", "", EngineIniFileName) ;
  }

  GetCurrentDirectory (sizeof (dir), dir) ;
  WritePrivateProfileString ("LastRender", "CurrentDirectory", dir, EngineIniFileName) ;
  str [0] = '\0' ;
  l = sizeof (str) ;
  if (POVMSUtil_GetString (msg, kPOVAttrib_InputFile, str, &l) == kNoErr)
  {
    splitpath (str, path, file) ;
    if (!running_demo && !demo_mode)
      WritePrivateProfileString ("LastRender", "SourceFile", get_full_name (file), EngineIniFileName) ;
    sprintf (dir, "POV-Ray - parsing '%s'", file) ;
    SetWindowText (main_window, dir) ;
    splitfn (str, NULL, file, NULL) ;
    if (!running_demo && !demo_mode)
      WritePrivateProfileString ("LastRender", "SceneFile", file, EngineIniFileName) ;
  }
  if (POVMSUtil_GetBool (msg, kPOVAttrib_OutputToFile, &b) == kNoErr && b == true)
  {
    str [0] = '\0' ;
    if (POVMSUtil_GetString (msg, kPOVAttrib_OutputFile, str, &l) == kNoErr)
    {
      splitpath (str, path, NULL) ;
      if (path [0] == '\0')
      {
        l = sizeof (path) ;
        if (POVMSUtil_GetString (msg, kPOVAttrib_OutputPath, path, &l) != kNoErr)
          path [0] = '\0' ;
        if (path [0] != '\0')
        {
          // path is expected to end with a '\'
          strcpy (output_file_name, path) ;
          strcat (output_file_name, str) ;
        }
        else
          strcpy (output_file_name, get_full_name (str)) ;
      }
      else
        strcpy (output_file_name, str) ;
      if (!running_demo && !demo_mode)
        WritePrivateProfileString ("LastRender", "OutputFile", output_file_name, EngineIniFileName) ;
      output_to_file = str [0] != '\0' ;
    }
  }
  else
    output_to_file = false ;

  if (!running_demo && !demo_mode)
  {
    if (POVMSUtil_GetBool (msg, kPOVAttrib_CreateHistogram, &b) == kNoErr && b == true)
    {
      str [0] = '\0' ;
      l = sizeof (str) ;
      if (POVMSUtil_GetString (msg, kPOVAttrib_CreateHistogram, str, &l) == kNoErr)
        WritePrivateProfileString ("LastRender", "HistogramFile", get_full_name (str), EngineIniFileName) ;
    }

    str [0] = '\0' ;
    l = sizeof (str) ;
    if (POVMSUtil_GetString (msg, kPOVAttrib_CreateIni, str, &l) == kNoErr && str [0] != '\0')
      WritePrivateProfileString ("LastRender", "IniOutputFile", get_full_name (str), EngineIniFileName) ;

    write_rerun_information (msg, &options) ;
    PutPrivateProfileInt ("Statistics", "StartRender", time (NULL), CurrentRerunFileName) ;

    str [0] = '\0' ;
    l = sizeof (str) ;
    if (POVMSUtil_GetString (msg, kPOVAttrib_CreateIni, str, &l) == kNoErr && str [0] != '\0')
      options.WriteFile (str, msg) ;
  }
}

void WinRenderFrontend::FatalError (POVMSObjectPtr msg, POVMSObjectPtr, int)
{
  int l = sizeof (ErrorFilename) ;
  if (POVMSUtil_GetString (msg, kPOVAttrib_FileName, ErrorFilename, &l) == kNoErr)
  {
    POVMSUtil_GetInt (msg, kPOVAttrib_Line, (int *) &ErrorLine) ;
    POVMSUtil_GetInt (msg, kPOVAttrib_Column, (int *) &ErrorCol) ;
  }
  l = sizeof (ErrorMessage) ;
  POVMSUtil_GetString (msg, kPOVAttrib_EnglishText, ErrorMessage, &l) ;
}

void WinRenderFrontend::OpenStreams (bool append)
{
  int         i ;
  char        *stype ;
  OTextStream *os ;
  msgtype     mtype ;

  for (i = 0 ; i < MAX_STREAMS ; i++)
  {
    os = NULL ;
    switch (i)
    {
      case BANNER_STREAM :
           mtype = mBanner ;
           stype = "banner" ;
           break ;

      case STATUS_STREAM :
           mtype = mStatus ;
           stype = "status" ;
           break ;

      case DEBUG_STREAM :
           mtype = mDebug ;
           stype = "debug" ;
           break ;

      case FATAL_STREAM :
           mtype = mFatal ;
           stype = "fatal" ;
           break ;

      case RENDER_STREAM :
           mtype = mRender ;
           stype = "render" ;
           break ;

      case STATISTIC_STREAM :
           mtype = mStatistics ;
           stype = "statistics" ;
           break ;

      case WARNING_STREAM :
           mtype = mWarning ;
           stype = "warning" ;
           break ;

      case ALL_STREAM :
           mtype = mAll ;
           stype = "'all'" ;
           break ;

      default :
           mtype = mUnknown ;
           stype = "'unknown'" ;
           break ;
    }

    if (streams [i] != NULL)
    {
      delete streams [i] ;
      streams [i] = NULL ;
    }

    // have to test this this since a previous iteration could have cancelled the render
    // and we don't want to bug the user with extra file permission requests if they've
    // already said no.
    if (!stop_rendering)
    {
      if (streamnames [i] != NULL)
      {
        if (WIN_Allow_File_Write (streamnames [i], POV_File_Text_Stream))
        {
          os = new OTextStream (streamnames [i], POV_File_Text_Stream, append) ;
          if (os == NULL)
            message_printf ("Could not %s %s stream to file %s\n", append ? "append" : "write", stype, streamnames [i]) ;
        }
        else
          cancel_render () ;
      }
    }
    streams [i] = new WinStreamBuffer (os, mtype, (i == ALL_STREAM), ((i == ALL_STREAM) || !consoleoutput [i]) && (i != STATUS_STREAM)) ;
  }
}

void WinRenderFrontend::CloseStreams (void)
{
  if (rendering)
    return ;
  for (int i = 0 ; i < MAX_STREAMS ; i++)
  {
    if (streams[i] != NULL)
      delete streams [i] ;
    streams [i] = NULL ;
    if (streamnames [i] != NULL)
    {
      delete[] streamnames [i] ;
      streamnames [i] = NULL ;
    }
  }
}

void WinRenderFrontend::FlushStreams (void)
{
  for (int i = 0 ; i < MAX_STREAMS ; i++)
    Flush (i) ;
}

void WinRenderFrontend::PrintToStatisticsStream (char *format, ...)
{
  char        localvsbuffer [1024] ;
  va_list     marker ;

  va_start (marker, format);
  vsnprintf (localvsbuffer, 1023, format, marker);
  va_end (marker) ;

  if (streams [STATISTIC_STREAM] != NULL)
    streams [STATISTIC_STREAM]->print (localvsbuffer) ;
  if (streams [ALL_STREAM] != NULL)
    streams [ALL_STREAM]->print (localvsbuffer) ;
}

WinProcessRenderOptions::WinProcessRenderOptions() : ProcessRenderOptions()
{
}

WinProcessRenderOptions::~WinProcessRenderOptions()
{
}

void WinProcessRenderOptions::ParseError(const char *format, ...)
{
  va_list marker;
  char error_buffer[1024];

  va_start(marker, format);
  vsnprintf(error_buffer, 1023, format, marker);
  va_end(marker);

  buffer_message (mFatal, error_buffer) ;
  buffer_message (mFatal, "\n") ;
  status_printf (StatusMessage, error_buffer) ;
}

void WinProcessRenderOptions::ParseErrorAt(ITextStream *file, const char *format, ...)
{
  va_list marker;
  char error_buffer[1024];

  va_start(marker, format);
  vsnprintf(error_buffer, 1023, format, marker);
  va_end(marker);

  buffer_message (mFatal, error_buffer) ;
  buffer_message (mFatal, "\n") ;
  strcpy (ErrorFilename, file->name()) ;
  ErrorLine = file->line() ;
  ErrorCol = 0 ;
  sprintf (ErrorMessage, "Error at line %d of file %s", ErrorLine, ErrorFilename) ;
}

void WinProcessRenderOptions::WriteError(const char *format, ...)
{
  va_list marker;
  char error_buffer[1024];

  va_start(marker, format);
  vsnprintf(error_buffer, 1023, format, marker);
  va_end(marker);

  buffer_message (mFatal, error_buffer) ;
  buffer_message (mFatal, "\n") ;
  status_printf (StatusMessage, error_buffer) ;
}

WinRenderFrontend::WinStreamBuffer::WinStreamBuffer (OTextStream *h, msgtype mtype, bool l, bool i) : TextStreamBuffer (1024*8, 128)
{
  handle = h ;
  type = mtype ;
  linebuffermode = l ;
  inhibitmode = i ;
  raw_buffer [0] = '\0' ;
}

WinRenderFrontend::WinStreamBuffer::~WinStreamBuffer ()
{
  if (handle != NULL)
    delete handle ;
}

void WinRenderFrontend::WinStreamBuffer::lineoutput (const char *str, unsigned int chars)
{
  char        buffer [1024] ;
  char        ln [32] ;

  if (type == mStatistics && CollectStats)
  {
    if (strcmp (str, "Render Statistics\n") != 0)
    {
      if (StatLine == 0)
      {
        StatLine = 1 ;
        sprintf (buffer, "Statistics for %s", output_file_name) ;
        WritePrivateProfileString ("Statistics", "StatLn00", buffer, CurrentRerunFileName) ;
      }
      if (strcmp (str, "Total Scene Processing Times\n") == 0 || strncmp (str, "CPU time used:", 14) == 0)
      {
        sprintf (ln, "StatLn%02d", StatLine++) ;
        WritePrivateProfileString ("Statistics", ln, "----------------------------------------------------------------------------", CurrentRerunFileName) ;
      }
      sprintf (ln, "StatLn%02d", StatLine++) ;
      strcpy (buffer, str) ;
      buffer [strlen (str) - 1] = '\0' ;
      WritePrivateProfileString ("Statistics", ln, buffer, CurrentRerunFileName) ;
    }
  }

  if (handle != NULL && type != mDebug)
    handle->printf ("%.*s\n", chars, str) ;
}

void WinRenderFrontend::WinStreamBuffer::directoutput (const char *str, unsigned int chars)
{
}

void WinRenderFrontend::WinStreamBuffer::rawoutput (const char *str, unsigned int chars)
{
  char        *s ;

  if (type == mDebug && handle != NULL)
    handle->printf ("%.*s", chars, str) ;

  if (type == mBanner || type == mFatal || type == mRender || type == mWarning || type == mStatistics || type == mDebug)
  {
    if (inhibitmode && type != mFatal && rendering)
      return ;
    if (!no_status_output || type == mFatal)
    {
      if (chars == 1 && *str == '\n')
      {
        buffer_stream_message (type, raw_buffer) ;
        raw_buffer [0] = '\0' ;
        return ;
      }
      // just drop the string entirely if it won't fit in (rather than truncating it).
      if (strlen (raw_buffer) + chars < sizeof (raw_buffer) - 4)
      {
        strncat (raw_buffer, str, chars) ;
        if ((s = strrchr (raw_buffer, '\n')) != NULL)
        {
          *s++ = '\0' ;
          buffer_stream_message (type, raw_buffer) ;
          strcpy (raw_buffer, s) ;
        }
      }
    }
  }
}

END_POV_FRONTEND_NAMESPACE
