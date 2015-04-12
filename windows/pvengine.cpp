/****************************************************************************
 *                pvengine.cpp
 *
 * This file implements Windows specific routines, WinMain, and message loops.
 *
 * Primary author: Christopher J. Cason.
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
 * $File: //depot/povray/3.5/windows/pvengine.cpp $
 * $Revision: #59 $
 * $Change: 4793 $
 * $DateTime: 2009/05/26 07:13:51 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

/*****************************************************************************/
/* NOTICE                                                                    */
/*                                                                           */
/* Much of this source code (all of pvengine.exe) should be considered on    */
/* its last legs. The base code was originally written back in early 1995    */
/* (with some of the code even predating that), and targeted at Win32s under */
/* Windows 3.1. An additional constraint was a desire for the code to  be    */
/* able to be compiled on a DEC Alpha under NT for the Alpha using only a    */
/* 'C' compiler, and for it to be able to run with or without the editor.    */
/*                                                                           */
/*****************************************************************************/

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION
#define WIN32_LEAN_AND_MEAN

#if !defined( __BORLANDC__ ) && !defined( __DMC__ ) && !defined( __MINGW32__ )
  #pragma comment(lib, "htmlhelp")
  #pragma comment(lib, "winmm")
#endif

#define DECLARE_TABLES

#ifdef _WIN64
#pragma pack(16)
#endif

#include <windows.h>
#include <htmlhelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <commctrl.h>

#ifdef _WIN64
#pragma pack()
#endif

#include "pvengine.h"
#include "resource.h"
#include "pvdialog.h"
#include "pvguiext.h"
#include "pvedit.h"
#include "pov_err.h"
#include "pov_util.h"
#include "pvbmp.h"
#include "benchmark.h"

#include "defaultplatformbase.h"
#include "pvfrontend.h"

// #define DEVELOPMENT

#define CLASSNAMEPREFIX "UnofficialPov36"

#ifdef DEVELOPMENT
  #ifdef _WIN64
    #define CAPTIONPREFIX "[WIN64]"
  #else
    #define CAPTIONPREFIX "[WIN32]"
  #endif
#else
  #define CAPTIONPREFIX ""
#endif

#ifdef _WIN64
  #define EDITDLLNAME         "cmedit64.dll"
  #define UPDATEDLLNAME       "povupdate64.dll"
  #define NEWESTVERSIONVAL    "NewestVersion64"
  #define VERSIONVAL          "VersionNo64"
  #define NEXTVERSIONCHECKVAL "NextVersionCheck64"
#else
  #define EDITDLLNAME         "cmedit32.dll"
  #define UPDATEDLLNAME       "povupdate32.dll"
  #define NEWESTVERSIONVAL    "NewestVersion"
  #define VERSIONVAL          "VersionNo"
  #define NEXTVERSIONCHECKVAL "NextVersionCheck"
#endif

typedef struct _DllVersionInfo
{
  DWORD cbSize;
  DWORD dwMajorVersion;                   // Major version
  DWORD dwMinorVersion;                   // Minor version
  DWORD dwBuildNumber;                    // Build number
  DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
} DLLVERSIONINFO;

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC) (DLLVERSIONINFO *);
typedef DWORD __stdcall shCopyType (HKEY, LPCTSTR, HKEY, DWORD) ;

#ifndef SM_CMONITORS
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#endif

extern POVMSContext POVMS_Render_Context ;
POVMSContext POVMS_Output_Context = NULL ;

BEGIN_POV_NAMESPACE
volatile POVMSContext POVMS_GUI_Context = NULL ;
volatile POVMSAddress RenderThreadAddr = POVMSInvalidAddress ;
volatile POVMSAddress GUIThreadAddr = POVMSInvalidAddress ;
END_POV_NAMESPACE

USING_POV_NAMESPACE
USING_POV_FRONTEND_NAMESPACE

//#define TIMED_BETA

// 1101859200 - Tue Nov 30 19:00:00 EST 2004
#define EXPIRE_AT       (1101859200 + 31 * 86400)

#define RTR_W           180
#define RTR_H           90

#define EDIT_FILE       1
#define RENDER_FILE     2

#define CONFIRM_STOP_THRESHOLD 900

int                     alert_sound ;
int                     statistics_count = -1 ;
int                     run_count ;
int                     render_anim_count ;
int                     argc ;
int                     io_restrictions ;
int                     tb_combo_sel ;
int                     cb_expect_selchange ;
int                     povray_return_code ;
int                     PerformanceScale = 1 ;
int                     seconds_for_last_line = -1 ;
int                     delay_next_status ;
int                     screen_origin_x ;
int                     screen_origin_y ;
int                     virtual_screen_width ;
int                     virtual_screen_height ;
char                    command_line [_MAX_PATH * 3] ;
char                    old_command_line [_MAX_PATH * 3] ;
char                    *argv [MAX_ARGV] ;
char                    source_file_name [_MAX_PATH] ;
char                    modulePath [_MAX_PATH] ;
char                    engineHelpPath [_MAX_PATH] ;
char                    lastRenderName [_MAX_PATH] ;
char                    lastBitmapName [_MAX_PATH] ;
char                    lastRenderPath [_MAX_PATH] ;
char                    lastBitmapPath [_MAX_PATH] ;
char                    lastQueuePath [_MAX_PATH] ;
char                    lastSecondaryIniFilePath [_MAX_PATH] ;
char                    DefaultRenderIniFileName [_MAX_PATH] ;
char                    SecondaryRenderIniFileName [_MAX_PATH] ;
char                    SecondaryRenderIniFileSection [64] ;
char                    background_file [_MAX_PATH] ;
char                    DocumentsPath [_MAX_PATH] ;
char                    BinariesPath [_MAX_PATH] ;
char                    LastInferredHome [_MAX_PATH] ;
char                    EngineIniFileName [_MAX_PATH] ;
char                    EngineIniDir [_MAX_PATH] ;
char                    RerunIniPath [_MAX_PATH] ;
char                    CurrentRerunFileName [_MAX_PATH] ;
char                    ToolIniFileName [_MAX_PATH] ;
char                    tool_commands [MAX_TOOLCMD] [MAX_TOOLCMDTEXT] ;
char                    tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
char                    requested_render_file [_MAX_PATH] ;
char                    ErrorMessage [4096] ;
char                    ErrorFilename [_MAX_PATH] ;
char                    RegionStr [128] ;
char                    TempRegionStr [128] ;
char                    render_percentage [128] ;
char                    last_render_percentage [128] ;
char                    output_file_name [_MAX_PATH] ;
char                    demo_file_name [_MAX_PATH] ;
char                    demo_ini_name [_MAX_PATH] ;
char                    status_buffer [1024] ;
char                    render_complete_sound [_MAX_PATH] ;
char                    parse_error_sound [_MAX_PATH] ;
char                    render_error_sound [_MAX_PATH] ;
char                    FontPath [_MAX_PATH] ;
bool                    render_complete_sound_enabled ;
bool                    parse_error_sound_enabled  ;
bool                    render_error_sound_enabled ;
bool                    rendering ;
bool                    stop_rendering ;
bool                    alert_on_completion ;
bool                    save_settings ;
bool                    IsW95UserInterface ;
bool                    IsW98 ;
bool                    IsWNT ;
bool                    IsW2k ;
bool                    IsWXP ;
bool                    running_demo ;
bool                    running_benchmark ;
bool                    fast_scroll ;
bool                    no_shellout_wait ;
bool                    tile_background = true ;
bool                    debugging ;
bool                    no_palette_warn ;
bool                    render_lock_up ;
bool                    hide_render_window = true ;
bool                    render_above_main = true ;
bool                    tips_enabled = true ;
bool                    demo_mode ;
bool                    ignore_auto_ini ;
bool                    newVersion ;
bool                    exit_after_render ;
bool                    system_noactive ;
bool                    one_instance = true ;
bool                    run_renderer ;
bool                    run_backend_thread = true ;
bool                    backend_thread_exception ;
bool                    use_toolbar = true ;
bool                    use_tooltips = true ;
bool                    use_editors = true ;
bool                    resizing ;
bool                    drop_to_editor ;
bool                    restore_command_line ;
bool                    render_requested ;
bool                    render_auto_close ;
bool                    renderwin_manually_closed ;
bool                    renderwin_hidden ;
bool                    rendersleep ;
bool                    noexec ;
bool                    ExtensionsEnabled = true ;
bool                    use_taskbar = true ;
bool                    main_window_hidden ;
bool                    splash_show_about ;
bool                    about_showing ;
bool                    use_renderanim = true ;
bool                    NoRestore ;
bool                    IsComCtl5 = false ;
bool                    IsComCtl6 = false ;
bool                    allow_rw_source ;
bool                    no_shell_outs = true ;
bool                    about_thread_quit ;
bool                    about_thread_running ;
bool                    hide_newuser_help ;
bool                    info_render_complete = false ;
bool                    no_status_output = false ;
bool                    temp_render_region = false ;
bool                    rendering_insert_menu = false ;
bool                    was_insert_render = false ;
bool                    rendering_animation = false ;
bool                    preserve_bitmap = false ;
bool                    first_frame = false ;
bool                    check_new_version ;
bool                    check_news ;
bool                    send_system_info ;
bool                    homeInferred = false ;
bool                    render_cooperate = false ;
bool                    output_to_file ;
bool                    UpdateCheckDone = false ;
bool                    WasFreshInstall = false ;
bool                    AutoAppendPaths ;
HWND                    toolbar_window ;
HWND                    aux_toolbar_window ;
HWND                    window_list [MAX_WINDOWS] ;
HWND                    toolbar_combobox ;
HWND                    rebar_window ;
HWND                    StatusWindow ;
HWND                    toolbar_cmdline ;
HWND                    tab_window ;
FILE                    *debugFile ;
HICON                   ourIcon ;
HICON                   renderIcon ;
HFONT                   about_font ;
HACCEL                  hAccelerators ;
HANDLE                  hAboutThread ;
HANDLE                  hMainThread ;
HANDLE                  hBackendThread ;
jmp_buf                 gWinSetJmpEnv ;
HBITMAP                 hBmpBackground ;
HBITMAP                 hBmpRendering ;
HBITMAP                 hBmpIcon ;
HBITMAP                 hBmpSplash ;
HBITMAP                 hBmpAbout ;
__int64                 PerformanceFrequency ;
__int64                 PerformanceCounter1 ;
__int64                 PerformanceCounter2 ;
__int64                 KernelTimeStart ;
__int64                 KernelTimeEnd ;
__int64                 UserTimeStart ;
__int64                 UserTimeEnd ;
__int64                 KernelTimeTotal ;
__int64                 UserTimeTotal ;
__int64                 CPUTimeTotal ;
unsigned                class_registered = 0 ;
unsigned                currentX = 0 ;
unsigned                currentY = 0 ;
unsigned                percentage_complete = 0 ;
unsigned                screen_width ;
unsigned                screen_height ;
unsigned                screen_depth ;
unsigned                background_width ;
unsigned                background_height ;
unsigned                loadRerun ;
unsigned                continueRerun ;
unsigned                seconds = 0 ;
unsigned                pixels = 0 ;
unsigned                toolheight = 0 ;
unsigned                statusheight = 0 ;
unsigned                render_priority = CM_RENDERPRIORITY_NORMAL ;
unsigned                Duty_Cycle = 9 ;
unsigned                on_completion = CM_COMPLETION_NOTHING ;
unsigned                window_count = 0 ;
unsigned                ErrorLine ;
unsigned                ErrorCol ;
unsigned                splash_time = 1 ;
unsigned                quit ;
unsigned                ClockTimeStart ;
unsigned                ClockTimeEnd ;
unsigned                ClockTimeTotal ;
unsigned                SleepTimeStart ;
unsigned                SleepTimeEnd ;
unsigned                SleepTimeTotal ;
unsigned                TimerTicks ;
HPALETTE                hPalApp ;
HPALETTE                hPalBitmap ;
COLORREF                background_colour ;
COLORREF                text_colour ;
COLORREF                custom_colours [16] ;
COLORREF                background_shade = RGB (1, 1, 1) ;
HINSTANCE               hInstance ;
HH_AKLINK               hh_aklink ;
OSVERSIONINFO           version_info ;
CRITICAL_SECTION        critical_section ;

char                    queued_files [MAX_QUEUE] [_MAX_PATH] ;
char                    dir [_MAX_PATH] ;
unsigned                queued_file_count = 0 ;
unsigned                auto_render = true ;
unsigned                timer_id ;
unsigned                about_timer_id ;

unsigned                panel_size ;

char                    PovMainWinClass [] = CLASSNAMEPREFIX "MainWinClass" ;
unsigned                mainwin_xpos ;
unsigned                mainwin_ypos ;
HWND                    main_window ;
HWND                    message_window ;
WINDOWPLACEMENT         mainwin_placement ;
int                     renderwin_xoffset ;
int                     renderwin_yoffset ;
int                     renderwin_left = CW_USEDEFAULT ;
int                     renderwin_top = CW_USEDEFAULT ;
char                    PovRenderWinClass [] = CLASSNAMEPREFIX "RenderWinClass" ;
unsigned                renderwin_max_width ;
unsigned                renderwin_max_height ;
unsigned                renderwin_8bits ;
unsigned                renderwin_flags = 0 ;
bool                    renderwin_active ;
bool                    renderwin_destroyed ;
HWND                    render_window ;

int                     render_bitmap_depth = -1 ;
long                    render_bitmap_bpl ;
unsigned                render_width ;
unsigned                render_height ;
uchar                   *render_bitmap_surface ;
BitmapInfo              render_bitmap ;
BitmapInfo              bitmap_template ;

char                    PovSplashWinClass [] = CLASSNAMEPREFIX "SplashWinClass" ;
HWND                    splash_window ;
unsigned                splash_width ;
unsigned                splash_height ;
HPALETTE                splash_palette ;

char                    PovAboutWinClass [] = CLASSNAMEPREFIX "AboutWinClass" ;
HWND                    about_window ;
HWND                    about_buttons [4] ;
unsigned                about_width ;
unsigned                about_height ;
HPALETTE                about_palette ;

int                     raw ;
int                     rah ;
char                    PovRenderAnimWinClass [] = CLASSNAMEPREFIX "RenderAnimWinClass" ;
HWND                    renderanim_window ;

char                    PovMessageWinClass [] = CLASSNAMEPREFIX "MessageWinClass" ;

char                    *otaTitle ;
char                    *otaText ;
char                    *otaHelpString ;
bool                    otaChecked ;

bool handle_main_command (WPARAM wParam, LPARAM lParam) ;

DefaultPlatformBase     platformBase ;
WinRenderFrontend       *frontEnd ;

extern int              message_xchar ;
extern int              message_ychar ;
extern int              message_scroll_pos_x ;
extern int              message_scroll_pos_y ;
extern int              top_message_row ;
extern int              message_count ;
extern int              message_cols ;
extern int              message_rows ;
extern int              listbox_xchar ;
extern int              listbox_ychar ;
extern int              EditFileCount ;
extern int              message_output_x ;
extern char             message_font_name [256] ;
extern char             *EditFiles [] ;
extern unsigned         message_font_size ;
extern unsigned         message_font_weight ;
extern bool             keep_messages ;
extern bool             MenuBarDraw ;
extern HFONT            message_font ;
extern HFONT            tab_font ;
extern HMENU            hMenuBar ;
extern HMENU            hMainMenu ;
extern HMENU            hPopupMenus ;

extern char             Actual_Output_Name [] ;

int                     InsertMenuSections [1024] ;
int                     InsertMenuSection ;
int                     InsertMenuSectionCount ;
bool                    StartInsertRender ;

typedef struct
{
  WORD        wVirtkey ;
  int         iMessage ;
  WORD        wRequest ;
} SCROLLKEYS ;

SCROLLKEYS key2scroll [] =
{
  { VK_END,   WM_VSCROLL, SB_BOTTOM   },
  { VK_PRIOR, WM_VSCROLL, SB_PAGEUP   },
  { VK_NEXT,  WM_VSCROLL, SB_PAGEDOWN },
  { VK_UP,    WM_VSCROLL, SB_LINEUP   },
  { VK_DOWN,  WM_VSCROLL, SB_LINEDOWN },
  { VK_LEFT,  WM_HSCROLL, SB_PAGEUP   },
  { VK_RIGHT, WM_HSCROLL, SB_PAGEDOWN },
  { -1,       -1,         -1          }
} ;

void debug (char *format, ...)
{
  char                  str [2048] ;
  va_list               arg_ptr ;
  FILE                  *f ;

  if (format == NULL)
  {
    unlink ("c:\\povdebug.txt") ;
    return ;
  }

  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  OutputDebugString (str) ;

  if ((f = fopen ("c:\\povdebug.txt", "a+t")) != NULL)
  {
    fprintf (f, "%u: %s", time (NULL), str) ;
    fclose (f) ;
  }
}

void SetCaption (LPCSTR str)
{
#ifndef DEVELOPMENT
  SetWindowText (main_window, str) ;
#else
  char        buffer [1024] ;

  sprintf (buffer, CAPTIONPREFIX " %s", str) ;
  SetWindowText (main_window, buffer) ;
#endif
}

DWORD backend_thread (LPDWORD param)
{
  int         count = 0 ;
  DWORD       code ;

#ifdef NDEBUG
  try
#endif
  {
    povray_init () ;
    if (POVMS_GetContextAddress (POVMS_Render_Context, (void **) &RenderThreadAddr) != 0)
    {
      PovMessageBox ("Failed to init POV-Ray rendering core", "Message Handling Subsystem") ;
      return (-1) ;
    }
    frontEnd = new WinRenderFrontend (POVMS_Output_Context, (void *) RenderThreadAddr) ;
    while (run_backend_thread)
    {
      // when a render starts, povray_cooperate() will not return until it's done
      povray_cooperate () ;

      if (count++ % 50 == 0)
      {
        if (GetExitCodeThread (hMainThread, &code))
        {
          if (code != STILL_ACTIVE)
          {
            MessageBox (NULL, "Main thread vanished from underneath us!", "POV-Ray backend thread", MB_OK | MB_ICONEXCLAMATION) ;
            break ;
          }
        }
      }

      // not strictly necessary (it's done during povray_cooperate() above)
      Sleep (10) ;
    }
    delete frontEnd ;
    povray_terminate () ;
  }
#ifdef NDEBUG
  catch (...)
  {
    char str [1024] ;
    backend_thread_exception = true ;
    sprintf (str, "The POV-Ray core rendering code threw an unhandled exception.\n\n") ;
    strcat (str, "The backend thread has been shut down and you will not be able to perform any further renders.\n\n") ;
    strcat (str, "It is STRONGLY RECOMMENDED that you save your work and exit POV-Ray now.\n\n") ;
    strcat (str, "(Note that if internal structures have been damaged POV-Ray may crash immediately after you click OK).") ;
    MessageBox (main_window, str, "Internal Error", MB_ICONSTOP) ;
    buffer_message (mIDE, "\n") ;
    if (rendering || run_renderer)
    {
      povray_return_code = -1 ;
      render_stopped () ;
      message_printf ("Returned from renderer due to exception\n") ;
    }
  }
#endif
  hBackendThread = NULL ;
  return (0) ;
}

char *GetExceptionDescription (DWORD code)
{
  switch (code)
  {
    case EXCEPTION_ACCESS_VIOLATION :
         return ("access violation") ;

    case EXCEPTION_DATATYPE_MISALIGNMENT :
         return ("datatype misalignment") ;

    case EXCEPTION_FLT_DENORMAL_OPERAND :
         return ("denormal floating point operand") ;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO :
         return ("floating point divide by zero") ;

    case EXCEPTION_FLT_INEXACT_RESULT :
         return ("inexact floating-point result") ;

    case EXCEPTION_FLT_INVALID_OPERATION :
         return ("invlalid floating-point operation") ;

    case EXCEPTION_FLT_OVERFLOW :
         return ("floating-point overflow") ;

    case EXCEPTION_FLT_STACK_CHECK :
         return ("floating-point stack over/underflow") ;

    case EXCEPTION_FLT_UNDERFLOW :
         return ("floating-point underflow") ;

    case EXCEPTION_INT_DIVIDE_BY_ZERO :
         return ("integer divide by zero") ;

    case EXCEPTION_INT_OVERFLOW :
         return ("integer overflow") ;

    case EXCEPTION_PRIV_INSTRUCTION :
         return ("execution of privileged instruction") ;

    case EXCEPTION_IN_PAGE_ERROR :
         return ("page error") ;

    case EXCEPTION_ILLEGAL_INSTRUCTION :
         return ("execution of illegal instruction") ;

    case EXCEPTION_NONCONTINUABLE_EXCEPTION :
         return ("continuation after noncontinuable exception") ;

    case EXCEPTION_STACK_OVERFLOW :
         return ("stack overflow") ;

    case EXCEPTION_INVALID_DISPOSITION :
         return ("invalid disposition") ;

    case EXCEPTION_GUARD_PAGE :
         return ("guard page") ;

    case EXCEPTION_INVALID_HANDLE :
         return ("invalid handle") ;

    default :
         return ("Unknown exception code") ;
  }
}

// returned value is in microseconds
__int64 GetCPUTime (bool Kernel = true, bool User = true)
{
  __int64     kt ;
  __int64     ut ;
  __int64     total = 0 ;
  FILETIME    ct ;
  FILETIME    et ;

  if (IsWNT)
  {
    if (!GetProcessTimes (GetCurrentProcess (), &ct, &et, (FILETIME *) &kt, (FILETIME *) &ut))
    {
      assert (false) ;
      return (0) ;
    }
    if (Kernel)
      total += kt ;
    if (User)
      total += ut ;
    return (total / 10) ;
  }
  else
  {
    // have to simulate the results for now
    // TODO: handle pause time
    ut = clock () ;
    if (User)
      total += ut * 1000 ;
    return (total) ;
  }
}

void mem_debug_output (char *format, ...)
{
  char                  str [2048] ;
  char                  *s ;
  time_t                t ;
  va_list               arg_ptr ;
  static FILE           *f = NULL ;

  if (format == NULL)
  {
    if (f)
      fclose (f) ;
    f = NULL ;
    return ;
  }
  if (f == NULL)
    if ((f = fopen ("mem_debug.log", "wt")) == NULL)
      return ;
  if (strlen (format) > sizeof (str) - 256)
    return ;
  time (&t) ;
  s = ctime (&t) ;
  memcpy (str, s + 11, 9) ;
  va_start (arg_ptr, format) ;
  vsprintf (str + 9, format, arg_ptr) ;
  va_end (arg_ptr) ;
  fputs (str, f) ;
}

#ifdef WIN_MEM_TRACKING

void *WIN_pov_malloc(size_t size, char *file, int line, char *msg)
{
  LZTimerOn () ;
  void *ptr = pov_malloc(size, file, line, msg) ;
  LZTimerOff () ;
  mem_debug_output ("malloc %p %d '%s' %I64u\n", ptr, size, msg, LZTimerRawCount ()) ;
  return (ptr) ;
}

void *WIN_pov_calloc(size_t nitems, size_t size, char *file, int line, char *msg)
{
  LZTimerOn () ;
  void *ptr = pov_calloc(nitems, size, file, line, msg) ;
  LZTimerOff () ;
  mem_debug_output ("calloc %p %d %d '%s' %I64u\n", ptr, nitems, size, msg, LZTimerRawCount ()) ;
  return (ptr) ;
}

void *WIN_pov_realloc(void *ptr, size_t size, char *file, int line, char *msg)
{
  LZTimerOn () ;
  void *p = pov_realloc(ptr, size, file, line, msg) ;
  LZTimerOff () ;
  mem_debug_output ("realloc %p %p %d '%s' %I64u\n", p, ptr, size, msg, LZTimerRawCount ()) ;
  return (p) ;
}

char *WIN_pov_strdup(char *str)
{
  LZTimerOn () ;
  char *s = pov_strdup (str) ;
  LZTimerOff () ;
  mem_debug_output ("strdup %p %p %d %I64u\n", s, str, strlen (str) + 1, LZTimerRawCount ()) ;
  return (s) ;
}

void WIN_pov_free(void *ptr, char *file, int line)
{
  LZTimerOn () ;
  pov_free(ptr, file, line) ;
  LZTimerOff () ;
  mem_debug_output ("free %p %I64u\n", ptr, LZTimerRawCount ()) ;
}

void WIN_mem_init()
{
  LZTimerOn () ;
  mem_init () ;
  LZTimerOff () ;
  mem_debug_output ("mem_init %I64u\n", LZTimerRawCount ()) ;
}

void WIN_mem_release_all()
{
  LZTimerOn () ;
  mem_release_all () ;
  LZTimerOff () ;
  mem_debug_output ("mem_release_all %I64u\n", LZTimerRawCount ()) ;
}

#endif // #ifdef WIN_MEM_TRACKING

char *PPS_String (unsigned pixels, unsigned renderseconds)
{
  static char str [128] ;

  if (rendersleep)
    return ("PAUSED") ;
  
  if (pixels / renderseconds < 5)
  {
    if (pixels * 60 / renderseconds < 5)
      sprintf (str, "%u PPH", pixels * 3600 / renderseconds) ;
    else
      sprintf (str, "%u PPM", pixels * 60 / renderseconds) ;
  }
  else
    sprintf (str, "%u PPS", pixels / renderseconds) ;
  return (str) ;
}

void PrintRenderTimes (int Finished, int NormalCompletion)
{
  unsigned    STT = SleepTimeTotal ;

  if (rendersleep)
  {
    SleepTimeEnd = clock () ;
    if (Finished)
    {
      SleepTimeTotal += SleepTimeEnd - SleepTimeStart ;
      STT = SleepTimeTotal ;
      rendersleep = false ;
    }
    else
      STT += SleepTimeEnd - SleepTimeStart ;
  }
  KernelTimeEnd = GetCPUTime (true, false) ;
  UserTimeEnd = GetCPUTime (false, true) ;
  KernelTimeTotal = KernelTimeEnd - KernelTimeStart ;
  UserTimeTotal = UserTimeEnd - UserTimeStart ;
  CPUTimeTotal = UserTimeTotal + KernelTimeTotal ;
  ClockTimeEnd = clock () ;
  ClockTimeTotal = ClockTimeEnd - ClockTimeStart - STT ;
  if (ClockTimeTotal >= CLOCKS_PER_SEC)
    status_printf (StatusPPS, PPS_String (pixels, ClockTimeTotal / CLOCKS_PER_SEC)) ;
  say_status_message (StatusRendertime, get_elapsed_time (ClockTimeTotal / CLOCKS_PER_SEC)) ;
  if (IsWNT != 0 && Finished != 0)
  {
    frontEnd->PrintToStatisticsStream ("CPU time used: kernel %.02f seconds, user %.02f seconds, total %.02f seconds\n",
                                       KernelTimeTotal / 1000000.0, UserTimeTotal / 1000000.0, CPUTimeTotal / 1000000.0) ;
    if (render_width * render_height > 0 && CPUTimeTotal > 0 && NormalCompletion)
    {
      char str [2048] ;
      sprintf (str, "Render averaged %.02f PPS over %u pixels\n", (double) (render_width * render_height) / (CPUTimeTotal / 1000000.0), render_width * render_height) ;
      frontEnd->PrintToStatisticsStream ("%s", str) ;
      frontEnd->PrintToStatisticsStream ("----------------------------------------------------------------------------\n") ;
      if (running_benchmark)
        MessageBox (main_window, str, "Benchmark Complete", MB_OK | MB_ICONINFORMATION) ;
    }
    else
      frontEnd->PrintToStatisticsStream ("----------------------------------------------------------------------------\n") ;
  }
}

bool OkToStopRendering (void)
{
  int total = (clock () - ClockTimeStart) / CLOCKS_PER_SEC ;
  if (total < CONFIRM_STOP_THRESHOLD)
    return (true) ;
  if (MessageBox (main_window, "You've been running this render for quite a while - really cancel ?", "Stop rendering ?", MB_ICONQUESTION | MB_YESNO) == IDYES)
    return (true) ;
  return (false) ;
}

void menuhelp (UINT idCommand)
{
  switch (idCommand)
  {
    case CM_FILEMENUHELP :
         hh_aklink.pszKeywords = "File Menu" ;
         break ;

    case CM_EDITMENUHELP :
         hh_aklink.pszKeywords = "Edit Menu" ;
         break ;

    case CM_SEARCHMENUHELP :
         hh_aklink.pszKeywords = "Search Menu" ;
         break ;

    case CM_TEXTMENUHELP :
         hh_aklink.pszKeywords = "Text Menu" ;
         break ;

    case CM_EDITORMENUHELP :
         hh_aklink.pszKeywords = "Editor Menu" ;
         break ;

    case CM_RENDERMENUHELP :
         hh_aklink.pszKeywords = "Render Menu" ;
         break ;

    case CM_OPTIONSMENUHELP :
         hh_aklink.pszKeywords = "Options Menu" ;
         break ;

    case CM_TOOLSMENUHELP :
         hh_aklink.pszKeywords = "Tools Menu" ;
         break ;

    case CM_GUIEXTMENUHELP :
         hh_aklink.pszKeywords = "GUI Extensions Menu" ;
         break ;

    case CM_RENDERWINMENUHELP :
         hh_aklink.pszKeywords = "Render Window Context Menu" ;
         break ;

    case CM_MESSAGEWINMENUHELP :
         hh_aklink.pszKeywords = "Message Window Context Menu" ;
         break ;

    default :
         hh_aklink.pszKeywords = NULL ;
         break ;
  }
  if (hh_aklink.pszKeywords != NULL)
    HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
}

// example taken from MSDN documentation
DWORD GetDllVersion (LPCTSTR lpszDllName)
{
  HINSTANCE hinstDll;
  DWORD dwVersion = 0;
  
  hinstDll = LoadLibrary(lpszDllName) ;
    
  if (hinstDll)
  {
    DLLGETVERSIONPROC pDllGetVersion;
    
    pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress (hinstDll, "DllGetVersion");
    
    if (pDllGetVersion)
    {
      DLLVERSIONINFO dvi;
      HRESULT hr;
      
      ZeroMemory(&dvi, sizeof (dvi));
      dvi.cbSize = sizeof (dvi);
      
      hr = (*pDllGetVersion) (&dvi);
      
      if (SUCCEEDED (hr))
        dwVersion = MAKELONG (dvi.dwMajorVersion, dvi.dwMinorVersion);
    }
    
    FreeLibrary (hinstDll);
  }
  return dwVersion;
}

void getvars (ExternalVarStruct *v)
{
  strcpy (v->command_line, command_line) ;
  strcpy (v->source_file_name, source_file_name) ;
  strcpy (v->lastRenderName, lastRenderName) ;
  strcpy (v->lastRenderPath, lastRenderPath) ;
  strcpy (v->lastQueuePath, lastQueuePath) ;
  strcpy (v->lastSecondaryIniFilePath, lastSecondaryIniFilePath) ;
  strcpy (v->DefaultRenderIniFileName, DefaultRenderIniFileName) ;
  strcpy (v->SecondaryRenderIniFileName, SecondaryRenderIniFileName) ;
  strcpy (v->SecondaryRenderIniFileSection, SecondaryRenderIniFileSection) ;
  strcpy (v->ourPath, modulePath) ;
  strcpy (v->engineHelpPath, engineHelpPath) ;
  strcpy (v->rendererHelpPath, "") ;
  strcpy (v->HomePath, BinariesPath) ;
  strcpy (v->EngineIniFileName, EngineIniFileName) ;
  strcpy (v->ToolIniFileName, ToolIniFileName) ;
  memcpy (v->queued_files, queued_files, sizeof (v->queued_files)) ;
  v->loadRerun = loadRerun ;
  v->continueRerun = continueRerun ;
  v->povray_return_code = povray_return_code ;
  v->rendering = rendering ;
  v->IsWin32 = true ;
  v->IsW95UserInterface = IsW95UserInterface ;
  v->running_demo = running_demo ;
  v->debugging = debugging ;
  v->isMaxiMinimized = false ;
  v->newVersion = newVersion ;
  v->use_threads = true ;
  v->use_toolbar = use_toolbar ;
  v->use_tooltips = use_tooltips ;
  v->use_editors = use_editors ;
  v->drop_to_editor = drop_to_editor ;
  v->rendersleep = rendersleep ;
  v->ExtensionsEnabled = ExtensionsEnabled ;
  v->queued_file_count = queued_file_count > OLD_MAX_QUEUE ? OLD_MAX_QUEUE : queued_file_count ;
  v->auto_render = auto_render ;
}

void setvars (ExternalVarStruct *v)
{
  strncpy (command_line, v->command_line, sizeof (command_line) - 1) ;
  SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
  strncpy (source_file_name, v->source_file_name, sizeof (source_file_name) - 1) ;
  strncpy (lastRenderName, v->lastRenderName, sizeof (lastRenderName) - 1) ;
  strncpy (lastRenderPath, v->lastRenderPath, sizeof (lastRenderPath) - 1) ;
  strncpy (lastQueuePath, v->lastQueuePath, sizeof (lastQueuePath) - 1) ;
  strncpy (lastSecondaryIniFilePath, v->lastSecondaryIniFilePath, sizeof (lastSecondaryIniFilePath) - 1) ;
  strncpy (DefaultRenderIniFileName, v->DefaultRenderIniFileName, sizeof (DefaultRenderIniFileName) - 1) ;
  strncpy (SecondaryRenderIniFileName, v->SecondaryRenderIniFileName, sizeof (SecondaryRenderIniFileName) - 1) ;
  strncpy (SecondaryRenderIniFileSection, v->SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection) - 1) ;
}

