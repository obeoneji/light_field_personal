/****************************************************************************
 *                pvmisc.cpp
 *
 * This module implements miscellaneous routines for the Windows build of POV.
 *
 * Author: Christopher J. Cason.
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
 * $File: //depot/povray/3.5/windows/pvmisc.cpp $
 * $Revision: #24 $
 * $Change: 4803 $
 * $DateTime: 2009/05/31 05:31:25 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#define WIN32_LEAN_AND_MEAN

#ifdef _WIN64
#pragma pack(16)
#include <windows.h>
#include <htmlhelp.h>
#include <shellapi.h>
#pragma pack()
#else
#include <windows.h>
#include <htmlhelp.h>
#include <shellapi.h>
#endif

#include <math.h>
#include <setjmp.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <commctrl.h>
#include <ctype.h>
#include <sys\stat.h>
#include "frame.h"
#include "colour.h"
#include "povray.h"
#include "pov_util.h"
#include "optout.h"
#include "pvengine.h"
#include "pvedit.h"
#include "resource.h"
#include "pvguiext.h"
#include "povmsend.h"

#include "defaultplatformbase.h"
#include "pvfrontend.h"

USING_POV_NAMESPACE
USING_POV_FRONTEND_NAMESPACE

extern int                                  alert_sound ;
extern int                                  message_xchar ;
extern int                                  message_ychar ;
extern int                                  renderwin_left ;
extern int                                  renderwin_top ;
extern int                                  io_restrictions ;
extern int                                  bandWidths [8] ;
extern int                                  screen_origin_x ;
extern int                                  screen_origin_y ;
extern int                                  virtual_screen_width ;
extern int                                  virtual_screen_height ;
extern char                                 message_font_name [256] ;
extern char                                 ourPath [_MAX_PATH] ;
extern char                                 DocumentsPath [_MAX_PATH] ;
extern char                                 BinariesPath [_MAX_PATH] ;
extern char                                 EngineIniFileName [_MAX_PATH] ;
extern char                                 RerunIniPath [_MAX_PATH] ;
extern char                                 CurrentRerunFileName [_MAX_PATH] ;
extern char                                 helpPath [_MAX_PATH] ;
extern char                                 lastRenderName [_MAX_PATH] ;
extern char                                 lastBitmapName [_MAX_PATH] ;
extern char                                 lastRenderPath [_MAX_PATH] ;
extern char                                 lastBitmapPath [_MAX_PATH] ;
extern char                                 lastQueuePath [_MAX_PATH] ;
extern char                                 lastSecondaryIniFilePath [_MAX_PATH] ;
extern char                                 SecondaryRenderIniFileName [_MAX_PATH] ;
extern char                                 SecondaryRenderIniFileSection [64] ;
extern char                                 background_file [_MAX_PATH ] ;
extern char                                 tool_commands [MAX_TOOLCMD] [MAX_TOOLCMDTEXT] ;
extern char                                 tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
extern char                                 source_file_name [_MAX_PATH] ;
extern char                                 ToolIniFileName [_MAX_PATH] ;
extern char                                 command_line [_MAX_PATH * 3] ;
extern char                                 queued_files [MAX_QUEUE] [_MAX_PATH] ;
extern char                                 RegionStr [] ;
extern char                                 EngineIniDir [] ;
extern char                                 engineHelpPath [_MAX_PATH] ;
extern char                                 render_complete_sound [_MAX_PATH] ;
extern char                                 parse_error_sound [_MAX_PATH] ;
extern char                                 render_error_sound [_MAX_PATH] ;
extern char                                 FontPath [_MAX_PATH] ;
extern bool                                 render_complete_sound_enabled ;
extern bool                                 parse_error_sound_enabled  ;
extern bool                                 render_error_sound_enabled ;
extern void                                 *CurrentEditor ;
extern unsigned                             message_font_size ;
extern unsigned                             message_font_weight ;
extern unsigned                             screen_width ;
extern unsigned                             screen_height ;
extern unsigned                             screen_depth ;
extern unsigned                             renderwin_8bits ;
extern unsigned                             auto_render ;
extern unsigned                             queued_file_count ;
extern unsigned                             renderwin_flags ;
extern unsigned                             render_priority ;
extern unsigned                             Duty_Cycle ;
extern unsigned                             on_completion ;
extern unsigned                             window_count ;
extern bool                                 keep_messages ;
extern bool                                 alert_on_completion ;
extern bool                                 save_settings ;
extern bool                                 running_demo ;
extern bool                                 fast_scroll ;
extern bool                                 renderwin_active ;
extern bool                                 renderwin_destroyed ;
extern bool                                 no_shellout_wait ;
extern bool                                 tile_background ;
extern bool                                 debugging ;
extern bool                                 no_palette_warn ;
extern bool                                 hide_render_window ;
extern bool                                 render_above_main ;
extern bool                                 IsW95UserInterface ;
extern bool                                 tips_enabled ;
extern bool                                 use_16bit_editor ;
extern bool                                 system_noactive ;
extern bool                                 IsWin32 ;
extern bool                                 one_instance ;
extern bool                                 use_toolbar ;
extern bool                                 use_tooltips ;
extern bool                                 use_editors ;
extern bool                                 expert_menus ;
extern bool                                 drop_to_editor ;
extern bool                                 render_auto_close ;
extern bool                                 ExtensionsEnabled ;
extern bool                                 use_taskbar ;
extern bool                                 use_renderanim ;
extern bool                                 allow_rw_source ;
extern bool                                 no_shell_outs ;
extern bool                                 hide_newuser_help ;
extern bool                                 IsW98 ;
extern bool                                 IsW2k ;
extern bool                                 preserve_bitmap ;
extern bool                                 check_new_version ;
extern bool                                 check_news ;
extern bool                                 send_system_info ;
extern bool                                 homeInferred ;
extern bool                                 AutoAppendPaths ;
extern HWND                                 render_window ;
extern HWND                                 main_window ;
extern HWND                                 toolbar_window ;
extern HWND                                 toolbar_cmdline ;
extern HWND                                 toolbar_combobox ;
extern HWND                                 rebar_window ;
extern FILE                                 *debugFile ;
extern HMENU                                hMainMenu ;
extern HMENU                                hRerunMenu ;
extern HMENU                                hToolsMenu ;
extern COLORREF                             background_colour ;
extern COLORREF                             text_colour ;
extern HINSTANCE                            hInstance ;
extern HH_AKLINK                            hh_aklink ;
extern WINDOWPLACEMENT                      mainwin_placement ;
extern CRITICAL_SECTION                     critical_section ;

#define MAX_DIRSPEC 70
char *WriteDirSpecs [MAX_DIRSPEC] ;
char *ReadDirSpecs [MAX_DIRSPEC] ;

typedef struct
{
  unsigned    id ;
  bool        *varptr ;
  bool        rval ;
  bool        autowrite ;
  char        *section ;
  char        *entry ;
  bool        defval ;
} toggle_struct ;

toggle_struct toggles [] =
{
// rval should be true if no special processing is required within handle_main_command
//  ID                       VarPtr                         RVal AutoWrite  Section        Name                       DefVal
  { CM_SAVE_SETTINGS,       (bool *) &save_settings,        false, true,  "General",       "SaveSettingsOnExit"      , true  },
  { CM_PRESERVEMESSAGES,    (bool *) &keep_messages,        true , true,  "Messages",      "KeepMessages"            , true  },
  { CM_FORCE8BITS,          (bool *) &renderwin_8bits,      false, true,  "RenderWindow",  "Use8BitMode"             , false },
  { CM_RENDERACTIVE,        (bool *) &renderwin_active,     true , true,  "RenderWindow",  "MakeActive"              , true  },
  { CM_RENDERABOVEMAIN,     (bool *) &render_above_main,    false, true,  "RenderWindow",  "KeepAboveMain"           , true  },
  { CM_RENDERHIDE,          (bool *) &hide_render_window,   true , true,  "RenderWindow",  "HideWhenMainMinimized"   , true  },
  { CM_ALERT,               (bool *) &alert_on_completion,  true , true,  "Renderer",      "AlertOnCompletion"       , true  },
  { CM_AUTORENDER,          (bool *) &auto_render,          true , true,  "Renderer",      "AutoRender"              , true  },
  { CM_SHELLOUTWAIT,        (bool *) &no_shellout_wait,     true , true,  "Renderer",      "NoShelloutWait"          , false },
  { CM_TIPOFTHEDAY,         (bool *) &tips_enabled,         true,  true,  "TipOfTheDay",   "Enabled"                 , true  },
  { CM_TILEDBACKGROUND,     (bool *) &tile_background,      false, true,  "General",       "TileBackground"          , true  },
  { CM_SYSTEMNOACTIVE,      (bool *) &system_noactive,      true,  true,  "Renderer",      "SystemNoActive"          , false },
  { CM_SINGLEINSTANCE,      (bool *) &one_instance,         false, false, "General",       "OneInstance"             , true  },
  { CM_USETOOLBAR,          (bool *) &use_toolbar,          false, true,  "MainWindow",    "UseToolbar"              , true  },
  { CM_USETOOLTIPS,         (bool *) &use_tooltips,         true,  true,  "MainWindow",    "UseTooltips"             , true  },
  { CM_RENDERAUTOCLOSE,     (bool *) &render_auto_close,    true,  true,  "RenderWindow",  "AutoClose"               , false },
  { CM_USEEXTENSIONS,       (bool *) &ExtensionsEnabled,    true,  true,  "GUIExtensions", "UseExtensions"           , true  },
  { CM_USERENDERANIM,       (bool *) &use_renderanim,       true,  true,  "MainWindow",    "UseRenderAnim"           , false },
  { CM_RW_SOURCE,           (bool *) &allow_rw_source,      true,  true,  "Scripting",     "ReadWriteSourceDir"      , true  },
  { CM_NO_SHELLOUTS,        (bool *) &no_shell_outs,        true,  true,  "Scripting",     "NoShellOuts"             , true  },
  { CM_HIDENEWUSERHELP,     (bool *) &hide_newuser_help,    false, true,  "General",       "HideNewUserHelp"         , false },
  { CM_PRESERVERENDERBITMAP,(bool *) &preserve_bitmap,      true,  true,  "RenderWindow",  "PreserveBitmap"          , true  },
#ifdef _WIN64
  { CM_CHECKNEWVERSION,     (bool *) &check_new_version,    true,  true,  "General",       "CheckNewVersion"         , false },
  { CM_CHECKNEWS,           (bool *) &check_news,           true,  true,  "General",       "CheckNews"               , false },
  { CM_SENDSYSTEMINFO,      (bool *) &send_system_info,     true,  true,  "General",       "SendSystemInfo"          , false },
#else
  { CM_CHECKNEWVERSION,     (bool *) &check_new_version,    true,  true,  "General",       "CheckNewVersion"         , true  },
  { CM_CHECKNEWS,           (bool *) &check_news,           true,  true,  "General",       "CheckNews"               , true  },
  { CM_SENDSYSTEMINFO,      (bool *) &send_system_info,     true,  true,  "General",       "SendSystemInfo"          , true  },
#endif
  { -1,                     (bool *) NULL,                  false, true,  "",              ""                        , false }
} ;

bool PutPrivateProfileInt (LPCSTR lpszSection, LPCSTR lpszEntry, UINT uiValue, LPCSTR lpszFilename)
{
  char        str [16] ;

  sprintf (str, "%d", uiValue) ;
  return (WritePrivateProfileString (lpszSection, lpszEntry, str, lpszFilename) != 0) ;
}

bool GetDontShowAgain (char *Name, char *lpszFilename)
{
  return (GetPrivateProfileInt ("DontShowAgain", Name, 0, lpszFilename) != 0) ;
}

void PutDontShowAgain (char *Name, bool dontShow, char *lpszFilename)
{
  PutPrivateProfileInt ("DontShowAgain", Name, dontShow ? 1 : 0, lpszFilename) ;
}

bool process_toggles (WPARAM wParam)
{
  toggle_struct         *t ;

  for (t = toggles ; t->id != -1 ; t++)
  {
    if (t->id == LOWORD (wParam))
    {
      *t->varptr = !*t->varptr ;
      PVCheckMenuItem (t->id, *t->varptr ? MF_CHECKED : MF_UNCHECKED) ;
      return (t->rval) ;
    }
  }
  return (false) ;
}

void set_toggles (void)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    PVCheckMenuItem (t->id, *t->varptr ? MF_CHECKED : MF_UNCHECKED) ;
}

void read_toggles (char *iniFilename)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    *t->varptr = (GetPrivateProfileInt (t->section, t->entry, t->defval, iniFilename) != 0);
}

void write_toggles (char *iniFilename)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    if (t->autowrite)
      PutPrivateProfileInt (t->section, t->entry, *t->varptr, iniFilename) ;
}

bool fileExists (char *filename)
{
  struct stat statBuf ;

  if (stat (filename, &statBuf) != 0 || !S_ISREG (statBuf.st_mode))
    return (false) ;
  return (true) ;
}

bool dirExists (char *filename)
{
  struct stat statBuf ;

  if (stat (filename, &statBuf) != 0 || !S_ISDIR (statBuf.st_mode))
    return (false) ;
  return (true) ;
}

void read_dir_restriction (char *iniSection, char **store, char *iniFilename)
{
  int         i ;
  char        EntryName [16] ;
  char        str1 [_MAX_PATH] ;
  char        str2 [_MAX_PATH] ;
  char        *s ;

  // we can afford to be generous with respect to errors here - if an entry
  // doesn't make it into the list, it won't reduce security, since the system
  // defaults to denying access.
  for (i = 0 ; i < MAX_DIRSPEC - 1 ; i++)
  {
    sprintf (EntryName, "%d", i) ;
    GetPrivateProfileString (iniSection, EntryName, "", str1, sizeof (str1), iniFilename) ;
    if (str1 [0] == '\0')
      continue ;
    if (strnicmp (str1, "%INSTALLDIR%", 12) == 0)
    {
      strcpy (str2, str1) ;
      strcpy (str1, BinariesPath) ;
      if (str1 [strlen (str1) - 1] == '\\')
        str1 [strlen (str1) - 1] = '\0' ;
      strcat (str1, str2 + 12) ;
    }
    else if (_strnicmp (str1, "%PROFILEDIR%", 12) == 0)
    {
      strcpy (str2, str1) ;
      strcpy (str1, DocumentsPath) ;
      if (str1 [strlen (str1) - 1] == '\\')
        str1 [strlen (str1) - 1] = '\0' ;
      strcat (str1, str2 + 12) ;
    }
    else if (strnicmp (str1, "%FONTDIR%", 9) == 0)
    {
      strcpy (str2, str1 + 9) ;
      strcpy (str1, FontPath) ;
      if (str1 [strlen (str1) - 1] == '\\')
        str1 [strlen (str1) - 1] = '\0' ;
      strcat (str1, str2) ;
    }
    if (str1 [strlen (str1) - 1] != '\\')
      strcat (str1, "\\") ;
    if (GetFullPathName (str1, sizeof (str2), str2, &s) == 0)
      continue ;
    strupr (str2) ;
    if ((s = (char *) malloc (strlen (str2) + 1)) == NULL)
      continue ;
    strcpy (s, str2) ;
    *store++ = s ;
  }
}

void read_dir_restrictions (char *iniFilename)
{
  io_restrictions = GetPrivateProfileInt ("Scripting", "IO Restrictions", 2, iniFilename) ;
  PVCheckMenuRadioItem (CM_IO_NO_RESTRICTIONS, CM_IO_RESTRICT_READWRITE, io_restrictions + CM_IO_NO_RESTRICTIONS) ;
  read_dir_restriction ("Permitted Input Paths", ReadDirSpecs, iniFilename) ;
  read_dir_restriction ("Permitted Output Paths", WriteDirSpecs, iniFilename) ;
}

void read_INI_settings (char *iniFilename)
{
  char        str [_MAX_PATH] ;

  mainwin_placement.showCmd = GetPrivateProfileInt ("MainWindow", "ShowCmd", SW_SHOWNORMAL, iniFilename) ;
  mainwin_placement.ptMinPosition.x = GetPrivateProfileInt ("MainWindow", "MinPositionX", -1, iniFilename) ;
  mainwin_placement.ptMinPosition.y = GetPrivateProfileInt ("MainWindow", "MinPositionY", -1, iniFilename) ;
  mainwin_placement.ptMaxPosition.x = GetPrivateProfileInt ("MainWindow", "MaxPositionX", -1, iniFilename) ;
  mainwin_placement.ptMaxPosition.y = GetPrivateProfileInt ("MainWindow", "MaxPositionY", -1, iniFilename) ;
  mainwin_placement.rcNormalPosition.left = GetPrivateProfileInt ("MainWindow", "NormalPositionLeft", 128, iniFilename) ;
  mainwin_placement.rcNormalPosition.top = GetPrivateProfileInt ("MainWindow", "NormalPositionTop", 128, iniFilename) ;
  mainwin_placement.rcNormalPosition.right = GetPrivateProfileInt ("MainWindow", "NormalPositionRight", -1, iniFilename) ;
  mainwin_placement.rcNormalPosition.bottom = GetPrivateProfileInt ("MainWindow", "NormalPositionBottom", -1, iniFilename) ;
  renderwin_left = GetPrivateProfileInt ("RenderWindow", "NormalPositionX", 256, iniFilename) ;
  renderwin_top = GetPrivateProfileInt ("RenderWindow", "NormalPositionY", 256, iniFilename) ;
  renderwin_flags = GetPrivateProfileInt ("RenderWindow", "Flags", 0, iniFilename) ;
  GetPrivateProfileString ("Messages", "Font", "Lucida Console", message_font_name, sizeof (message_font_name), iniFilename) ;
  message_font_size = GetPrivateProfileInt ("Messages", "FontSize", 8, iniFilename) ;
  message_font_weight = GetPrivateProfileInt ("Messages", "FontWeight", FW_NORMAL, iniFilename) ;
  fast_scroll = (GetPrivateProfileInt ("Messages", "FastScroll", false, iniFilename) != 0);
  alert_sound = GetPrivateProfileInt ("Renderer", "AlertSound", MB_ICONASTERISK, iniFilename) ;
  render_priority = GetPrivateProfileInt ("Renderer", "Priority", CM_RENDERPRIORITY_NORMAL, iniFilename) ;
  Duty_Cycle = GetPrivateProfileInt ("Renderer", "DutyCycle", 9, iniFilename) ;
  on_completion = GetPrivateProfileInt ("Renderer", "Completion", CM_COMPLETION_NOTHING, iniFilename) ;
  no_palette_warn = (GetPrivateProfileInt ("General", "NoPaletteWarn", false, iniFilename) != 0);
  GetPrivateProfileString ("General", "LastRenderName", "", lastRenderName, sizeof (lastRenderName), iniFilename) ;
  GetPrivateProfileString ("General", "LastRenderPath", "", lastRenderPath, sizeof (lastRenderPath), iniFilename) ;
  GetPrivateProfileString ("General", "LastQueuePath", "", lastQueuePath, sizeof (lastQueuePath), iniFilename) ;
  GetPrivateProfileString ("General", "LastBitmapName", "*.bmp", lastBitmapName, sizeof (lastBitmapName), iniFilename) ;
  GetPrivateProfileString ("General", "LastBitmapPath", "", lastBitmapPath, sizeof (lastBitmapPath), iniFilename) ;

  sprintf (str, "%sini", DocumentsPath) ;
  GetPrivateProfileString ("General", "LastINIPath", str, lastSecondaryIniFilePath, sizeof (lastSecondaryIniFilePath), iniFilename) ;
  if (homeInferred && !dirExists (lastSecondaryIniFilePath))
    strcpy (lastSecondaryIniFilePath, str) ;

  sprintf (str, "%sini\\quickres.ini", DocumentsPath) ;
  GetPrivateProfileString ("General", "SecondaryINIFile", str, SecondaryRenderIniFileName, sizeof (SecondaryRenderIniFileName), iniFilename) ;
  if (homeInferred && !fileExists (SecondaryRenderIniFileName))
    strcpy (SecondaryRenderIniFileName, str) ;

  GetPrivateProfileString ("General", "SecondaryINISection", "[512x384, No AA]", SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection), iniFilename) ;
  GetPrivateProfileString ("General", "BackgroundFile", screen_depth > 8 ? "0" : "1", background_file, sizeof (background_file), iniFilename) ;

  text_colour = GetPrivateProfileInt ("General", "TextColour", RGB (255, 255, 255), iniFilename) ;
  background_colour = GetPrivateProfileInt ("General", "BackgroundColour", GetSysColor (COLOR_BTNFACE), iniFilename) ;
  if (!debugging)
    debugging = (GetPrivateProfileInt ("General", "Debug", 0, iniFilename) != 0);
  drop_to_editor = (GetPrivateProfileInt ("General", "DropToEditor", 1, iniFilename) != 0);
  use_editors = (GetPrivateProfileInt ("General", "UseEditors", 1, iniFilename) != 0);
  for (int i = 0 ; i < 6 ; i++)
  {
    sprintf (str, "Band%dWidth", i) ;
    bandWidths [i] = GetPrivateProfileInt ("ToolBar", str, 0, iniFilename) ;
  }
  read_toggles (iniFilename) ;
  read_dir_restrictions (iniFilename) ;

  sprintf (str, "%ssounds\\Render Finished.wav", BinariesPath) ;
  GetPrivateProfileString ("Sounds", "RenderCompleteSound", str, render_complete_sound, _MAX_PATH, iniFilename) ;
  sprintf (str, "%ssounds\\Parse Error.wav", BinariesPath) ;
  GetPrivateProfileString ("Sounds", "ParseErrorSound", str, parse_error_sound, _MAX_PATH, iniFilename) ;
  sprintf (str, "%ssounds\\Render Cancelled.wav", BinariesPath) ;
  GetPrivateProfileString ("Sounds", "RenderErrorSound", str, render_error_sound, _MAX_PATH, iniFilename) ;
  render_complete_sound_enabled = GetPrivateProfileInt ("Sounds", "RenderCompleteSoundEnabled", TRUE, iniFilename) != 0 ;
  parse_error_sound_enabled = GetPrivateProfileInt ("Sounds", "ParseErrorSoundEnabled", TRUE, iniFilename) != 0 ;
  render_error_sound_enabled = GetPrivateProfileInt ("Sounds", "RenderErrorSoundEnabled", TRUE, iniFilename) != 0 ;

  AutoAppendPaths = GetPrivateProfileInt ("General", "AutoAppendPaths", TRUE, iniFilename) != 0 ;

  // apply the principle of least astonishment: otherwise the render window could
  // turn up on the non-primary monitor, which may be switched off or otherwise not
  // being paid attention to. (technically we should confine it to the same monitor
  // that the app is being started on, but this will do for now).
  if (renderwin_left < 0)
    renderwin_left = 0 ;
  if (renderwin_top < 0)
    renderwin_top = 0 ;
  if (renderwin_left > screen_width - 32)
    renderwin_left = screen_width - 32 ;
  if (renderwin_top > screen_height - 32)
    renderwin_top = screen_height - 32 ;
}

void write_INI_settings (char *iniFilename)
{
  char                  *s ;
  char                  str [32] ;
  REBARBANDINFO         rebarInfo ;

  if (strlen (RegionStr) > 0)
  {
    if ((s = strstr (command_line, RegionStr)) != NULL)
      strcpy (s, s + strlen (RegionStr)) ;
    else if ((s = strstr (command_line, RegionStr + 1)) != NULL)
      strcpy (s, s + strlen (RegionStr) - 1) ;
    SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
  }

  rebarInfo.cbSize = sizeof (REBARBANDINFO) ;
  rebarInfo.fMask = RBBIM_SIZE ;
  for (int i = 0 ; i < 6 ; i++)
  {
    sprintf (str, "Band%dWidth", i) ;
    // under XP64 the SendMessage() call returns TRUE but the cx value is not altered.
    // so we detect that and avoid writing the data unless the problem is fixed.
    rebarInfo.cx = 0x12345678 ;
    if (SendMessage (rebar_window, RB_GETBANDINFO, i, (LPARAM) (LPREBARBANDINFO) &rebarInfo))
      if (rebarInfo.cx != 0x12345678)
        PutPrivateProfileInt ("ToolBar", str, rebarInfo.cx, iniFilename) ;
  }

  PutPrivateProfileInt ("MainWindow", "ShowCmd", mainwin_placement.showCmd, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionLeft", mainwin_placement.rcNormalPosition.left, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionTop", mainwin_placement.rcNormalPosition.top, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionRight", mainwin_placement.rcNormalPosition.right, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionBottom", mainwin_placement.rcNormalPosition.bottom, iniFilename) ;
  PutPrivateProfileInt ("RenderWindow", "NormalPositionX", renderwin_left, iniFilename) ;
  PutPrivateProfileInt ("RenderWindow", "NormalPositionY",  renderwin_top, iniFilename) ;
  PutPrivateProfileInt ("RenderWindow", "Flags",  renderwin_flags, iniFilename) ;
  PutPrivateProfileInt ("Messages", "FontSize", message_font_size, iniFilename) ;
  PutPrivateProfileInt ("Messages", "FontWeight", message_font_weight, iniFilename) ;
//PutPrivateProfileInt ("Messages", "FastScroll", fast_scroll, iniFilename) ;
  PutPrivateProfileInt ("Renderer", "AlertSound", alert_sound, iniFilename) ;
  PutPrivateProfileInt ("Renderer", "Completion", on_completion, iniFilename) ;
  PutPrivateProfileInt ("Renderer", "Priority", render_priority, iniFilename) ;
  PutPrivateProfileInt ("Renderer", "DutyCycle", Duty_Cycle, iniFilename) ;
  PutPrivateProfileInt ("General", "TextColour", text_colour, iniFilename) ;
  PutPrivateProfileInt ("General", "BackgroundColour", background_colour, iniFilename) ;
  PutPrivateProfileInt ("General", "DropToEditor", drop_to_editor, iniFilename) ;
  WritePrivateProfileString ("Messages", "Font", message_font_name, iniFilename) ;
  WritePrivateProfileString ("General", "LastRenderName",  lastRenderName, iniFilename) ;
  WritePrivateProfileString ("General", "LastRenderPath", lastRenderPath, iniFilename) ;
  WritePrivateProfileString ("General", "LastQueuePath", lastQueuePath, iniFilename) ;
  WritePrivateProfileString ("General", "LastBitmapName",  lastBitmapName, iniFilename) ;
  WritePrivateProfileString ("General", "LastBitmapPath", lastBitmapPath, iniFilename) ;
  WritePrivateProfileString ("General", "CommandLine", command_line, iniFilename) ;
  WritePrivateProfileString ("General", "LastINIPath", lastSecondaryIniFilePath, iniFilename) ;
  WritePrivateProfileString ("General", "SecondaryINIFile", SecondaryRenderIniFileName, iniFilename) ;
  WritePrivateProfileString ("General", "SecondaryINISection", SecondaryRenderIniFileSection, iniFilename) ;
  WritePrivateProfileString ("General", "BackgroundFile", background_file, iniFilename) ;
  write_toggles (iniFilename) ;
  WritePrivateProfileString ("General", "Version", POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER, iniFilename) ;
  WritePrivateProfileString ("Sounds", "RenderCompleteSound", render_complete_sound, iniFilename) ;
  WritePrivateProfileString ("Sounds", "ParseErrorSound", parse_error_sound, iniFilename) ;
  WritePrivateProfileString ("Sounds", "RenderErrorSound", render_error_sound, iniFilename) ;
  PutPrivateProfileInt ("Sounds", "RenderCompleteSoundEnabled", render_complete_sound_enabled, iniFilename) ;
  PutPrivateProfileInt ("Sounds", "ParseErrorSoundEnabled", parse_error_sound_enabled, iniFilename) ;
  PutPrivateProfileInt ("Sounds", "RenderErrorSoundEnabled", render_error_sound_enabled, iniFilename) ;
}

void GetRelativeClientRect (HWND hParent, HWND hChild, RECT *rect)
{
  POINT       *points = (POINT *) rect ;

  GetWindowRect (hChild, rect) ;
  ScreenToClient (hParent, points++) ;
  ScreenToClient (hParent, points) ;
}

#define MONITOR_CENTER     0x0001        // center rect to monitor
#define MONITOR_CLIP     0x0000        // clip rect to monitor
#define MONITOR_WORKAREA 0x0002        // use monitor work area
#define MONITOR_AREA     0x0000        // use monitor entire area

#if 0
// modified sample code taken from MSDN
// have to fix this so we dynamically link to the monitor functions,
// otherwise POVWIN won't load on WIn95 systems
void ClipOrCenterRectToMonitor (LPRECT prc, bool center)
{
  int         w = prc->right  - prc->left ;
  int         h = prc->bottom - prc->top ;
  RECT        rc ;
  HMONITOR    hMonitor ;
  MONITORINFO mi ;

  // get the nearest monitor to the passed rect.
  hMonitor = MonitorFromRect (prc, MONITOR_DEFAULTTONEAREST) ;

  // get the work area or entire monitor rect.
  mi.cbSize = sizeof (mi) ;
  GetMonitorInfo (hMonitor, &mi) ;
  rc = mi.rcMonitor;

  // center or clip the passed rect to the monitor rect
  if (center)
  {
    prc->left   = rc.left + (rc.right  - rc.left - w) / 2 ;
    prc->top    = rc.top  + (rc.bottom - rc.top  - h) / 2 ;
    prc->right  = prc->left + w ;
    prc->bottom = prc->top  + h ;
  }
  else
  {
    prc->left   = max(rc.left, min(rc.right-w,  prc->left)) ;
    prc->top    = max(rc.top,  min(rc.bottom-h, prc->top)) ;
    prc->right  = prc->left + w ;
    prc->bottom = prc->top  + h ;
  }
}
#endif

void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, bool bRepaint, bool checkBorders)
{
  int         difference ;
  int         width ;
  int         height ;
  int         x ;
  int         y ;
  int         twidth ;
  int         theight ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  if (hRelativeTo != NULL && IsWindowVisible (hRelativeTo))
  {
    WINDOWPLACEMENT wp ;
    wp.length = sizeof (WINDOWPLACEMENT) ;
    GetWindowPlacement (hRelativeTo, &wp) ;
    if (wp.showCmd == SW_SHOWMINIMIZED)
      hRelativeTo = GetDesktopWindow () ;
  }
  else
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;
  GetWindowRect (hTarget, &targetRect) ;
  width = targetRect.right - targetRect.left ;
  height = targetRect.bottom - targetRect.top ;
  difference = relativeToRect.right - relativeToRect.left - width ;
  x = relativeToRect.left + difference / 2 ;
  difference = relativeToRect.bottom - relativeToRect.top - height ;
  y = relativeToRect.top + difference / 2 ;
  MoveWindow (hTarget, x, y, width, height, bRepaint && !checkBorders) ;
  if (checkBorders)
  {
    GetWindowRect (hTarget, &targetRect) ;
    if (targetRect.left < screen_origin_x)
      OffsetRect (&targetRect, screen_origin_x - targetRect.left, 0) ;
    if (targetRect.top < screen_origin_y)
      OffsetRect (&targetRect, 0, screen_origin_y - targetRect.top) ;
    if (targetRect.right > virtual_screen_width + screen_origin_x)
      OffsetRect (&targetRect, -(targetRect.right - (virtual_screen_width + screen_origin_x)), 0) ;
    if (targetRect.bottom > virtual_screen_height + screen_origin_y)
      OffsetRect (&targetRect, 0, -(targetRect.bottom - (virtual_screen_height + screen_origin_y))) ;
    twidth = targetRect.right - targetRect.left ;
    theight = targetRect.bottom - targetRect.top ;
    MoveWindow (hTarget, targetRect.left, targetRect.top, twidth, theight, bRepaint) ;
  }
}

void FitWindowInWindow (HWND hRelativeTo, HWND hTarget)
{
  int         rwidth ;
  int         rheight ;
  int         twidth ;
  int         theight ;
  int         x ;
  int         y ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  GetWindowRect (hTarget, &targetRect) ;

  if (hRelativeTo == NULL)
  {
    if (targetRect.right > virtual_screen_width + screen_origin_x)
      OffsetRect (&targetRect, -(targetRect.right - (virtual_screen_width + screen_origin_x)), 0) ;
    if (targetRect.bottom > virtual_screen_height + screen_origin_y)
      OffsetRect (&targetRect, 0, -(targetRect.bottom - (virtual_screen_height + screen_origin_y))) ;
    if (targetRect.left < screen_origin_x)
      OffsetRect (&targetRect, screen_origin_x - targetRect.left, 0) ;
    if (targetRect.top < screen_origin_y)
      OffsetRect (&targetRect, 0, screen_origin_y - targetRect.top) ;
    twidth = targetRect.right - targetRect.left ;
    theight = targetRect.bottom - targetRect.top ;
    MoveWindow (hTarget, targetRect.left, targetRect.top, twidth, theight, true) ;
    return ;
  }

  // if window is not visible GetWindowRect() is not reliable.
  if (!IsWindowVisible (hRelativeTo))
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;

  twidth = targetRect.right - targetRect.left ;
  theight = targetRect.bottom - targetRect.top ;
  rwidth = relativeToRect.right - relativeToRect.left ;
  rheight = relativeToRect.bottom - relativeToRect.top ;
  x = targetRect.left ;
  y = targetRect.top ;

  if (twidth > rwidth)
    twidth = rwidth ;
  if (theight > rheight)
    theight = rheight ;
  if (x < relativeToRect.left)
    x = relativeToRect.left ;
  if (y < relativeToRect.top)
    y = relativeToRect.top ;
  if (x + twidth > relativeToRect.right)
    x = relativeToRect.right - twidth ;
  if (y + theight > relativeToRect.bottom)
    y = relativeToRect.bottom - theight ;

  MoveWindow (hTarget, x, y, twidth, theight, true) ;
}

void CenterOffset (HWND win, int id, int offx, int offy, int offw, int offh)
{
  int         x ;
  int         y ;
  int         w ;
  int         h ;
  RECT        R ;
  HWND        dlg = GetDlgItem (win, id) ;
  POINT       P ;

  if (dlg)
  {
    GetWindowRect (dlg, &R) ;

    P.x = R.left ;
    P.y = R.top ;

    ScreenToClient (win, &P) ;

    x = P.x ;
    y = P.y ;
    w = R.right - R.left ;
    h = R.bottom - R.top  ;

    SetWindowPos (dlg, NULL, x + offx, y + offy, w + offw, h + offh, SWP_NOZORDER) ;
  }
}

void SetupExplorerDialog (HWND win)
{
  int         dx ;
  int         dy ;
  RECT        winPos ;

  win = GetParent (win) ;
  GetWindowRect (win, &winPos) ;

  dx = screen_width * 2 / 3 - (winPos.right  - winPos.left) ;
  dy = screen_height * 2 / 3 - (winPos.bottom - winPos.top) ;

  SetWindowPos (win, NULL, 0, 0, screen_width * 2 / 3, screen_height * 2 / 3, SWP_NOZORDER | SWP_NOACTIVATE) ;
  CenterWindowRelative (main_window, win, true) ;
  FitWindowInWindow (NULL, win) ;

  CenterOffset (win, 1091,    0,  0,   0,  0) ;
  CenterOffset (win, 1137,    0,  0,   0,  0) ;
  CenterOffset (win, 1088,    0,  0,   0,  0) ;
  CenterOffset (win, 1120,    0,  0,  dx, dy) ;
  CenterOffset (win, 1090,    0, dy,   0,  0) ;
  CenterOffset (win, 1152,    0, dy,  dx,  0) ;
  CenterOffset (win, 1089,    0, dy,   0,  0) ;
  CenterOffset (win, 1136,    0, dy,  dx,  0) ;
  CenterOffset (win, 1040,    0, dy,   0,  0) ;
  CenterOffset (win,    1, dx  , dy,   0,  0) ;
  CenterOffset (win,    2, dx  , dy,   0,  0) ;
  CenterOffset (win, 1038, dx  , dy,   0,  0) ;
}

int get_file_type (char *filename)
{
  char        ext [_MAX_EXT] ;

  splitfn (filename, NULL, NULL, ext) ;
  strupr (ext) ;

  if (strcmp (ext, ".POV") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".INC") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".INI") == 0)
    return (fileINI) ;
  else if (strcmp (ext, ".PPM") == 0)
    return (filePPM) ;
  else if (strcmp (ext, ".PGM") == 0)
    return (filePGM) ;
  else if (strcmp (ext, ".PBM") == 0)
    return (filePBM) ;
  else if (strcmp (ext, ".PNG") == 0)
    return (filePNG) ;
  else if (strcmp (ext, ".GIF") == 0)
    return (fileGIF) ;
  else if (strcmp (ext, ".BMP") == 0)
    return (fileBMP) ;
  else
    return (fileUnknown) ;
}

char *get_full_name (char *s)
{
  char                  dir [_MAX_PATH + 1] ;
  static char           str [_MAX_PATH] ;

  if (*s == 0)
    return (s) ;
  splitpath (s, str, NULL) ;
  if (str [0] == '\0')
  {
#ifdef TIME_WARP
    // workaround for suspected Win32s bug
    SetCurrentDirectory (".") ;
#endif
    GetCurrentDirectory (sizeof (dir), dir) ;
    joinPath (str, dir, s) ;
    return (str) ;
  }
  return (s) ;
}

char *get_previous_rerun (int id, char *section, char *entry)
{
  char        filename [_MAX_PATH] ;
  static char text [_MAX_PATH] ;

  sprintf (filename, "%sRerun%02d.Ini", RerunIniPath, id) ;
  GetPrivateProfileString (section, entry, "(NULL)", text, sizeof (text), filename) ;
  return (strcmp (text, "(NULL)") == 0 ? NULL : text) ;
}

char *get_current_rerun (char *section, char *entry)
{
  static char text [_MAX_PATH] ;

  GetPrivateProfileString (section, entry, "(NULL)", text, sizeof (text), CurrentRerunFileName) ;
  return (strcmp (text, "(NULL)") == 0 ? NULL : text) ;
}

char *get_current_stats (char *entry)
{
  static char text [_MAX_PATH] ;

  GetPrivateProfileString ("Statistics", entry, "(NULL)", text, sizeof (text), CurrentRerunFileName) ;
  return (strcmp (text, "(NULL)") == 0 ? NULL : text) ;
}

// Note: Windows NT's INI to Registry file mapping must never be enabled on these files.
// POV-Ray allows multiple entries with the same name in the same section. so we access them
// via file I/O rather than the INI file functions.
void write_rerun_information (POVMSObjectPtr msg, ProcessRenderOptions *options)
{
  OStream     *os ;
  time_t      tm ;
  char        dir [_MAX_PATH] ;

  if (running_demo)
    return ;

  // flush the INI file cache
  WritePrivateProfileString (NULL, NULL, NULL, CurrentRerunFileName) ;

  GetCurrentDirectory (sizeof (dir), dir) ;

  // clobber the old file by creating a new one
  if ((os = New_Checked_OStream (CurrentRerunFileName, POV_File_Text_INI, false)) != NULL)
  {
    POVMSObject obj;
    int err = 0;
    OStream& out = *os ;
    OTextStream ots (CurrentRerunFileName, os) ; // ots will delete os

    ots.printf ("[Renderer]\n") ;

    err = POVMSObject_New (&obj, kPOVObjectClass_ROptions);
    if (err != 0)
      return;
    if (err == 0)
      err = BuildRenderOptions (&obj);

    options->WriteFile (&ots, msg) ;

    ots.printf ("\n[Environment]\n") ;
    tm = time (NULL) ;
    ots.printf ("Date=%s", ctime (&tm)) ;
    ots.printf ("CurrentDirectory=%s\n\n", dir) ;
  }
  else
    message_printf ("Cannot open file %s to write rerun information\r\n", CurrentRerunFileName) ;
}

void rotate_rerun_entries (void)
{
  int         i ;
  char        filename1 [_MAX_PATH] ;
  char        filename2 [_MAX_PATH] ;
  char        *s1 ;
  char        *s2 ;

  if (!running_demo)
  {
    s1 = get_current_rerun ("Renderer", "Input_File_Name") ;
    s2 = get_previous_rerun (0, "Renderer", "Input_File_Name") ;
    if (s1 == NULL || s2 == NULL || stricmp (s1, s2))
    {
      sprintf (filename1, "%sRerun%02d.Ini", RerunIniPath, MAX_RERUN - 1) ;
      WritePrivateProfileString (NULL, NULL, NULL, filename1) ; // flush the INI file cache
      DELETE_FILE (filename1) ;
      for (i = MAX_RERUN - 1 ; i >= 0 ; i--)
      {
        sprintf (filename1, "%sRerun%02d.Ini", RerunIniPath, i) ;
        sprintf (filename2, "%sRerun%02d.Ini", RerunIniPath, i + 1) ;
        WritePrivateProfileString (NULL, NULL, NULL, filename1) ;
        rename (filename1, filename2) ;
      }
      sprintf (filename2, "%sRerun00.Ini", RerunIniPath) ;
      // flush the INI file cache
      WritePrivateProfileString (NULL, NULL, NULL, filename2) ;
      rename (CurrentRerunFileName, filename2) ;
    }
    else
    {
      sprintf (filename2, "%sRerun00.Ini", RerunIniPath) ;
      // flush the INI file cache
      WritePrivateProfileString (NULL, NULL, NULL, filename2) ;
      DELETE_FILE (filename2) ;
      rename (CurrentRerunFileName, filename2) ;
    }
  }
}

void add_rerun_to_menu (void)
{
  int                   i ;
  int                   count ;
  char                  *s ;
  char                  str1 [_MAX_PATH + 5] ;
  char                  str2 [_MAX_PATH] ;
  char                  chars [] = "0123456789ABCDEF" ;

  clear_menu (hRerunMenu) ;
  AppendMenu (hRerunMenu, MF_STRING, CM_RERUNDIALOG, "Rerun &Dialog") ;
  if ((s = get_current_rerun ("Renderer", "Input_File_Name")) != NULL)
  {
    AppendMenu (hRerunMenu, MF_SEPARATOR, -1, "-") ;
    splitpath (s, NULL, str2) ;
    sprintf (str1, "Rer&un %s", str2) ;
    AppendMenu (hRerunMenu, MF_STRING, CM_RERUNCURRENT, str1) ;
    if ((s = get_current_rerun ("Renderer", "Output_To_File")) != NULL)
    {
      if (stricmp (s, "ON") == 0)
      {
        sprintf (str1, "&Continue %s", str2) ;
        AppendMenu (hRerunMenu, MF_STRING, CM_CONTINUECURRENT, str1) ;
      }
      else
        AppendMenu (hRerunMenu, MF_STRING | MF_GRAYED, CM_CONTINUECURRENT, "&Continue not available (no output file)") ;
    }
  }

  for (i = count = 0 ; i < MAX_RERUN ; i++)
  {
    if ((s = get_previous_rerun (i, "Renderer", "Input_File_Name")) != NULL)
    {
      if (count == 0)
        AppendMenu (hRerunMenu, MF_SEPARATOR, -1, "-") ;
      splitpath (s, NULL, str2) ;
      sprintf (str1, "&%c - %s", chars [count++], str2) ;
      AppendMenu (hRerunMenu, MF_STRING, CM_RERUN + i, str1) ;
    }
  }
  DrawMenuBar (main_window) ;
}

void update_menu_for_render (bool rendering)
{
  int         i ;
  char        *s ;

  if (rendering)
  {
    PVModifyMenu (CM_FILERENDER, MF_STRING, CM_FILERENDER, "&Stop Rendering\tAlt+G") ;
    PVEnableMenuItem (CM_COMMANDLINE, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERINSERT, MF_GRAYED) ;
    PVEnableMenuItem (CM_SOURCEFILE, MF_GRAYED) ;
    for (i = 0 ; i < MAX_RERUN ; i++)
      PVEnableMenuItem (CM_RERUN + i, MF_GRAYED) ;
    PVEnableMenuItem (CM_RERUNCURRENT, MF_GRAYED) ;
    PVEnableMenuItem (CM_CONTINUECURRENT, MF_GRAYED) ;
    PVEnableMenuItem (CM_DEMO, MF_GRAYED) ;
    PVEnableMenuItem (CM_BENCHMARK, MF_GRAYED) ;
    PVEnableMenuItem (CM_FORCE8BITS, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERSLEEP, MF_ENABLED) ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0L) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_FILERENDER, MAKELONG (1, 0)) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_STOPRENDER, MAKELONG (0, 0)) ;
    EnableWindow (toolbar_cmdline, false) ;
  }
  else
  {
    PVEnableMenuItem (CM_SOURCEFILE, MF_ENABLED) ;
    PVEnableMenuItem (CM_COMMANDLINE, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERINSERT, MF_ENABLED) ;
    PVEnableMenuItem (CM_DEMO, MF_ENABLED) ;
    PVEnableMenuItem (CM_BENCHMARK, MF_ENABLED) ;
    PVModifyMenu (CM_FILERENDER, MF_STRING, CM_FILERENDER, "&Start Rendering\tAlt+G") ;
    for (i = 0 ; i < MAX_RERUN ; i++)
      PVEnableMenuItem (CM_RERUN + i, MF_ENABLED) ;
    PVEnableMenuItem (CM_RERUNCURRENT, MF_ENABLED) ;
    if ((s = get_current_rerun ("Renderer", "Output_To_File")) != NULL)
      if (stricmp (s, "ON") == 0)
        PVEnableMenuItem (CM_CONTINUECURRENT, MF_ENABLED) ;
    PVEnableMenuItem (CM_FORCE8BITS, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERSLEEP, MF_GRAYED) ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0L) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_FILERENDER, MAKELONG (0, 0)) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_STOPRENDER, MAKELONG (1, 0)) ;
    EnableWindow (toolbar_cmdline, true) ;
  }
  PVEnableMenuItem (CM_RENDERSHOW, renderwin_destroyed ? MF_ENABLED : MF_GRAYED) ;
  DrawMenuBar (main_window) ;
}

void update_queue_status (bool write_files)
{
  int         i ;
  char        str [64] ;

  if (queued_file_count == 0)
  {
    PVModifyMenu (CM_CLEARQUEUE, MF_STRING, CM_CLEARQUEUE, "C&lear Queue (no entries)") ;
    PVEnableMenuItem (CM_CLEARQUEUE, MF_GRAYED) ;
  }
  else
  {
    sprintf (str, "C&lear Queue (%d %s)", queued_file_count, queued_file_count == 1 ? "entry" : "entries") ;
    PVModifyMenu (CM_CLEARQUEUE, MF_STRING, CM_CLEARQUEUE, str) ;
    PVEnableMenuItem (CM_CLEARQUEUE, queued_file_count ? MF_ENABLED : MF_GRAYED) ;
  }
  if (write_files)
  {
    PutPrivateProfileInt ("FileQueue", "QueueCount", queued_file_count, EngineIniFileName) ;
    for (i = 0 ; i < MAX_QUEUE ; i++)
    {
      sprintf (str, "QueuedFile%d", i) ;
      WritePrivateProfileString ("FileQueue", str, i < queued_file_count ? queued_files [i] : NULL, EngineIniFileName) ;
    }
  }
}

void fill_rerun_listbox (HWND hlb, char *idList)
{
  char                  str [256] ;
  char                  *s ;
  char                  dhms [32] = "No Time\t" ;
  unsigned              i ;
  unsigned long         seconds ;
  RECT                  rect ;

  GetClientRect (hlb, &rect) ;
  for (i = 0 ; i < MAX_RERUN ; i++)
  {
    if ((s = get_previous_rerun (i, "Renderer", "Input_File_Name")) == NULL)
      continue ;
    splitpath (s, NULL, str) ;
    strcat (str, "\t") ;
    if ((s = get_previous_rerun (i, "Statistics", "StartRender")) != NULL)
    {
      seconds = atol (s) ;
      if ((s = get_previous_rerun (i, "Statistics", "FinishRender")) != NULL)
      {
        seconds = atol (s) - seconds ;
        sprintf (dhms, "%ud %02uh %02um %02us\t", seconds / 86400, seconds % 86400 / 3600, seconds % 3600 / 60, seconds % 60) ;
      }
    }
    strcat (str, dhms) ;
    if ((s = get_previous_rerun (i, "Environment", "Date")) == NULL)
      s = "No Date" ;
    strcat (str, s) ;
    SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) str) ;
    *idList++ = (char) i ;
  }
}

void fill_statistics_listbox (HWND hlb, int id)
{
  char                  str [64] ;
  char                  *s ;
  unsigned              i ;

  for (i = 0 ; i < 256 ; i++)
  {
    sprintf (str, "StatLn%02d", i) ;
    if ((s = get_previous_rerun (id, "Statistics", str)) != NULL)
      SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) s) ;
    else
      break ;
  }
}

void resize_listbox_dialog (HWND hDlg, int idLb, int chars)
{
  int         difference ;
  HWND        hLb ;
  HWND        hBtn ;
  RECT        lbRect ;
  RECT        btnRect ;
  RECT        dlgRect ;

  hLb = GetDlgItem (hDlg, idLb) ;
  hBtn = GetDlgItem (hDlg, IDOK) ;
  GetRelativeClientRect (hDlg, hLb, &lbRect) ;
  GetRelativeClientRect (hDlg, hBtn, &btnRect) ;
  GetWindowRect (hDlg, &dlgRect) ;
  difference = message_xchar * (chars + 2) - (lbRect.right - lbRect.left) ;
  lbRect.right += difference ;
  MoveWindow (hLb, lbRect.left, lbRect.top, lbRect.right - lbRect.left, lbRect.bottom - lbRect.top, true) ;
  btnRect.left += difference / 2 ;
  btnRect.right += difference / 2 ;
  MoveWindow (hBtn, btnRect.left, btnRect.top, btnRect.right - btnRect.left, btnRect.bottom - btnRect.top, true) ;
  dlgRect.right += difference ;
  MoveWindow (hDlg, dlgRect.left, dlgRect.top, dlgRect.right - dlgRect.left, dlgRect.bottom - dlgRect.top, true) ;
}

#if 0
char *save_demo_file (void)
{
  GetTempPath (sizeof (filename), filename) ;
  strcat (filename, "POVDEMO.$$$") ;
  if ((hrsc = FindResource (hInst, MAKEINTRESOURCE (ID_DEMOFILE), RT_RCDATA)) == NULL)
  {
    PovMessageBox ("Cannot locate file resource\r\n(internal error)", "Cannot run demo") ;
    return (NULL) ;
  }
  if ((hglobal = LoadResource (hInst, hrsc)) == NULL)
  {
    PovMessageBox ("Cannot load file resource", "Cannot run demo") ;
    return (NULL) ;
  }
  if ((s = LockResource (hglobal)) == NULL)
  {
    PovMessageBox ("Cannot lock file resource", "Cannot run demo") ;
    return (NULL) ;
  }
  size = SizeofResource (hInst, hrsc) ;
  if ((outH = _lcreat (filename, 0)) == HFILE_ERROR)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    return (NULL) ;
  }

  if (_lwrite (outH, s, size) != size)
  {
    PovMessageBox ("Cannot write temporary file", "Cannot run demo") ;
    return (NULL) ;
  }
  _lclose (outH) ;
}
#endif

int splitfn (char *filename, char *path, char *name, char *ext)
{
  char        *s ;
  char        str [_MAX_PATH] ;

  filename = strcpy (str, filename) ;

  if (path != NULL)
    *path = '\0' ;
  if (name != NULL)
    *name = '\0' ;
  if (ext != NULL)
    *ext = '\0' ;

  if ((s = strrchr (filename, '.')) != NULL)
  {
    if (ext)
      strcpy (ext, s) ;
    *s = '\0' ;
  }

  if ((s = strrchr (filename, SEPARATOR)) != NULL)
  {
    if (name)
      strcpy (name, s + 1) ;
    *++s = '\0' ;
    if (path)
      strcpy (path, filename) ;
  }
  else
    if (name)
      strcpy (name, filename) ;

  return (0) ;
}

void splitpath (char *filename, char *path, char *name)
{
  char        str [_MAX_PATH] ;

  splitfn (filename, path, name, str) ;
  if (name != NULL)
    strcat (name, str) ;
}

void load_tool_menu (char *iniFilename)
{
  int         i ;
  int         count ;
  char        str [32] ;
  char        entry [256] ;
  char        *s ;

  memset (tool_commands, 0, sizeof (tool_commands)) ;
  memset (tool_help, 0, sizeof (tool_help)) ;

  DeleteMenu (hToolsMenu, 1, MF_BYCOMMAND) ;
  for (i = 0 ; i < MAX_TOOLCMD ; i++)
    DeleteMenu (hToolsMenu, CM_FIRSTTOOL + i, MF_BYCOMMAND) ;

  for (i = count = 0 ; i < MAX_TOOLCMD ; i++)
  {
    sprintf (str, "Item%d", i) ;

    GetPrivateProfileString ("Command", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
      continue ;
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) >= MAX_TOOLCMDTEXT)
    {
      message_printf ("Tool command %s is too long\n", str) ;
      s [MAX_TOOLCMDTEXT - 1] = '\0' ;
    }
    strcpy (tool_commands [count], s) ;

    GetPrivateProfileString ("Menu", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
    {
      message_printf ("Tool menu entry %s is missing\n", str) ;
      continue ;
    }
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) > 31)
    {
      message_printf ("Tool menu entry %s is too long\n", str) ;
      continue ;
    }
    if (count == 0)
      AppendMenu (hToolsMenu, MF_SEPARATOR, 1, "-") ;
    AppendMenu (hToolsMenu, MF_STRING, CM_FIRSTTOOL + count, s) ;

    GetPrivateProfileString ("Help", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
    {
      count++ ;
      continue ;
    }
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) >= MAX_TOOLHELPTEXT)
    {
      message_printf ("Tool help %s is too long\n", str) ;
      s [MAX_TOOLHELPTEXT - 1] = '\0' ;
    }
    strcpy (tool_help [count++], s) ;
  }
  message_printf ("Loaded %d %s from %s into Tool Menu.\n", count, (count != 1 ? "tools" : "tool"), iniFilename) ;
}

char *parse_tool_command (char *command)
{
  char                  *s ;
  char                  ExternalStr [512] ;
  static char           str [512] ;

  str [0] = '\0' ;
  while (*command == ' ')
    command++ ;
  strcpy (ExternalStr, command) ;
  ExternalParseToolCommand (ExternalStr) ;
  if (strlen (ExternalStr))
    command = ExternalStr ;
  for (s = str ; *command ; command++)
  {
    if (strlen (str) >= sizeof (str) - _MAX_PATH)
      break ;
    if (*command == '%')
    {
      if (*++command == '%')
      {
        *s++ = *command ;
        continue ;
      }
      switch (toupper (*command))
      {
        case 'I' :
             s += sprintf (s, "%sini\\", DocumentsPath) ;
             break ;

        case 'T' :
             s += sprintf (s, "%s", ToolIniFileName) ;
             break ;

        case 'H' :
             s += sprintf (s, "%s", BinariesPath) ;
             break ;

        case 'R' :
             s += joinPath (s, lastRenderPath, lastRenderName) ;
             break ;

        case 'S' :
             s += sprintf (s, "%s", source_file_name) ;
             break ;

        case 'N' :
             s += sprintf (s, "%s", SecondaryRenderIniFileName) ;
             break ;

        case 'D' :
             s += GetPrivateProfileString ("LastRender", "CurrentDirectory", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '0' :
             s += GetPrivateProfileString ("LastRender", "SourceFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '1' :
             s += GetPrivateProfileString ("LastRender", "OutputFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '2' :
             s += GetPrivateProfileString ("LastRender", "SceneFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '3' :
             s += GetPrivateProfileString ("LastRender", "HistogramFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '4' :
             s += GetPrivateProfileString ("LastRender", "IniOutputFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;
      }
      continue ;
    }
    if (s == str && isspace (*command))
      continue ;
    *s++ = *command ;
  }
  *s = '\0' ;
  return (str) ;
}

char *get_elapsed_time (int seconds)
{
  static char str [19] ;

  str [0] = '\0' ;
  sprintf (str, "%ud %02uh %02um %02us",
           seconds / 86400,
           seconds % 86400 / 3600,
           seconds % 3600 / 60,
           seconds % 60) ;
  return (str) ;
}

char *clean (char *s)
{
  static char           str [_MAX_PATH] ;

  while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' )
    s++ ;
  s = strncpy (str, s, sizeof (str) - 1) ;
  if (*s == '\0')
    return (str) ;
  for (s += strlen (s) - 1 ; *s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' ; s--)
    *s = '\0' ;
  return (str) ;
}

void extract_ini_sections (char *filename, HWND hwnd)
{
  char                  str [256] ;
  char                  *s1 ;
  char                  *s2 ;
  FILE                  *inF ;
  static char           inbuf [16384] ;

  /*
  ** flush the INI file cache
  */
  WritePrivateProfileString (NULL, NULL, NULL, filename) ;
  SendMessage (hwnd, CB_RESETCONTENT, 0, 0L) ;

  if ((inF = fopen (filename, "rt")) == NULL)
    return ;

  setvbuf (inF, inbuf, _IOFBF, sizeof (inbuf)) ;
  while (fgets (str, sizeof (str), inF) != NULL)
  {
    s1 = clean (str) ;
    if (*s1 == '[')
    {
      if ((s2 = strchr (s1, ']')) != NULL)
      {
        *++s2  = '\0' ;
        SendMessage (hwnd, CB_ADDSTRING, 0, (LPARAM) s1) ;
      }
    }
  }
  fclose (inF) ;
  SendMessage (hwnd, CB_SETCURSEL, 0, 0L) ;
}