bool HaveWin95OrLater (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ;
}

bool HaveWin98OrLater (void)
{
  if (version_info.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
    return (false) ;
  if (version_info.dwMajorVersion < 4)
    return (false) ;
  if (version_info.dwMajorVersion > 4)
    return (true) ;
  return (version_info.dwMinorVersion > 0) ;
}

bool HaveNT4OrLater (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT && version_info.dwMajorVersion >= 4) ;
}

bool HaveWin2kOrLater (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT && version_info.dwMajorVersion >= 5) ;
}

bool HaveWinXPOrLater (void)
{
  if (version_info.dwPlatformId != VER_PLATFORM_WIN32_NT || version_info.dwMajorVersion < 5)
    return (false) ;
  return (version_info.dwMajorVersion > 5 || (version_info.dwMajorVersion == 5 && version_info.dwMinorVersion > 0)) ;
}

#ifdef __WATCOMC__                        /* Watcom C/C++ C32 */
void Fix_Watcom_Bug(char *s)
{
  char *p=s;

  while(*p != '\0')
  {
    if (*p==' ')
    {
      *p='0';
    }
    p++;
  }
}
#endif

void set_render_priority (unsigned priority)
{
  switch (priority)
  {
    case CM_RENDERPRIORITY_LOW :
         SetPriorityClass (GetCurrentProcess (), IDLE_PRIORITY_CLASS) ;
         break ;

    case CM_RENDERPRIORITY_NORMAL :
         SetPriorityClass (GetCurrentProcess (), NORMAL_PRIORITY_CLASS) ;
         break ;

    case CM_RENDERPRIORITY_HIGH :
         SetPriorityClass (GetCurrentProcess (), HIGH_PRIORITY_CLASS) ;
         break ;
  }
}

void display_cleanup (void)
{
  if (render_window != NULL)
  {
    DestroyWindow (render_window) ;
    render_window = NULL ;
    renderwin_destroyed = false ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  }
  if (!rendering_animation || !preserve_bitmap)
  {
    if (render_bitmap_surface != NULL)
    {
      free (render_bitmap_surface) ;
      render_bitmap_surface = NULL ;
      PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
    }
  }
}

// we can't allow LoadBitmap to load our background bitmaps 'cause if we're running
// a 256-colour mode, it will map the incoming resource to 16 colours ...
// LoadImage () doesn't exist under Win32s, either. sigh.
HBITMAP NonBogusLoadBitmap (HINSTANCE hInst, LPSTR lpszBitmap)
{
  void        *p ;
  HRSRC       hres ;
  HGLOBAL     hg ;
  HBITMAP     hBitmap ;

  if ((hres = FindResource (hInst, lpszBitmap, RT_BITMAP)) == NULL)
    return (NULL) ;
  if ((hg = LoadResource (hInst, hres)) == NULL)
    return (NULL) ;
  if ((p = LockResource (hg)) == NULL)
  {
    FreeResource (hg) ;
    return (NULL) ;
  }
  hBitmap = lpDIBToBitmap (p, hPalApp) ;
  FreeResource (hg) ;
  return (hBitmap) ;
}

HBITMAP NonBogusLoadBitmapAndPalette (HINSTANCE hInst, LPSTR lpszBitmap)
{
  void        *p ;
  HRSRC       hres ;
  HGLOBAL     hg ;
  HBITMAP     hBitmap ;

  if ((hres = FindResource (hInst, lpszBitmap, RT_BITMAP)) == NULL)
    return (NULL) ;
  if ((hg = LoadResource (hInst, hres)) == NULL)
    return (NULL) ;
  if ((p = LockResource (hg)) == NULL)
  {
    FreeResource (hg) ;
    return (NULL) ;
  }
  hBitmap = lpDIBToBitmapAndPalette (p) ;
  FreeResource (hg) ;
  return (hBitmap) ;
}

void SplashScreen (HWND hwnd)
{
  BITMAP                bm ;

  if ((hBmpSplash = NonBogusLoadBitmapAndPalette (hInstance, MAKEINTRESOURCE (BMP_SPLASH_8BPP))) != NULL)
  {
    GetObject (hBmpSplash, sizeof (BITMAP), (LPSTR) &bm) ;
    splash_width = bm.bmWidth ;
    splash_height = bm.bmHeight ;
    splash_palette = hPalBitmap ;
    splash_window = CreateWindowEx (0, //WS_EX_TOOLWINDOW,
                                    PovSplashWinClass,
                                    "POV-Ray",
                                    WS_POPUP,
                                    (screen_width - splash_width) / 2,
                                    (screen_height - splash_height) / 2,
                                    splash_width,
                                    splash_height,
                                    hwnd,
                                    NULL,
                                    hInstance,
                                    NULL) ;
    CenterWindowRelative (hwnd, splash_window, false, true) ;
    ShowWindow (splash_window, SW_SHOWNORMAL) ;
  }
}

void validatePath (char *s)
{
  if (s [1] == ':' && strlen (s) < 4)
    return ;
  s += strlen (s) - 1 ;
  if (*s == '\\')
    *s = '\0' ;
}

int joinPath (char *out, char *path, char *name)
{
  strcpy (out, path) ;
  if (path[0] != '\0' && path [strlen (path) - 1] != '\\')
    strcat (out, "\\") ;
  strcat (out, name) ;
  return ((int) strlen (out)) ;
}

void setRunOnce (void)
{
#ifndef NOSETRUNONCE
  char        str [_MAX_PATH] ;
  HKEY        key ;
  DWORD       result ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER,
                      "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
                      0,
                      "",
                      REG_OPTION_NON_VOLATILE,
                      KEY_WRITE,
                      NULL,
                      &key,
                      &result) == ERROR_SUCCESS)
  {
    GetModuleFileName (hInstance, str, sizeof (str)) ;
    RegSetValueEx (key, "POV-Ray for Windows", 0, REG_SZ, (BYTE *) str, (int) strlen (str) + 1) ;
    RegCloseKey (key) ;
  }
#endif
}

bool reg_printf (bool useHKCU, char *keyName, char *valName, char *format, ...)
{
  char                  str [2048] ;
  HKEY                  hKey ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return (false) ;
  if (RegCreateKeyEx (useHKCU ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    va_start (arg_ptr, format) ;
    vsprintf (str, format, arg_ptr) ;
    va_end (arg_ptr) ;
    RegSetValueEx (hKey, valName, 0, REG_SZ, (BYTE *) str, (int) strlen (str) + 1) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

// conditional version of reg_printf
bool cond_reg_printf (char *keyName, char *valName, char *format, ...)
{
  char                  str [2048] ;
  DWORD                 len = sizeof (str) ;
  HKEY                  hKey ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return (false) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (hKey, valName, 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
    {
      RegCloseKey (hKey) ;
      // it already exists - if it doesn't have zero length we don't update it
      if (str [0])
        return (true) ;
    }
    else
      RegCloseKey (hKey) ;
  }
  if (RegCreateKeyEx (HKEY_CURRENT_USER, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    va_start (arg_ptr, format) ;
    vsprintf (str, format, arg_ptr) ;
    va_end (arg_ptr) ;
    RegSetValueEx (hKey, valName, 0, REG_SZ, (BYTE *) str, (int) strlen (str) + 1) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

static bool reg_dword (char *keyName, char *valName, DWORD value)
{
  HKEY                  hKey ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    RegSetValueEx (hKey, valName, 0, REG_DWORD, (BYTE *) &value, 4) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

char *GetInstallTime (void)
{
  HKEY        key ;
  DWORD       len ;
  static char str [64] ;

  len = sizeof (str) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, "InstallTime", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
    {
      RegCloseKey (key) ;
      return (str) ;
    }
    RegCloseKey (key) ;
  }
  return (NULL) ;
}

bool checkRegKey (void)
{
  char        str [MAX_PATH] ;
  __int64     file_time ;
  HKEY        key ;
  DWORD       len = sizeof (str) ;
  SYSTEMTIME  system_time ;

  if (GetInstallTime () == NULL)
  {
    GetSystemTime (&system_time) ;
    SystemTimeToFileTime (&system_time, (FILETIME *) &file_time) ;
    reg_printf (true, "Software\\" REGKEY, "InstallTime", "%I64u", file_time) ;
  }

  if (RegOpenKeyEx (HKEY_CURRENT_USER,
                    "Software\\" REGKEY "\\" REGVERKEY "\\Windows",
                    0,
                    KEY_READ,
                    &key) == ERROR_SUCCESS)
  {
    RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) str, &len) ;
    RegCloseKey (key) ;
    if (str [0] == '\0')
      return (false) ;
  }
  else
    return (false) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER,
                    "Software\\" REGKEY "\\CurrentVersion\\Windows",
                    0,
                    KEY_READ | KEY_WRITE,
                    &key) == ERROR_SUCCESS)
  {
    RegQueryValueEx (key, VERSIONVAL, 0, NULL, (BYTE *) str, &len) ;
    if (strcmp (str, POV_RAY_VERSION) != 0)
      RegSetValueEx (key, VERSIONVAL, 0, REG_SZ, (BYTE *) POV_RAY_VERSION, (int) strlen (POV_RAY_VERSION) + 1) ;
    RegCloseKey (key) ;
  }
  else
    return (false) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER,
                    "Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit",
                    0,
                    KEY_READ,
                    &key) == ERROR_SUCCESS)
  {
    RegCloseKey (key) ;
    return (true) ;
  }
  return (false) ;
}

bool getHome (void)
{
  HKEY        key ;
  DWORD       len ;

  if (debugging)
    debug ("querying registry\n") ;
  BinariesPath [0] = '\0' ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    len = sizeof (LastInferredHome) ;
    RegQueryValueEx (key, "LastInferredHome", 0, NULL, (BYTE *) LastInferredHome, &len) ;
    len = sizeof (BinariesPath) ;
    RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) BinariesPath, &len) ;
    RegCloseKey (key) ;
    if (debugging)
      debug ("Win32 getHome () succeeded (HKCU), HomePath is '%s'\n", BinariesPath) ;
    return (true) ;
  }
  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                    "Software\\" REGKEY "\\" REGVERKEY "\\Windows",
                    0,
                    KEY_READ,
                    &key) == ERROR_SUCCESS)
  {
    RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) BinariesPath, &len) ;
    RegCloseKey (key) ;
    if (debugging)
      debug ("Win32 getHome () succeeded (HKLM), HomePath is '%s'\n", BinariesPath) ;
    return (true) ;
  }
  return (false) ;
}

bool inferHome (void)
{
  char        exePath [_MAX_PATH] ;
  char        *s ;

  if (GetModuleFileName (NULL, exePath, _MAX_PATH) == 0)
    return (false) ;

  // find path component
  if ((s = strrchr (exePath, '\\')) == NULL)
    return (false) ;
  *s = '\0' ;
  
  // now step up one directory
  if ((s = strrchr (exePath, '\\')) == NULL)
    return (false) ;
  *++s = '\0' ;

  // now look for some standard directories
  strcpy (s, "help") ;
  if (!dirExists (exePath))
  {
    strcpy (s, "sounds") ;
    if (!dirExists (exePath))
    {
      strcpy (s, "tiles") ;
      if (!dirExists (exePath))
        return (false) ;
    }
  }
  *s = '\0' ;
  strcpy (BinariesPath, exePath) ;
  homeInferred = true ;
  return (true) ;
}

bool cloneRegistry (void)
{
  HKEY        key1 ;
  DWORD       result ;
  DWORD       value ;

  if (debugging)
    debug ("attempting to create registry entries\n") ;
  if (!homeInferred)
    if (!reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "Home", "%s", BinariesPath))
      return (false) ;

  if (!homeInferred)
    reg_printf (true, "Software\\" REGKEY "\\CurrentVersion\\Windows", "Home", "%s", BinariesPath) ;
  reg_printf (true, "Software\\" REGKEY "\\CurrentVersion\\Windows", VERSIONVAL, "%s", POV_RAY_VERSION) ;

  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open0",   "%sChanges.txt,1,1,0,0,8,2",                   BinariesPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent0", "%sChanges.txt,1,1,0,0,8,2",                   BinariesPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open1",   "%sRevision.txt,1,1,0,0,8,2",                  BinariesPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent1", "%sRevision.txt,1,1,0,0,8,2",                  BinariesPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open2",   "%sscenes\\advanced\\biscuit.pov,1,1,0,6,8,2", DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent2", "%sscenes\\advanced\\biscuit.pov,1,1,0,6,8,2", DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open3",   "%sscenes\\advanced\\woodbox.pov,1,1,0,6,8,2", DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent3", "%sscenes\\advanced\\woodbox.pov,1,1,0,6,8,2", DocumentsPath) ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit", 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key1, &result) != ERROR_SUCCESS)
    return (false) ;
  value = 3 ;
  RegSetValueEx (key1, "CurrentTab", 0, REG_DWORD, (BYTE *) &value, sizeof (value)) ;
  RegCloseKey (key1) ;
  return (true) ;
}

char *getCommandLine (void)
{
  HKEY        key ;
  static char str [2048] ;
  DWORD       len = sizeof (str) ;

  str [0] = '\0' ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    RegQueryValueEx (key, "Command Line", 0, NULL, (BYTE *) str, &len) ;
    RegCloseKey (key) ;
  }
  return (str) ;
}

int parse_commandline (char *s)
{
  char        *prevWord = NULL ;
  char        inQuote = '\0' ;
  static char str [_MAX_PATH * 3] ;
  static char filename [_MAX_PATH] ;

  argc = 0 ;
  GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
  argv [argc++] = filename ;
  s = strncpy (str, s, sizeof (str) - 1) ;
  while (*s)
  {
    switch (*s)
    {
      case '"' :
      case '\'' :
           if (inQuote)
           {
             if (*s == inQuote)
               inQuote = 0 ;
           }
           else
           {
             inQuote = *s ;
             if (prevWord == NULL)
               prevWord = s ;
           }
           break ;

      case ' ' :
      case '\t' :
           if (!inQuote)
           {
             if (prevWord != NULL)
             {
               *s = '\0' ;
               argv [argc++] = prevWord ;
               prevWord = NULL ;
             }
           }
           break ;

      default :
           if (prevWord == NULL)
             prevWord = s ;
           break ;
    }
    if (argc >= MAX_ARGV - 1)
      break ;
    s++ ;
  }
  if (prevWord != NULL && argc < MAX_ARGV - 1)
    argv [argc++] = prevWord ;
  argv [argc] = NULL ;
  return (argc) ;
}

bool StripPathComponent (char *path, int number)
{
  if (number > 1)
    if (!StripPathComponent (path, number - 1))
      return (false) ;
  char *s = path + strlen (path) - 1 ;
  if (*s == '\\')
    *s-- = '\0' ;
  while (s > path && *s != '\\')
    s-- ;
  *s = '\0' ;
  return (path [0] != '\0') ;
}

int InstallSettings (char *args, bool quiet)
{
  char        base [_MAX_PATH] ;
  char        oldbase [_MAX_PATH] ;
  char        str [_MAX_PATH] ;
  char        *s ;

  // we attempt to infer install dir if it's not supplied
  if (args == NULL)
  {
    if (getcwd (base, sizeof (base) - 1) == NULL)
    {
      if (!quiet)
        MessageBox (NULL, "Could not get current directory - cannot infer home path. Please supply it on the command-line",
                          "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      return (5) ;
    }
    if (strlen (base) < 3 || (strlen (base) == 3 && base [1] == ':' && base [2] == '\\'))
    {
      if (!quiet)
      {
        sprintf (str, "Current dir '%s' is root - cannot infer home path. Please supply it on the command-line.", base) ;
        MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      }
      return (10) ;
    }
    if (!StripPathComponent (base, 1) || strcmp (base, "\\\\") == 0)
    {
      if (!quiet)
        MessageBox (NULL, "Cannot infer home path. Please supply it on the command-line.",
                          "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      return (15) ;
    }
  }
  else
  {
    strcpy (str, args) ;
    s = str + strlen (str) - 1 ;
    if (*s == '\\')
      *s = '\0' ;
    strcpy (oldbase, str) ;
    if (GetFullPathName (str, sizeof (base), base, &s) == 0)
    {
      sprintf (str, "GetFullPathName() for '%s' failed [0x%08x]", oldbase, GetLastError ()) ;
      MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    }
  }
  strcpy (oldbase, base) ;
  if (!dirExists (base))
  {
    if (!quiet)
    {
      sprintf (str, "Could not stat '%s'", base) ;
      MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    }
    return (20) ;
  }
  sprintf (str, "%s\\ini\\pvengine.ini", base) ;
  if (!fileExists (str))
  {
    if (args == NULL && GetModuleFileName (NULL, base, _MAX_PATH) != 0 && StripPathComponent (base, 3))
    {
      if (!quiet)
      {
        sprintf (str, "Could not find the following file:\n  '%s\\ini\\pvengine.ini'\nTrying module path.", oldbase) ;
        MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      }
      return (InstallSettings (base, quiet)) ;
    }
    if (!quiet)
    {
      sprintf (str, "Could not find the following file:\n  '%s\\ini\\pvengine.ini'\nCannot install settings.", oldbase) ;
      MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    }
    return (25) ;
  }
  sprintf (str, "%s\\ini\\povray.ini", base) ;
  if (!fileExists (str))
  {
    if (args == NULL && GetModuleFileName (NULL, base, _MAX_PATH) != 0 && StripPathComponent (base, 3))
    {
      if (!quiet)
      {
        sprintf (str, "Could not find the following file:\n  '%s\\renderer\\povray.ini'\nTrying module path.", oldbase) ;
        MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      }
      return (InstallSettings (base, quiet)) ;
    }
    if (!quiet)
    {
      sprintf (str, "Could not find the following file:\n  '%s\\renderer\\povray.ini'\nCannot install settings.", oldbase) ;
      MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    }
    return (30) ;
  }
  if (!reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "Home", "%s", base))
  {
    if (!quiet)
      MessageBox (NULL, "Failed to write to HKCU in registry", "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    return (35) ;
  }

  // it's ok for this to fail as they may not have administrative rights
  reg_printf (false, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "Home", "%s", base) ;

  if (!quiet)
  {
    sprintf (str, "[Home path is %s]\n\nSuccess!", base) ;
    MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONINFORMATION) ;
  }
  return (0) ;
}

void CheckFreshInstall (void)
{
#if 0
  char        str [256] ;
#endif
  HKEY        hKey ;
  DWORD       len = 4 ;
  DWORD       val = 0 ;

  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (hKey, "FreshInstall", 0, NULL, (BYTE *) &val, &len) == ERROR_SUCCESS)
    {
      if (len == 4 && val != 0)
      {
        // do any other special actions we want to do after a fresh install.
        // currently we don't do anything in particular.
        WasFreshInstall = true ;
        RegDeleteValue (hKey, "FreshInstall") ;
      }
    }
    RegCloseKey (hKey) ;
  }
}

int execute_tool (char *s)
{
  int                   error ;
  STARTUPINFO           startupInfo ;
  PROCESS_INFORMATION   procInfo ;

  if (strlen (s) == 0)
  {
    PovMessageBox ("No command to run!", "Tool Error") ;
    return (0) ;
  }

  if (*s == '$')
  {
    switch (toupper (s [1]))
    {
      case 'S' :
           s += 2 ;
           while (*s == ' ')
             s++ ;
           if (strlen (s) == 0)
           {
             PovMessageBox ("No file to open!", "Tool Error") ;
             return (0) ;
           }
           if ((error = PtrToInt (ShellExecute (main_window, "open", s, NULL, NULL, SW_SHOWNORMAL))) <= 32)
             PovMessageBox ("ShellExecute failed", "Tool Error") ;
           return (error) ;
    }
  }

  startupInfo.cb               = sizeof (STARTUPINFO) ;
  startupInfo.lpReserved       = 0 ;
  startupInfo.lpDesktop        = NULL ;
  startupInfo.lpTitle          = NULL ;
  startupInfo.dwX              = 0 ;
  startupInfo.dwY              = 0 ;
  startupInfo.dwXSize          = 0 ;
  startupInfo.dwYSize          = 0 ;
  startupInfo.dwXCountChars    = 0 ;
  startupInfo.dwYCountChars    = 0 ;
  startupInfo.dwFillAttribute  = 0 ;
  startupInfo.dwFlags          = STARTF_USESHOWWINDOW ;
  startupInfo.wShowWindow      = SW_SHOW ;
  startupInfo.cbReserved2      = 0 ;
  startupInfo.lpReserved2      = 0 ;

  if (CreateProcess (NULL, s, NULL, NULL, false, 0, NULL, NULL, &startupInfo, &procInfo) == false)
  {
    error = GetLastError () ;
    PovMessageBox ("Could not run program", "Tool Error") ;
    return (error) ;
  }

  // clean up
  CloseHandle (procInfo.hProcess) ;
  CloseHandle (procInfo.hThread) ;

  return (0) ;
}

void RenderInsertMenu (void)
{
  int         val ;
  char        str [_MAX_PATH] ;
  char        *s1 ;
  char        *s2 ;
  FILE        *f ;

  stop_rendering = false ;
  sprintf (str, "%sInsert Menu\\Images.ini", DocumentsPath) ;
  if ((f = fopen (str, "rt")) == NULL)
  {
    MessageBox (main_window, "Cannot open 'Images.ini' in Insert Menu directory", "Insert Menu Images", MB_OK | MB_ICONEXCLAMATION) ;
    return ;
  }
  InsertMenuSection = InsertMenuSectionCount = 0 ;
  while (fgets (str, sizeof (str), f) != NULL)
  {
    s1 = clean (str) ;
    if (*s1 == '[')
    {
      if ((s2 = strchr (s1, ']')) != NULL)
      {
        *s2  = '\0' ;
        val = atoi (++s1) ;
        if (val == 0)
          continue ;
        InsertMenuSections [InsertMenuSectionCount++] = val ;
        if (InsertMenuSectionCount == sizeof (InsertMenuSections) / sizeof (int))
          break ;
      }
    }
  }
  fclose (f) ;
  if (InsertMenuSectionCount == 0)
  {
    MessageBox (main_window, "No insert menu sections found in 'Images.ini'", "Insert Menu Images", MB_OK | MB_ICONSTOP) ;
    return ;
  }
  sprintf (str, "There are %d insert menu images to render. Press OK to start rendering these now.\n\n"
                "Once the render has started you can press the 'Stop Rendering' button to cancel the render job.", InsertMenuSectionCount) ;
  if (MessageBox (main_window, str, "Insert Menu Images", MB_OKCANCEL | MB_ICONINFORMATION) == IDCANCEL)
    return ;
  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  update_menu_for_render (true) ;
  rendering_insert_menu = was_insert_render = no_status_output = true ;
  EditShowMessages (true) ;
  CalculateClientWindows (true) ;
  ShowWindow (message_window, SW_SHOW) ;
  sprintf (str, "%sInsert Menu", DocumentsPath) ;
  SetCurrentDirectory (str) ;
  StartInsertRender = true ;
}

void cancel_render (void)
{
  stop_rendering = true ;
  if (frontEnd->GetState () > WinRenderFrontend::kReady)
  {
    try
    {
      frontEnd->StopRender () ;
    }
    catch (int)
    {
      if (rendering)
        MessageBox (NULL, "Failed to send stop rendering message", "POVMS error", MB_OK | MB_ICONEXCLAMATION) ;
    }
  }
}

bool start_rendering (bool ignore_source_file)
{
  int                   i ;
  int                   err ;
  int                   w ;
  int                   h ;
  int                   l ;
  char                  str [_MAX_PATH + 32] ;
  char                  filename [_MAX_PATH] ;
  char                  rerunfile [_MAX_PATH] ;
  char                  section [16] ;
  POVMSObject           obj ;

  if (backend_thread_exception)
  {
    MessageBox (main_window, "Cannot start render as the backend thread "
                             "has been shut down due to a previous exception.",
                             "Error", MB_ICONSTOP) ;
    if (restore_command_line)
    {
      strcpy (command_line, old_command_line) ;
      SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (UINT_PTR) command_line) ;
      restore_command_line = false ;
    }
    loadRerun = continueRerun = 0 ;
    running_benchmark = running_demo = false ;
    return (false) ;
  }

  ErrorMessage [0] = '\0' ;
  ErrorFilename [0] = '\0' ;
  status_buffer [0] = '\0' ;
  povray_return_code = 0 ;
  renderwin_destroyed = false ;
  rendersleep = false ;
  SleepTimeTotal = 0 ;
  render_anim_count = 0 ;
  renderwin_hidden = renderwin_manually_closed = false ;
  rendering_animation = false ;
  stop_rendering = false ;
  was_insert_render = false ;
  percentage_complete = 0 ;
  first_frame = true ;
  render_width = render_height = 0 ;
  Frame.Screen_Width = Frame.Screen_Height = 0 ;
  KernelTimeStart = GetCPUTime (true, false) ;
  UserTimeStart = GetCPUTime (false, true) ;
  CPUTimeTotal = KernelTimeTotal = UserTimeTotal = 0 ;
  ClockTimeStart = clock () ;
  SleepTimeTotal = ClockTimeTotal = 0 ;
  status_printf (StatusPPS, "") ;
  say_status_message (StatusRendertime, "") ;
  pixels = 0 ;
  output_to_file = false ;

  say_status_message (StatusMessage, "") ;

  if (!StartInsertRender)
  {
    PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
    update_menu_for_render (true) ;
    SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
    if (!temp_render_region)
      if (RegionStr [0] != '\0' && strstr (command_line, RegionStr + 1) == NULL)
        RegionStr [0] = '\0' ;
  }

  try
  {
    // set up render options
    WinProcessRenderOptions options ;

    if ((err = POVMSObject_New (&obj, kPOVObjectClass_ROptions)) != kNoErr)
      throw (err) ;

    // This is the count used by the write statistics routine for the line number
    statistics_count = 0 ;

    if ((err = options.ParseFile (DefaultRenderIniFileName, &obj)) != kNoErr)
      throw (err) ;
    if (AutoAppendPaths)
    {
      sprintf (str, "Library_Path=\"%sinclude\"", DocumentsPath) ;
      options.ParseString (str, &obj, false) ;
      sprintf (str, "Library_Path=\"%s\"", FontPath) ;
      options.ParseString (str, &obj, false) ;
    }

    if (!StartInsertRender)
    {
      if (loadRerun)
      {
        strcpy (section, "Renderer") ;
        // if loadRerun == 1, load last failed render. otherwise, load render number
        // loadRerun - 2, where render number 0 is the most recent.
        switch (loadRerun)
        {
          case 1 :
              strcpy (rerunfile, CurrentRerunFileName) ;
              break ;

          default :
              sprintf (rerunfile, "%sRerun%02d.Ini", RerunIniPath, loadRerun - 2) ;
              break ;
        }

        loadRerun = 0 ;

        // turn off continue trace in case it's on, since it seems to confuse people.
        WritePrivateProfileString (section, "Continue_Trace", "Off", rerunfile) ;

        // flush the INI file
        WritePrivateProfileString (NULL, NULL, NULL, rerunfile) ;

        sprintf (str, "%s[%s]", rerunfile, section) ;
        if ((err = options.ParseFile (str, &obj)) != kNoErr)
          throw (err) ;

        if (continueRerun)
          if ((err = POVMSUtil_SetBool (&obj, kPOVAttrib_ContinueTrace, true)) != kNoErr)
            throw (err) ;
        continueRerun = 0 ;

        GetPrivateProfileString (section, "Input_File_Name", "", filename, sizeof (filename), rerunfile) ;
        if (strlen (filename))
        {
          wrapped_printf ("%s file '%s' using rerun information.", continueRerun ? "Continuing" : "Rendering", filename) ;
          GetPrivateProfileString ("Environment", "CurrentDirectory", "", dir, sizeof (dir), rerunfile) ;
          if (strlen (dir))
          {
            SetCurrentDirectory (dir) ;
            wrapped_printf ("Render directory is '%s'.", dir) ;
          }
        }
        else
        {
          message_printf ("Error : Specified rerun information not found.\n") ;
          update_menu_for_render (false) ;
          running_benchmark = running_demo = false ;
          return (false) ;
        }
      }
      else
      {
        // non-rerun processing
        if (running_demo == 0)
        {
          if (SecondaryRenderIniFileName [0] != '\0')
          {
            if (SecondaryRenderIniFileName [strlen (SecondaryRenderIniFileName) - 1] != '\\')
            {
              splitpath (SecondaryRenderIniFileName, NULL, str) ;
              if (str [0] != '\0')
              {
                if (SecondaryRenderIniFileSection [0] == '\0')
                  wrapped_printf ("Preset INI file is '%s'.", SecondaryRenderIniFileName) ;
                else
                  wrapped_printf ("Preset INI file is '%s', section is '%s'.", SecondaryRenderIniFileName, SecondaryRenderIniFileSection) ;
                sprintf (str, "%s%s", SecondaryRenderIniFileName, SecondaryRenderIniFileSection) ;
                if ((err = options.ParseFile (str, &obj)) != kNoErr)
                  throw (err) ;
              }
            }
          }

          if (!ignore_source_file && strlen (source_file_name) != 0)
          {
            wrapped_printf ("Preset source file is '%s'.", source_file_name) ;
            splitpath (source_file_name, dir, NULL) ;
            SetCurrentDirectory (dir) ;
            if (fileExists ("povray.ini"))
            {
              wrapped_printf ("Directory '%s' contains file 'povray.ini' - merging it.", dir) ;
              if ((err = options.ParseFile ("povray.ini", &obj)) != kNoErr)
                throw (err) ;
            }
            switch (get_file_type (source_file_name))
            {
              case filePOV :
              case fileINC :
                  if ((err = POVMSUtil_SetString (&obj, kPOVAttrib_InputFile, source_file_name)) != kNoErr)
                    throw (err) ;
                  break ;

              case fileINI :
                  if ((err = options.ParseFile (source_file_name, &obj)) != kNoErr)
                    throw (err) ;
                  break ;

              default :
                  message_printf ("POV-Ray for Windows doesn't recognize this file type ; assuming POV source.\n") ;
                  if ((err = POVMSUtil_SetString (&obj, kPOVAttrib_InputFile, source_file_name)) != kNoErr)
                    throw (err) ;
                  break ;
            }
          }
        }
      }

      if (RegionStr [0] != 0)
      {
        if (strstr (command_line, RegionStr) == NULL && strstr (command_line, RegionStr + 1) == NULL)
        {
          if (!running_demo)
            message_printf ("Selected render region is '%s'.\n", RegionStr + 1) ;
          if ((err = options.ParseString (RegionStr, &obj, false)) != kNoErr)
            throw (err) ;
        }
      }

      if (strlen (command_line))
      {
        if (!running_demo)
          wrapped_printf ("Rendering using command line '%s'.", command_line) ;
        if ((err = options.ParseString (command_line, &obj, false)) != kNoErr)
          throw (err) ;
      }

      i = sizeof (str) ;
      if ((err = POVMSUtil_GetString (&obj, kPOVAttrib_InputFile, str, &i)) != kNoErr)
      {
        message_printf ("No source file specified, either directly or via an INI file.\nDon't know what to render!\n") ;
        PovMessageBox ("No source file specified, either directly or via an INI file.", "Don't know what to render!") ;
        PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
        update_menu_for_render (false) ;
        if (restore_command_line)
        {
          strcpy (command_line, old_command_line) ;
          SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (UINT_PTR) command_line) ;
          restore_command_line = false ;
        }
        running_benchmark = running_demo = false ;
        return (false) ;
      }
      status_printf (StatusMessage, "Parsing %s", str) ;
    }
    else
    {
      // we are rendering the insert menu
      if (InsertMenuSection >= InsertMenuSectionCount)
      {
        PovMessageBox ("Insert menu render error - we should be stopped already!", "Internal Error") ;
        PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
        update_menu_for_render (false) ;
        if (restore_command_line)
        {
          strcpy (command_line, old_command_line) ;
          SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (UINT_PTR) command_line) ;
          restore_command_line = false ;
        }
        running_benchmark = running_demo = false ;
        return (false) ;
      }
      int section = InsertMenuSections [InsertMenuSection] ;
      sprintf (str, "Images.ini[%d]", section) ;
      if ((err = options.ParseFile (str, &obj)) != kNoErr)
        throw (err) ;
			if ((err = POVMSUtil_SetInt (&obj, kPOVAttrib_OutputFileType, 's')) != kNoErr)
        throw (err) ;
      status_printf (StatusMessage, "Rendering Insert Menu entry %d of %d", InsertMenuSection + 1, InsertMenuSectionCount) ;

      // see if the file is continuable - if it is turn on continue trace.
      str [0] = '\0' ;
      l = sizeof (str) ;
      if (POVMSUtil_GetString (&obj, kPOVAttrib_OutputFile, str, &l) == kNoErr)
        if (POVMSUtil_GetInt (&obj, kPOVAttrib_Width, &w) == kNoErr && POVMSUtil_GetInt (&obj, kPOVAttrib_Height, &h) == kNoErr)
          if (BMP_Is_Continuable (str, w, h))
            if ((err = POVMSUtil_SetBool (&obj, kPOVAttrib_ContinueTrace, true)) != kNoErr)
              throw (err) ;
    }

    POVMS_Object ropts (obj) ;
    if (homeInferred)
    {
      POVMS_List list ;
      if (ropts.Exist (kPOVAttrib_LibraryPath))
        ropts.Get (kPOVAttrib_LibraryPath, list) ;
      sprintf (str, "%sINCLUDE", DocumentsPath) ;
      POVMS_Attribute attrib (str) ;
      list.Append (attrib) ;
      ropts.Set (kPOVAttrib_LibraryPath, list) ;
    }

    // we deliberately do this twice - once here and once in WinRenderFrontEnd::RenderOptions
    i = sizeof (str) ;
    if (POVMSUtil_GetString (&obj, kPOVAttrib_CreateIni, str, &i) == kNoErr && str [0] != '\0')
      if ((err = options.WriteFile (str, &obj)) != kNoErr)
        throw (err) ;

    frontEnd->StartRender (ropts) ;
  }
  catch (int errorCode)
  {
    loadRerun = continueRerun = 0 ;
    running_benchmark = running_demo = false ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
    update_menu_for_render (false) ;
    if (restore_command_line)
    {
      strcpy (command_line, old_command_line) ;
      SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (UINT_PTR) command_line) ;
      restore_command_line = false ;
    }
    EditShowMessages (true) ;
    if (errorCode != kCannotOpenFileErr && errorCode != kParseErr && errorCode != kOutOfMemoryErr)
    {
      sprintf (str, "Failed to set render options (%d)", errorCode) ;
      MessageBox (main_window, str, "Error", MB_OK | MB_ICONSTOP) ;
      say_status_message (StatusMessage, str) ;
    }
    povray_return_code = errorCode ;
    if (ErrorFilename [0] != '\0')
    {
      if (status_buffer [0] != '\0')
      {
        say_status_message (StatusMessage, status_buffer) ;
        status_buffer [0] = '\0' ;
      }
      EditShowParseError (ErrorFilename, ErrorMessage, ErrorLine, ErrorCol) ;
      if (parse_error_sound_enabled)
      {
        PlaySound (parse_error_sound, NULL, SND_NOWAIT | SND_ASYNC | SND_NODEFAULT) ;
        if (!running_demo && !demo_mode)
          FeatureNotify ("ParserErrorSound",
                        "POV-Ray - Parse Error Sound",
                        "You can change the sound played upon parse errors "
                        "from the Render Menu.\n\n"
                        "Click Help for more information.",
                        "sounds", false) ;
      }
    }
    buffer_message (mDivider, "\n") ;
    EditShowMessages (true) ;

    return (false) ;
  }

  if (!StartInsertRender)
  {
    if (use_renderanim)
      SetTimer (renderanim_window, 2, 100, NULL) ;
    if (MenuBarDraw)
    {
      DrawMenuBar (main_window) ;
      MenuBarDraw = false ;
    }
    bool show = EditShowMessages (true) ;
    CalculateClientWindows (true) ;
    if (show)
      ShowWindow (message_window, SW_SHOW) ;
    PutPrivateProfileInt ("Info", "Rendering", 1, EngineIniFileName) ;
    display_cleanup () ;
  }

  WIN_Pre_Pixel (-1, -1, 0) ;

  if (render_window)
    SetClassLongPtr (render_window, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, IDC_ARROW)) ;

  ExternalEvent (EventStartRendering, 0) ;

  set_render_priority (render_priority) ;

  // FIXME - ought to wait for the render to start here
  rendering = true ;
  // StartProfile () ;
  return (true) ;
}

void render_stopped (void)
{
  char        *s ;
  char        str [4096] ;
  bool        was_bench_or_demo = running_benchmark || running_demo ;
  bool        was_demo = running_demo ;

  loadRerun = continueRerun = 0 ;
  running_benchmark = running_demo = false ;

  run_renderer = false ;
  rendering = false ;

  if (message_output_x > 0)
    buffer_message (mIDE, "\n") ;

  s = EditGetFilename (true) ;
  if (s != NULL && *s != '\0')
  {
    sprintf (str, "POV-Ray - %s", s) ;
    SetCaption (str) ;
  }
  else
    SetCaption ("POV-Ray for Windows") ;

  PrintRenderTimes (true, povray_return_code == 0) ;
  ExternalEvent (EventStopRendering, povray_return_code) ;

  // EndProfile () ;

  if (restore_command_line && !rendering_insert_menu)
  {
    strcpy (command_line, old_command_line) ;
    SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
    restore_command_line = false ;
  }

  if (rendering_insert_menu)
  {
    if (InsertMenuSection < InsertMenuSectionCount - 1 && !(stop_rendering || quit || backend_thread_exception))
    {
      wrapped_printf ("Completed rendering Insert Menu section %d. result = %d", InsertMenuSections [InsertMenuSection++], povray_return_code) ;
      StartInsertRender = true ;
      return ;
    }
    StartInsertRender = rendering_insert_menu = no_status_output = false ;
    was_insert_render = true ;
  }
  else if (was_bench_or_demo)
  {
    strcpy (command_line, old_command_line) ;
    restore_command_line = false ;
    DELETE_FILE (demo_file_name) ;
    DELETE_FILE (demo_ini_name) ;
    if (demo_mode)
    {
      PovMessageBox ("Demonstration completed. POV-Ray will now exit.", "Finished test run") ;
      PostQuitMessage (0) ;
    }
  }

  HEAPSHRINK
  update_menu_for_render (false) ;
  PutPrivateProfileInt ("Info", "Rendering", 0, EngineIniFileName) ;
  set_render_priority (CM_RENDERPRIORITY_NORMAL) ;

  if (!stop_rendering)
  {
    if (!was_demo && !demo_mode)
      PutPrivateProfileInt ("Statistics", "FinishRender", time (NULL), CurrentRerunFileName) ;
    rotate_rerun_entries () ;
    buffer_message (mIDE, "POV-Ray finished\n") ;
  }
  else
    buffer_message (mIDE, "Render cancelled by user.\n") ;

  if (!rendering_insert_menu && !was_bench_or_demo)
    add_rerun_to_menu () ;

  buffer_message (mDivider, "\n") ;

  if (use_renderanim)
  {
    KillTimer (renderanim_window, 2) ;
    InvalidateRect (renderanim_window, NULL, false) ;
  }

  if (quit != 0 || exit_after_render)
  {
    DestroyWindow (main_window) ;
    return ;
  }

  if (render_window)
  {
    SetWindowText (render_window, "Render Window") ;
    SetClassLongPtr (render_window, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, IDC_CROSS)) ;
    if (render_auto_close)
      handle_main_command (CM_RENDERCLOSE, 0) ;
  }
  if (main_window_hidden)
    TaskBarModifyIcon (main_window, 0, "POV-Ray (Restore: DblClk ; Menu: Mouse2)") ;
  InvalidateRect (renderanim_window, NULL, false) ;

  if (povray_return_code == 0 && render_complete_sound_enabled)
  {
    PlaySound (render_complete_sound, NULL, SND_ASYNC | SND_NODEFAULT) ;
    if (!was_demo && !demo_mode)
      FeatureNotify ("RenderCompleteSound",
                     "POV-Ray - Render Complete Sound",
                     "You can change the sound played upon completion of rendering "
                     "from the Render Menu.\n\nIt is also possible to tell POV-Ray "
                     "for Windows to do other things when a render stops (such as "
                     "display a message or exit.)",
                     "sounds", false) ;
  }

  if (povray_return_code == 0)
  {
    say_status_message (StatusMessage, "") ;
    EditShowMessages (false) ;
    CalculateClientWindows (false) ;
    switch (on_completion)
    {
      case CM_COMPLETION_EXIT :
           DestroyWindow (main_window) ;
           break ;

      case CM_COMPLETION_MESSAGE :
           PovMessageBox ("Render completed", "Message from POV-Ray for Windows") ;
           break ;
    }
    if (!was_bench_or_demo && !demo_mode && !rendering_insert_menu && !was_insert_render)
    {
      if (output_to_file)
      {
        status_printf (StatusMessage, "Output -> '%s'", output_file_name) ;
        sprintf (str, "Your output file has been written to the following location:\n\n"
                      "  %s\n\n"
                      "Press F1 to learn more about how to control where files are written.",
                      output_file_name) ;
        FeatureNotify ("OutputFileLocation", "POV-Ray - Output File Notification", str, "Output_File_Name", false) ;
      }
      else
      {
        FeatureNotify ("OutputFileOff",
                       "POV-Ray - No Output File",
                       "A render has completed but file output was turned off. No file "
                       "was written.\n\nPress F1 for help on output file control.",
                       "Output_To_File",
                       false) ;
      }
    }
  }
  else
  {
    if (ErrorMessage [0] != '\0')
      say_status_message (StatusMessage, ErrorMessage) ;
    else if (stop_rendering)
      say_status_message (StatusMessage, "Render cancelled by user") ;
    if (ErrorFilename [0] != '\0')
      EditShowParseError (ErrorFilename, ErrorMessage, ErrorLine, ErrorCol) ;
    if (stop_rendering)
    {
      if (render_error_sound_enabled)
      {
        PlaySound (render_error_sound, NULL, SND_ASYNC | SND_NODEFAULT) ;
        if (!was_demo && !demo_mode)
          FeatureNotify ("RenderErrorSound",
                        "POV-Ray - Render Stopped Sound",
                        "You can change the sound played upon render errors/cancellation "
                        "from the Render Menu.",
                        "sounds", false) ;
      }
    }
    else
    {
      if (parse_error_sound_enabled)
      {
        PlaySound (parse_error_sound, NULL, SND_NOWAIT | SND_ASYNC | SND_NODEFAULT) ;
        if (!was_demo && !demo_mode)
          FeatureNotify ("ParserErrorSound",
                        "POV-Ray - Parse Error Sound",
                        "You can change the sound played upon parse errors "
                        "from the Render Menu.\n\n"
                        "Click Help for more information.",
                        "sounds", false) ;
      }
    }
  }
}

UINT WINAPI ofn_hook_fn (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         SetupExplorerDialog (hwnd) ;
         break ;
  }
  return (false) ;
}

void init_ofn (OPENFILENAME *ofn, HWND hWnd, char *title, char *name, int maxlen, char *lastPath, char *defaultExt)
{
  ofn->lStructSize = sizeof (OPENFILENAME) ;
  ofn->hwndOwner = hWnd ;
  ofn->hInstance = hInstance ;
  ofn->lpstrCustomFilter = NULL ;
  ofn->nMaxCustFilter = 0 ;
  ofn->nFilterIndex = 1 ;
  ofn->lpstrTitle = title ;
  ofn->lpstrFile = name ;
  ofn->nMaxFile = maxlen ;
  ofn->lpstrFileTitle = NULL ;
  ofn->nMaxFileTitle = 0 ;
  ofn->lpstrInitialDir = lastPath ;
  ofn->Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR ;
  if (IsW95UserInterface)
    ofn->Flags |= OFN_EXPLORER ;
  ofn->nFileOffset = 0 ;
  ofn->nFileExtension = 0 ;
  ofn->lpstrDefExt = defaultExt ;
  ofn->lCustData = 0L ;
  ofn->lpfnHook = NULL ;
  ofn->lpTemplateName = NULL ;
}

char *file_open (HWND hWnd)
{
  int           result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  strcpy (name, lastRenderName) ;
  validatePath (lastRenderPath) ;
  init_ofn (&ofnTemp, hWnd, "Render File", name, sizeof (name), lastRenderPath, "pov") ;
  ofnTemp.lpstrFilter = "POV source and INI (*.pov;*.ini)\0*.pov;*.ini\0POV files (*.pov)\0*.pov\0INI files (*.ini)\0*.ini\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
  {
    strcpy (lastRenderPath, name) ;
    // this removes the name AND the trailing '\' [which is what we want]
    lastRenderPath [ofnTemp.nFileOffset - 1] = '\0' ;
    validatePath (lastRenderPath) ;
    strcpy (lastRenderName, name + ofnTemp.nFileOffset) ;
  }
  return (result ? name : NULL) ;
}

char *select_sound (HWND hWnd, char *currentSound)
{
  int           result ;
  char          path [_MAX_PATH] ;
  static char   name [_MAX_PATH] ;
  OPENFILENAME  ofnTemp ;

  splitpath (currentSound, path, name) ;
  validatePath (path) ;
  init_ofn (&ofnTemp, hWnd, "Select Sound File", name, sizeof (name), path, "wav") ;
  ofnTemp.Flags &= ~OFN_HIDEREADONLY ;
  ofnTemp.lpstrFilter = "Sound Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
    return (name) ;
  return (NULL) ;
}

void add_queue (HWND hWnd, HWND hlb)
{
  int           queue_count ;
  char          name [8192] ;
  char          *s ;
  char          str [_MAX_PATH] ;
  OPENFILENAME  ofnTemp ;

  queue_count = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
  if (queue_count >= MAX_QUEUE)
  {
    PovMessageBox ("File queue is full", "Cannot add any more files!") ;
    return ;
  }
  strcpy (name, lastRenderName) ;
  name [strlen (name) + 1] = '\0' ;
  validatePath (lastQueuePath) ;
  init_ofn (&ofnTemp, hWnd, "Add to Queue", name, sizeof (name), lastQueuePath, "pov") ;
  ofnTemp.lpstrFilter = "POV source and INI (*.pov;*.ini)\0*.pov;*.ini\0POV files (*.pov)\0*.pov\0INI files (*.ini)\0*.ini\0All Files (*.*)\0*.*\0" ;
  ofnTemp.Flags |= OFN_ALLOWMULTISELECT ;
  if (GetOpenFileName (&ofnTemp) != false)
  {
    // convert spaces into NULL's if we're not using the new interface so it works with the below code
    if (!IsW95UserInterface)
      for (s = name ; *s ; s++)
        if (*s == ' ')
          *s = '\0' ;
    if (ofnTemp.nFileOffset < strlen (name))
    {
      strcpy (lastQueuePath, name) ;
      lastQueuePath [ofnTemp.nFileOffset - 1] = '\0' ;
      SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) name) ;
    }
    else
    {
      s = name ;
      strcpy (lastQueuePath, name) ;
      for (s += strlen (s) + 1 ; *s ; s += strlen (s) + 1)
      {
        if (queue_count++ >= MAX_QUEUE)
        {
          PovMessageBox ("File queue is full", "Cannot add any more files!") ;
          return ;
        }
        joinPath (str, lastQueuePath, s) ;
        strlwr (str) ;
        SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) str) ;
      }
    }
  }
}

char *get_ini_file (HWND hWnd, char *path)
{
  int           result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  validatePath (path) ;
  init_ofn (&ofnTemp, hWnd, "Choose INI File", name, sizeof (name), path, "ini") ;
  ofnTemp.lpstrFilter = "INI files (*.ini)\0*.ini\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
  {
    strcpy (path, name) ;
    path [ofnTemp.nFileOffset - 1] = '\0' ;
  }
  return (result ? name : NULL) ;
}

char *get_background_file (HWND hWnd)
{
  int           result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  strcpy (name, lastBitmapName) ;
  validatePath (lastBitmapPath) ;
  init_ofn (&ofnTemp, hWnd, "Tile Bitmap File", name, sizeof (name), lastBitmapPath, "bmp") ;
  ofnTemp.lpstrFilter = "BMP files (*.bmp)\0*.bmp\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
  {
    strcpy (lastBitmapPath, name) ;
    lastBitmapPath [ofnTemp.nFileOffset - 1] = '\0' ;
    strcpy (lastBitmapName, name + ofnTemp.nFileOffset) ;
  }
  return (result ? name : NULL) ;
}

void get_font (void)
{
  HDC         hdc ;
  HFONT       hfont ;
  HFONT       hfontOld ;
  LOGFONT     lf ;
  CHOOSEFONT  cf ;
  TEXTMETRIC  tm ;

  hdc = GetDC (message_window) ;
  memset(&cf, 0, sizeof (CHOOSEFONT)) ;
  cf.lStructSize = sizeof (CHOOSEFONT) ;
  cf.hwndOwner = main_window ;
  cf.lpLogFont = &lf ;
  cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT ;
  cf.nFontType = SCREEN_FONTTYPE ;
  get_logfont (hdc, &lf) ;
  if (ChooseFont (&cf))
  {
    if ((hfont = CreateFontIndirect (&lf)) == NULL)
    {
      PovMessageBox ("Failed to create message font", "Cannot change to selected font") ;
      ReleaseDC (message_window, hdc) ;
      return ;
    }
    hfontOld = (HFONT)SelectObject (hdc, hfont) ;
    GetTextMetrics (hdc, &tm) ;
    message_xchar = tm.tmAveCharWidth ;
    message_ychar = tm.tmHeight + tm.tmExternalLeading ;
    SelectObject (hdc, hfontOld) ;
    DeleteObject (message_font) ;
    message_font = hfont ;
    PovInvalidateRect (message_window, NULL, true) ;
    message_font_size = -MulDiv (lf.lfHeight, 72, GetDeviceCaps (hdc, LOGPIXELSY)) ;
    message_font_weight = lf.lfWeight ;
    strncpy (message_font_name, lf.lfFaceName, sizeof (message_font_name) - 1) ;
  }
  ReleaseDC (message_window, hdc) ;
}

void DragFunction (HDROP handle)
{
  int         cFiles ;
  int         i ;
  char        szFile [_MAX_PATH] ;
  HDIB        hDIB ;
  bool        calc = 0 ;
  BITMAP      bm ;

  cFiles = DragQueryFile (handle, -1, NULL, 0) ;
  if (rendering)
    message_printf ("\n") ;
  for (i = 0 ; i < cFiles ; i++)
  {
    DragQueryFile (handle, i, szFile, sizeof (szFile)) ;
    switch (get_file_type (szFile))
    {
      case filePOV :
      case fileINI :
      case fileINC :
           if (!use_editors || !drop_to_editor)
             break ;
           if ((EditGetFlags () & EDIT_CAN_OPEN) == 0)
           {
             say_status_message (StatusMessage, "Cannot open dropped file - max editor count reached") ;
             message_printf ("Cannot open dropped file - max editor count reached\n") ;
           }
           else
             EditOpenFile (szFile) ;
           continue ;

      case fileBMP :
           if (screen_depth < 8)
           {
             PovMessageBox ("Tiled bitmaps not supported in this color depth", "File ignored") ;
             continue ;
           }
           if ((hDIB = LoadDIB (szFile)) != NULL)
           {
             strcpy (background_file, szFile) ;
             DeleteObject (hBmpBackground) ;
             hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
             DeleteObject (hDIB) ;
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = true ;
             PovInvalidateRect (message_window, NULL, true) ;
           }
           else
             PovMessageBox ("Failed to load bitmap file", "Error") ;
           continue ;

      default :
           if (!ExternalDragFunction (szFile, dfRealDrop))
           {
             if (!use_editors || !drop_to_editor)
             {
               say_status_message (StatusMessage, "Dropped file ignored (must be .POV, .INC, or .INI if destination is renderer)") ;
               wrapped_printf ("Dropped file '%s' ignored (must be .POV, .INC, or .INI if destination is renderer).", szFile) ;
             }
             else
             {
               if ((EditGetFlags () & EDIT_CAN_OPEN) == 0)
               {
                 say_status_message (StatusMessage, "Cannot open dropped file - max editor count reached") ;
                 message_printf ("Cannot open dropped file - max editor count reached\n") ;
               }
               else
                 EditOpenFile (szFile) ;
             }
           }
           continue ;
    }
    if (queued_file_count < MAX_QUEUE)
    {
      strcpy (queued_files [queued_file_count++], szFile) ;
      wrapped_printf ("File '%s' dropped as queue entry %d.", szFile, queued_file_count) ;
    }
    else
      wrapped_printf ("render queue full ; file '%s' ignored.", szFile) ;
  }
  if (rendering)
    message_printf ("\n") ;
  DragFinish (handle) ;
  update_queue_status (true) ;
  if (calc)
    CalculateClientWindows (true) ;
  FeatureNotify ("DropFiles",
                 "POV-Ray - Drag and Drop",
                 "POV-Ray can do one of several things when you drop files onto it, "
                 "depending on the state of the 'Drop to Editor' option and the type "
                 "of file dropped. For example if the file is a .POV or .INI you can "
                 "chose whether POV-Ray opens it or renders it.\n\nPress F1 for more "
                 "information.",
                 "drag and drop",
                 true) ;
}

HPALETTE create_palette (RGBQUAD *rgb, int entries)
{
  int         i ;
  HDC         hdc ;
  LogPal      Palette = { 0x300, 256 } ;

  if (screen_depth == 8 || render_bitmap_depth == 8)
  {
    if (rgb)
    {
      Palette.entries = entries ;
      for (i = 0 ; i < entries ; i++, rgb++)
      {
        Palette.pe [i].peRed = rgb->rgbRed ;
        Palette.pe [i].peGreen = rgb->rgbGreen ;
        Palette.pe [i].peBlue = rgb->rgbBlue ;
        Palette.pe [i].peFlags = PC_NOCOLLAPSE ;
      }
    }
    else
    {
      // Copy the halftone palette into the DIB palette entries, and read the
      // current system palette entries to ensure we have an identity palette mapping.
      hdc = GetDC (NULL) ;
      memcpy (bitmap_template.colors, halftonePal, sizeof (halftonePal)) ;
      GetSystemPaletteEntries (hdc, 0, 256, Palette.pe) ;
      for (i = 0 ; i < 10 ; i++)
      {
        bitmap_template.colors [i].rgbRed = Palette.pe [i].peRed ;
        bitmap_template.colors [i].rgbGreen = Palette.pe [i].peGreen ;
        bitmap_template.colors [i].rgbBlue = Palette.pe [i].peBlue ;
        Palette.pe [i].peFlags = 0 ;
        bitmap_template.colors [i + 246].rgbRed = Palette.pe [i + 246].peRed ;
        bitmap_template.colors [i + 246].rgbGreen = Palette.pe [i + 246].peGreen ;
        bitmap_template.colors [i + 246].rgbBlue = Palette.pe [i + 246].peBlue ;
        Palette.pe [i + 246].peFlags = 0 ;
      }
      while (i < 246)
      {
        Palette.pe [i].peRed = bitmap_template.colors [i].rgbRed ;
        Palette.pe [i].peGreen = bitmap_template.colors [i].rgbGreen ;
        Palette.pe [i].peBlue = bitmap_template.colors [i].rgbBlue ;
        Palette.pe [i++].peFlags = PC_NOCOLLAPSE ;
      }
      ReleaseDC (NULL, hdc) ;
    }
    return (CreatePalette ((LOGPALETTE *) &Palette)) ;
  }
  return (NULL) ;
}

bool create_render_bitmap (int width, int height)
{
  if (render_bitmap_surface != NULL)
  {
    free (render_bitmap_surface) ;
    render_bitmap_surface = NULL ;
  }
  render_bitmap = bitmap_template ;
  render_bitmap.header.biSize = sizeof (BITMAPINFOHEADER) ;
  render_bitmap.header.biWidth = width ;
  render_bitmap.header.biHeight = height ;
  render_bitmap.header.biPlanes = 1 ;
  render_bitmap.header.biBitCount = (WORD) render_bitmap_depth ;
  render_bitmap.header.biCompression = BI_RGB ;
  if (render_bitmap_depth == 8)
  {
    // round out the bits per line to a multiple of four
    render_bitmap_bpl = (width + 3) & ~3 ;
    render_bitmap.header.biClrUsed = 256 ;
  }
  else
  {
    render_bitmap_bpl = (width * 3 + 3) & ~3 ;
    render_bitmap.header.biClrUsed = 0 ;
  }
  render_bitmap.header.biSizeImage = render_bitmap_bpl * height ;
  render_bitmap.header.biClrImportant = 0 ;
  return ((render_bitmap_surface = (BYTE *) calloc (1, render_bitmap.header.biSizeImage)) != NULL) ;
}

bool create_render_window (bool force)
{
  int                   width ;
  int                   height ;
  unsigned              flags ;
  RECT                  rect ;

  if (!force && render_window != NULL && rendering_animation && !first_frame)
    return (true) ;
  if (render_window)
    DestroyWindow (render_window) ;
  renderwin_manually_closed = renderwin_destroyed = false ;
  renderwin_xoffset = renderwin_yoffset = 0 ;
  renderwin_flags = 0 ;
  rect.left = 0 ;
  rect.top = 0 ;
  rect.right = render_width ;
  rect.bottom = render_height ;
  flags = WS_OVERLAPPEDWINDOW ;

  AdjustWindowRect (&rect, flags, false) ;

  // left and top will probably be negative
  width = renderwin_max_width = rect.right - rect.left ;
  height = renderwin_max_height = rect.bottom - rect.top ;

  if (width > screen_width - 64)
    width = screen_width - 64 ;
  if (height > screen_height - 48)
    height = screen_height - 48 ;

  if (renderwin_left < screen_origin_x)
    renderwin_left = screen_origin_x ;
  if (renderwin_top < screen_origin_y)
    renderwin_top = screen_origin_y ;

  if (renderwin_left + width > virtual_screen_width + screen_origin_x)
    renderwin_left = virtual_screen_width - width + screen_origin_x ;
  if (renderwin_top + height > virtual_screen_height + screen_origin_y)
    renderwin_top = virtual_screen_height - height + screen_origin_y ;

  if ((render_window = CreateWindowEx (0,
                                       PovRenderWinClass,
                                       "POVRAY",
                                       flags | renderwin_flags,
                                       renderwin_left,
                                       renderwin_top,
                                       width,
                                       height,
                                       render_above_main ? main_window : NULL,
                                       NULL,
                                       hInstance,
                                       NULL)) == NULL)
  {
    return (false) ;
  }

  if (width < renderwin_max_width)
  {
    SetScrollRange (render_window, SB_HORZ, 0, render_width - width, false) ;
    SetScrollPos (render_window, SB_HORZ, 0, true) ;
  }

  if (height < renderwin_max_height)
  {
    SetScrollRange (render_window, SB_VERT, 0, render_height - height, false) ;
    SetScrollPos (render_window, SB_VERT, 0, true) ;
  }

  if (!main_window_hidden && !renderwin_hidden)
  {
    mainwin_placement.length = sizeof (WINDOWPLACEMENT) ; 
    GetWindowPlacement (main_window, &mainwin_placement) ;
    if (mainwin_placement.showCmd != SW_SHOWMINIMIZED)
    {
      flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS | SWP_SHOWWINDOW | SWP_NOACTIVATE ;
      if (GetForegroundWindow () == main_window && render_above_main)
        flags &= ~SWP_NOACTIVATE ;
      if (renderwin_active)
        flags &= ~SWP_NOACTIVATE ;
      SetWindowPos (render_window, main_window, 0, 0, 0, 0, flags) ;
    }
    else
      renderwin_flags = WS_MINIMIZE ;
  }

  SetClassLongPtr (render_window, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, rendering ? IDC_ARROW : IDC_CROSS)) ;

  return (true) ;
}

int renderwin_init (void)
{
  if (about_thread_running)
    return (0) ;
  if (!rendering_animation || first_frame)
    display_cleanup () ;
  if (!renderwin_manually_closed)
  {
    if (create_render_window (false) == false)
    {
      PovMessageBox ("Failed to create display window", "Initialize Display - Fatal Error") ;
      display_cleanup () ;
      return (1) ;
    }
  }
  else
    renderwin_destroyed = true ;
  if (!rendering_animation || !preserve_bitmap || first_frame)
  {
    if (create_render_bitmap (render_width, render_height) == false)
    {
      PovMessageBox ("Failed to allocate display bitmap", "Initialize Display - Fatal Error") ;
      display_cleanup () ;
      return (1) ;
    }
    uchar *p ;
    uchar dither ;
    int colour ;
    if (render_bitmap_depth == 8)
    {
      for (int y = 0 ; y < render_bitmap.header.biHeight ; y++)
      {
        for (int x = 0 ; x < render_bitmap.header.biWidth ; x++)
        {
          colour = (x & 8) == (y & 8) ? 0xff : 0xc0 ;
          p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x ;
          dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
          *p = 20 + div51 [colour] + (mod51 [colour] > dither) +
                    mul6 [div51 [colour] + (mod51 [colour] > dither)] +
                    mul36 [div51 [colour] + (mod51 [colour] > dither)] ;
        }
      }
    }
    else
    {
      for (int y = 0 ; y < render_bitmap.header.biHeight ; y++)
      {
        p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl ;
        for (int x = 0 ; x < render_bitmap.header.biWidth ; x += 8)
        {
          if (x + 8 <= render_bitmap.header.biWidth)
          {
            memset (p, (x & 8) == (y & 8) ? 0xff : 0xc0, 24) ;
            p += 24 ;
          }
          else
            memset (p, (x & 8) == (y & 8) ? 0xff : 0xc0, (render_bitmap.header.biWidth - x) * 3) ;
        }
      }
    }
  }
  if (render_window != NULL && ((rendering_animation && !preserve_bitmap) || first_frame))
    InvalidateRect (render_window, NULL, false) ;
  return (0) ;
}

int WIN_Display_Init (int x, int y)
{
  if (about_thread_running)
    return (1) ;
  render_width = x ;
  render_height = y ;
  if (SendMessage (main_window, CREATE_RENDERWIN_MESSAGE, 0, 0L))
    povray_exit (-1) ;
  WIN_Display_Plot (-1, -1, 0, 0, 0, 0) ;
  ExternalEvent (EventDisplayInit, MAKELONG (x, y)) ;
  return (1);
}

void WIN_Display_Finished (void)
{
  if (about_thread_running)
    return ;
  PovInvalidateRect (render_window, NULL, false) ;
  ExternalEvent (EventDisplayFinished, 0) ;
}

void WIN_Display_Close (void)
{
  if (about_thread_running)
    return ;
  ExternalEvent (EventDisplayClose, 0) ;
}

void WIN_Display_Plot (int x, int y, int Red, int Green, int Blue, int Alpha)
{
  int                   oldMode ;
  int                   dest_width ;
  int                   dest_height ;
  int                   dest_xoffset ;
  int                   dest_yoffset ;
  uchar                 *p ;
  uchar                 dither ;
  HDC                   hdc ;
  RECT                  rect ;
  UINT                  backColor ;
  double                aspect_ratio ;
  double                screen_aspect ;
  static int            lastx = 0, lasty = 0 ;

  if (about_thread_running)
    return ;

  ExternalDisplayPlot (x, y, Red, Green, Blue, Alpha) ;
  if (x == -1 || y == -1)
  {
    lastx = lasty = 0 ;
    strcpy (render_percentage, "0%") ;
    return ;
  }
  if (x < 0 || x >= render_width || y < 0 || y >= render_height)
    return ;
  if (y != lasty)
  {
    percentage_complete = (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1) ;
    sprintf (render_percentage, "%d%%", (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1)) ;
  }
  if (render_bitmap_surface == NULL)
    return ;
  
  if (Alpha && (opts.Options & OUTPUT_ALPHA))
  {
    backColor = (x & 8) == (y & 8) ? 0xff * Alpha : 0xc0 * Alpha ;
    Red = ((unsigned int) Red * (255 - Alpha) + backColor) / 255 ;
    Blue = ((unsigned int) Blue * (255 - Alpha) + backColor) / 255 ;
    Green = ((unsigned int) Green * (255 - Alpha) + backColor) / 255 ;
  }  

  if (render_window != NULL && (x < lastx || y != lasty) && !IsIconic (render_window))
  {
    // Blt the completed scanline to the display
    if ((hdc = GetDC (render_window)) == NULL)
      return ;
    oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
    if (hPalApp)
    {
      SelectPalette (hdc, hPalApp, false) ;
      RealizePalette (hdc) ;
    }
    if (IsZoomed (render_window))
    {
      GetClientRect  (render_window, &rect) ;
      aspect_ratio = (double) render_width / render_height ;
      screen_aspect = (double) rect.right / rect.bottom ;
      if (aspect_ratio >= screen_aspect)
      {
        dest_width = rect.right ;
        dest_height = (int) ((double) rect.right / aspect_ratio) ;
      }
      else
      {
        dest_width = (int) ((double) rect.bottom * aspect_ratio) ;
        dest_height = rect.bottom ;
      }
      dest_xoffset = (rect.right - dest_width) / 2 ;
      dest_yoffset = (rect.bottom - dest_height) / 2 ;
      StretchDIBits (hdc,
                     dest_xoffset,
                     (dest_height * lasty) / render_height + dest_yoffset,
                     dest_width,
                     (dest_height + render_height - 1) / render_height,
                     0,
                     render_bitmap.header.biHeight - 1 - lasty,
                     render_bitmap.header.biWidth,
                     1,
                     render_bitmap_surface,
                     (LPBITMAPINFO) &render_bitmap,
                     DIB_RGB_COLORS,
                     SRCCOPY) ;
    }
    else
    {
      StretchDIBits (hdc,
                     -renderwin_xoffset,
                     lasty - renderwin_yoffset,
                     render_width,
                     1,
                     0,
                     render_bitmap.header.biHeight - 1 - lasty,
                     render_bitmap.header.biWidth,
                     1,
                     render_bitmap_surface,
                     (LPBITMAPINFO) &render_bitmap,
                     DIB_RGB_COLORS,
                     SRCCOPY) ;
    }
    SetStretchBltMode (hdc, oldMode) ;
    ReleaseDC (render_window, hdc) ;
  }
  lastx = x ;
  lasty = y ;
  if (render_bitmap_depth == 8)
  {
    p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x ;
    dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
    *p = 20 + div51 [Red] + (mod51 [Red] > dither) +
              mul6 [div51 [Green] + (mod51 [Green] > dither)] +
              mul36 [div51 [Blue] + (mod51 [Blue] > dither)] ;
  }
  else
  {
    p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x * 3 ;
    p [0] = Blue ;
    p [1] = Green ;
    p [2] = Red ;
  }
}