int select_combo_item_ex (HWND hwnd, char *s)
{
  int         i ;
  char        str [256] ;

  if ((i = SendMessage (hwnd, CB_GETCOUNT, 0, 0)) < 0)
    return (i) ;
  while (i--)
  {
    SendMessage (hwnd, CB_GETLBTEXT, i, (LPARAM) str) ;
    if (strcmp (s, str) == 0)
    {
      SendMessage (hwnd, CB_SETCURSEL, i, 0) ;
      return (i) ;
    }
  }
  return (-1) ;
}

void extract_ini_sections_ex (char *filename, HWND hwnd)
{
  extract_ini_sections (filename, hwnd) ;
}

bool PovInvalidateRect (HWND hWnd, CONST RECT *lpRect, bool bErase)
{
  if (hWnd != NULL)
    return (InvalidateRect (hWnd, lpRect, bErase) != 0) ;
  return (0) ;
}

bool TaskBarAddIcon (HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
  NOTIFYICONDATA        tnid ;

  memset (&tnid, 0, sizeof (tnid)) ;
  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;
  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;
  tnid.uCallbackMessage = TASKBAR_NOTIFY_MESSAGE ;
  tnid.hIcon = hicon ;
  lstrcpyn (tnid.szTip, lpszTip, sizeof(tnid.szTip)) ;

  return (Shell_NotifyIcon (NIM_ADD, &tnid) != 0) ;
}

bool TaskBarModifyIcon (HWND hwnd, UINT uID, LPSTR lpszTip)
{
  NOTIFYICONDATA        tnid ;

  memset (&tnid, 0, sizeof (tnid)) ;
  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;
  tnid.uFlags = NIF_TIP ;
  lstrcpyn (tnid.szTip, lpszTip, sizeof(tnid.szTip)) ;

  return (Shell_NotifyIcon (NIM_MODIFY, &tnid) != 0) ;
}

bool TaskBarDeleteIcon (HWND hwnd, UINT uID)
{
  NOTIFYICONDATA        tnid ;

  memset (&tnid, 0, sizeof (tnid)) ;
  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;

  return (Shell_NotifyIcon (NIM_DELETE, &tnid) != 0) ;
}

bool ShowRestrictionMessage (char *Message, char *Dir)
{
  int         i ;
  char        str [8192] ;

  // the main window code will expect this to be already set up if it gets a WM_HELP
  hh_aklink.pszKeywords = "I/O Restrictions" ;

  if (Dir == NULL)
  {
    sprintf (str, "%s\nPress the HELP button to learn about I/O Restrictions.", Message) ;
    MessageBox (main_window, str, "I/O Restriction Activated", MB_ICONERROR | MB_OK | MB_HELP) ;
    return (false) ;
  }

  sprintf (str, "%s\n"
                "Press OK to grant temporary read/write permission in that directory (and its subdirectories).\n"
                "Press CANCEL to halt the render.\n\n"
                "Press the HELP button to learn about I/O Restrictions.",
                Message) ;

  if (MessageBox (main_window, str, "I/O Restriction Activated", MB_ICONSTOP | MB_OKCANCEL | MB_HELP | MB_DEFBUTTON2) != IDOK)
    return (false) ;

  for (i = 0 ; i < MAX_DIRSPEC - 1 ; i++)
    if (WriteDirSpecs [i] == NULL)
      break ;
  if (i == MAX_DIRSPEC - 1)
  {
    MessageBox (main_window, "Ran out of room to store directory grant permission", "I/O Restriction Error", MB_ICONERROR | MB_OK) ;
    // return true anyhow
    return (true) ;
  }

  WriteDirSpecs [i] = strdup (Dir) ;
  return (true) ;
}