void WIN_Display_Plot_Rect (int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha)
{
  int         x ;
  int         y ;
  int         width ;
  int         height ;
  int         oldMode ;
  int         dest_width ;
  int         dest_height ;
  int         dest_xoffset ;
  int         dest_yoffset ;
  int         R ;
  int         G ;
  int         B ;
  uchar       *p ;
  uchar       dither ;
  HDC         hdc ;
  RECT        rect ;
  UINT        backColor ;
  double      aspect_ratio ;
  double      screen_aspect ;

  if (about_thread_running)
    return ;
  ExternalDisplayPlotRect (x1, y1, x2, y2, Red, Green, Blue, Alpha) ;
  if (render_bitmap_surface == NULL)
    return ;
  if (x1 < 0 || x1 >= render_width || x2 < 0 || x2 >= render_width)
    return ;
  if (y1 < 0 || y1 >= render_height || y2 < 0 || y2 >= render_height)
    return ;
  if (x1 == x2 && y1 == y2)
  {
    WIN_Display_Plot (x1, y1, Red, Green, Blue, Alpha) ;
    return ;
  }
  R = Red ;
  G = Green ;
  B = Blue ;
  for (y = y1 ; y <= y2 ; y++)
  {
    if (render_bitmap_depth == 8)
    {
      p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x1 ;
      for (x = x1 ; x <= x2 ; x++)
      {
        dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
        if (Alpha && (opts.Options & OUTPUT_ALPHA))
        {
          backColor = (x & 8) == (y & 8) ? 0xff * Alpha : 0xc0 * Alpha ;
          R = ((unsigned int) Red * (255 - Alpha) + backColor) / 255 ;
          B = ((unsigned int) Blue * (255 - Alpha) + backColor) / 255 ;
          G = ((unsigned int) Green * (255 - Alpha) + backColor) / 255 ;
        }  
        *p++ = 20 + div51 [R] + (mod51 [R] > dither) +
                    mul6 [div51 [G] + (mod51 [G] > dither)] +
                    mul36 [div51 [B] + (mod51 [B] > dither)] ;
      }
    }
    else
    {
      p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x1 * 3 ;
      for (x = x1 ; x <= x2 ; x++)
      {
        if (Alpha && (opts.Options & OUTPUT_ALPHA))
        {
          backColor = (x & 8) == (y & 8) ? 0xff * Alpha : 0xc0 * Alpha ;
          *p++ = ((unsigned int) Blue * (255 - Alpha) + backColor) / 255 ;
          *p++ = ((unsigned int) Green * (255 - Alpha) + backColor) / 255 ;
          *p++ = ((unsigned int) Red * (255 - Alpha) + backColor) / 255 ;
        }
        else
        {
          *p++ = Blue ;
          *p++ = Green ;
          *p++ = Red ;
        }
      }
    }
  }
  if (render_window != NULL && !IsIconic (render_window))
  {
    hdc = GetDC (render_window) ;
    oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
    if (hPalApp)
    {
      SelectPalette (hdc, hPalApp, false) ;
      RealizePalette (hdc) ;
    }
    if (IsZoomed (render_window))
    {
      GetClientRect  (render_window, &rect) ;
      aspect_ratio = (double) render_width / render_height ;
      screen_aspect = (double) rect.right / rect.bottom ;
      if (aspect_ratio >= screen_aspect)
      {
        dest_width = rect.right ;
        dest_height = (int) ((double) rect.right / aspect_ratio) ;
      }
      else
      {
        dest_width = (int) ((double) rect.bottom * aspect_ratio) ;
        dest_height = rect.bottom ;
      }
      x = dest_width * x1 / render_width ;
      y = dest_height * y1 / render_height ;
      width = (dest_width * (x2 - x1 + 1) + render_width - 1) / render_width ;
      height = (dest_height * (y2 - y1 + 1) + render_height - 1) / render_height ;
      dest_xoffset = (rect.right - dest_width) / 2 ;
      dest_yoffset = (rect.bottom - dest_height) / 2 ;
      StretchDIBits (hdc,
                     x + dest_xoffset,
                     y + dest_yoffset,
                     width,
                     height,
                     x1,
                     render_bitmap.header.biHeight - 1 - y2,
                     x2 - x1 + 1,
                     y2 - y1 + 1,
                     render_bitmap_surface,
                     (LPBITMAPINFO) &render_bitmap,
                     DIB_RGB_COLORS,
                     SRCCOPY) ;
    }
    else
    {
      x = x1 ;
      y = y1 ;
      width = x2 - x1 + 1 ;
      height = y2 - y1 + 1 ;
      StretchDIBits (hdc,
                     x - renderwin_xoffset,
                     y - renderwin_yoffset,
                     width,
                     height,
                     x1,
                     render_bitmap.header.biHeight - 1 - y2,
                     x2 - x1 + 1,
                     y2 - y1 + 1,
                     render_bitmap_surface,
                     (LPBITMAPINFO) &render_bitmap,
                     DIB_RGB_COLORS,
                     SRCCOPY) ;
    }
    SetStretchBltMode (hdc, oldMode) ;
    ReleaseDC (render_window, hdc) ;
  }
}

void WIN_Pre_Pixel (int x, int y, COLOUR colour)
{
  char                  str1 [_MAX_PATH] ;
  char                  str2 [128] ;
  static int            lastY = -1 ;
  static int            last_seconds = -1 ;

  if (about_thread_running)
    return ;
  if (x != -1 && y != -1 && render_width == 0 && render_height == 0)
  {
    render_width = Frame.Screen_Width ;
    render_height = Frame.Screen_Height ;
  }
  ExternalWinPrePixel (x, y, colour) ;

  currentX = x ;

  /* Short circuit unless scanline has changed */
  if (y == -1)
  {
    seconds_for_last_line = last_seconds = lastY = -1 ;
    return ;
  }
  if (y == lastY)
    return ;

  lastY = currentY = y ;

  if (seconds != last_seconds)
  {
    seconds_for_last_line = seconds - last_seconds ;
    last_seconds = seconds ;
    percentage_complete = (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1) ;
    sprintf (render_percentage, "%d%%", (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1)) ;
    if (!IsIconic (main_window))
    {
      splitpath (opts.Input_File_Name, NULL, str2) ;
      sprintf (str1, "%s:line %d (of %d)", str2, y - opts.First_Line + 1, opts.Last_Line - opts.First_Line) ;
    }
    else
      sprintf (str1, "POV-Ray [Rendering %d%%]", percentage_complete) ;
    SetCaption (str1) ;
  }
}

void WIN_Post_Pixel (int x, int y, COLOUR colour)
{
  if (about_thread_running)
    return ;
  ExternalWinPostPixel (x, y, colour) ;
  pixels++ ;
}

void WIN_Banner (const char *s)
{
  if (about_thread_running)
    return ;
  if (strcmp (s, "\n\f") == 0)
    s = "\f" ;
  buffer_message (mBanner, s) ;
}

void WIN_Warning (const char *s)
{
  if (about_thread_running)
    return ;
  if (strcmp (s, "\n\f") == 0)
    s = "\f" ;

  // turn on fast scrolling in case a scene is generating hundreds of warning messages
  bool old_fast_scroll = fast_scroll ;
  fast_scroll = true ;
  buffer_message (mWarning, s) ;
  fast_scroll = old_fast_scroll ;
}

void WIN_Render_Info (const char *s)
{
}

void WIN_Status_Info (const char *s)
{
}

void WIN_Debug_Info (const char *s)
{
  if (about_thread_running)
    return ;
  if (strcmp (s, "\n\f") == 0)
    s = "\f" ;
  buffer_message (mDebug, s) ;
}

void WIN_Fatal (const char *s)
{
  if (about_thread_running)
    return ;
  if (strcmp (s, "\n\f") == 0)
  {
    buffer_message (mFatal, "\f") ;
    return ;
  }
  if (strncmp (s, "File: ", 6) != 0)
    strncat (ErrorMessage, s, sizeof (ErrorMessage) - strlen (ErrorMessage) - 1) ;
  else
    buffer_message (mDivider, "\n") ;
  buffer_message (mFatal, s) ;
}

void WIN_Statistics (const char *s)
{
  if (about_thread_running)
    return ;
  if (strcmp (s, "\n\f") == 0)
    s = "\f" ;
  buffer_message (mStatistics, s) ;
}

void WIN_Startup (void)
{
  if (about_thread_running)
    return ;
  if (!keep_messages)
    clear_messages () ;
  ExternalEvent (EventWinStartup, 0) ;
}

void WIN_Finish (int n)
{
  if (about_thread_running)
    return ;
  if (keep_messages)
    buffer_message (mIDE, "\n") ;
  ExternalEvent (EventWinFinish, n) ;
}

void WIN_Assert (const char *message, const char *filename, int line)
{
  char        str1 [1024] ;
  char        str2 [1024] ;

  if (rendering)
  {
    if (stop_rendering)
      return ;
    sprintf (str1, "Assertion: line %d of file %s", line, filename) ;
    strcpy (str2, message) ;
    strcat (str2, "\r\n\r\nPress CANCEL to halt rendering") ;
    if (MessageBox (main_window, str2, str1, MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
      PostMessage (main_window, WM_COMMAND, CM_STOPRENDER, 0) ;
  }
  else
  {
    sprintf (str1, "Assertion: line %d of file %s", line, filename) ;
    MessageBox (main_window, message, str1, MB_ICONEXCLAMATION) ;
  }
}

// priority == 0 when called from a higher level (e.g. in Trace_Pixel()).
void WIN_Cooperate (int priority)
{
  DWORD                 code ;
  static unsigned       last_seconds = -1 ;

#ifndef NDEBUG
  if (priority == 0)
  {
    // check 'just in case'
    if (GetExitCodeThread (hMainThread, &code))
    {
      if (code != STILL_ACTIVE)
      {
        MessageBox (NULL, "Main thread vanished from underneath us!", "POV-Ray backend thread", MB_OK | MB_ICONEXCLAMATION) ;
        ExitThread (0) ;
      }
    }
  }
#endif

  if (priority >= 2)
    Sleep (10) ;

  if (!rendering || about_thread_running)
    return ;

  while (rendersleep && run_backend_thread && !(quit || stop_rendering))
  {
    ExternalEvent (EventWinCooperate, priority) ;
    if (GetExitCodeThread (hMainThread, &code))
    {
      if (code != STILL_ACTIVE)
      {
        MessageBox (NULL, "Main thread vanished from underneath us!", "POV-Ray backend thread", MB_OK | MB_ICONEXCLAMATION) ;
        ExitThread (0) ;
      }
    }
    Sleep (100) ;
  }

  ExternalEvent (EventWinCooperate, priority) ;

  if (seconds != last_seconds && Duty_Cycle < 9)
  {
    int sleep_seconds = 10 - (Duty_Cycle + 1) ;
    last_seconds = seconds ;
    int remainder = seconds % 10 ;
    if (remainder + sleep_seconds >= 10)
    {
      status_printf (StatusMessage, "Sleeping %d second%s (duty cycle setting)", 10 - remainder, 10 - remainder > 1 ? "s" : "") ;
      Sleep (1000) ;
    }
    else
    {
      char str [2048] ;
      SendMessage (StatusWindow, SB_GETTEXT, StatusMessage, (LPARAM) str) ;
      if (strstr (str, "second (duty cycle setting") != NULL)
        status_printf (StatusMessage, "") ;
    }
  }
}

POVMSAddress WIN_GetOutputContext (void)
{
  POVMSAddress          addr = POVMSInvalidAddress ;

  if (POVMS_GetContextAddress (POVMS_GUI_Context, &addr) != kNoErr)
    return (POVMSInvalidAddress) ;
  return (addr) ;
}

void WIN_Debug_Log (unsigned int from, const char *msg)
{
  if (debugging)
    OutputDebugString (msg) ;
}

int WIN_System (char *s)
{
  int                   n ;
  char                  *message = "POV-Ray is running an external application. Do you want to halt this application ?\n\n"
                                   "If you specify YES this application will be terminated, but note that this could lead\nto an incomplete clean-up. "
                                   "The use of this facility is not recommended.\n\n"
                                   "If you specify NO the application will remain running\n\nIf you specify CANCEL POV-Ray will continue rendering." ;
  char                  str [8192] ;
  MSG                   msg ;
  DWORD                 code = WAIT_TIMEOUT ;
  STARTUPINFO           startupInfo ;
  PROCESS_INFORMATION   procInfo ;

  // a bit of a hold-over from before no_shell_outs was integerated. we'll keep it though.
  if (noexec)
  {
    wrapped_printf ("External exec request '%s' made but NOEXEC set.", s) ;
    MessageBox (main_window, "External exec request made but NOEXEC set\n\nPOV-Ray will now exit", "Security warning", MB_ICONSTOP) ;
    if (!quit)
    {
      quit = time (NULL) ;
      cancel_render () ;
    }
    return (-1) ;
  }

  if (no_shell_outs)
  {
    buffer_message (mHorzLine, "\n") ;
    sprintf (str, "Script security settings do not permit '%.256s' to be run\n", s) ;
    message_printf ("%s", str) ;
    message_printf ("See the options menu or the POVWIN help file for more information\n") ;
    buffer_message (mHorzLine, "\n") ;
    hh_aklink.pszKeywords = "I/O Restrictions" ;
    strcat (str, "\nPress HELP for more information.") ;
    MessageBox (main_window, str, "POV-Ray for Windows", MB_ICONERROR | MB_OK | MB_HELP) ;
    WIN_Finish (-1) ;
    return (-1) ;
  }

  if (ExternalWinSystem (s, &n))
    return (n) ;

  buffer_message (mHorzLine, "\n") ;
  buffer_message (mIDE, "POV-Ray is creating another process\n") ;
  wrapped_printf ("  %s", s) ;

  startupInfo.cb               = sizeof (STARTUPINFO) ;
  startupInfo.lpReserved       = 0 ;
  startupInfo.lpDesktop        = NULL ;
  startupInfo.lpTitle          = NULL ;
  startupInfo.dwX              = 0 ;
  startupInfo.dwY              = 0 ;
  startupInfo.dwXSize          = 0 ;
  startupInfo.dwYSize          = 0 ;
  startupInfo.dwXCountChars    = 0 ;
  startupInfo.dwYCountChars    = 0 ;
  startupInfo.dwFillAttribute  = 0 ;
  startupInfo.dwFlags          = system_noactive ? STARTF_USESHOWWINDOW : 0 ;
  startupInfo.wShowWindow      = SW_SHOWMINNOACTIVE ;
  startupInfo.cbReserved2      = 0 ;
  startupInfo.lpReserved2      = 0 ;

  if (CreateProcess (NULL, s, NULL, NULL, false, 0, NULL, NULL, &startupInfo, &procInfo) == false)
  {
    buffer_message (mIDE, "Could not create process\n") ;
    buffer_message (mIDE, "\n") ;
    return (GetLastError ()) ;
  }

  if (no_shellout_wait == 0)
  {
    // now wait for the process to exit
    while (code == WAIT_TIMEOUT && no_shellout_wait == 0)
    {
      POVMS_ProcessMessages(POVMS_GUI_Context, false) ;
      if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE) != 0)
      {
#ifdef HTMLHELP_FIXED
        if (HtmlHelp (NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD) &msg))
          continue ;
#endif
        if (!TranslateAccelerator (main_window, hAccelerators, &msg))
        {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
        }
      }
      code = WaitForSingleObject (procInfo.hProcess, 25) ;
      if (quit || stop_rendering)
      {
        GetExitCodeProcess (procInfo.hProcess, &code) ;
        if (code == STILL_ACTIVE)
        {
          // find out what the user wants to do with the process
          switch (MessageBox (main_window, message, quit ? "Exit POV-Ray" : "Stop rendering", MB_ICONQUESTION | MB_APPLMODAL | MB_YESNOCANCEL | MB_DEFBUTTON2))
          {
            case IDYES :
                 TerminateProcess (procInfo.hProcess, 1) ;
                 break ;

            case IDNO :
                 break ;

            case IDCANCEL :
                 quit = 0 ;
                 stop_rendering = false ;
                 code = WAIT_TIMEOUT ;
                 continue ;
          }
        }
        CloseHandle (procInfo.hProcess) ;
        CloseHandle (procInfo.hThread) ;
        buffer_message (mHorzLine, "\n") ;
        povray_exit(-1) ;
      }
    }
    GetExitCodeProcess (procInfo.hProcess, &code) ;
  }
  else
    code = 0 ;

  // clean up
  CloseHandle (procInfo.hProcess) ;
  CloseHandle (procInfo.hThread) ;
  buffer_message (mHorzLine, "\n") ;

  // code now has the application's return code
  return (code) ;
}

void WIN_SaveRerun (void)
{
}

int WIN_Shellout (int Type)
{
  char        str [64] ;

  // this is a good place to pick up info (such as animation status)
  switch (Type)
  {
    case PRE_SCENE_SHL :
         break ;

    case PRE_FRAME_SHL :
         rendering_animation = opts.FrameSeq.FrameType == FT_MULTIPLE_FRAME ;
         if (rendering_animation)
         {
           sprintf (str, "Rendering frame %d of %d", opts.FrameSeq.FrameNumber, opts.FrameSeq.FinalFrame) ;
           say_status_message (StatusMessage, str) ;
           strcat (str, "\n") ;
           buffer_message (mDivider, "\n") ;
           buffer_message (mIDE, str) ;
           buffer_message (mDivider, "\n") ;
           delay_next_status = 1000 ;
         }
         break ;

    case POST_FRAME_SHL :
         first_frame = false ;  
         break ;

    case POST_SCENE_SHL :
         break ;

    case USER_ABORT_SHL :
         break ;
  }
  return (pov_shellout ((SHELLTYPE) Type)) ;
}

void WIN_Write_Line (COLOUR *line, int y)
{
}

void WIN_Assign_Pixel (int x, int y, COLOUR colour)
{
  ExternalAssignPixel (x, y, colour) ;
}

void WIN_PrintOtherCredits (void)
{
  char        *s = DISTRIBUTION_MESSAGE_2 ;

  while (*s == ' ' || *s == '\t')
    s++ ;
  message_printf ("This is an UNSUPPORTED UNOFFICIAL COMPILE by %s.\n", s) ;
}

int WIN_Allow_File_Write (const char *Filename, const unsigned int FileType)
{
  if (io_restrictions < 1)
    return (TRUE) ;
  return (TestAccessAllowed (Filename, FileType, TRUE)) ;
}

int WIN_Allow_File_Read (const char *Filename, const unsigned int FileType)
{
  if (io_restrictions < 2)
    return (TRUE) ;
  if (running_demo)
    if (stricmp (Filename, demo_file_name) == 0 || stricmp (Filename, demo_ini_name) == 0)
      return (true) ;
  return (TestAccessAllowed (Filename, FileType, FALSE)) ;
}

void PovMessageBox (char *message, char *title)
{
  MessageBox (main_window, message, title, MB_ICONEXCLAMATION) ;
}

void detect_graphics_config (void)
{
  HDC   hdc ;

  hdc = GetDC (NULL) ;
  screen_depth = GetDeviceCaps (hdc, BITSPIXEL) ;
  render_bitmap_depth = (GetDeviceCaps (hdc, BITSPIXEL) > 8 && renderwin_8bits == 0) ? 24 : 8 ;
  screen_width = GetDeviceCaps (hdc, HORZRES) ;
  screen_height = GetDeviceCaps (hdc, VERTRES) ;
  if (GetSystemMetrics (SM_CMONITORS) > 1)
  {
    screen_origin_x = GetSystemMetrics (SM_XVIRTUALSCREEN) ;
    screen_origin_y = GetSystemMetrics (SM_YVIRTUALSCREEN) ;
    virtual_screen_width = GetSystemMetrics (SM_CXVIRTUALSCREEN) ;
    virtual_screen_height = GetSystemMetrics (SM_CYVIRTUALSCREEN) ;
  }
  else
  {
    screen_origin_x = screen_origin_y = 0 ;
    virtual_screen_width = screen_width ;
    virtual_screen_height = screen_height ;
  }
  ReleaseDC (NULL, hdc) ;
}

// Clear the system palette when we start to ensure an identity palette mapping
void clear_system_palette (void)
{
  int         Counter ;
  HDC         ScreenDC ;
  LogPal      Palette = { 0x300, 256 } ;
  HPALETTE    ScreenPalette ;

  // Reset everything in the system palette to black
  for (Counter = 0 ; Counter < 256 ; Counter++)
  {
    Palette.pe [Counter].peRed = 0 ;
    Palette.pe [Counter].peGreen = 0 ;
    Palette.pe [Counter].peBlue = 0 ;
    Palette.pe [Counter].peFlags = PC_NOCOLLAPSE ;
  }

  // Create, select, realize, deselect, and delete the palette
  ScreenDC = GetDC (NULL) ;
  ScreenPalette = CreatePalette ((LOGPALETTE *) &Palette) ;
  if (ScreenPalette)
  {
    ScreenPalette = SelectPalette (ScreenDC, ScreenPalette, false) ;
    RealizePalette (ScreenDC) ;
    ScreenPalette = SelectPalette (ScreenDC, ScreenPalette, false) ;
    DeleteObject (ScreenPalette) ;
  }
  ReleaseDC (NULL, ScreenDC) ;
}

void WIN_About_Plot (int y, unsigned char *line, bool blit, int lastfps, int avfps)
{
  HDC                   hdc ;
  uchar                 *p ;

  if (about_thread_quit)
    return ;
  if (render_bitmap_surface == NULL)
    return ;

  // line is in blue - green - red format.
  p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl ;
  memcpy (p, line, render_bitmap_depth <= 8 ? RTR_W : RTR_W * 3) ;

  if (blit)
  {
    if ((hdc = GetDC (about_window)) == NULL)
      return ;
    if (hPalApp)
    {
      SelectPalette (hdc, hPalApp, false) ;
      RealizePalette (hdc) ;
    }
    StretchDIBits (hdc,
                   12,
                   30,
                   RTR_W,
                   RTR_H,
                   0,
                   0,
                   RTR_W,
                   RTR_H,
                   render_bitmap_surface,
                   (LPBITMAPINFO) &render_bitmap,
                   DIB_RGB_COLORS,
                   SRCCOPY) ;
    ReleaseDC (about_window, hdc) ;
    if (lastfps)
      status_printf (StatusPPS, "%d (%d) FPS", avfps, lastfps) ;
  }
}

void create_about_font (void)
{
  LOGFONT     lf ;

  HDC hdc = GetDC (NULL) ;
  memset (&lf, 0, sizeof (LOGFONT)) ;
  lf.lfHeight = -10 ;
  lf.lfWeight = FW_NORMAL ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  lf.lfQuality = PROOF_QUALITY ;
  strcpy (lf.lfFaceName, "Verdana") ;
  if ((about_font = CreateFontIndirect (&lf)) == NULL)
  {
    strcpy (lf.lfFaceName, "Arial") ;
    about_font = CreateFontIndirect (&lf) ;
  }
  ReleaseDC (NULL, hdc) ;
}

LRESULT CALLBACK PovAboutWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   index ;
  HDC                   hdc ;
  HDC                   hdcMemory ;
  bool                  f ;
  DWORD                 threadId = 0 ;
  DWORD                 threadParam = 0 ;
  HBITMAP               oldBmp ;
  HPALETTE              oldPalette ;
  PAINTSTRUCT           ps ;
  struct stat           st ;
  LPDRAWITEMSTRUCT      lpdis ;

  switch (message)
  {
    case WM_CREATE :
         splash_show_about = 0 ;
         splash_time = 0 ;
         about_showing = true ;
         about_thread_quit = false ;
         about_thread_running = false ;
         break ;

    case WM_DESTROY :
         about_showing = false ;
         about_thread_quit = true ;
         CloseHandle (hAboutThread) ;
         hAboutThread = NULL ;
         EnableWindow (main_window, TRUE) ;
         DeleteObject (hBmpAbout) ;
         if (about_palette)
         {
           DeleteObject (about_palette) ;
           about_palette = NULL ;
         }
         hBmpAbout = NULL ;
         break ;

    case WM_COMMAND :
         if (HIWORD (wParam) != BN_CLICKED)
           return (false) ;
         for (index = 0 ; index < 4 ; index++)
           if (about_buttons [index] == (HWND) lParam)
             break ;
         if (index == 4)
           break ;
         switch (index)
         {
           case 0 :
                // if help file is missing or something, default to internal viewer
                if (stat (engineHelpPath, &st) == 0)
                {
                  hh_aklink.pszKeywords = "POV-Ray License" ;
                  if (HtmlHelp (main_window, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink))
                    return (0) ;
                }
                DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), main_window, (DLGPROC) PovLegalDialogProc, (LPARAM) main_window) ;
                break ;

           case 1 :
                save_povlegal () ;
                break ;

           case 2 :
                ShellExecute (NULL, NULL, "http://www.povray.org/products/", NULL, NULL, SW_SHOWNORMAL) ;
                break ;

           case 3 :
                about_showing = false ;
                EnableWindow (main_window, TRUE) ;
                SetFocus (main_window) ;
                DestroyWindow (hwnd) ;
                break ;
         }
         return (0) ;

    case WM_DRAWITEM :
         lpdis = (LPDRAWITEMSTRUCT) lParam ; 
         for (index = 0 ; index < 4 ; index++)
           if (about_buttons [index] == lpdis->hwndItem)
             break ;
         if (index == 4)
           return (0) ;
         index *= 2 ;
         if (lpdis->itemState & ODS_SELECTED)
           index++ ;
         hdc = lpdis->hDC ;
         if (about_palette)
           SelectPalette (hdc, about_palette, true) ;
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpAbout) ; 
         BitBlt (hdc, 0, 0, 164, 32, hdcMemory, 0, index * 32 + about_height, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         return (1) ;

    case WM_KEYDOWN :
         if (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_SPACE)
         {
           EnableWindow (main_window, TRUE) ;
           SetFocus (main_window) ;
           DestroyWindow (hwnd) ;
           return (0) ;
         }

         return (0) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (about_palette)
         {
           SelectPalette (hdc, about_palette, false) ;
           RealizePalette (hdc) ;
         }
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpAbout) ;
         BitBlt (hdc, 0, 0, about_width, about_height, hdcMemory, 0, 0, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == hwnd)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (about_palette)
         {
           hdc = GetDC (hwnd) ;
           oldPalette = SelectPalette (hdc, about_palette, false) ;
           f = (RealizePalette (hdc) != 0);
           SelectPalette (hdc, oldPalette, false) ;
           ReleaseDC (hwnd, hdc) ;
           if (f)
             PovInvalidateRect (hwnd, NULL, true) ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

void ShowAboutBox (void)
{
  int         oldMode ;
  MSG         msg ;
  HDC         hdcMemory ;
  char        *s = "Version " POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER ;
  SIZE        size ;
  HFONT       oldFont ;
  BITMAP      bm ;
  HBITMAP     oldBmp ;
  COLORREF    oldColour ;

  if ((hBmpAbout = NonBogusLoadBitmapAndPalette (hInstance, MAKEINTRESOURCE (BMP_ABOUT))) != NULL)
  {
    GetObject (hBmpAbout, sizeof (BITMAP), (LPSTR) &bm) ;
    about_width = bm.bmWidth ;
    about_height = bm.bmHeight - 256 ;
    about_palette = hPalBitmap ;

    if (about_font == NULL)
      create_about_font () ;

    hdcMemory = CreateCompatibleDC (NULL) ;
    oldFont = (HFONT) SelectObject (hdcMemory, about_font) ;
    oldMode = SetBkMode (hdcMemory, TRANSPARENT) ;
    oldColour = SetTextColor (hdcMemory, RGB (0, 0, 0)) ;
    oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpAbout) ;
    GetTextExtentPoint (hdcMemory, s, (int) strlen (s), &size) ;
    ExtTextOut (hdcMemory, 194 + (374 - size.cx) / 2, 26, 0, NULL, s, (int) strlen (s), NULL) ;
    SetTextColor (hdcMemory, oldColour) ;
    SetBkMode (hdcMemory, oldMode) ;
    SelectObject (hdcMemory, oldFont) ;
    SelectObject (hdcMemory, oldBmp) ;
    DeleteDC (hdcMemory) ;

    about_window = CreateWindowEx (0,//WS_EX_TOOLWINDOW,
                                   PovAboutWinClass,
                                   "POV-Ray",
                                   WS_POPUP,
                                   (screen_width - about_width) / 2,
                                   (screen_height - about_height) / 2,
                                   about_width,
                                   about_height,
                                   main_window,
                                   NULL,
                                   hInstance,
                                   NULL) ;
    about_buttons [0] = CreateWindow ("BUTTON",
                                      "&Read POV-Ray License",
                                      WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      18,
                                      144,
                                      164,
                                      32,
                                      about_window,
                                      NULL,
                                      hInstance,
                                      NULL) ;
    about_buttons [1] = CreateWindow ("BUTTON",
                                      "&Save POV-Ray License",
                                      WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      210,
                                      144,
                                      164,
                                      32,
                                      about_window,
                                      NULL,
                                      hInstance,
                                      NULL) ;
    about_buttons [2] = CreateWindow ("BUTTON",
                                      "&POV-Ray Products",
                                      WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      405,
                                      144,
                                      164,
                                      32,
                                      about_window,
                                      NULL,
                                      hInstance,
                                      NULL) ;
    about_buttons [3] = CreateWindow ("BUTTON",
                                      "&POV-Ray Products",
                                      WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                      213,
                                      436,
                                      164,
                                      32,
                                      about_window,
                                      NULL,
                                      hInstance,
                                      NULL) ;
    CenterWindowRelative (main_window, about_window, false, true) ;
    ShowWindow (about_window, SW_SHOWNORMAL) ;
    SetFocus (about_window) ;
    EnableWindow (main_window, FALSE) ;
    while (about_showing && GetMessage (&msg, NULL, 0, 0) != 0 && quit == 0)
    {
      if (!TranslateAccelerator (main_window, hAccelerators, &msg))
      {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
      }
    }
  }
}