bool TestAccessAllowed (const char *Filename, unsigned int FileType, bool IsWrite)
{
  char        str [_MAX_PATH] ;
  char        str1 [_MAX_PATH] ;
  char        str2 [_MAX_PATH] ;
  char        file [_MAX_FNAME] ;
  char        msg [_MAX_PATH + 256] ;
  char        *s ;
  char        **dirspec ;

  if (GetFullPathName (Filename, sizeof (str), str, &s) == 0)
  {
    buffer_message (mDivider, "\n") ;
    WIN_Fatal ("Could not resolve full path when testing access permission") ;
    ShowRestrictionMessage ("Could not resolve full path when testing access permission.\n", NULL) ;
    return (FALSE) ;
  }
  if (s == NULL)
  {
    buffer_message (mDivider, "\n") ;
    WIN_Fatal ("Could not resolve full path when testing access permission") ;
    ShowRestrictionMessage ("Could not resolve full path when testing access permission.\n", NULL) ;
    return (FALSE) ;
  }
  strcpy (file, s) ;
  *s-- = '\0' ;
  if (*s != '\\')
    strcat (s, "\\") ;

  // GetLongPathname is only available on W98/W2k or later
//GetLongPathName (str, str1, sizeof (str1)) ;
  strcpy (str1, str) ;

  GetShortPathName (str, str2, sizeof (str2)) ;
  strupr (str1) ;
  strupr (str2) ;
  if (str1 [strlen (str1) - 1] != '\\')
    strcat (str1, "\\") ;
  if (str2 [strlen (str2) - 1] != '\\')
    strcat (str2, "\\") ;

  if (IsWrite)
  {
    // we do special-case hard-coded exclusion test(s) here

    // never allow write access to pvengine.ini in case a script attempts
    // to modify the portion that sets these directory restrictions.
    // (as a general case we don't allow write access to our INI dir at all).
    if (strncmp (EngineIniDir, str1, strlen (EngineIniDir)) == 0 || strncmp (EngineIniDir, str2, strlen (EngineIniDir)) == 0)
    {
      buffer_message (mDivider, "\n") ;
      sprintf (msg, "Special-case restriction prohibits write access to '%s' for '%s'.\r\n", str, file) ;
      WIN_Fatal (msg) ;
      WIN_Fatal ("See the Options menu or section 1.4 in the POVWIN help file for more information\r\n") ;
      ShowRestrictionMessage (msg, NULL) ;
      return (FALSE) ;
    }
  }
  else
  {
    for (dirspec = ReadDirSpecs ; *dirspec != NULL ; dirspec++)
      if (strncmp (*dirspec, str1, strlen (*dirspec)) == 0 || strncmp (*dirspec, str2, strlen (*dirspec)) == 0)
        return (TRUE) ;
    // access for write implies access for read (this is by design).
    // so we check the write specs below instead of giving an error here.
  }

  for (dirspec = WriteDirSpecs ; *dirspec != NULL ; dirspec++)
    if (strncmp (*dirspec, str1, strlen (*dirspec)) == 0 || strncmp (*dirspec, str2, strlen (*dirspec)) == 0)
      return (TRUE) ;

  // check this last to ensure any hard-coded exclusions above are applied
  if (allow_rw_source)
  {
    // determine if the file is in the current directory (but not below it)
    if (GetCurrentDirectory (sizeof (msg), msg) != 0)
    {
      strupr (msg) ;
      if (msg [strlen (msg) - 1] != '\\')
        strcat (msg, "\\") ;
      if (strcmp (str1, msg) == 0 || strcmp (str2, msg) == 0)
        return (TRUE) ;
    }
  }

  sprintf (msg, "I/O restriction prohibits %s access to '%s' for '%s'.\r\n", IsWrite ? "write" : "read", str, file) ;
  if (ShowRestrictionMessage (msg, str1))
    return (TRUE) ;
  buffer_message (mDivider, "\n") ;
  WIN_Fatal (msg) ;
  WIN_Fatal ("See the Options menu or section 1.4 in the POVWIN help file for more information\r\n") ;
  return (FALSE) ;
}