INT_PTR CALLBACK PovCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char        *s ;
  char        str [_MAX_PATH * 2] ;
  char        str1 [_MAX_PATH] ;
  char        str2 [_MAX_PATH] ;
  static char editINIname [_MAX_PATH] ;
  static char editFilename [_MAX_PATH] ;
  static char lastIniFilename [_MAX_PATH] ;
  static bool had_region = false ;
  static struct stat statbuf1 ;
  static struct stat statbuf2 ;

  switch (message)
  {
    case WM_INITDIALOG :
         had_region = false ;
         hh_aklink.pszKeywords = "command-line options" ;
         if (use_editors)
         {
           if ((EditGetFlags () & EDIT_MSG_SELECTED) == 0)
           {
             if ((s = EditGetFilename (false)) != NULL)
             {
               EnableWindow (GetDlgItem (hDlg, IDC_PRESETSOURCEPATH), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_PRESETSOURCENAME), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_SOURCEDEFAULT), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_BROWSESOURCEFILE), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), false) ;
               splitpath (s, lastRenderPath, lastRenderName) ;
               EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), false) ;
             }
           }
           SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath) ;
           SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName) ;
         }
         else
         {
           SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath) ;
           SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName) ;
           EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), false) ;
           EnableWindow (GetDlgItem (hDlg, IDC_EDITINI), false) ;
         }
         SendDlgItemMessage (hDlg, IDC_PRESETSOURCENAME, EM_LIMITTEXT, 64, 0L) ;
         SendDlgItemMessage (hDlg, IDC_INIFILENAME, EM_LIMITTEXT, 64, 0L) ;
         strupr (SecondaryRenderIniFileName) ;
         validatePath (lastRenderPath) ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         if (strlen (TempRegionStr))
         {
           if (strlen (command_line))
           {
             strcpy (str, command_line) ;
             strcat (str, TempRegionStr) ;
           }
           else
             strcpy (str, TempRegionStr + 1) ;
           strcpy (RegionStr, TempRegionStr) ;
           TempRegionStr [0] = '\0' ;
           SetDlgItemText (hDlg, IDC_COMMANDLINE, str) ;
         }
         else
         {
           SetDlgItemText (hDlg, IDC_COMMANDLINE, command_line) ;
           if (RegionStr [0])
             if (strstr (command_line, RegionStr + 1) != NULL)
               had_region = true ;
         }
         if (SecondaryRenderIniFileName [0] != '\0')
         {
           if (SecondaryRenderIniFileName [strlen (SecondaryRenderIniFileName) - 1] != '\\')
           {
             splitpath (SecondaryRenderIniFileName, str1, str2) ;
             validatePath (str1) ;
             strcpy (editINIname, str2) ;
             SetDlgItemText (hDlg, IDC_INIFILEPATH, str1) ;
             SetDlgItemText (hDlg, IDC_INIFILENAME, str2) ;
             extract_ini_sections (SecondaryRenderIniFileName, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
             SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
             SendDlgItemMessage (hDlg, IDC_INIFILESECTION, CB_SELECTSTRING, -1, (LPARAM) SecondaryRenderIniFileSection) ;
             strcpy (lastIniFilename, SecondaryRenderIniFileName) ;
             stat (SecondaryRenderIniFileName, &statbuf1) ;
           }
           else
             SetDlgItemText (hDlg, IDC_INIFILEPATH, SecondaryRenderIniFileName) ;
         }
         SetFocus (GetDlgItem (hDlg, IDC_COMMANDLINE)) ;
         return (false) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam)) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_INIFILESECTION :
                if (HIWORD (wParam) == CBN_SETFOCUS)
                {
                  stat (lastIniFilename, &statbuf2) ;
                  if (statbuf1.st_atime != statbuf2.st_atime)
                  {
                    statbuf1 = statbuf2 ;
                    GetDlgItemText (hDlg, IDC_INIFILESECTION, str, sizeof (str)) ;
                    extract_ini_sections (lastIniFilename, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    SendDlgItemMessage (hDlg, IDC_INIFILESECTION, CB_SELECTSTRING, -1, (LPARAM) str) ;
                  }
                  return (true) ;
                }
                return (false) ;

           case IDC_EDITRENDER :
                GetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcat (str1, "\\") ;
                strcat (str1, str2) ;
                if (EditOpenFile (str1))
                {
                  EndDialog (hDlg, false) ;
                  return (true) ;
                }
                return (true) ;

           case IDC_EDITINI :
                GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcat (str1, "\\") ;
                strcat (str1, str2) ;
                if (EditOpenFile (str1))
                {
                  EndDialog (hDlg, false) ;
                  return (true) ;
                }
                return (true) ;

           case IDC_BROWSEINIFILE :
                if ((s = get_ini_file (hDlg, lastSecondaryIniFilePath)) != NULL)
                {
                  strupr (s) ;
                  splitpath (s, str1, str2) ;
                  validatePath (str1) ;
                  SetDlgItemText (hDlg, IDC_INIFILEPATH, str1) ;
                  SetDlgItemText (hDlg, IDC_INIFILENAME, str2) ;
                  if (strcmp (s, lastIniFilename))
                  {
                    extract_ini_sections (s, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    strcpy (lastIniFilename, s) ;
                    stat (lastIniFilename, &statbuf1) ;
                  }
                }
                return (true) ;

           case IDC_INIFILENAME :
                if (HIWORD (wParam) == EN_KILLFOCUS)
                {
                  GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                  GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                  validatePath (str1) ;
                  joinPath (str, str1, str2) ;
                  if (stricmp (str, lastIniFilename))
                  {
                    extract_ini_sections (str, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    strcpy (lastIniFilename, str) ;
                  }
                  return (true) ;
                }
                if (HIWORD (wParam) == EN_UPDATE)
                {
                  GetDlgItemText (hDlg, IDC_INIFILENAME, str, sizeof (str)) ;
                  if (strchr (str, '\\') != NULL)
                    SetDlgItemText (hDlg, IDC_INIFILENAME, editINIname) ;
                  else
                    strcpy (editINIname, str) ;
                }
                return (true) ;

           case IDC_RESETINI :
                SetDlgItemText (hDlg, IDC_INIFILENAME, "") ;
                GetDlgItemText (hDlg, IDC_INIFILEPATH, lastIniFilename, sizeof (lastIniFilename)) ;
                SendMessage (GetDlgItem (hDlg, IDC_INIFILESECTION), CB_RESETCONTENT, 0, 0L) ;
                return (true) ;

           case IDC_INIDEFAULT :
                sprintf (str, "%ini", DocumentsPath) ;
                SetDlgItemText (hDlg, IDC_INIFILEPATH, str) ;
                SetDlgItemText (hDlg, IDC_INIFILENAME, "quickres.ini") ;
                SendMessage (hDlg, WM_COMMAND, (EN_KILLFOCUS << 16) | IDC_INIFILENAME, 0L) ;
                return (true) ;

           case IDC_PRESETSOURCENAME :
                if (HIWORD (wParam) == EN_UPDATE)
                {
                  GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str, sizeof (str)) ;
                  if (strchr (str, '\\') != NULL)
                    SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, editFilename) ;
                  else
                    strcpy (editFilename, str) ;
                }
                return (true) ;

           case IDC_BROWSESOURCEFILE :
                if ((s = file_open (hDlg)) != NULL)
                {
                  splitpath (s, str1, str2) ;
                  validatePath (str1) ;
                  SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str1) ;
                  SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str2) ;
                }
                return (true) ;

           case IDC_SOURCEDEFAULT :
                sprintf (str, "%sSCENES\\ADVANCED", DocumentsPath) ;
                SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str) ;
                SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, "BISCUIT.POV") ;
                return (true) ;

           case IDC_SET :
           case IDC_RENDER :
           case IDC_RENDERa:    // IDC_RENDERa is defined because the render dialog
                                // has 2 render buttons on it, and VC++ complains
                if (!running_demo)
                {
                  GetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath, sizeof (lastRenderPath)) ;
                  GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName, sizeof (lastRenderName)) ;
                  validatePath (lastRenderPath) ;
//                strupr (lastRenderPath) ;
//                strupr (lastRenderName) ;
                  joinPath (source_file_name, lastRenderPath, lastRenderName) ;
                }
                GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcpy (lastSecondaryIniFilePath, str1) ;
                joinPath (SecondaryRenderIniFileName, str1, str2) ;
                strupr (SecondaryRenderIniFileName) ;
                GetDlgItemText (hDlg, IDC_INIFILESECTION, SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection)) ;
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line)) ;
                SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
                extract_ini_sections_ex (SecondaryRenderIniFileName, toolbar_combobox) ;
                SendMessage (toolbar_combobox, CB_ADDSTRING, 0, (LPARAM) "More Resolutions ...") ;
                tb_combo_sel = select_combo_item_ex (toolbar_combobox, SecondaryRenderIniFileSection) ;
                if (tb_combo_sel == -1)
                  tb_combo_sel = 0 ;

                // was there a region string on the command line before it was edited ?
                if (had_region)
                {
                  // if so, is it still there now, in an unchanged form ?
                  // (we look at RegionStr + 1 since they always start with a space).
                  if (strstr (command_line, RegionStr + 1) == NULL)
                  {
                    // it's not, so we remove the region string.
                    RegionStr [0] = '\0' ;
                  }
                }

                if (LOWORD (wParam) == IDC_RENDER || LOWORD(wParam) == IDC_RENDERa)
                {
                  if (EditSaveModified (NULL) == 0)
                    return (true) ;
                  EndDialog (hDlg, true) ;
                }
                else
                  EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_COMMANDHELP :
                hh_aklink.pszKeywords = "command-line options" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovShortCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static bool had_region = false ;

  switch (message)
  {
    case WM_INITDIALOG :
         had_region = false ;
         hh_aklink.pszKeywords = "command-line options" ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetDlgItemText (hDlg, IDC_COMMANDLINE, command_line) ;
         if (RegionStr [0])
           if (strstr (command_line, RegionStr + 1) != NULL)
             had_region = true ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_RENDER :
           case IDC_RENDERa:
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line) - 1) ;
                SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;

                // was there a region string on the command line before it was edited ?
                if (had_region)
                {
                  // if so, is it still there now, in an unchanged form ?
                  // (we look at RegionStr + 1 since they always start with a space).
                  if (strstr (command_line, RegionStr + 1) == NULL)
                  {
                    // it's not, so we remove the region string.
                    RegionStr [0] = '\0' ;
                  }
                }

                if (LOWORD (wParam) == IDC_RENDER || LOWORD(wParam) == IDC_RENDERa)
                {
                  if (EditSaveModified (NULL) == 0)
                    return (true) ;
                  EndDialog (hDlg, true) ;
                }
                else
                  EndDialog (hDlg, false) ;

                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_COMMANDHELP :
                hh_aklink.pszKeywords = "command-line options" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovFileQueueDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   i ;
  char                  str [64] ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "File Queue" ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         for (i = 0 ; i < queued_file_count ; i++)
           SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) queued_files [i]) ;
         sprintf (str, "Queue has %d entr%s", queued_file_count, queued_file_count != 1 ? "ies" : "y") ;
         SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
         CheckDlgButton (hDlg, IDC_RELOADQUEUE, GetPrivateProfileInt ("FileQueue", "ReloadOnStartup", 0, EngineIniFileName)) ;
         CheckDlgButton (hDlg, IDC_AUTORENDER, auto_render) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                queued_file_count = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                if (queued_file_count > MAX_QUEUE)
                  queued_file_count = MAX_QUEUE ;
                for (i = 0 ; i < queued_file_count ; i++)
                  SendMessage (hlb, LB_GETTEXT, i, (LPARAM) queued_files [i]) ;
                auto_render = IsDlgButtonChecked (hDlg, IDC_AUTORENDER) ;
                PVCheckMenuItem (CM_AUTORENDER, auto_render ? MF_CHECKED : MF_UNCHECKED) ;
                PutPrivateProfileInt ("FileQueue", "ReloadOnStartup", IsDlgButtonChecked (hDlg, IDC_RELOADQUEUE), EngineIniFileName) ;
                update_queue_status (true) ;
                DeleteObject (hbr) ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_DELETEFILE :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                if ((i = SendMessage (hlb, LB_GETCURSEL, 0, 0)) != LB_ERR)
                {
                  SendMessage (hlb, LB_DELETESTRING, i, 0) ;
                  if (i)
                    i-- ;
                  SendMessage (hlb, LB_SETCURSEL, i, 0) ;
                }
                i = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                sprintf (str, "Queue will have %d entr%s", i, i != 1 ? "ies" : "y") ;
                SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
                return (true) ;

           case IDC_ADDFILE :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                add_queue (hDlg, hlb) ;
                i = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                sprintf (str, "Queue will have %d entr%s", i, i != 1 ? "ies" : "y") ;
                SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = "File Queue" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_FILEQUEUE)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = listbox_ychar ;
           return (true) ;
         }
         else
           return (false) ;

    case WM_DRAWITEM :
         if (wParam == IDC_FILEQUEUE)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           draw_ordinary_listbox (d, true) ;
           return (true) ;
         }
         else
           return (false) ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovStatisticsDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  HWND                  *data = (HWND *) lParam ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "Rerun" ;
         resize_listbox_dialog (hDlg, IDC_LISTBOX, 76) ;
         CenterWindowRelative (*data, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetWindowText (hDlg, "Render Statistics") ;
         hlb = GetDlgItem (hDlg, IDC_LISTBOX) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         fill_statistics_listbox (hlb, PtrToInt (data [1])) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, false) ;
                return (true) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_LISTBOX)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = listbox_ychar ;
           return (true) ;
         }
         else
           return (false) ;

    case WM_DRAWITEM :
         if (wParam == IDC_LISTBOX)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           d->itemState &= ~ODS_SELECTED ;
           draw_ordinary_listbox (d, false) ;
           return (true) ;
         }
         else
           return (false) ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovRerunDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   selItem ;
  HWND                  data [2] ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static char           rerunID [MAX_RERUN] ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "Rerun" ;
         hlb = GetDlgItem (hDlg, IDC_RERUNLIST) ;
         fill_rerun_listbox (hlb, rerunID) ;
         SendMessage (hlb, LB_SETCURSEL, 0, 0) ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         if (rendering)
           EnableWindow (GetDlgItem (hDlg, IDC_RERUN), false) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_RERUN :
                selItem = SendDlgItemMessage (hDlg, IDC_RERUNLIST, LB_GETCURSEL, 0, 0) ;
                if (!rendering && selItem != LB_ERR)
                {
                  DeleteObject (hbr) ;
                  EndDialog (hDlg, rerunID [selItem] + 2) ;
                }
                return (true) ;

           case IDC_RERUNLIST :
                if (HIWORD (wParam) == LBN_DBLCLK)
                {
                  selItem = SendMessage ((HWND) lParam, LB_GETCURSEL, 0, 0) ;
                  if (!rendering && selItem != LB_ERR)
                  {
                    DeleteObject (hbr) ;
                    EndDialog (hDlg, rerunID [selItem] + 2) ;
                  }
                }
                return (true) ;

           case IDC_STATISTICS :
                selItem = SendDlgItemMessage (hDlg, IDC_RERUNLIST, LB_GETCURSEL, 0, 0) ;
                if (selItem != LB_ERR)
                {
                  data [0] = (HWND) hDlg ;
                  data [1] = (HWND) rerunID [selItem] ;
                  hh_aklink.pszKeywords = "Rerun Dialog" ;
                  hh_aklink.pszKeywords = "Rerun Dialog" ;
                  DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), hDlg, (DLGPROC) PovStatisticsDialogProc, (LPARAM) data) ;
                }
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = "Rerun" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_RERUNLIST)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = listbox_ychar ;
           return (true) ;
         }
         else
           return (false) ;

    case WM_DRAWITEM :
         if (wParam == IDC_RERUNLIST)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           draw_rerun_listbox (d) ;
           return (true) ;
         }
         else
           return (false) ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovFeatureAdviceDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = otaHelpString ;
         SetWindowText (hDlg, otaTitle) ;
         SetDlgItemText (hDlg, IDC_ADVICETEXT, otaText) ;
         CheckDlgButton (hDlg, IDC_DONTTELLMEAGAIN, otaChecked ? BST_CHECKED : BST_UNCHECKED) ;
         if (lParam == 0)
           lParam = (LPARAM) GetDesktopWindow () ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                otaChecked = IsDlgButtonChecked (hDlg, IDC_DONTTELLMEAGAIN) == BST_CHECKED ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                otaChecked = false ;
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = otaHelpString ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;

  }
  return (false) ;
}

INT_PTR CALLBACK PovSoundsDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char        *s ;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "sounds" ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetDlgItemText (hDlg, IDC_SOUND_RENDERCOMPLETE, render_complete_sound) ;
         SetDlgItemText (hDlg, IDC_SOUND_PARSEERROR, parse_error_sound) ;
         SetDlgItemText (hDlg, IDC_SOUND_RENDERERROR, render_error_sound) ;
         CheckDlgButton (hDlg, IDC_ENABLE_RENDERCOMPLETESOUND, render_complete_sound_enabled ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_ENABLE_PARSEERRORSOUND, parse_error_sound_enabled ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_ENABLE_RENDERERRORSOUND, render_error_sound_enabled ? BST_CHECKED : BST_UNCHECKED) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_BROWSE_RENDERCOMPLETESOUND:
                if ((s = select_sound (hDlg, render_complete_sound)) != NULL)
                  SetDlgItemText (hDlg, IDC_SOUND_RENDERCOMPLETE, s) ;
                return (true) ;

           case IDC_BROWSE_PARSEERRORSOUND:
                if ((s = select_sound (hDlg, parse_error_sound)) != NULL)
                  SetDlgItemText (hDlg, IDC_SOUND_PARSEERROR, s) ;
                return (true) ;

           case IDC_BROWSE_RENDERERRORSOUND:
                if ((s = select_sound (hDlg, render_error_sound)) != NULL)
                  SetDlgItemText (hDlg, IDC_SOUND_RENDERERROR, s) ;
                return (true) ;

           case IDOK :
                render_complete_sound_enabled = IsDlgButtonChecked (hDlg, IDC_ENABLE_RENDERCOMPLETESOUND) == BST_CHECKED ;
                parse_error_sound_enabled = IsDlgButtonChecked (hDlg, IDC_ENABLE_PARSEERRORSOUND) == BST_CHECKED ;
                render_error_sound_enabled = IsDlgButtonChecked (hDlg, IDC_ENABLE_RENDERERRORSOUND) == BST_CHECKED ;
                GetDlgItemText (hDlg, IDC_SOUND_RENDERCOMPLETE, render_complete_sound, _MAX_PATH) ;
                GetDlgItemText (hDlg, IDC_SOUND_PARSEERROR, parse_error_sound, _MAX_PATH) ;
                GetDlgItemText (hDlg, IDC_SOUND_RENDERERROR, render_error_sound, _MAX_PATH) ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = "sounds" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

void FeatureNotify (char *labelStr, char *titleStr, char *textStr, char *helpStr, bool checked)
{
  bool        result = false ;

  if (GetDontShowAgain (labelStr, EngineIniFileName))
    return ;
  otaTitle = titleStr ;
  otaText = textStr ;
  hh_aklink.pszKeywords = otaHelpString = helpStr ;
  otaChecked = checked ;
  if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_FEATUREADVICE), main_window, (DLGPROC) PovFeatureAdviceDialogProc, (LPARAM) main_window) > 0)
    if (otaChecked)
      PutDontShowAgain (labelStr, true, EngineIniFileName) ;
}

void CalculateClientWindows (bool redraw)
{
  RECT        rect ;

  GetClientRect (main_window, &rect) ;
  rect.bottom -= toolheight + statusheight ;
  if (!use_editors)
    MoveWindow (message_window, 0, toolheight, rect.right, rect.bottom, redraw) ;
  else
    SetEditorPosition (0, toolheight, rect.right, rect.bottom) ;
}

// handle a WM_CHAR message destined for the toolbar commandline edit control.
// return true if the message is to be discarded.
bool handle_toolbar_cmdline (UINT wParam, UINT lParam)
{
  if (wParam == VK_RETURN)
  {
    SendMessage (main_window, WM_COMMAND, CM_FILERENDER, 0) ;
    return (true) ;
  }
  if (wParam == VK_ESCAPE)
  {
    EditSetFocus () ;
    return (true) ;
  }
  if (wParam == 0x01) // ctrl-a
  {
    SendMessage(toolbar_cmdline, EM_SETSEL, 0, -1);
    return (true) ;
  }
  return (false) ;
}

/*
 ** Return true if we are to return 0 to Windows, false if we are to continue.
 */
bool handle_main_command (WPARAM wParam, LPARAM lParam)
{
  int         n ;
  char        *s ;
  char        filename [_MAX_PATH] ;
  HDIB        hDIB ;
  HWND        oldHwnd ;
  RECT        rect ;
  BITMAP      bm ;
  HBITMAP     hBMP ;
  CHOOSECOLOR cc ;
  struct stat st ;
  static char str [_MAX_PATH] ;

  if (process_toggles (wParam))
    return (true) ;

  if (LOWORD (wParam) >= CM_FIRSTTOOL && LOWORD (wParam) <= CM_LASTTOOL)
  {
    s = parse_tool_command (tool_commands [LOWORD (wParam) - CM_FIRSTTOOL]) ;
    if (GetPrivateProfileInt ("General", "Debug", 0, ToolIniFileName))
      message_printf ("Tool request - in '%s', out '%s'\n", tool_commands [LOWORD (wParam) - CM_FIRSTTOOL], s) ;
    else
      execute_tool (s) ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_FIRSTMENUHELP && LOWORD (wParam) <= CM_LASTMENUHELP)
  {
    menuhelp (LOWORD (wParam)) ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_DUTYCYCLE_10 && LOWORD (wParam) <= CM_DUTYCYCLE_100)
  {
    PVCheckMenuRadioItem (CM_DUTYCYCLE_10, CM_DUTYCYCLE_100, LOWORD (wParam)) ;
    Duty_Cycle = LOWORD (wParam) - CM_DUTYCYCLE_10 ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_FIRSTEDITNOTIFY && LOWORD (wParam) <= CM_LASTEDITNOTIFY)
  {
    switch (LOWORD (wParam) - CM_FIRSTEDITNOTIFY)
    {
      case NotifyTabChange :
           if ((lParam & EDIT_MSG_SELECTED) == 0)
           {
             build_editor_menu (hMainMenu) ;
             PVEnableMenuItem (CM_FILESAVE, (lParam & EDIT_CURRENT_MODIFIED) ? MF_ENABLED : MF_GRAYED) ;
             PVEnableMenuItem (CM_FILECLOSE, MF_ENABLED) ;
             s = EditGetFilename (true) ;
             if (s != NULL && *s != '\0')
             {
               sprintf (str, rendersleep ? "POV-Ray (paused) - %s" : "POV-Ray - %s", s) ;
               SetCaption (str) ;
             }
             else
               SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
           }
           else
           {
             build_main_menu (hMainMenu, true) ;
             PVEnableMenuItem (CM_FILESAVE, MF_GRAYED) ;
             PVEnableMenuItem (CM_FILECLOSE, MF_GRAYED) ;
             SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
           }
           break ;

      case NotifyModifiedChange :
           PVEnableMenuItem (CM_FILESAVE, lParam ? MF_ENABLED : MF_GRAYED) ;
           break ;

      case NotifyExitRequest :
           handle_main_command (CM_FILEEXIT, 0) ;
           break ;
    }
    return (true) ;
  }

  switch (LOWORD (wParam))
  {
    case CM_HIDENEWUSERHELP :
         set_newuser_menus (hide_newuser_help) ;
         return (0) ;

    case CM_IO_NO_RESTRICTIONS :
    case CM_IO_RESTRICT_WRITE :
    case CM_IO_RESTRICT_READWRITE :
         io_restrictions = LOWORD (wParam) - CM_IO_NO_RESTRICTIONS ;
         PVCheckMenuRadioItem (CM_IO_NO_RESTRICTIONS, CM_IO_RESTRICT_READWRITE, LOWORD (wParam)) ;
         PutPrivateProfileInt ("Scripting", "IO Restrictions", io_restrictions, EngineIniFileName) ;
         return (0) ;

    case CM_SHOWMAINWINDOW :
         if (main_window_hidden)
         {
           ShowWindow (main_window, SW_SHOW) ;
           ShowWindow (main_window, SW_RESTORE) ;
           if (render_window)
           {
             ShowWindow (render_window, SW_SHOW) ;
             renderwin_hidden = false ;
           }
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (use_taskbar)
         {
           if (TaskBarAddIcon (main_window, 0, ourIcon, "POV-Ray (Restore: DblClk ; Menu: Mouse2)"))
           {
             ShowWindow (main_window, SW_MINIMIZE) ;
             ShowWindow (main_window, SW_HIDE) ;
             if ((render_above_main || hide_render_window) && render_window != NULL)
             {
               ShowWindow (render_window, SW_HIDE) ;
               renderwin_hidden = true ;
             }
             PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Restore &Main Window from System Tray") ;
             main_window_hidden = true ;
             return (0) ;
           }
         }
         return (0) ;

    case CM_FILENEW :
         EditOpenFile (NULL) ;
         return (0) ;

    case CM_FILEOPEN :
         EditBrowseFile (true) ;
         return (0) ;

    case CM_FILESAVE :
         EditSaveFile (NULL) ;
         return (0) ;

    case CM_FILECLOSE :
         EditCloseFile (NULL) ;
         return (0) ;

    case CM_RENDERSLEEP :
         if (!(rendersleep = (SendMessage (toolbar_window, TB_ISBUTTONCHECKED, (WPARAM) CM_RENDERSLEEP, 0L) != 0)))
         {
           SetCaption ("POV-Ray for Windows") ;
           FlashWindow (main_window, 0) ;
           SleepTimeEnd = clock () ;
           SleepTimeTotal += SleepTimeEnd - SleepTimeStart ;
           say_status_message (StatusPPS, "") ;
         }
         else
         {
           SleepTimeStart = clock () ;
           SetCaption ("POV-Ray - render paused") ;
           status_printf (StatusPPS, PPS_String (pixels, ClockTimeTotal / CLOCKS_PER_SEC)) ;
           say_status_message (StatusPPS, "PAUSED") ;
         }
         break ;

    case CM_DROPEDITOR :
    case CM_DROPRENDERER :
         PVCheckMenuItem (CM_DROPEDITOR, LOWORD (wParam) == CM_DROPEDITOR ? MF_CHECKED : MF_UNCHECKED) ;
         PVCheckMenuItem (CM_DROPRENDERER, LOWORD (wParam) == CM_DROPRENDERER ? MF_CHECKED : MF_UNCHECKED) ;
         drop_to_editor = LOWORD (wParam) == CM_DROPEDITOR ;
         break ;

    case CM_RENDERPRIORITY_LOW :
    case CM_RENDERPRIORITY_NORMAL :
    case CM_RENDERPRIORITY_HIGH :
         render_priority = LOWORD (wParam) ;
         PVCheckMenuRadioItem (CM_RENDERPRIORITY_LOW, CM_RENDERPRIORITY_HIGH, render_priority) ;
         // only change render priority when the renderer is running
         if (rendering)
           set_render_priority (render_priority) ;
         return (true) ;

    case CM_COMPLETION_EXIT :
    case CM_COMPLETION_NOTHING :
    case CM_COMPLETION_MESSAGE :
         PVCheckMenuItem (on_completion, MF_UNCHECKED) ;
         on_completion = LOWORD (wParam) ;
         PVCheckMenuItem (on_completion, MF_CHECKED) ;
         return (true) ;

    case CM_PREVWINDOW :
         EditNextTab (false) ;
         return (true) ;

    case CM_NEXTWINDOW :
         EditNextTab (true) ;
         return (true) ;

    case CM_USETOOLBAR :
         if (rebar_window == NULL)
           return (true) ;
         ShowWindow (rebar_window, use_toolbar ? SW_SHOW : SW_HIDE) ;
         // this seems to be needed to get the rebar to redraw properly with v4.72 of comctrl32.dll.
         InvalidateRect (main_window, NULL, true) ;
         toolheight = 0 ;
         GetClientRect (main_window, &rect) ;
         SendMessage (main_window, WM_SIZE, SIZE_RESTORED, MAKELPARAM (rect.right, rect.bottom)) ;
         return (true) ;

    case CM_USERENDERANIM :
         use_renderanim = !use_renderanim ;
         InvalidateRect (renderanim_window, NULL, true) ;
         return (true) ;

    case CM_SINGLEINSTANCE :
         PutPrivateProfileInt ("General", "OneInstance", one_instance, EngineIniFileName) ;
         return (true) ;

    case CM_FILEEXIT :
         if (rendering)
         {
           if (MessageBox (main_window,
                           "POV-Ray is currently rendering - do you want to stop ?",
                           "Stop rendering ?",
                           MB_ICONQUESTION | MB_YESNO) == IDYES)
           {
             if (!EditCanClose (true))
               return (true) ;
             if (!quit)
               quit = time (NULL) ;
             cancel_render () ;
           }
         }
         else
         {
           if (!EditCanClose (true))
             return (true) ;
           DestroyWindow (main_window) ;
         }
         return (true) ;

    case CM_FILERENDER :
    case CM_STOPRENDER :
         if (!rendering)
         {
           if (EditSaveModified (NULL) == 0)
             return (true) ;
           // EDIT_MSG_SELECTED is only ever set if use_editors == true
           if ((EditGetFlags () & EDIT_MSG_SELECTED) == 0)
           {
             if ((s = EditGetFilename (false)) == NULL)
             {
               PovMessageBox ("No file to render in current editor tab!", "Cannot render") ;
               return (true) ;
             }
             n = get_file_type (s) ;
             if (n == filePOV || n == fileINI || !ExternalDragFunction (s, dfRenderEditor))
               PostMessage (main_window, EDITOR_RENDER_MESSAGE, 0, (LPARAM) s) ;
             return (true) ;
           }
           SetForegroundWindow (main_window) ;
           if (!ExternalDragFunction (source_file_name, dfRenderMessage))
             start_rendering (false) ;
         }
         else
         {
           if (OkToStopRendering ())
           {
             if (rendersleep)
             {
               SleepTimeEnd = clock () ;
               SleepTimeTotal += SleepTimeEnd - SleepTimeStart ;
               rendersleep = false ;
             }
             stop_rendering = true ;
             cancel_render () ;
           }
         }
         return (true) ;

    case CM_SAVE_SETTINGS :
         PutPrivateProfileInt ("General", "SaveSettingsOnExit", save_settings, EngineIniFileName) ;
         return (true) ;

    case CM_DUMPPANE :
         dump_pane_to_clipboard () ;
         return (true) ;

    case CM_CLEARMESSAGES :
         clear_messages () ;
         PovInvalidateRect (message_window, NULL, false) ;
         UpdateWindow (message_window) ;
         return (true) ;

    case CM_FORCE8BITS :
         detect_graphics_config () ;
         if (hPalApp)
           DeleteObject (hPalApp) ;
         hPalApp = create_palette (NULL, 0) ;
         buffer_message (mIDE, render_bitmap_depth == 24 ? "Using 24-bit internal bitmap\n" :
                                                           renderwin_8bits ? "Using 8-bit dithered internal bitmap (menu setting)\n" :
                                                                             "Using 8-bit dithered internal bitmap (4 or 8-bit video mode)\n") ;
         return (true) ;

    case CM_RENDERABOVEMAIN :
         // simply re-parenting doesn't seem to have the desired effect. sigh.
         if (render_window != NULL)
         {
           oldHwnd = render_window ;
           render_window = NULL ;
           ShowWindow (oldHwnd, SW_HIDE) ;
           SetForegroundWindow (main_window) ;
           DestroyWindow (oldHwnd) ;
           renderwin_manually_closed = false ;
           create_render_window (true) ;
         }
         PVEnableMenuItem (CM_RENDERHIDE, render_above_main ? MF_GRAYED : MF_ENABLED) ;
         PVEnableMenuItem (CM_RENDERACTIVE, render_above_main ? MF_GRAYED : MF_ENABLED) ;
         return (true) ;

    case CM_USEEDITOR :
         PutPrivateProfileInt ("General", "UseEditors", !use_editors, EngineIniFileName) ;
         PVCheckMenuItem (CM_USEEDITOR, use_editors ? MF_CHECKED : MF_UNCHECKED) ;
         if (MessageBox (main_window,
                        "POV-Ray for Windows needs to re-start for this to take effect immediately.\n\n"
                        "Re-start POV-Ray ?",
                        "Re-start POV-Ray for Windows ?",
                        MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
         {
           GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox,
                          CB_GETLBTEXT,
                          SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0),
                          (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
             {
               strcpy (command_line, old_command_line) ;
               SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
               restore_command_line = false ;
             }
             write_INI_settings (EngineIniFileName) ;
             EditSaveState () ;
           }
           DestroyWindow (main_window) ;
           execute_tool (filename) ;
         }
         return (true) ;

    case CM_HELPABOUT :
         if (splash_show_about)
           return (true) ;
         if (seconds >= splash_time)
         {
           splash_time = seconds + 5 ;
           splash_show_about = true ;
           SplashScreen (main_window) ;
         }
         else
           ShowAboutBox () ;
         return (true) ;

    case CM_SHOWNEXTTIP :
         DialogBoxParam (hInstance,
                         MAKEINTRESOURCE (IDD_TIP),
                         main_window, (DLGPROC)
                         PovTipDialogProc,
                         (LPARAM) main_window) ;
         return (true) ;

    case CM_TOOLBARCMDLINE :
         if (!rendering)
         {
           SetFocus (toolbar_cmdline) ;
           SendMessage (toolbar_cmdline, EM_SETSEL, 0, -1) ;
         }
         return (true) ;

    case CM_RENDERINSERT :
         if (!rendering)
           RenderInsertMenu () ;
         return (true) ;

    case CM_COMMANDLINE :
         if (!rendering)
         {
           if (DialogBoxParam (hInstance,
                               MAKEINTRESOURCE (IDD_COMMANDLINE),
                               main_window,
                               (DLGPROC) PovCommandLineDialogProc,
                               (LPARAM) main_window))
           {
             if (!ExternalDragFunction (source_file_name, dfRenderCommandLine))
               start_rendering (false) ;
           }
         }
         return (true) ;

    case CM_TILEDBACKGROUND :
         PVModifyMenu (CM_TILEDBACKGROUND,
                       MF_STRING,
                       CM_TILEDBACKGROUND,
                       tile_background ? "&Select Plain Background" : "&Select Tiled Background") ;
         if (tile_background && hBmpBackground == NULL)
         {
           if ((hBmpBackground = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_BACKGROUND00))) != NULL)
           {
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = true ;
             PovInvalidateRect (message_window, NULL, true) ;
           }
           else
           {
             tile_background = false ;
             // make sure this messagebox is AFTER we set tile_background to false !
             PovMessageBox ("Failed to load internal bitmap", "Error") ;
             PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Tiled Background") ;
             background_file [0] = '\0' ;
           }
           return (true) ;
         }
         else
           PovInvalidateRect (message_window, NULL, true) ;
         return (true) ;

    case CM_BACKGROUNDCOLOUR :
         memset (&cc, 0, sizeof (CHOOSECOLOR)) ;
         cc.lStructSize = sizeof (CHOOSECOLOR) ;
         cc.hwndOwner = main_window ;
         cc.rgbResult = background_colour ;
         cc.Flags = CC_PREVENTFULLOPEN | CC_RGBINIT ;
         cc.lpCustColors = custom_colours ;
         if (ChooseColor (&cc))
         {
           background_colour = cc.rgbResult ;
           PovInvalidateRect (message_window, NULL, true) ;
         }
         return (true) ;

    case CM_BACKGROUNDBITMAP :
         if ((s = get_background_file (main_window)) != NULL)
         {
           if ((hDIB = LoadDIB (s)) != NULL)
           {
             strcpy (background_file, s) ;
             DeleteObject (hBmpBackground) ;
             hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
             DeleteObject (hDIB) ;
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = true ;
             PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Plain Background") ;
             background_shade = RGB (1, 1, 1) ;
             PovInvalidateRect (message_window, NULL, true) ;
           }
           else
             PovMessageBox ("Failed to load bitmap file", "Error") ;
         }
         return (true) ;

    case CM_BACKGROUNDSTD + 0 :
    case CM_BACKGROUNDSTD + 1 :
    case CM_BACKGROUNDSTD + 2 :
    case CM_BACKGROUNDSTD + 3 :
    case CM_BACKGROUNDSTD + 4 :
    case CM_BACKGROUNDSTD + 5 :
    case CM_BACKGROUNDSTD + 6 :
    case CM_BACKGROUNDSTD + 7 :
    case CM_BACKGROUNDSTD + 8 :
    case CM_BACKGROUNDSTD + 9 :
         if ((hBMP = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_BACKGROUND00 + (LOWORD (wParam) - CM_BACKGROUNDSTD)))) != NULL)
         {
           DeleteObject (hBmpBackground) ;
           hBmpBackground = hBMP ;
           if (GetObject (hBmpBackground, sizeof (BITMAP), (LPVOID) &bm) == 0)
           {
             PovMessageBox ("Failed to load internal bitmap", "Error") ;
             tile_background = false ;
             return (true) ;
           }
           background_width = bm.bmWidth ;
           background_height = bm.bmHeight ;
           background_file [0] = '0' + (char) (LOWORD (wParam) - CM_BACKGROUNDSTD) ;
           background_file [1] = '\0' ;
           switch (LOWORD (wParam))
           {
             case CM_BACKGROUNDSTD + 0 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 1 :
                  background_shade = RGB (0, 0, 0) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 2 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 3 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 4 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 5 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (0, 0, 0) ;
                  break ;
           }
           tile_background = true ;
           PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Plain Background") ;
           PovInvalidateRect (message_window, NULL, true) ;
         }
         else
           PovMessageBox ("Failed to load internal bitmap", "Error") ;
         return (true) ;

    case CM_TEXTCOLOUR :
         memset (&cc, 0, sizeof (CHOOSECOLOR)) ;
         cc.lStructSize = sizeof (CHOOSECOLOR) ;
         cc.hwndOwner = main_window ;
         cc.rgbResult = text_colour ;
         cc.Flags = CC_PREVENTFULLOPEN | CC_RGBINIT ;
         cc.lpCustColors = custom_colours ;
         if (ChooseColor (&cc))
         {
           text_colour = cc.rgbResult ;
           PovInvalidateRect (message_window, NULL, true) ;
         }
         return (true) ;

    case CM_FONT :
         get_font () ;
         return (true) ;

    case CM_RENDERSHOW :
         if (renderwin_destroyed)
         {
           renderwin_manually_closed = false ;
           create_render_window (true) ;
           PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
           PVEnableMenuItem (CM_RENDERCLOSE, MF_ENABLED) ;
           renderwin_destroyed = false ;
         }
         return (true) ;

    case CM_RENDERCLOSE :
         if (render_above_main && GetForegroundWindow () == render_window)
           SetForegroundWindow (main_window) ;
         if (render_window != NULL)
           DestroyWindow (render_window) ;
         renderwin_manually_closed = true ;
         return (true) ;

    case CM_CLEARQUEUE :
         queued_file_count = 0 ;
         update_queue_status (true) ;
         return (true) ;

    case CM_FILEQUEUE :
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_FILEQUEUE), main_window, (DLGPROC) PovFileQueueDialogProc, (LPARAM) main_window) ;
         return (true) ;

    case CM_SOURCEFILE :
         if (!rendering)
         {
           if ((s = file_open (main_window)) != NULL)
           {
             strcpy (source_file_name, s) ;
             splitpath (source_file_name, lastRenderPath, lastRenderName) ;
             validatePath (lastRenderPath) ;
             if (!ExternalDragFunction (source_file_name, dfRenderSourceFile))
               start_rendering (false) ;
           }
         }
         return (true) ;

    case CM_RERUN + 0 :
    case CM_RERUN + 1 :
    case CM_RERUN + 2 :
    case CM_RERUN + 3 :
    case CM_RERUN + 4 :
    case CM_RERUN + 5 :
    case CM_RERUN + 6 :
    case CM_RERUN + 7 :
    case CM_RERUN + 8 :
    case CM_RERUN + 9 :
    case CM_RERUN + 10 :
    case CM_RERUN + 11 :
    case CM_RERUN + 12 :
    case CM_RERUN + 13 :
    case CM_RERUN + 14 :
    case CM_RERUN + 15 :
         if (!rendering)
         {
           loadRerun = LOWORD (wParam) - CM_RERUN + 2 ;
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
             start_rendering (false) ;
         }
         return (true) ;

    case CM_RERUNCURRENT :
         if (!rendering)
         {
           loadRerun = 1 ;
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
             start_rendering (false) ;
         }
         return (true) ;

    case CM_CONTINUECURRENT :
         if (!rendering)
         {
           loadRerun = 1 ;
           continueRerun = 1 ;
           start_rendering (false) ;
         }
         return (true) ;

    case CM_RERUNDIALOG :
         hh_aklink.pszKeywords = "Rerun Dialog" ;
         if ((loadRerun = DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_RERUN), main_window, (DLGPROC) PovRerunDialogProc, (LPARAM) main_window)) != 0)
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
             start_rendering (false) ;
         return (true) ;

    case CM_RENDERSOUNDS :
         hh_aklink.pszKeywords = "sounds" ;
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SOUNDS), main_window, (DLGPROC) PovSoundsDialogProc, (LPARAM) main_window) ;
         return (true) ;

    case CM_DEMO :
         if (!rendering && !running_demo)
         {
           if (save_demo_file (demo_file_name, demo_ini_name) != NULL)
           {
             if (!demo_mode)
             {
               running_demo = true ;
               if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
               {
                 if (!restore_command_line)
                   strcpy (old_command_line, command_line) ;
                 restore_command_line = true ;
                 sprintf (command_line, "Include_Ini=\"%s\" Input_File_Name=\"%s\" ", demo_ini_name, demo_file_name) ;
                 strupr (command_line) ;
                 strcat (command_line, old_command_line) ;
                 ignore_auto_ini = true ;
                 start_rendering (true) ;
               }
             }
             else
             {
               if (!restore_command_line)
                 strcpy (old_command_line, command_line) ;
               restore_command_line = true ;
               sprintf (command_line, "Include_Ini=\"%s\" Input_File_Name=\"%s\" ", demo_ini_name, demo_file_name) ;
               running_demo = true ;
               start_rendering (true) ;
             }
           }
         }
         return (true) ;

    case CM_BENCHMARK :
         if (rendering)
           return (0) ;
         n = Get_Benchmark_Version () ;
         hh_aklink.pszKeywords = "Run Benchmark" ;
         sprintf (str, "This will run the standard POV-Ray 3.5/3.6 benchmark v%x.%02x.\n", n / 256, n % 256) ;
         strcat (str, "This will take a little while [e.g. about 15 minutes on a modern CPU].\n") ;
         strcat (str, "There will be no display or file output. Press HELP for more information.\n\n") ;
         strcat (str, "Continue ?") ;
         if (MessageBox (main_window, str, "Standard Benchmark", MB_YESNO | MB_ICONINFORMATION | MB_HELP) == IDNO)
           return (0) ;
         GetTempPath (_MAX_PATH - 16, demo_file_name) ;
         if (demo_file_name[0] != '\0' && demo_file_name [strlen (demo_file_name) - 1] != '\\')
           strcat (demo_file_name, "\\") ;
         strcpy (demo_ini_name, demo_file_name) ;
         strcat (demo_file_name, "POVBENCH.$$1") ;
         strcat (demo_ini_name, "POVBENCH.$$2") ;
         if (Write_Benchmark_File (demo_file_name, demo_ini_name))
         {
           strcpy (old_command_line, command_line) ;
           restore_command_line = true ;
           sprintf (command_line, "Include_Ini=\"%s\" Input_File_Name=\"%s\"", demo_ini_name, demo_file_name) ;
           running_benchmark = running_demo = true ;
           message_printf ("Running standard POV-Ray benchmark version %x.%02x\n", n / 256, n % 256) ;
           buffer_message (mDivider, "\n") ;
           status_printf (0, "Running standard POV-Ray benchmark version %x.%02x\n", n / 256, n % 256) ;
           start_rendering (true) ;
         }
         else
           PovMessageBox ("Failed to write temporary files", "Benchmark Failed") ;
         return (true) ;

    case CM_LOADTOOLMENU :
         ExternalEvent (EventLoadToolMenu, 0) ;
         load_tool_menu (ToolIniFileName) ;
         break ;

    case CM_HELPLOOKUP :
         if (GetFocus () == render_window)
         {
           hh_aklink.pszKeywords = "Render Window" ;
           HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
           return (true) ;
         }
         if (GetFocus () == toolbar_cmdline)
         {
           hh_aklink.pszKeywords = "Toolbar Command Line" ;
           HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
           return (true) ;
         }
         if (GetFocus () == toolbar_combobox)
         {
           hh_aklink.pszKeywords = "Preset Rendering Options" ;
           HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
           return (true) ;
         }
         if ((EditGetFlags () & EDIT_MSG_SELECTED) != 0)
           HtmlHelp (NULL, engineHelpPath, HH_DISPLAY_TOC, 0) ;
         else
           EditContextHelp () ;
         return (true) ;

    case CM_HELPPOVWIN :
         hh_aklink.pszKeywords = "welcome" ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case CM_HELPSCENE :
         hh_aklink.pszKeywords = "Scene Description Language" ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case CM_GOPOVRAYORG :
         ShellExecute (NULL, NULL, "http://www.povray.org/", NULL, NULL, SW_SHOWNORMAL) ;
         return (true) ;

    case CM_GOIRTC :
         ShellExecute (NULL, NULL, "http://www.irtc.org/", NULL, NULL, SW_SHOWNORMAL) ;
         return (true) ;

    case CM_HELPBUGS :
         hh_aklink.pszKeywords = "Bug Reports" ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case CM_HELPIRTCCD :
         ShellExecute (NULL, NULL, "http://www.povray.org/products/", NULL, NULL, SW_SHOWNORMAL) ;
         return (true) ;

    case CM_POVLEGAL :
         if (stat (engineHelpPath, &st) == 0)
         {
           hh_aklink.pszKeywords = "POV-Ray License" ;
           if (HtmlHelp (main_window, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink))
             return (true) ;
         }
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), main_window, (DLGPROC) PovLegalDialogProc, (LPARAM) main_window) ;
         return (true) ;
   }
   return (false) ;
}

inline int MulDivNoRound (int value, int mul_by, int div_by)
{
  return ((int) ((__int64) value * mul_by / div_by)) ;
}

LRESULT CALLBACK PovRenderWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   dest_width ;
  int                   dest_height ;
  int                   dest_xoffset ;
  int                   dest_yoffset ;
  int                   oldMode ;
  HDC                   hdc ;
  HDC                   hdcMemory ;
  RECT                  rect ;
  HPEN                  hpen ;
  char                  str [512] ;
  char                  *s ;
  bool                  zoomed = IsZoomed (hwnd) != 0 ;
  POINT                 pt ;
  POINT                 pts [5] ;
  short                 x1 ;
  short                 y1 ;
  short                 x2 ;
  short                 y2 ;
  double                aspect_ratio ;
  double                screen_aspect ;
  HBITMAP               oldBmp ;
  MINMAXINFO            *pInfo ;
  PAINTSTRUCT           ps ;
  static HBITMAP        errorBitmap = NULL ;
  static int            RBand = 0 ;
  static POINT          RB1 ;
  static POINT          RB2 ;
  static int            lastX = -1 ;
  static int            lastY = -1 ;

  // only initialize these variables if we need to
  switch (message)
  {
    case WM_LBUTTONDOWN :
    case WM_LBUTTONUP :
    case WM_MOUSEMOVE :
    case WM_PAINT :
         GetClientRect (hwnd, &rect) ;
         if (zoomed)
         {
           aspect_ratio = (double) render_width / render_height ;
           screen_aspect = (double) rect.right / rect.bottom ;
           if (aspect_ratio >= screen_aspect)
           {
             dest_width = rect.right ;
             dest_height = (int) ((double) rect.right / aspect_ratio) ;
           }
           else
           {
             dest_width = (int) ((double) rect.bottom * aspect_ratio) ;
             dest_height = rect.bottom ;
           }
           dest_xoffset = (rect.right - dest_width) / 2 ;
           dest_yoffset = (rect.bottom - dest_height) / 2 ;
         }
         else
         {
           dest_xoffset = dest_yoffset = 0 ;
           dest_width = rect.right ;
           dest_height = rect.bottom ;
         }
  }

  switch (message)
  {
    case WM_SETFOCUS :
         // this will handle the annoying situation where another window is
         // in the z-order below the render window but above the main window.
         if (render_above_main)
           SetWindowPos (main_window, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) ;
         break ;

    case WM_CHAR :
         // if any character is pressed, pass focus to the main window
         EditSetFocus () ;
         return (0) ;

    case WM_CLOSE :
         renderwin_manually_closed = true ;
         if (!running_demo && !demo_mode)
           FeatureNotify ("RenderwinClose",
                          "POV-Ray - Render Window",
                          "If you find that the render window gets in your way during "
                          "or after rendering, there are a number of options to control it.\n\n"
                          "Press F1 to learn more.",
                          "Render Window Menu",
                          true) ;
         break ;
    
    case RENDERWIN_CLOSE_MESSAGE :
         DestroyWindow (hwnd) ;
         return (0) ;

    case WM_CREATE :
         PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
         PVEnableMenuItem (CM_RENDERCLOSE, MF_ENABLED) ;
         RBand = 0 ;
         lastX = lastY = -1 ;
         break ;

    case WM_COMMAND :
         if (handle_main_command (wParam, lParam))
           return (0) ;
         break ;

    case WM_LBUTTONDOWN :
         if (rendering)
         {
           FlashWindow (hwnd, TRUE) ;
           MessageBeep (-1) ;
           FlashWindow (hwnd, FALSE) ;
           return (0) ;
         }
         if (render_width < 2 || render_height < 2)
           return (0) ;
         RBand = 1 ;
         if ((wParam & MK_SHIFT) != 0)
           RBand++ ;
         if (zoomed)
         {
           RB1.x = MAX (MIN (LOWORD (lParam), dest_xoffset + dest_width - 1), dest_xoffset) ;
           RB1.y = MAX (MIN (HIWORD (lParam), dest_yoffset + dest_height - 1), dest_yoffset) ;
         }
         else
         {
           RB1.x = LOWORD (lParam) ;
           RB1.y = HIWORD (lParam) ;
         }
         RB2 = RB1 ;
         SetCapture (hwnd) ;
         return (0) ;

    case WM_LBUTTONUP :
    case WM_MOUSEMOVE :
         x2 = LOWORD (lParam) ;
         y2 = HIWORD (lParam) ;
         if (message == WM_MOUSEMOVE)
           if (x2 == lastX && y2 == lastY)
             return (0) ;
         lastX = x2 ;
         lastY = y2 ;
         if (RBand)
         {
           hdc = GetDC (hwnd) ;
           pts [0] = RB1 ;
           pts [1] = RB1 ;
           pts [1].x = RB2.x ;
           pts [2] = RB2 ;
           pts [3] = RB2 ;
           pts [3].x = RB1.x ;
           pts [4] = RB1 ;
           hpen = CreatePen (PS_DOT, 1, RGB (192, 192, 192)) ;
           hpen = (HPEN) SelectObject (hdc, hpen) ;
           oldMode = SetROP2 (hdc, R2_XORPEN) ;
           Polyline (hdc, pts, 5) ;
           if (zoomed)
           {
             RB2.x = x2 = MAX (MIN (x2, dest_xoffset + dest_width - 1), dest_xoffset) ;
             RB2.y = y2 = MAX (MIN (y2, dest_yoffset + dest_height - 1), dest_yoffset) ;

             x2 -= dest_xoffset ;
             y2 -= dest_yoffset ;
             x2 = MulDivNoRound (x2, render_width, dest_width) ;
             y2 = MulDivNoRound (y2, render_height, dest_height) ;

             // the values in RB1 have already been clipped
             x1 = RB1.x - dest_xoffset ;
             y1 = RB1.y - dest_yoffset ;
             x1 = MulDivNoRound (x1, render_width, dest_width) ;
             y1 = MulDivNoRound (y1, render_height, dest_height) ;
           }
           else
           {
             RB2.x = x2 = MAX (MIN (x2, dest_width - 1), 0) ;
             RB2.y = y2 = MAX (MIN (y2, dest_height - 1), 0) ;
             x2 += renderwin_xoffset ;
             y2 += renderwin_yoffset ;
             x1 = RB1.x + renderwin_xoffset ;
             y1 = RB1.y + renderwin_yoffset ;
           }
           if (x1 > x2)
             x1 ^= x2 ^= x1 ^= x2 ;
           if (y1 > y2)
             y1 ^= y2 ^= y1 ^= y2 ;
           if (message != WM_LBUTTONUP)
           {
             sprintf (str, "%d,%d - %d,%d", x1, y1, x2, y2) ;
             SetWindowText (hwnd, str) ;
             pts [1].x = RB2.x ;
             pts [2] = RB2 ;
             pts [3] = RB2 ;
             pts [3].x = RB1.x ;
             Polyline (hdc, pts, 5) ;
             SetROP2 (hdc, oldMode) ;
             DeleteObject (SelectObject (hdc, hpen)) ;
             ReleaseDC (hwnd, hdc) ;
           }
           else
           {
             ReleaseCapture () ;
             SetROP2 (hdc, oldMode) ;
             DeleteObject (SelectObject (hdc, hpen)) ;
             ReleaseDC (hwnd, hdc) ;
             SetWindowText (hwnd, "Render Window") ;
             if ((x2 - x1 > 2) && (y2 - y1 > 2))
             {
               sprintf (str, "Selection is %d,%d to %d,%d\n\n", x1, y1, x2, y2) ;
               if (RBand == 1)
               {
                 RBand = 0 ;
                 strcat (str, "Press OK to render this region now.\n") ;
                 strcat (str, "(You may shift-drag to set a permanent region next time).") ;
                 if (MessageBox (main_window, str, "Render region", MB_OKCANCEL) == IDOK)
                 {
                   if (EditSaveModified (NULL) == 0)
                     return (true) ;
                   if (RegionStr [0])
                   {
                     if ((s = strstr (command_line, RegionStr)) != NULL)
                       strcpy (s, s + strlen (RegionStr)) ;
                     else if ((s = strstr (command_line, RegionStr + 1)) != NULL)
                       strcpy (s, s + strlen (RegionStr) - 1) ;
                     SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
                     RegionStr [0] = '\0' ;
                   }
                   sprintf (RegionStr,
                            " +sc%f +sr%f +ec%f +er%f",
                            (float) x1 / (render_width - 1),
                            (float) y1 / (render_height - 1),
                            (float) x2 / (render_width - 1),
                            (float) y2 / (render_height - 1)) ;
                   temp_render_region = true ;
                   start_rendering (false) ;
                   temp_render_region = false ;
                 }
               }
               else
               {
                 RBand = 0 ;
                 strcat (str, "Press OK to append this region to the command-line.\n") ;
                 if (MessageBox (main_window, str, "Render region", MB_OKCANCEL) == IDOK)
                 {
                   if (RegionStr [0])
                   {
                     if ((s = strstr (command_line, RegionStr)) != NULL)
                       strcpy (s, s + strlen (RegionStr)) ;
                     else if ((s = strstr (command_line, RegionStr + 1)) != NULL)
                       strcpy (s, s + strlen (RegionStr) - 1) ;
                     SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
                   }
                   sprintf (TempRegionStr,
                            " +sc%f +sr%f +ec%f +er%f",
                            (float) x1 / (render_width - 1),
                            (float) y1 / (render_height - 1),
                            (float) x2 / (render_width - 1),
                            (float) y2 / (render_height - 1)) ;
                   PostMessage (main_window, WM_COMMAND, CM_COMMANDLINE, 0) ;
                 }
               }
             }
             else
               RBand = 0 ;
           }
         }
         else
         {
           if (!rendering)
           {
             x2 += renderwin_xoffset - dest_xoffset ;
             y2 += renderwin_yoffset - dest_yoffset ;
             if (zoomed)
             {
               x2 = MulDivNoRound (x2, render_width, dest_width) ;
               y2 = MulDivNoRound (y2, render_height, dest_height) ;
               x2 = MAX (MIN (x2, (int) render_width - 1), 0) ;
               y2 = MAX (MIN (y2, (int) render_height - 1), 0) ;
             }
             sprintf (str, "%d,%d", x2, y2) ;
             SetWindowText (hwnd, str) ;
           }
         }
         return (0) ;

    case WM_RBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           ClientToScreen (render_window, &pt) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 1), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
         }
         return (0) ;

    case WM_GETMINMAXINFO :
         if (zoomed)
           break ;
         pInfo = (MINMAXINFO *) lParam ;
         pInfo->ptMaxTrackSize.x = renderwin_max_width ;
         pInfo->ptMaxTrackSize.y = renderwin_max_height ;
         break ;

    case WM_MOVE :
         if (render_window == NULL)
           break ;
         if (!IsIconic (hwnd) && !zoomed)
         {
           GetWindowRect (hwnd, &rect) ;
           renderwin_left = rect.left ;
           renderwin_top = rect.top ;
         }
         return (0) ;

    case WM_SIZE :
         if (render_window == NULL)
           break ;
         switch (wParam)
         {
           case SIZE_MINIMIZED :
                renderwin_flags = WS_MINIMIZE ;
                return (0) ;

           case SIZE_MAXIMIZED :
                renderwin_flags = WS_MAXIMIZE ;
                SetScrollRange (render_window, SB_HORZ, 0, 0, true) ;
                SetScrollRange (render_window, SB_VERT, 0, 0, true) ;
                PovInvalidateRect (render_window, NULL, false) ;
                UpdateWindow (render_window) ;
                SetClassLongPtr (render_window, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, rendering ? IDC_ARROW : IDC_CROSS)) ;
                return (0) ;

           case SIZE_RESTORED :
                renderwin_flags = 0 ;
                PovInvalidateRect (render_window, NULL, false) ;
                UpdateWindow (render_window) ;
                break ;

           default :
                return (0) ;
         }

         // to get here we must be handling SIZE_RESTORED.

         // one problem we have here is that if we create one scroll bar, it takes away some of the client
         // area of the other direction (i.e. if we create a scroll bar for the X direction, it takes away
         // some of the Y client area). therefore we should create a scroll bar for that direction also.
         GetWindowRect (render_window, &rect) ;
         if (rect.right - rect.left < renderwin_max_width || rect.bottom - rect.top < renderwin_max_height)
         {
           if (renderwin_xoffset >= render_width - LOWORD (lParam))
             renderwin_xoffset = render_width - LOWORD (lParam) ;
           SetScrollRange (render_window, SB_HORZ, 0, render_width - LOWORD (lParam), false) ;
           SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, true) ;

           if (renderwin_yoffset >= render_height - HIWORD (lParam))
             renderwin_yoffset = render_height - HIWORD (lParam) ;
           SetScrollRange (render_window, SB_VERT, 0, render_height - HIWORD (lParam), false) ;
           SetScrollPos (render_window, SB_VERT, renderwin_yoffset, true) ;
         }
         else
         {
           renderwin_xoffset = 0 ;
           renderwin_yoffset = 0 ;
           SetScrollRange (render_window, SB_VERT, 0, 0, true) ;
           SetScrollRange (render_window, SB_HORZ, 0, 0, true) ;
         }
         return (0) ;

    case WM_VSCROLL :
         GetClientRect (render_window, &rect) ;
         switch (LOWORD (wParam))
         {
           case SB_LINEDOWN :
                if (renderwin_yoffset >= render_height - rect.bottom) break ;
                SetScrollRange (render_window, SB_VERT, 0, render_height - rect.bottom, false) ;
                SetScrollPos (render_window, SB_VERT, ++renderwin_yoffset, true) ;
                ScrollWindow (render_window, 0, -1, NULL, NULL) ;
                break ;

           case SB_LINEUP :
                if (renderwin_yoffset == 0) break ;
                SetScrollRange (render_window, SB_VERT, 0, render_height - rect.bottom, false) ;
                SetScrollPos (render_window, SB_VERT, --renderwin_yoffset, true) ;
                ScrollWindow (render_window, 0, 1, NULL, NULL) ;
                break ;

           case SB_PAGEDOWN :
                renderwin_yoffset += rect.bottom ;
                if (renderwin_yoffset > render_height - rect.bottom)
                  renderwin_yoffset = render_height - rect.bottom ;
                SetScrollPos (render_window, SB_VERT, renderwin_yoffset, true) ;
                PovInvalidateRect (hwnd, NULL, false) ;
                break ;

           case SB_PAGEUP :
                renderwin_yoffset -= rect.bottom ;
                if (renderwin_yoffset < 0)
                  renderwin_yoffset = 0 ;
                SetScrollPos (render_window, SB_VERT, renderwin_yoffset, true) ;
                PovInvalidateRect (hwnd, NULL, false) ;
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                renderwin_yoffset = HIWORD (wParam) ;
                SetScrollPos (render_window, SB_VERT, renderwin_yoffset, true) ;
                PovInvalidateRect (hwnd, NULL, false) ;
                break ;
         }
         return (0) ;

    case WM_HSCROLL :
         GetClientRect (render_window, &rect) ;
         switch (LOWORD (wParam))
         {
           case SB_LINERIGHT :
                if (renderwin_xoffset >= render_width - rect.right) break ;
                SetScrollRange (render_window, SB_HORZ, 0, render_width - rect.right, false) ;
                SetScrollPos (render_window, SB_HORZ, ++renderwin_xoffset, true) ;
                ScrollWindow (render_window, -1, 0, NULL, NULL) ;
                break ;

             case SB_LINELEFT :
                if (renderwin_xoffset == 0) break ;
                SetScrollRange (render_window, SB_HORZ, 0, render_width - rect.right, false) ;
                SetScrollPos (render_window, SB_HORZ, --renderwin_xoffset, true) ;
                ScrollWindow (render_window, 1, 0, NULL, NULL) ;
                break ;

           case SB_PAGERIGHT :
                renderwin_xoffset += rect.right ;
                if (renderwin_xoffset > render_width - rect.right)
                  renderwin_xoffset = render_width - rect.right ;
                SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, true) ;
                PovInvalidateRect (hwnd, NULL, false) ;
                break ;

           case SB_PAGELEFT :
                renderwin_xoffset -= rect.right ;
                if (renderwin_xoffset < 0)
                  renderwin_xoffset = 0 ;
                SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, true) ;
                PovInvalidateRect (hwnd, NULL, false) ;
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                renderwin_xoffset = HIWORD (wParam) ;
                SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, true) ;
                PovInvalidateRect (hwnd, NULL, false) ;
                break ;
         }
         return (0) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (IsIconic (render_window))
         {
           EndPaint (hwnd, &ps) ;
           return (0) ;
         }
         oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
         if (hPalApp)
         {
           SelectPalette (hdc, hPalApp, false) ;
           RealizePalette (hdc) ;
         }
         if (zoomed)
         {
           GetClientRect (hwnd, &rect) ;
           if (dest_width < rect.right)
           {
             BitBlt (hdc, 0, 0, dest_xoffset + 1, rect.bottom, NULL, 0, 0, BLACKNESS) ;
             BitBlt (hdc, dest_width + dest_xoffset, 0, dest_xoffset + 1, rect.bottom + 1, NULL, 0, 0, BLACKNESS) ;
           }
           if (dest_height < rect.bottom)
           {
             BitBlt (hdc, 0, 0, rect.right, dest_yoffset + 1, NULL, 0, 0, BLACKNESS) ;
             BitBlt (hdc, 0, dest_height + dest_yoffset, rect.right, dest_yoffset + 1, NULL, 0, 0, BLACKNESS) ;
           }
         }
         else
         {
           dest_xoffset = -renderwin_xoffset ;
           dest_yoffset = -renderwin_yoffset ;
           dest_width = render_width ;
           dest_height = render_height ;
           GetClientRect (hwnd, &rect) ;
           if (rect.right > dest_width)
             BitBlt (hdc, dest_width, 0, rect.right - dest_width, rect.bottom, NULL, 0, 0, BLACKNESS) ;
           if (rect.bottom > dest_height)
             BitBlt (hdc, 0, dest_height, rect.right, rect.bottom - dest_height, NULL, 0, 0, BLACKNESS) ;
         }
         if (StretchDIBits (hdc,
                            dest_xoffset,
                            dest_yoffset,
                            dest_width,
                            dest_height,
                            0,
                            0,
                            render_bitmap.header.biWidth,
                            render_bitmap.header.biHeight,
                            render_bitmap_surface,
                            (LPBITMAPINFO) &render_bitmap,
                            DIB_RGB_COLORS,
                            SRCCOPY) <= 0)
         {
           // hmmmm ... it seems we've run into a Windows bug of some form. When rendering a
           // large scene file (it used some 200mb of swap plus 80+mb of real memory on a 128mb
           // box) at a resolution of 1280x1024 (same as screen resolution) on Windows NT 4.0,
           // StretchDIBits () was observed to return zero (which is not failure, but not success
           // either :).

           GetClientRect (render_window, &rect) ;
           BitBlt (hdc, 0, 0, dest_width, dest_height, NULL, 0, 0, WHITENESS) ;
           if (errorBitmap == NULL)
             errorBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_STRETCHDIBITS)) ;
           hdcMemory = CreateCompatibleDC (hdc) ;
           oldBmp = (HBITMAP)SelectObject (hdcMemory, errorBitmap) ;
           BitBlt (hdc, rect.right / 2 - 157, rect.bottom / 2 - 10, 315, 21, hdcMemory, 0, 0, SRCCOPY) ;
           SelectObject (hdcMemory, oldBmp) ;
           DeleteDC (hdcMemory) ;
         }
         SetStretchBltMode (hdc, oldMode) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_DESTROY :
         // it is possible for an old render window to be in the process of being destroyed
         // when a new one has already been created. check for this here.
         if (render_window == hwnd)
         {
           render_window = NULL ;
           renderwin_destroyed = true ;
           PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
           PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovMainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   i ;
  char                  *s ;
  char                  str [4096] ;
  bool                  f ;
  HDC                   hdc ;
  RECT                  rect ;
  POINT                 pt ;
  NMHDR                 *nmh ;
  DWORD                 result = 0 ;
  HPALETTE              oldPalette ;
  TOOLTIPTEXT           *t  ;
  COPYDATASTRUCT        *cd ;

  switch (message)
  {
    case POVMS_QUEUE_MESSAGE:
         if (!backend_thread_exception)
           POVMS_ProcessMessages (POVMS_GUI_Context, false) ;
         break ;
  
    case WM_COPYDATA :
         cd = (COPYDATASTRUCT *) lParam ;
         if (cd->dwData == EDIT_FILE)
         {
           strncpy (str, (char *) cd->lpData, sizeof (str) - 1) ;
           str [sizeof (str) - 1] = '\0' ;
           if (EditGetFlags () & EDIT_CAN_OPEN)
             EditOpenFile (str) ;
           return (0) ;
         }
         if (cd->dwData == RENDER_FILE)
         {
           strncpy (str, (char *) cd->lpData, sizeof (str) - 1) ;
           str [sizeof (str) - 1] = '\0' ;
           if (rendering)
             return (0) ;
           strcpy (source_file_name, str) ;
           start_rendering (false) ;
           return (0) ;
         }
         return (1) ;

    case COPY_COMMANDLINE_MESSAGE :
         command_line [sizeof (command_line) - 1] = '\0' ;
         strncpy (command_line, (LPCSTR) lParam, sizeof (command_line) - 1) ;
         SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
         SendMessage (toolbar_cmdline, EM_SETSEL, 0, strlen (command_line)) ;
         SetFocus (toolbar_cmdline) ;
         return (true) ;
    
    case WM_HELP :
         // we expect that whatever routine caused the WM_HELP would have set up the keyword
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;
         
    case KEYWORD_LOOKUP_MESSAGE :
         hh_aklink.pszKeywords = (LPCSTR) lParam ;
         if (strncmp (hh_aklink.pszKeywords, "oooo", 4) == 0)
           hh_aklink.pszKeywords = ""  ;
         if (strncmp (hh_aklink.pszKeywords, "//", 2) == 0)
           hh_aklink.pszKeywords = ""  ;
         if (strncmp (hh_aklink.pszKeywords, "oooo", 4) == 0)
           hh_aklink.pszKeywords = ""  ;
         if (strncmp (hh_aklink.pszKeywords, "//", 2) == 0)
           hh_aklink.pszKeywords = ""  ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case TASKBAR_NOTIFY_MESSAGE :
         if (lParam == WM_LBUTTONDBLCLK)
         {
           ShowWindow (main_window, SW_SHOW) ;
           ShowWindow (main_window, SW_RESTORE) ;
           if (render_window)
           {
             ShowWindow (render_window, SW_SHOW) ;
             renderwin_hidden = false ;
           }
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (lParam == WM_RBUTTONDOWN)
         {
           if (hPopupMenus != NULL)
           {
             GetCursorPos (&pt) ;
             SetForegroundWindow (main_window) ;
             TrackPopupMenu (GetSubMenu (hPopupMenus, 0), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
             return (0) ;
           }
         }
         return (0) ;

    case WM_ENTERSIZEMOVE :
         if (!IsW95UserInterface)
           break ;
         resizing = true ;
         break ;

    case WM_EXITSIZEMOVE :
         if (!IsW95UserInterface)
           break ;
         resizing = false ;
         InvalidateRect (message_window, NULL, true) ;
         break ;

    case WM_SETFOCUS :
         // After a dialog has been displayed, Windows will give the focus
         // back to our main window. We need to farm the focus off to whatever
         // window should have it. EditSetFocus () will handle this for us.
         EditSetFocus () ;
         return (0) ;

    case EDITOR_RENDER_MESSAGE :
         if (rendering)
         {
           if (OkToStopRendering ())
           {
             stop_rendering = true ;
             cancel_render () ;
           }
           return (0) ;
         }
         strcpy (source_file_name, (char *) lParam) ;
         splitpath (source_file_name, lastRenderPath, lastRenderName) ;
         if (!ExternalDragFunction (source_file_name, dfRenderEditor))
           start_rendering (false) ;
         return (0) ;

    case CREATE_RENDERWIN_MESSAGE :
         return (renderwin_init ()) ;

    case GUIEXT_CREATE_EDITOR :
         if (wParam == 0)
         {
           message_printf ("External application or GUIEXT sent zero-length wParam for GUIEXT_CREATE_EDITOR\n") ;
           return (0) ;
         }
         if (IsBadReadPtr ((void *) lParam, wParam + 1))
         {
           message_printf ("External application or GUIEXT sent bad paramstr address for GUIEXT_CREATE_EDITOR\n") ;
           return (0) ;
         }
         if (((char *) lParam) [wParam] != '\0')
         {
           message_printf ("External application or GUIEXT sent non-NULL terminated paramstr for GUIEXT_CREATE_EDITOR\n") ;
           return (0) ;
         }
         return (EditExternalOpenFile ((char *) lParam)) ;

    case WM_NOTIFY :
         nmh = (NMHDR *) lParam ;
         if (nmh->hwndFrom == tab_window)
         {
           EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
           break ;
         }
         if (nmh->hwndFrom == rebar_window)
         {
           switch (nmh->code)
           {
             case RBN_HEIGHTCHANGE :
                  if (!use_toolbar)
                    break ;
                  GetClientRect (rebar_window, &rect) ;

                  // under XP with comctrl v6 it has been noticed that this event will occur
                  // when the main window is minimized, before the parent's WM_SIZE message
                  // is received, and that the return value from GetClientRect () seems to
                  // be rather strange (e.g. 0,0,202,0 where the actual height is about 75.)
                  if (rect.right == rect.left)
                    break ;

                  toolheight = rect.bottom ;
                  CalculateClientWindows (true) ;

                // need this due to an issue with Windows 95
                  if (top_message_row)
                  {
                    ShowScrollBar (message_window, SB_VERT, false) ;
                    ShowScrollBar (message_window, SB_VERT, true) ;
                  }
                  if (need_hscroll ())
                  {
                    ShowScrollBar (message_window, SB_HORZ, false) ;
                    ShowScrollBar (message_window, SB_HORZ, true) ;
                  }
                  break ;
           }
           break ;
         }
         switch (nmh->code)
         {
           case TTN_NEEDTEXT :
                t = (TOOLTIPTEXT *) lParam ;
                if (use_tooltips == 0)
                {
                  t->lpszText = NULL ;
                  t->hinst = 0 ;
                  break ;
                }
                t->hinst = hInstance ;
                t->lpszText = MAKEINTRESOURCE (t->hdr.idFrom) ;
                return (0) ;
         }
         break ;

    case RENDER_MESSAGE :
         s = getCommandLine () ;
         if (rendering && (strlen (s) || wParam))
         {
           PovMessageBox ("Cannot accept new command - already rendering", "Warning") ;
           return (0) ;
         }
         if (main_window_hidden)
         {
           ShowWindow (main_window, SW_SHOW) ;
           ShowWindow (main_window, SW_RESTORE) ;
           if (render_window != NULL)
           {
             ShowWindow (render_window, SW_SHOW) ;
             renderwin_hidden = false ;
           }
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (wParam == 0)
         {
           if (strlen (s) == 0)
             return (0) ;
           if (!restore_command_line)
             strcpy (old_command_line, command_line) ;
           restore_command_line = true ;
           strcpy (command_line, s) ;
           start_rendering (true) ;
           strcpy (command_line, old_command_line) ;
           SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
           restore_command_line = false ;
         }
         else
           handle_main_command (CM_DEMO, 0) ;
         return (0) ;

    case WM_CREATE :
         main_window = hwnd ;
         hMainMenu = CreateMenu () ;
         build_main_menu (hMainMenu, false) ;
         SetMenu (main_window, hMainMenu) ;
         break ;

    case WM_QUERYENDSESSION :
         if (rendering)
         {
           if (MessageBox (main_window, "POV-Ray is currently rendering - do you want to stop ?", "Stop rendering ?", MB_ICONQUESTION | MB_YESNO) != IDYES)
             return (false) ;
           if (!EditCanClose (true))
             return (false) ;
           if (!quit)
             quit = time (NULL) ;
           cancel_render () ;
           return (true) ;
         }
         return (EditCanClose (true)) ;

    case WM_ENDSESSION :
         if (wParam != 0)
         {
           setRunOnce () ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
             {
               strcpy (command_line, old_command_line) ;
               SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
               restore_command_line = false ;
             }
             write_INI_settings (EngineIniFileName) ;
             EditSaveState () ;
           }
         }
         break ;

    case WM_COMMAND :
         if ((HANDLE) lParam == toolbar_cmdline)
         {
           // need to use EN_CHANGE rather than EN_KILLFOCUS as the command-line dialog
           // will grab and possibly modify command_line before focus is lost, should it
           // be activated while the edit control has focus.
           if (HIWORD (wParam) == EN_CHANGE)
           {
             SendMessage (toolbar_cmdline, WM_GETTEXT, sizeof (command_line) - 1, (LPARAM) command_line) ;
             return (0) ;
           }
         }
         if ((HANDLE) lParam == toolbar_combobox)
         {
           if (HIWORD (wParam) == CBN_CLOSEUP)
           {
             cb_expect_selchange++ ;
             return (0) ;
           }
           if (HIWORD (wParam) == CBN_SELCHANGE)
           {
             i = SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0) ;
             if (i == SendMessage (toolbar_combobox, CB_GETCOUNT, 0, 0) - 1)
             {
               SendMessage (toolbar_combobox, CB_SETCURSEL, tb_combo_sel, 0) ;
               if (cb_expect_selchange)
               {
                 hh_aklink.pszKeywords = "Adding New Resolutions" ;
                 HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
               }
             }
             else
               tb_combo_sel = i ;
             cb_expect_selchange = 0 ;
             return (0) ;
           }
         }
         if (EditPassOnMessage (hwnd, message, wParam, lParam, &result))
           return (result) ;
         if (ExtensionsEnabled)
           if (LOWORD (wParam) >= CM_FIRSTGUIEXT && LOWORD (wParam) <= CM_LASTGUIEXT)
             return (ExternalMenuSelect (LOWORD (wParam))) ;
         if (handle_main_command (wParam, lParam))
           return (0) ;
         break ;

    case WM_INITMENU :
         EditUpdateMenus ((HMENU) wParam) ;
         EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
         break ;

    case WM_ACTIVATEAPP :
         EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
         break ;

    case WM_TIMER :
         if (delay_next_status)
         {
           delay_next_status -= 250 ;
           if (delay_next_status < 0)
             delay_next_status = 0 ;
         }
         if (status_buffer [0] != '\0' && delay_next_status == 0)
         {
           say_status_message (StatusMessage, status_buffer) ;
           status_buffer [0] = '\0' ;
         }
         EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
         if (TimerTicks++ % 4 != 3)
           break ;
         seconds++ ;
         if (splash_window != NULL && seconds >= splash_time)
         {
           DestroyWindow (splash_window) ;
           splash_window = NULL ;
           if (splash_show_about)
             ShowAboutBox () ;
         }
         ExternalEvent (EventTimer, seconds) ;
         if (MenuBarDraw)
         {
           DrawMenuBar (main_window) ;
           MenuBarDraw = false ;
         }
         if (!rendering)
         {
           if (auto_render)
           {
             if (queued_file_count)
             {
               queued_file_count-- ;
               update_queue_status (true) ;
               strcpy (source_file_name, queued_files [0]) ;
               memcpy (queued_files [0], queued_files [1], sizeof (queued_files) - sizeof (queued_files [0])) ;
               splitpath (source_file_name, dir, NULL) ;
               SetCurrentDirectory (dir) ;
               if (seconds < 60 && GetPrivateProfileInt ("Info", "Rendering", 0, EngineIniFileName) != 0)
               {
                 // don't run the file if we were rendering when POV exited.
                 // [Rendering should only be set if there was an abnormal exit.]
                 PutPrivateProfileInt ("Info", "Rendering", 0, EngineIniFileName) ;
                 message_printf ("Skipping queued file '%s' (possible abnormal exit)\n", source_file_name) ;
                 buffer_message (mDivider, "\n") ;
               }
               else
                 if (!ExternalDragFunction (source_file_name, dfRenderFileQueue))
                   start_rendering (false) ;
             }
           }
         }
         else
         {
           PrintRenderTimes (false, false) ;
           if ((IsIconic (main_window) && !IsW95UserInterface) && render_bitmap_surface != NULL)
             SendMessage (main_window, WM_NCPAINT, 0, 0L) ;
           if (rendersleep)
             FlashWindow (main_window, seconds & 0x01) ;
           if (strcmp (render_percentage, last_render_percentage) != 0)
           {
             strcpy (last_render_percentage, render_percentage) ;
             sprintf (str, "%s complete", render_percentage) ;
             SetWindowText (render_window, str) ;
             if (main_window_hidden)
             {
               sprintf (str, "POV-Ray [%s complete] (Restore: DblClk ; Menu: Mouse2)", render_percentage) ;
               TaskBarModifyIcon (main_window, 0, str) ;
             }
           }
         }
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == main_window)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (hPalApp)
         {
           hdc = GetDC (main_window) ;
           oldPalette = SelectPalette (hdc, hPalApp, false) ;
           f = (RealizePalette (hdc) != 0);
           SelectPalette (hdc, oldPalette, false) ;
           ReleaseDC (main_window, hdc) ;
           if (f)
           {
             PovInvalidateRect (hwnd, NULL, true) ;
             if ((EditGetFlags () & EDIT_MSG_SELECTED) == 0)
               PovInvalidateRect (message_window, NULL, true) ;
             PovInvalidateRect (render_window, NULL, true) ;
           }
         }
         return (0) ;

    case WM_SIZE :
         if (main_window_hidden)
         {
           // perhaps another process has sent us a SIZE_RESTORED or something
           if (render_window)
           {
             ShowWindow (render_window, SW_SHOW) ;
             renderwin_hidden = false ;
           }
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
         }
         mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
         GetWindowPlacement (main_window, &mainwin_placement) ;
         SendMessage (rebar_window, WM_SIZE, wParam, lParam) ;
//       MoveWindow (rebar_window, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
         SendMessage (StatusWindow, WM_SIZE, wParam, lParam) ;
         ResizeStatusBar (StatusWindow) ;
         switch (wParam)
         {
           case SIZE_MINIMIZED :
                SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
                if ((render_above_main || hide_render_window) && render_window != NULL)
                {
                  ShowWindow (render_window, SW_HIDE) ;
                  renderwin_hidden = true ;
                }
                ExternalEvent (EventSize, wParam) ;
                return (0) ;

           case SIZE_RESTORED :
                SendMessage (StatusWindow, WM_SIZE, wParam, lParam) ;
                s = EditGetFilename (true) ;
                if (s != NULL && *s != '\0')
                {
                  sprintf (str, rendersleep ? "POV-Ray (paused) - %s" : "POV-Ray - %s", s) ;
                  SetCaption (str) ;
                }
                else
                  SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
                // ***** fall through *****

           case SIZE_MAXIMIZED :
                if (render_window != NULL)
                {
                  ShowWindow (render_window, renderwin_active ? SW_SHOW : SW_SHOWNA) ;
                  renderwin_hidden = false ;
                }
                SendMessage (toolbar_window, TB_AUTOSIZE, 0, 0) ;
                if (use_toolbar && rebar_window != NULL)
                {
                  GetClientRect (rebar_window, &rect) ;
                  toolheight = rect.bottom ;
                }
                CalculateClientWindows (true) ;

                // need this due to an issue with Windows 95
                if (top_message_row)
                {
                  ShowScrollBar (message_window, SB_VERT, false) ;
                  ShowScrollBar (message_window, SB_VERT, true) ;
                }
                if (need_hscroll ())
                {
                  ShowScrollBar (message_window, SB_HORZ, false) ;
                  ShowScrollBar (message_window, SB_HORZ, true) ;
                }
                ExternalEvent (EventSize, wParam) ;
                break ;

           case SIZE_MAXHIDE :
           case SIZE_MAXSHOW :
           default :
                ExternalEvent (EventSize, wParam) ;
                return (0) ;
         }
         return (0) ;

    case WM_MOVE :
         mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
         GetWindowPlacement (main_window, &mainwin_placement) ;
         ExternalEvent (EventMove, lParam) ;
         return (0) ;

    case WM_ERASEBKGND :
         if (IsIconic (main_window))
         {
           BitBlt ((HDC) wParam, 0, 0, 36, 36, NULL, 0, 0, BLACKNESS) ;
           return (1) ;
         }
         break ;

    case WM_DROPFILES :
         DragFunction ((HDROP) wParam) ;
         return (0) ;

    case WM_CHAR :
         switch ((char) wParam)
         {
           case 0x0f : // ctrl-o
                EditBrowseFile (true) ;
                return (0) ;

           case 0x0e : // ctrl-n (close enough to shift-ctrl-n ;)
                EditOpenFile (NULL) ;
                return (0) ;
         }
         if (EditPassOnMessage (hwnd, message, wParam, lParam, &result))
           return (0) ;
         break ;

    case WM_KEYDOWN :
         for (i = 0 ; key2scroll [i].wVirtkey != 0xffff ; i++)
         {
           if (wParam == key2scroll [i].wVirtkey)
           {
             SendMessage (message_window, key2scroll [i].iMessage, key2scroll [i].wRequest, 0L) ;
             return (0) ;
           }
         }
         break ;

    case WM_MENUSELECT :
         if (EditPassOnMessage (hwnd, message, wParam, lParam, &result))
           return (result) ;
         handle_menu_select (wParam, lParam) ;
         return (0) ;

    case WM_SHOWWINDOW :
         if (debugging)
           message_printf ("WM_SHOWWINDOW [%08x, %08x]\n", wParam, lParam) ;
         break ;

    case WM_CLOSE :
         if (debugging)
           message_printf ("Close requested, rendering is %d, quit is %u\n", rendering, quit) ;
         if (rendering && !quit)
         {
           if (MessageBox (main_window,
                           "POV-Ray is currently rendering - do you want to stop ?",
                           "Stop rendering",
                           MB_ICONQUESTION | MB_YESNO) == IDNO)
           {
             if (debugging)
               message_printf ("User tells us we can't close\n") ;
             return (0) ;
           }
         }
         if (!EditCanClose (true))
         {
           if (debugging)
             message_printf ("Editor tells us we can't close\n") ;
           return (0) ;
         }
         ExternalEvent (EventClose, 0) ;
         if (timer_id != 0)
           KillTimer (main_window, timer_id) ;
         DragAcceptFiles (main_window, false) ;
         if (!rendering || quit)
         {
           DestroyWindow (main_window) ;
         }
         else
         {
           if (!quit)
             quit = time (NULL) ;
           cancel_render () ;
         }
         return (0) ;

    case WM_DESTROY :
         if (debugging)
           message_printf ("Destroy requested, rendering is %d, quit is %u\n", rendering, quit) ;
         if (!quit)
         {
           quit = time (NULL) ;
           if (rendering)
             cancel_render () ;
         }
         ExternalEvent (EventDestroy, 0) ;
         if (save_settings)
         {
           SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
           if (restore_command_line)
           {
             strcpy (command_line, old_command_line) ;
             SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
             restore_command_line = false ;
           }
           write_INI_settings (EngineIniFileName) ;
           EditSaveState () ;
         }
         PostQuitMessage (0) ;
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovMessageWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   nhs ;
  int                   msg ;
  int                   mousewheel ;
  HDC                   hdc ;
  RECT                  rect ;
  POINT                 pt ;
  PAINTSTRUCT           ps ;
  static bool           captured = false ;
  static POINT          mbdownPoint ;

  switch (message)
  {
    case WM_MOUSEWHEEL :
         mousewheel = (short) (wParam >> 16) / WHEEL_DELTA ;
         if ((LOWORD (wParam) & (MK_MBUTTON | MK_CONTROL)) == 0)
         {
           msg = mousewheel < 0 ? SB_LINEDOWN : SB_LINEUP ;
           mousewheel *= 3 ;
         }
         else
           msg = mousewheel < 0 ? SB_PAGEDOWN : SB_PAGEUP ;
         mousewheel = abs (mousewheel) ;
         while (mousewheel--)
           PovMessageWndProc (hwnd, WM_VSCROLL, msg, 0) ;
         return (0) ;

    case WM_KEYDOWN :
         PostMessage (main_window, message, wParam, lParam) ;
         return (0) ;

    case WM_LBUTTONDOWN :
         SetCapture (hwnd) ;
         captured = true ;
         GetCursorPos (&mbdownPoint) ;
         return (0) ;

    case WM_LBUTTONUP :
         if (!captured)
           return (0) ;
         ReleaseCapture () ;
         captured = false ;
         GetCursorPos (&pt) ;
         GetWindowRect (hwnd, &rect) ;
         if (pt.x < rect.left || pt.y < rect.top || pt.x > rect.right || pt.y > rect.bottom)
           return (0) ;
         if (abs (mbdownPoint.x - pt.x) > 3 || abs (mbdownPoint.y - pt.y) > 3)
         {
           hh_aklink.pszKeywords = "text streams" ;
           MessageBox (hwnd,
                       "You may use the Edit menu to copy the contents of this message pane to the clipboard\n\n"
                       "Press Help to learn how to direct the POV-Ray text output streams to a file",
                       "Text Selection Not Supported In This Window",
                       MB_OK | MB_ICONINFORMATION | MB_HELP) ;
         }
         return (0) ;

    case WM_RBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           ClientToScreen (hwnd, &pt) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 0),
                           TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                           pt.x,
                           pt.y,
                           0,
                           main_window,
                           NULL) ;
         }
         return (0) ;

    case WM_ERASEBKGND :
         return (1) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (hPalApp)
         {
           SelectPalette (hdc, hPalApp, false) ;
           RealizePalette (hdc) ;
         }
         paint_display_window (hdc) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_SIZE :
         if (message_count)
         {
           GetClientRect (hwnd, &rect) ;
           message_scroll_pos_x = 0 ;
           message_scroll_pos_y = message_count - rect.bottom / message_ychar ;
           if (message_scroll_pos_y < 0)
             message_scroll_pos_y = 0 ;
         }
         update_message_display (None) ;
         PovInvalidateRect (hwnd, NULL, true) ;
         return (0) ;

    case WM_VSCROLL :
         switch (LOWORD (wParam))
         {
           case SB_LINEDOWN :
                if (message_scroll_pos_y < message_count - message_rows)
                {
                  message_scroll_pos_y++ ;
                  ScrollWindow (hwnd, 0, -message_ychar, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_LINEUP :
                if (message_scroll_pos_y > 0)
                {
                  message_scroll_pos_y-- ;
                  ScrollWindow (hwnd, 0, message_ychar, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_PAGEDOWN :
                if (message_scroll_pos_y < message_count - message_rows)
                {
                  message_scroll_pos_y += message_rows ;
                  if (message_scroll_pos_y > message_count - message_rows)
                    message_scroll_pos_y = message_count - message_rows ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_PAGEUP :
                if (message_scroll_pos_y > 0)
                {
                  message_scroll_pos_y -= message_rows ;
                  if (message_scroll_pos_y < 0)
                    message_scroll_pos_y = 0 ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                message_scroll_pos_y = HIWORD (wParam) ;
                PovInvalidateRect (hwnd, NULL, true) ;
                update_message_display (None) ;
                break ;
         }
         return (0) ;

    case WM_HSCROLL :
         nhs = need_hscroll () ;
         switch (LOWORD (wParam))
         {
           case SB_LINERIGHT :
                if (message_scroll_pos_x < nhs)
                {
                  message_scroll_pos_x++ ;
                  ScrollWindow (hwnd, -message_xchar, 0, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_LINELEFT :
                if (message_scroll_pos_x > 0)
                {
                  message_scroll_pos_x-- ;
                  ScrollWindow (hwnd, message_xchar, 0, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_PAGERIGHT :
                if (message_scroll_pos_x < nhs)
                {
                  message_scroll_pos_x += message_cols ;
                  if (message_scroll_pos_x > nhs)
                    message_scroll_pos_x = nhs ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_PAGELEFT :
                if (message_scroll_pos_x > 0)
                {
                  message_scroll_pos_x -= message_cols ;
                  if (message_scroll_pos_x < 0)
                    message_scroll_pos_x = 0 ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                message_scroll_pos_x = HIWORD (wParam) ;
                PovInvalidateRect (hwnd, NULL, true) ;
                update_message_display (None) ;
                break ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovSplashWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HDC         hdc ;
  HDC         hdcMemory ;
  bool        f ;
  HBITMAP     oldBmp ;
  HPALETTE    oldPalette ;
  PAINTSTRUCT ps ;

  switch (message)
  {
    case WM_DESTROY :
         if (screen_depth <= 8)
         {
           hdc = GetDC (hwnd) ;
           BitBlt (hdc, 0, 0, splash_width, splash_height, NULL, 0, 0, BLACKNESS) ;
           ReleaseDC (hwnd, hdc) ;
         }
         DeleteObject (hBmpSplash) ;
         hBmpSplash = NULL ;
         if (splash_palette)
         {
           DeleteObject (splash_palette) ;
           splash_palette = NULL ;
         }
         break ;

    case WM_SYSKEYDOWN :
    case WM_KEYDOWN :
    case WM_LBUTTONUP :
         DestroyWindow (hwnd) ;
         if (splash_show_about)
           ShowAboutBox () ;
         return (0) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (splash_palette)
         {
           SelectPalette (hdc, splash_palette, false) ;
           RealizePalette (hdc) ;
         }
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpSplash) ;
         BitBlt (hdc, 0, 0, splash_width, splash_height, hdcMemory, 0, 0, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == hwnd)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (splash_palette)
         {
           hdc = GetDC (hwnd) ;
           oldPalette = SelectPalette (hdc, splash_palette, false) ;
           f = (RealizePalette (hdc) != 0);
           SelectPalette (hdc, oldPalette, false) ;
           ReleaseDC (hwnd, hdc) ;
           if (f)
             PovInvalidateRect (hwnd, NULL, true) ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovRenderAnimWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   line ;
  int                   page ;
  int                   lastpage ;
  HDC                   hdc ;
  HDC                   hdcMemory ;
  HBITMAP               oldBmp ;
  MINMAXINFO            *mm ;
  PAINTSTRUCT           ps ;
  static int            pages ;
  static BITMAP         bm ;
  static HBITMAP        hBmp ;

  switch (message)
  {
    case WM_SIZE :
         if (LOWORD (lParam) > 120)
           SetWindowPos (hwnd, NULL, 0, 0, 120, 42, SWP_NOMOVE | SWP_NOREDRAW) ;
         return (0) ;

    case WM_GETMINMAXINFO :
         mm = (MINMAXINFO *) lParam ;
         mm->ptMaxSize.x = 120 ;
         mm->ptMaxSize.y = 42 ;
         return (0) ;

    case WM_CREATE :
         hBmp = (HBITMAP)LoadImage (hInstance, MAKEINTRESOURCE (screen_depth <= 8 ? BMP_RENDERANIM_4BPP : BMP_RENDERANIM), IMAGE_BITMAP, 0, 0, 0) ;
         GetObject (hBmp, sizeof (BITMAP), (LPSTR) &bm) ;
         pages = bm.bmHeight / 42 ;
         break ;

    case WM_TIMER :
         if (use_renderanim && rendering)
         {
           if (hBmp == NULL || rendersleep)
             return (0) ;
           line = render_anim_count % 42 ;
           page = (render_anim_count / 42) % pages + 1 ;
           if (page >= pages)
             page = 0 ;
           hdc = GetDC (hwnd) ;
           hdcMemory = CreateCompatibleDC (hdc) ;
           oldBmp = (HBITMAP)SelectObject (hdcMemory, hBmp) ;
           BitBlt (hdc, 0, line, 120, 1, hdcMemory, 0, page * 42 + line, SRCCOPY) ;
           SelectObject (hdcMemory, oldBmp) ;
           DeleteDC (hdcMemory) ;
           ReleaseDC (hwnd, hdc) ;
           if (render_anim_count++ == 0)
             InvalidateRect (hwnd, NULL, false) ;
         }
         return (0) ;

    case WM_PAINT :
         if (hBmp == NULL)
           break ;
         hdc = BeginPaint (hwnd, &ps) ;
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = (HBITMAP)SelectObject (hdcMemory, hBmp) ;
         if (use_renderanim && rendering)
         {
           line = render_anim_count % 42 ;
           page = (render_anim_count / 42) % pages + 1 ;
           if (page >= pages)
             page = 0 ;
           lastpage = render_anim_count / 42 % pages ;
           BitBlt (hdc, 0, line, 120, 42 - line, hdcMemory, 0, lastpage * 42 + line, SRCCOPY) ;
           BitBlt (hdc, 0, 0, 120, line, hdcMemory, 0, page * 42, SRCCOPY) ;
         }
         else
           BitBlt (hdc, 0, 0, 120, 42, hdcMemory, 0, (pages - 1) * 42, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

  }

  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

int register_classes (void)
{
  WNDCLASSEX  wc ;

  // the parameter to RegisterClass is considered CONST, so we
  // can assume that the structure is not changed by the calls.
  wc.cbSize        = sizeof (wc) ;
  wc.hIconSm       = NULL ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = hInstance ;
  wc.lpszMenuName  = NULL ;
  wc.hbrBackground = NULL ;

  // Register the main window class.
  wc.style         = 0 ;
  wc.lpfnWndProc   = PovMainWndProc ;
  wc.hIcon         = ourIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovMainWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the message window class.
  wc.style         = 0 ;
  wc.lpfnWndProc   = PovMessageWndProc ;
  wc.hIcon         = NULL ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovMessageWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the render window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovRenderWndProc ;
  wc.hIcon         = renderIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_CROSS) ;
  wc.lpszClassName = PovRenderWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the splash window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovSplashWndProc ;
  wc.hIcon         = ourIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovSplashWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the about window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovAboutWndProc ;
  wc.hIcon         = ourIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovAboutWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the render animation window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovRenderAnimWndProc ;
  wc.hIcon         = NULL ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.hbrBackground = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
  wc.lpszClassName = PovRenderAnimWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  return (true) ;
}

void cleanup_all (void)
{
  ExternalCleanupAll () ;
  run_backend_thread = false ;
  for (int i = 0 ; i < 500 ; i++)
  {
    if (hBackendThread == NULL)
      break ;
    Sleep (10) ;
  }
  if (hBackendThread != NULL)
  {
    debug_output ("Have to nuke backend thread\n") ;
#ifdef _DEBUG
    MessageBox (NULL, "Have to nuke backend thread", "cleanup_all", MB_OK | MB_ICONEXCLAMATION) ;
#endif
    TerminateThread (hBackendThread, -1) ;
  }
  debug_output ("Calling EditUnload ()\n") ;
  EditUnload () ;
  debug_output ("Returned from EditUnload ()\n") ;
  if (use_taskbar)
    TaskBarDeleteIcon (main_window, 0) ;
  debug_output ("Deleting critical section\n") ;
  DeleteCriticalSection (&critical_section) ;
  debug_output ("Critical section deleted\n") ;
  display_cleanup () ;
  if (hBmpBackground != NULL)
    DeleteObject (hBmpBackground) ;
  if (hBmpRendering != NULL)
    DeleteObject (hBmpRendering) ;
  if (hBmpIcon != NULL)
    DeleteObject (hBmpIcon) ;
  if (hBmpSplash != NULL)
    DeleteObject (hBmpSplash) ;
  if (hMenuBar)
    DestroyMenu (hMenuBar) ;
  if (hMainMenu)
    DestroyMenu (hMainMenu) ;
  if (hPopupMenus)
    DestroyMenu (hPopupMenus) ;
  if (hPalApp)
    DeleteObject (hPalApp) ;
  if (message_font)
    DeleteObject (message_font) ;
  if (about_font)
    DeleteObject (about_font) ;
  if (tab_font)
    DeleteObject (tab_font) ;
  if (ourIcon)
    DestroyIcon (ourIcon) ;
  if (renderIcon)
    DestroyIcon (renderIcon) ;
  debug_output ("Unregistering classes\n") ;
  UnregisterClass (PovRenderWinClass, hInstance) ;
  UnregisterClass (PovMessageWinClass, hInstance) ;
  UnregisterClass (PovMainWinClass, hInstance) ;
  UnregisterClass (PovSplashWinClass, hInstance) ;
  UnregisterClass (PovRenderAnimWinClass, hInstance) ;
}

#ifdef _MSC_VER
void _ASMAPI LZTimerOn (void)
{
  if (!QueryPerformanceCounter ((LARGE_INTEGER *) &PerformanceCounter1))
    PerformanceCounter1 = 0 ;
}

void _ASMAPI LZTimerOff (void)
{
  if (!QueryPerformanceCounter ((LARGE_INTEGER *) &PerformanceCounter2))
    PerformanceCounter2 = 0 ;
}

ulong _ASMAPI LZTimerCount (void)
{
  if (PerformanceCounter1 == 0 || PerformanceCounter2 < PerformanceCounter1)
    return (0) ;
  return ((ulong) ((PerformanceCounter2 - PerformanceCounter1) / PerformanceScale)) ;
}

__int64 LZTimerRawCount (void)
{
  if (PerformanceCounter1 == 0 || PerformanceCounter2 < PerformanceCounter1)
    return (0) ;
  return (PerformanceCounter2 - PerformanceCounter1) ;
}
#endif // #ifdef _MSC_VER

int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
  int                   show_state ;
  int                   i ;
  int                   id = 0 ;
  int                   w, h ;
  int                   err ;
  char                  str [_MAX_PATH * 2] ;
  char                  *s = szCmdLine ;
  bool                  exit_loop = false ;
  unsigned              n ;
  MSG                   msg ;
  HDC                   hDC ;
  HDIB                  hDIB ;
  RECT                  rect ;
  HWND                  hwnd ;
  DWORD                 help_cookie ;
  DWORD                 threadId = 0 ;
  DWORD                 threadParam = 0 ;
  BITMAP                bm ;
  HBITMAP               hBMP ;
  struct stat           statbuf ;
  WINDOWPLACEMENT       placement ;
#ifdef DEVELOPMENT
  struct stat           st ;
#endif

#ifdef TIMED_BETA
  if (time (NULL) > EXPIRE_AT)
  {
    MessageBox (NULL, "This pre-release version of POV-Ray for Windows has expired", "Pre-release expired", MB_OK | MB_ICONSTOP) ;
    exit (1) ;
  }
#endif

  // need to do this now in case we display a centered dialog.
  // will call it again later once INI file has been read.
  detect_graphics_config () ;

  hInstance = hInst ;
  hMainThread = GetCurrentThread () ;

  while (*s == ' ' || *s == '\t')
    s++ ;
  if (stricmp (s, "/install") == 0 || strnicmp (s, "/install ", 9) == 0 || stricmp (s, "/qinstall") == 0 || strnicmp (s, "/qinstall ", 10) == 0)
  {
    bool quiet = s [1] == 'q' || s [1] == 'Q' ;
    while (*s && *s != ' ')
      s++ ;
    while (*s == ' ' || *s == '"' || *s == '\'')
      s++ ;
    szCmdLine = s ;
    if (*s)
    {
      s += strlen (s) - 1 ;
      while (s > szCmdLine && *s == ' ' || *s == '"' || *s == '\'')
        *s-- = '\0' ;
    }
    if (*szCmdLine == '\0')
      szCmdLine = NULL ;
    return (InstallSettings (szCmdLine, quiet)) ;
  }

  if (SHGetFolderPath (NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, str) == NOERROR)
  {
    strcat(str, "\\POV-Ray");
    if (!dirExists(str))
      CreateDirectory(str, NULL);
    strcat(str, "\\v3.6");
    if (!dirExists(str))
      CreateDirectory(str, NULL);
    strcat(str, "\\");
    strcpy(DocumentsPath, str);
  }

  getHome () ;
  if (BinariesPath [0] == '\0')
  {
    inferHome () ;
    if (BinariesPath [0] == '\0')
    {
      MessageBox (NULL,
                  "ERROR : Cannot find Home entry in registry (and cannot infer it).\n\n"
                  "This entry should have been set by the installation program.\n\n"
                  "POV-Ray can usually infer the installation path but that requires a\n"
                  "standard layout of directories, which also seems to be absent.\n\n"
                  "If you did not install using the correct installation procedure, please\n"
                  "do this before running POV-Ray for Windows. You can also try running\n"
                  "with the '/INSTALL' or '/INSTALL <installdir>' option.",
                  "Critical Error",
                  MB_ICONSTOP) ;
      return (1) ;
    }
  }
  if (BinariesPath [strlen (BinariesPath) - 1] != '\\')
    strcat (BinariesPath, "\\") ;
  if (DocumentsPath[0] == '\0')
    strcpy(DocumentsPath, BinariesPath);
  sprintf(str, "%sini", DocumentsPath);
  if (!dirExists(str))
    CreateDirectory(str, NULL);
  sprintf(ToolIniFileName, "%sini\\pvtools.ini", DocumentsPath);
  sprintf(EngineIniFileName, "%sini\\pvengine.ini", DocumentsPath);
  sprintf (EngineIniDir, "%sini\\", DocumentsPath) ;
  if (!checkRegKey ())
    if (!cloneRegistry ())
      MessageBox (NULL,
                  "ERROR : Could not clone registry - POV-Ray may not work correctly for this user.",
                  "Critical Error",
                  MB_ICONERROR) ;
  if (homeInferred)
  {
    if (stricmp (LastInferredHome, BinariesPath) != 0)
    {
      sprintf (str, "POV-Ray for Windows did not find the expected registry entries present.\n"
                    "This typically means that it has not been installed via the installation program.\n"
                    "You can correct this by running with the '/INSTALL' or '/INSTALL <installdir>' option.\n\n"
                    "POV-Ray has inferred the installation path to be the following:\n\n"
                    "\t%s\n\n"
                    "This message will be displayed each time the inferred path changes.",
                    BinariesPath) ;
      MessageBox (NULL, str, "Warning", MB_ICONINFORMATION) ;
    }
    reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "LastInferredHome", "%s", BinariesPath) ;
  }

  CheckFreshInstall () ;

  sprintf (DefaultRenderIniFileName, "%sini\\povray.ini", DocumentsPath) ;
  sprintf (RerunIniPath, "%sini\\rerun\\", DocumentsPath) ;
  sprintf (CurrentRerunFileName, "%sini\\rerun", DocumentsPath) ;
  if (!dirExists(CurrentRerunFileName))
      mkdir(CurrentRerunFileName);
  strcat(CurrentRerunFileName, "\\current.ini");

  GetModuleFileName (hInst, str, sizeof (str) - 1) ;
  splitpath (str, modulePath, NULL) ;
  if (modulePath[strlen(modulePath) - 1] == '\\')
    modulePath[strlen(modulePath) - 1] = '\0';
   
  sprintf (engineHelpPath, "%shelp\\povray36.chm", BinariesPath) ;
  HtmlHelp (NULL, NULL, HH_INITIALIZE, (DWORD_PTR) &help_cookie) ;
  memset (&hh_aklink, 0, sizeof (hh_aklink)) ;
  hh_aklink.cbStruct = sizeof (hh_aklink) ;
  hh_aklink.fIndexOnFail = true ;
  hh_aklink.pszWindow = "POV-Ray Help" ;

  SHGetSpecialFolderPath (NULL, FontPath, CSIDL_FONTS, false) ;

  one_instance = GetPrivateProfileInt ("General", "OneInstance", 1, EngineIniFileName) != 0 ;
  if ((hwnd = FindWindow (PovMainWinClass, NULL)) != NULL)
  {
    if (one_instance)
    {
      if (IsIconic (hwnd))
        ShowWindow (hwnd, SW_RESTORE) ;
      SetForegroundWindow (hwnd) ;
      FeatureNotify ("OneInstanceSet",
                     "POV-Ray - 'Keep Single Instance' Feature",
                     "You have started POV-Ray for Windows while another copy is running, "
                     "and the 'Keep Single Instance' option is turned on (see Options menu). "
                     "In this case the other copy is activated rather than starting a new "
                     "instance of the program.\n\nClick &Help for information on this feature.",
                     "Keep Single Instance",
                     false) ;
      SetForegroundWindow (hwnd) ;
      if ((s = preparse_instance_commandline (szCmdLine)) != NULL)
      {
        if (parse_commandline (s) > 1)
        {
          PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
          return (1) ;
        }
        COPYDATASTRUCT cd ;
        cd.dwData = EDIT_FILE ;
        for (i = 0 ; i < EditFileCount ; i++)
        {
          s = EditFiles [i] ;
          if ((isalpha (s [0]) && s [1] == ':') || (s [0] == '\\' && s [1] == '\\'))
          {
            cd.cbData = (int) strlen (s) + 1 ;
            cd.lpData = s ;
          }
          else
          {
            GetCurrentDirectory (sizeof (str), str) ;
            if (s [0] != '\\')
            {
              strcat (str, "\\") ;
              strcat (str, s) ;
            }
            else
              strcpy (str + 2, s) ;
            cd.cbData = (int) strlen (str) + 1 ;
            cd.lpData = str ;
          }
          SendMessage (hwnd, WM_COPYDATA, NULL, (LPARAM) &cd) ;
          free (EditFiles [i]) ;
          EditFiles [i] = NULL ;
        }
        EditFileCount = 0 ;
        if (render_requested)
        {
          cd.dwData = RENDER_FILE ;
          s = requested_render_file ;
          if ((isalpha (s [0]) && s [1] == ':') || (s [0] == '\\' && s [1] == '\\'))
          {
            cd.cbData = (int) strlen (s) + 1 ;
            cd.lpData = s ;
          }
          else
          {
            GetCurrentDirectory (sizeof (str), str) ;
            if (s [0] != '\\')
            {
              strcat (str, "\\") ;
              strcat (str, s) ;
            }
            else
              strcpy (str + 2, s) ;
            cd.cbData = (int) strlen (str) + 1 ;
            cd.lpData = str ;
          }
          SendMessage (hwnd, WM_COPYDATA, NULL, (LPARAM) &cd) ;
        }
        SetForegroundWindow (hwnd) ;
      }
      else
        return (1) ;
      return (0) ;
    }
    else
    {
      // one_instance isn't set. we should continue as per normal.
      // however see if we need to notify the user about this.
      FeatureNotify ("OneInstanceUnset",
                     "POV-Ray - 'Keep Single Instance' Feature",
                     "You have started POV-Ray for Windows while another copy is running, "
                     "and the 'Keep Single Instance' option is turned off (see Options menu). "
                     "In this case a new instance of the program is started rather than "
                     "activating the existing instance of the program.\n\nClick &Help for more "
                     "information on this feature.",
                     "Keep Single Instance",
                     false) ;
    }
  }

  if (strnicmp (szCmdLine, "/DEBUG", 6) == 0)
  {
    debugging = true ;
    debug (NULL) ;
    debugFile = fopen ("c:\\povray.dbg", "wt") ;
  }

  SetThreadPriority (hMainThread, THREAD_PRIORITY_ABOVE_NORMAL) ;
  version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  GetVersionEx (&version_info) ;
  use_editors = true ;

  IsW98 = HaveWin98OrLater () ;
  IsWNT = HaveNT4OrLater () ;
  IsW2k = HaveWin2kOrLater () ;
  IsWXP = HaveWinXPOrLater () ;
  if (HaveWin95OrLater () || HaveNT4OrLater ())
    IsW95UserInterface = true ;

  ourIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IsWXP ? IDI_PVENGINE_XP : IDI_PVENGINE)) ;
  renderIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IsWXP ? IDI_RENDERWINDOW_XP : IDI_RENDERWINDOW)) ;

  if (hPrev == NULL)
    if (register_classes () == false)
      MessageBox (NULL, "ERROR : Could not register classes", "Error", MB_ICONSTOP) ;

  IsComCtl5 = GetDllVersion (TEXT ("comctl32.dll")) >= MAKELONG (5,0) ;
  IsComCtl6 = GetDllVersion (TEXT ("comctl32.dll")) >= MAKELONG (6,0) ;

  detect_graphics_config () ;
  clear_system_palette () ;
  hPalApp = create_palette (NULL, 0) ;
  SplashScreen (NULL) ;

  if (splash_window != NULL)
  {
    while (PeekMessage (&msg, splash_window, 0, WM_USER - 1, PM_REMOVE) != 0 && quit == 0)
    {
      TranslateMessage (&msg) ;
      DispatchMessage (&msg) ;
    }
  }

  if (!QueryPerformanceFrequency ((LARGE_INTEGER *) &PerformanceFrequency))
    PerformanceFrequency = 0 ;
  if (PerformanceFrequency > 1999999)
    PerformanceScale = PerformanceFrequency / 1000000 ;

  init_menus () ;

  if (debugging)
    debug ("INI path is '%s'\n", EngineIniFileName) ;

  IsW95UserInterface = GetPrivateProfileInt ("General", "UseW95UserInterface", 1, EngineIniFileName) != 0 ;
  info_render_complete = GetPrivateProfileInt ("Info", "RenderCompleteSound", 0, EngineIniFileName) != 0 ;

  read_INI_settings (EngineIniFileName) ;

  if (!IsW95UserInterface)
  {
    PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_GRAYED) ;
    use_taskbar = false ;
  }

  create_about_font () ;

  InitializeCriticalSection (&critical_section) ;

  if (GetPrivateProfileInt ("General", "BigSplash", 0, EngineIniFileName) + 86400 < time (NULL))
  {
    splash_time = 4 ;
    if (splash_window != NULL)
      PutPrivateProfileInt ("General", "BigSplash", time (NULL), EngineIniFileName) ;
  }

  GetPrivateProfileString ("General", "CommandLine", "", old_command_line, sizeof (old_command_line), EngineIniFileName) ;
  strcpy (command_line, old_command_line) ;
  if ((szCmdLine = preparse_commandline (szCmdLine)) != NULL)
  {
    if (parse_commandline (szCmdLine) > 1 && !demo_mode)
    {
      restore_command_line = true ;
      strncpy (command_line, szCmdLine, sizeof (command_line) - 1) ;
    }
  }

  sprintf (str, "%s\\" EDITDLLNAME, modulePath) ;
  if (!LoadEditorDLL (str, true))
  {
     sprintf (str, "%sbin\\" EDITDLLNAME, BinariesPath) ;
     if (!LoadEditorDLL (str, false))
       use_editors = false ;
  }

  GetPrivateProfileString ("General", "Version", "", str, (DWORD) strlen (str), EngineIniFileName) ;
  if (debugging)
    debug ("INI version is %s, and we are %s\n", str, POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER) ;

  if (strcmp (str, POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER) != 0)
  {
    // we don't want to set the newVersion flag if the only thing that changed
    // was the compiler used to generate the binary. in this case we add an
    // explicit check for the intel, msvc, and watcom versions.
    if ((s = strstr (str, ".icl")) != NULL)
      strcpy (s, s + 4) ;
    else if ((s = strstr (str, ".msvc")) != NULL)
      strcpy (s, s + 5) ;
    else if ((s = strstr (str, ".watcom")) != NULL)
      strcpy (s, s + 7) ;
    // strip off any trailing digits from the compiler version
    if (s)
      while (isdigit (*s))
        strcpy (s, s + 1) ;
    if (strcmp (str, POV_RAY_VERSION "." PVENGINE_VER) != 0)
      newVersion = true ;
  }

  if ((run_count = GetPrivateProfileInt ("General", "RunCount", 0, EngineIniFileName)) == 0 || newVersion)
  {
    if (screen_depth < 8)
    {
      MessageBox (NULL,
                  "NOTE : POV-Ray for Windows was not designed to run in 16-color mode. "
                  "While the program will operate, it is recommended that you use a minimum "
                  "graphics mode of 800x600x256.",
                  "Warning - running in 16-color mode",
                  MB_ICONEXCLAMATION) ;
      tile_background = false ;
    }
    if (screen_width < 800)
    {
      MessageBox (NULL,
                  "NOTE : POV-Ray for Windows was not designed to run at less than 800x600.\n\n"
                  "While the program will operate, it is recommended that you use a minimum "
                  "graphics mode of 800x600x256.",
                  "Warning - running at less than 800x600",
                  MB_ICONEXCLAMATION) ;
    }
  }
  PutPrivateProfileInt ("General", "RunCount", ++run_count, EngineIniFileName) ;

  if (screen_depth < 8)
    tile_background = false ;

  /* Create the main window */
  placement = mainwin_placement ;
  placement.length = sizeof (WINDOWPLACEMENT) ;
  w = mainwin_placement.rcNormalPosition.right - mainwin_placement.rcNormalPosition.left ;
  h = mainwin_placement.rcNormalPosition.bottom - mainwin_placement.rcNormalPosition.top ;
  if (w <= 0)
    w = 700 ;
  if (h <= 0)
    h = screen_height - 75 ;

  main_window = CreateWindowEx (0,
                                PovMainWinClass,
                                "POV-Ray for Windows",
                                WS_OVERLAPPEDWINDOW,
                                mainwin_placement.rcNormalPosition.left,
                                mainwin_placement.rcNormalPosition.top,
                                w,
                                h,
                                NULL,
                                NULL,
                                hInst,
                                NULL) ;

  if (main_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create main window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  EditSetNotifyBase (main_window, CM_FIRSTEDITNOTIFY) ;

  if ((StatusWindow = CreateStatusbar (main_window)) == NULL)
  {
    MessageBox (main_window, "ERROR : Could not create statusbar", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  if ((timer_id = SetTimer (main_window, 1, 250, NULL)) == 0)
  {
    DestroyWindow (splash_window) ;
    splash_window = NULL ;
  }
  else
    DragAcceptFiles (main_window, true) ;

  // if it's a paletted display, make sure the splash window is shown and removed
  // before the main window is displayed, to avoid palette flashing
  if (screen_depth <= 8 && splash_window != NULL)
  {
    if (timer_id != 0)
    {
      while (seconds < splash_time && GetMessage (&msg, NULL, 0, 0) != 0 && quit == 0)
      {
        if (debugging)
          debug_output ("SplashLoop: %08x %08x %08x %08x\n", msg.hwnd, msg.message, msg.wParam, msg.lParam) ;
        if (!TranslateAccelerator (main_window, hAccelerators, &msg))
        {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
        }
      }
    }
    DestroyWindow (splash_window) ;
    splash_window = NULL ;
  }

  if (tile_background && background_file [1])
  {
    if ((hDIB = LoadDIB (background_file)) != NULL)
    {
      hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
      DeleteObject (hDIB) ;
      GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
      background_width = bm.bmWidth ;
      background_height = bm.bmHeight ;
    }
    else
    {
      PovMessageBox ("Failed to load bitmap file", "Error") ;
      strcpy (background_file, "0") ;
    }
  }

  if (tile_background && hBmpBackground == NULL && screen_depth >= 8)
  {
    if (isdigit (background_file [0]) && background_file [1] == '\0')
      id = background_file [0] - '0' ;
    SendMessage (main_window, WM_COMMAND, CM_BACKGROUNDSTD + id, 1L) ;
  }

  if ((hBMP = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_ICON))) != NULL)
    hBmpIcon = hBMP ;

  if (lastBitmapPath [0] == '\0')
    sprintf (lastBitmapPath, "%stiles", BinariesPath) ;
  if (lastRenderPath [0] == '\0')
  {
    sprintf (lastRenderPath, "%sscenes\\advanced", DocumentsPath) ;
    strcpy (lastRenderName, "biscuit.pov") ;
  }
  if (lastQueuePath [0] == '\0')
    sprintf (lastQueuePath, "%sscenes", DocumentsPath) ;
  GetPrivateProfileString ("Editor", "LastPath", "", str, sizeof (str), EngineIniFileName) ;
  validatePath (lastRenderPath) ;
  if (str [0] == '\0')
    WritePrivateProfileString ("Editor", "LastPath", lastRenderPath, EngineIniFileName) ;
  if (lastRenderName [0] != '\0' && !demo_mode)
    joinPath (source_file_name, lastRenderPath, lastRenderName) ;

  add_rerun_to_menu () ;

  if (use_editors)
    if ((tab_window = InitialiseEditor (main_window, StatusWindow, BinariesPath, DocumentsPath)) == NULL)
      use_editors = 0 ;

  message_window = CreateWindowEx (WS_EX_CLIENTEDGE,
                                   PovMessageWinClass,
                                   "",
                                   WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                   0,
                                   0,
                                   0,
                                   0,
                                   use_editors ? tab_window : main_window,
                                   NULL,
                                   hInst,
                                   NULL) ;
  if (message_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create message window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  if (initialise_message_display ())
  {
    cleanup_all () ;
    return (1) ;
  }
  EditSetMessageWindow (message_window) ;

  if ((rebar_window = create_rebar (main_window)) == NULL)
  {
    MessageBox (main_window, "ERROR : Could not create internal window #1", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }
  if ((toolbar_window = create_toolbar (rebar_window)) == NULL)
  {
    MessageBox (main_window, "ERROR : Could not create internal window #2", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }
  if (!use_toolbar)
  {
    ShowWindow (rebar_window, SW_HIDE) ;
    toolheight = 0 ;
  }
  extract_ini_sections_ex (SecondaryRenderIniFileName, toolbar_combobox) ;
  SendMessage (toolbar_combobox, CB_ADDSTRING, 0, (LPARAM) "More Resolutions ...") ;
  tb_combo_sel = select_combo_item_ex (toolbar_combobox, SecondaryRenderIniFileSection) ;
  if (tb_combo_sel == -1)
    tb_combo_sel = 0 ;

  setup_menus (use_editors) ;
  build_main_menu (hMainMenu, use_editors) ;

  set_toggles () ;

  if (!use_editors)
  {
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILENEW, 0L) ;
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILEOPEN, 0L) ;
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILESAVE, 0L) ;
  }
  else
    EditRestoreState (!NoRestore) ;

  if (use_editors)
    PVCheckMenuItem (CM_USEEDITOR, MF_CHECKED) ;
  PVEnableMenuItem (CM_RENDERHIDE, render_above_main ? MF_GRAYED : MF_ENABLED) ;
  PVEnableMenuItem (CM_RENDERACTIVE, render_above_main ? MF_GRAYED : MF_ENABLED) ;
  PVCheckMenuItem (on_completion, MF_CHECKED) ;
  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  PVEnableMenuItem (CM_RENDERSLEEP, MF_GRAYED) ;

  PVCheckMenuRadioItem (CM_RENDERPRIORITY_LOW, CM_RENDERPRIORITY_HIGH, render_priority) ;

  PVCheckMenuItem (drop_to_editor ? CM_DROPEDITOR : CM_DROPRENDERER, MF_CHECKED) ;

  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  PVModifyMenu (CM_TILEDBACKGROUND,
                MF_STRING,
                CM_TILEDBACKGROUND,
                tile_background ? "&Select Plain Background" : "&Select Tiled Background") ;
  PVCheckMenuRadioItem (CM_DUTYCYCLE_10, CM_DUTYCYCLE_100, CM_DUTYCYCLE_10 + Duty_Cycle) ;
  if (screen_depth < 8)
  {
    PVEnableMenuItem (CM_TILEDBACKGROUND, MF_GRAYED) ;
    PVEnableMenuItem (CM_BACKGROUNDBITMAP, MF_GRAYED) ;
    for (i = 0 ; i < 16 ; i++)
      PVEnableMenuItem (CM_BACKGROUNDSTD + i, MF_GRAYED) ;
  }
  set_newuser_menus (hide_newuser_help) ;

  switch (placement.showCmd)
  {
    case SW_SHOWNORMAL :
         show_state = SW_SHOW ;
         break ;

    case SW_SHOWMINIMIZED :
//       show_state = SW_SHOWMINNOACTIVE ;
         show_state = SW_SHOW ;
         break ;

    case SW_SHOWMAXIMIZED :
         show_state = SW_SHOWMAXIMIZED ;
         break ;

    default :
         show_state = SW_SHOW ;
         break ;
  }

  placement.showCmd = show_state ;
  placement.flags = (placement.ptMinPosition.x == -1 && placement.ptMinPosition.y == -1) ? 0 : WPF_SETMINPOSITION ;
  if (placement.rcNormalPosition.right <= 0 || placement.rcNormalPosition.bottom <= 0)
  {
    placement.rcNormalPosition.right = placement.rcNormalPosition.left + message_xchar * 115 ;
    placement.rcNormalPosition.bottom = placement.rcNormalPosition.top + message_ychar * 75 ;
  }

  placement.length = sizeof (WINDOWPLACEMENT) ;
  SetWindowPlacement (main_window, &placement) ;
  if (splash_window != NULL)
    SetActiveWindow (splash_window) ;
  if (show_state != SW_SHOWMAXIMIZED)
    FitWindowInWindow (NULL, main_window) ;

  // only needed for earlier versions of common control DLL ...
  if (use_toolbar && !IsComCtl5)
  {
    hDC = GetDC (toolbar_window) ;
    GetClientRect (toolbar_window, &rect) ;
    FillRect (hDC, &rect, (HBRUSH) GetStockObject (LTGRAY_BRUSH)) ;
    ReleaseDC (toolbar_window, hDC) ;
  }

  // fixes visual glitch with ComCtl6
  if (use_toolbar && IsComCtl6)
  {
    hDC = GetDC (main_window) ;
    GetClientRect (rebar_window, &rect) ;
    FillRect (hDC, &rect, (HBRUSH) GetStockObject (LTGRAY_BRUSH)) ;
    ReleaseDC (main_window, hDC) ;
  }

  if (ExtensionsEnabled)
    LoadGUIExtensions () ;

  buffer_message (mIDE, "Persistence of Vision Raytracer(tm) for Windows.\n") ;
  buffer_message (mIDE, "POV-Ray for Windows is part of the POV-Ray(tm) suite of programs.\n") ;
  buffer_message (mIDE, "  This is version " POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER ".\n") ;
  buffer_message (mIDE, "Copyright 1991-2003 Persistence of Vision Team.\n") ;
  buffer_message (mIDE, "Copyright 2003-2009 Persistence of Vision Raytracer Pty. Ltd.\n") ;
  WIN_PrintOtherCredits () ;
  buffer_message (mIDE, "  It is usable only under the conditions in the POV-Ray end-user license.\n") ;
  buffer_message (mIDE, "  Select Help|About (or press Alt+B) for more information and a copy of the license.\n") ;
  buffer_message (mIDE, "\n") ;
  buffer_message (mIDE, "The terms POV-Ray and Persistence of Vision Raytracer are trademarks of\n") ;
  buffer_message (mIDE, "  Persistence of Vision Raytracer Pty. Ltd.\n") ;
  if (render_bitmap_depth != 24)
  {
    buffer_message (mIDE, "\n") ;
    buffer_message (mIDE, renderwin_8bits ? "Using 8-bit dithered internal bitmap (menu setting)\n" :
                                            "Using 8-bit dithered internal bitmap (4 or 8-bit video mode)\n") ;
  }

  err = POVMS_OpenContext ((void **) &POVMS_GUI_Context) ;
  if (err == kNoErr)
    POVMS_GetContextAddress (POVMS_GUI_Context, (void **) &GUIThreadAddr) ;
  if (err != 0)
  {
    PovMessageBox ("Failed to install message handling receiver", "Message Handling Subsystem") ;
    cleanup_all () ;
    return (1) ;
  }

  POVMS_Output_Context = POVMS_GUI_Context ;
  hBackendThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) backend_thread, &threadParam, 0, &threadId) ;

  for (i = 0 ; i < 200 ; i++)
  {
    if (RenderThreadAddr != POVMSInvalidAddress)
      break ;
    Sleep (25) ;
  }
  if (RenderThreadAddr == POVMSInvalidAddress)
  {
    PovMessageBox ("Backend thread failed to complete init", "Message Handling Subsystem") ;
    cleanup_all () ;
    return (1) ;
  }
  SetThreadPriority (hBackendThread, THREAD_PRIORITY_BELOW_NORMAL) ;

  buffer_message (mIDE, "\n") ;
  strcpy (tool_commands [0], "notepad.exe \"%ipovray.ini\"") ;
  WIN_PrintOtherCredits () ;
  buffer_message (mIDE, "Based on the original Windows port by Christopher J. Cason.\n") ;
  buffer_message (mDivider, "\n") ;

#ifdef BUILD_OPTIONS
  if (debugging)
  {
    message_printf ("Built with " COMPILER_NAME " version %d using options '" BUILD_OPTIONS "'\n", COMPILER_VERSION) ;
    buffer_message (mDivider, "\n") ;
  }
#endif

  load_tool_menu (ToolIniFileName) ;
  if (GetPrivateProfileInt ("FileQueue", "ReloadOnStartup", 0, EngineIniFileName))
  {
    queued_file_count = GetPrivateProfileInt ("FileQueue", "QueueCount", 0, EngineIniFileName) ;
    if (queued_file_count > MAX_QUEUE)
      queued_file_count = MAX_QUEUE ;
    for (i = 0 ; i < queued_file_count ; i++)
    {
      sprintf (str, "QueuedFile%d", i) ;
      GetPrivateProfileString ("FileQueue", str, "", queued_files [i], sizeof (queued_files [0]), EngineIniFileName) ;
    }
    if (queued_file_count != 0)
      message_printf ("Loaded %d entr%s into file queue\n", queued_file_count, queued_file_count == 1 ? "y" : "ies") ;
    update_queue_status (false) ;
  }
  buffer_message (mDivider, "\n") ;

  if (GetPrivateProfileInt ("General", "CheckColorsInc", 1, EngineIniFileName) == 1)
  {
    sprintf (str, "%sinclude\\colors.inc", DocumentsPath) ;
    if (stat (str, &statbuf) != 0)
    {
      if (MessageBox (NULL,
                      "WARNING : Cannot find COLORS.INC in expected location.\n\n"
                      "This file is important for the normal operation of POV-Ray. It is included "
                      "with the POV-Ray for Windows distribution. If you did not install using the "
                      "correct installation procedure please attend to this before running POV-Ray "
                      "for Windows.\n\nIf, however, you have chosen to change the location of this file "
                      "or do not need it, you may ignore this warning as long as you have updated "
                      "POVRAY.INI to the new path, or do not use any standard scenes that require it.\n\n"
                      "Do you want to see this warning again ?",
                      "Warning - COLORS.INC is missing",
                      MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
                        PutPrivateProfileInt ("General", "CheckColorsInc", 0, EngineIniFileName) ;
    }
  }

  if (demo_mode)
  {
    message_printf ("Running demonstration\n") ;
    argc = 0 ;
    handle_main_command (CM_DEMO, 0) ;
  }

  SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;

  if (debugging)
  {
    message_printf ("My window handle is %08lx\n", main_window) ;
    if (HaveWin95OrLater ())
      message_printf ("Win95 or later detected\n") ;
    if (HaveWin98OrLater ())
      message_printf ("Win98 or later detected\n") ;
    if (HaveNT4OrLater ())
      message_printf ("WinNT or later detected\n") ;
    if (HaveWin2kOrLater ())
      message_printf ("Win2k or later detected\n") ;
    if (HaveWinXPOrLater ())
      message_printf ("WinXP or later detected\n") ;
    if (IsW95UserInterface)
      message_printf ("Windows 95 user interface flag is set\n") ;
  }

  for (i = 0 ; i < EditFileCount ; i++)
  {
    if (EditGetFlags () & EDIT_CAN_OPEN)
      EditOpenFile (EditFiles [i]) ;
    free (EditFiles [i]) ;
    EditFiles [i] = NULL ;
  }
  EditFileCount = 0 ;

  if (run_count > 1 || !demo_mode)
  {
    n = GetPrivateProfileInt ("General", "ItsAboutTime", 0, EngineIniFileName) ;
    if (time (NULL) > n /*|| newVersion*/)
    {
      ShowAboutBox () ;
      PutPrivateProfileInt ("General",
                            "ItsAboutTime",
                            n ? time (NULL) + 14L * 86400L : time (NULL) + 86400L,
                            EngineIniFileName) ;
    }
  }

  if (tips_enabled && argc <= 1 && show_state != SW_SHOWMINNOACTIVE && !demo_mode)
  {
    n = GetPrivateProfileInt ("TipOfTheDay", "LastTipTime", 0, EngineIniFileName) ;
    if (time (NULL) >= n + 86400L)
    {
      PutPrivateProfileInt ("TipOfTheDay", "LastTipTime", time (NULL), EngineIniFileName) ;
      DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_TIP), main_window, (DLGPROC) PovTipDialogProc, (LPARAM) main_window) ;
    }
  }

  // automatically call the rendering engine if there were any parameters on the command line
  if (!rendering && (argc > 1 || render_requested))
  {
    if (render_requested)
    {
      wrapped_printf ("Requested render file is '%s'", requested_render_file) ;
      strcpy (source_file_name, requested_render_file) ;
    }
    if (argc > 1)
      wrapped_printf ("Calling rendering engine with parameters '%s'", command_line) ;
    start_rendering (!render_requested) ;
  }

  if (debugging)
    debug ("Entering GetMessage () loop\n") ;

  if (debugging)
  {
    double hu = 1.0 / (PerformanceFrequency / PerformanceScale) ;
    message_printf ("Units of histogram timer are %f microseconds\n", hu * 1000000) ;
  }

  if (homeInferred)
    message_printf ("Warning: running with inferred home path due to missing registry entry.\n\n") ;

#ifdef _DEBUG
  if (sizeof (ExternalVarStruct) != 0x9350)
    PovMessageBox ("Compatibility problem - ExternalVarStruct has changed size", "Warning") ;
#endif

  while (!exit_loop)
  {
    // since the render thread can really slow things down for the UI (this becomes
    // a problem if the user sets the render priority to high and then finds the
    // UI unresponsive when attempting to change it back), we only sleep here if
    // there's no events in the queue that we feel the need to handle immediately.
    // (if one of these messages turns up while we're sleeping we'll get woken).
    // N.B. longer wait times resulted in complaints from some testers, apparently
    // there's some machines out there that don't exit the wait for some types of
    // input.
    MsgWaitForMultipleObjects (0, NULL, FALSE, 10, QS_ALLINPUT) ;
//  MsgWaitForMultipleObjects (0, NULL, FALSE, 10, QS_HOTKEY | QS_KEY | QS_MOUSEBUTTON) ;

    if (!backend_thread_exception)
      while (POVMS_ProcessMessages (POVMS_GUI_Context, false) == kFalseErr) { /* do nothing */ } ;
    if (StartInsertRender)
    {
      if (!backend_thread_exception)
        if (!rendering)
          start_rendering (false) ;
      StartInsertRender = false ;
    }

    if (quit != 0)
      if (quit + 15 < time (NULL))
        break ;

    while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
      {
        exit_loop = true ;
        break ;
      }

//    if (debugging)
//      debug_output ("MainLoop: %08x %08x %08x %08x\n", msg.hwnd, msg.message, msg.wParam, msg.lParam) ;

      // we have to disable all these calls because HTML Help has a bug (on some
      // platforms - I can't work out the exact conditions that triggers it) which
      // will cause the cursor to flash between normal and the busy state (normally
      // an hourglass) several times a second, continually (even if help isn't open).
      // See job #124.
  #ifdef HTMLHELP_FIXED
      if (HtmlHelp (NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD) &msg))
        continue ;
  #else
      // we need to pass on these messages, otherwise help navigation messages
      // will go to us instead of the help window
      if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
        if (!IsChild (main_window, msg.hwnd))
          if (HtmlHelp (NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD_PTR) &msg))
            continue ;
  #endif
      if (!TranslateAccelerator (main_window, hAccelerators, &msg))
      {
        TranslateMessage (&msg) ;
        if (msg.hwnd == toolbar_cmdline)
        {
          if (msg.message == WM_CHAR)
            if (handle_toolbar_cmdline (msg.wParam, msg.lParam))
              continue ;
        }
        DispatchMessage (&msg) ;
      }
    }
  }

  debug_output ("Dropping out of message loop\n") ;
  cleanup_all () ;
  debug_output ("Calling fcloseall ()\n") ;
  fcloseall () ;
#ifndef _WIN64
  debug_output ("Calling HtmlHelp ()\n") ;
  // win64 - get exception during HH_UNINITIALIZE
  HtmlHelp (NULL, NULL, HH_UNINITIALIZE, (DWORD) help_cookie) ;
#endif
  debug_output ("exiting WinMain()\n") ;
  return (0) ;
}

