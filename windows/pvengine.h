/****************************************************************************
 *                pvengine.h
 *
 * This file contains PVENGINE specific defines.
 *
 * Author: Christopher J Cason.
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
 * $File: //depot/povray/3.5/windows/pvengine.h $
 * $Revision: #37 $
 * $Change: 4793 $
 * $DateTime: 2009/05/26 07:13:51 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef PVENGINE_H_INCLUDED
#define PVENGINE_H_INCLUDED

#include <math.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <assert.h>
#include <sys\stat.h>
#include <excpt.h>
#include "frame.h"
#include "optout.h"
#include "povms.h"
#include "povmsgid.h"

#define PVENGINE_VER              "unofficial-" POVRAY_PLATFORM_NAME

#include <time.h>

#define __USECTL3D__
#define OPTIMISATION              "Pentium 4"
#define MAX_MESSAGE               1024
#define MAX_ARGV                  256
#define STOP_BEING_NICE           4
#define INIFILENAME               "pvengine.ini"
#define RERUNFILENAME             "pvrerun.ini"
#define TOOLFILENAME              "pvtools.ini"
#define MAX_TOOLCMD               32
#define MAX_TOOLCMDTEXT           128
#define MAX_TOOLHELPTEXT          128
#define MIN_EDITOR_VERSION        100
#define MAX_EDITOR_VERSION        199
#define MAX_WINDOWS               16
#define MAX_EDIT_FILES            32
#define POV_INTERNAL_STREAM       ((FILE *) 1L)

// ----------------------------------------------------------------------
// message definitions used to be here but have been moved to pvedit.h.
// this is to allow the C++Builder editor code to see them without having
// to include this file (and thus a bunch of others that this one needs).
// ----------------------------------------------------------------------

// if this is more than 16 then add_rerun_to_menu () needs to be modified.
#define MAX_RERUN       16

#define NUM_BUTTONS     16
#define HDIB            HANDLE
#define SEPARATOR       '\\'

// OPTIMISATION is correctly spelt, unless you're an American (who prefer a 'z').
#ifndef OPTIMISATION
#define OPTIMISATION    "Pentium"
#endif

#define DRAWFASTRECT(hdc,lprc) ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL)

#define RGBBLACK     RGB(0,0,0)
#define RGBRED       RGB(128,0,0)
#define RGBGREEN     RGB(0,128,0)
#define RGBBLUE      RGB(0,0,128)

#define RGBBROWN     RGB(128,128,0)
#define RGBMAGENTA   RGB(128,0,128)
#define RGBCYAN      RGB(0,128,128)
#define RGBLTGRAY    RGB(192,192,192)

#define RGBGRAY      RGB(128,128,128)
#define RGBLTRED     RGB(255,0,0)
#define RGBLTGREEN   RGB(0,255,0)
#define RGBLTBLUE    RGB(0,0,255)

#define RGBYELLOW    RGB(255,255,0)
#define RGBLTMAGENTA RGB(255,0,255)
#define RGBLTCYAN    RGB(0,255,255)
#define RGBWHITE     RGB(255,255,255)

typedef unsigned char uchar ;

typedef enum
{
  mUnknown = 0,
  mAll = 1,
  All = 1,
  mIDE,
  mBanner,
  mWarning,
  mRender,
  mStatus,
  mDebug,
  mFatal,
  mStatistics,
  mDivider,
  mHorzLine,
} msgtype ;

typedef enum
{
  None,
  CR,
  LF
} lftype ;

typedef enum
{
  filePOV,
  fileINC,
  fileINI,
  fileTGA,
  filePPM,
  filePGM,
  filePBM,
  filePNG,
  fileGIF,
  fileBMP,
  fileUnknown
} filetypes ;

// Bitmap header info with palette included

typedef struct
{
  BITMAPINFOHEADER      header ;
  RGBQUAD               colors [256] ;
} BitmapInfo ;

// Windows LOGPALETTE palette structure

typedef struct
{
  WORD                  version ;
  WORD                  entries ;
  PALETTEENTRY          pe [256] ;
} LogPal ;

typedef struct
{
  bool        ncEnabled ;
  bool        menuWasUp ;
  bool        hasCaption ;
  bool        hasBorder ;
  bool        hasSizeableBorder ;
  bool        hasStatusBar ;
  bool        hasMenuBar ;
  bool        sysMenuOverride ;
  bool        isMaxiMinimized ;
  HWND        hWnd ;
  HFONT       hMenuBarFont ;
  HFONT       hStatusBarFont ;
  HFONT       hSystemFont ;
  ushort      captionTotal ;
  ushort      captionInternal ;
  ushort      captionBorderLeft ;
  ushort      captionBorderRight ;
  ushort      captionBorderTop ;
  ushort      captionBorderBottom ;
  ushort      borderWidth ;
  ushort      borderHeight ;
  ushort      buttonWidth ;
  ushort      buttonHeight ;
  ushort      sizing ;
  ushort      statusBarTotal ;
  ushort      statusBarBorder ;
  ushort      menuBarTotal ;
  ushort      menuBarBorder ;
} pvncStruct ;

void debug (char *format, ...) ;
void PovMessageBox (char *message, char *title) ;
int initialise_message_display (void) ;
void erase_display_window (HDC hdc, int xoffset, int yoffset) ;
void paint_display_window (HDC hdc) ;
void buffer_message (msgtype message_type, const char *s) ;
void buffer_stream_message (msgtype message_type, const char *s) ;
void clear_messages (void) ;
int update_message_display (lftype lf) ;
void debug_output (char *format, ...) ;
void message_printf (char *format, ...) ;
void wrapped_printf (char *format, ...) ;
void dump_pane_to_clipboard (void) ;
bool PutPrivateProfileInt (LPCSTR lpszSection, LPCSTR lpszEntry, UINT uiValue, LPCSTR lpszFilename) ;
int  WIN_Display_Init (int x, int y) ;
void WIN_Display_Finished (void) ;
void WIN_Display_Close (void) ;
void WIN_Display_Plot (int x, int y, int Red, int Green, int Blue, int Alpha) ;
void WIN_Display_Plot_Rect (int x1, int x2, int y1, int y2, int Red, int Green, int Blue, int Alpha) ;
void WIN_Banner(const char *s) ;
void WIN_Warning(const char *s) ;
void WIN_Render_Info(const char *s) ;
void WIN_Status_Info(const char *s) ;
void WIN_Debug_Info(const char *s) ;
void WIN_Fatal(const char *s) ;
void WIN_Statistics(const char *s) ;
void WIN_Startup(void) ;
void WIN_Finish(int n) ;
void WIN_Cooperate(int n) ;
int WIN_Povray (int argc, char **argv) ;
int WIN_System (char *s) ;
void get_logfont (HDC hdc, LOGFONT *lf) ;
int create_message_font (HDC hdc, LOGFONT *lf) ;
void status_printf (int nSection, char *format, ...) ;
void SetupExplorerDialog (HWND win) ;
void validatePath (char *s) ;
int joinPath (char *out, char *path, char *name) ;
void UpdateTabbedWindow (int current, bool force) ;
void CalculateClientWindows (bool redraw) ;
bool start_rendering (bool ignore_source_file) ;
bool HaveWin95 (void) ;
HPALETTE create_palette (RGBQUAD *rgb, int count) ;
void render_stopped (void) ;
void FeatureNotify (char *labelStr, char *titleStr, char *textStr, char *helpStr, bool checked) ;
void cancel_render (void) ;

// file PVMISC.C

int get_file_type (char *filename) ;
void rotate_rerun_entries (void) ;
void read_INI_settings (char *iniFilename) ;
void write_INI_settings (char *iniFilename) ;
void add_rerun_to_menu (void) ;
void update_menu_for_render (bool rendering) ;
void update_queue_status (bool write_files) ;
void fill_rerun_listbox (HWND hlb, char *idList) ;
void draw_rerun_listbox (DRAWITEMSTRUCT *d) ;
void draw_ordinary_listbox (DRAWITEMSTRUCT *d, bool fitpath) ;
void fill_statistics_listbox (HWND hlb, int id) ;
void resize_listbox_dialog (HWND hDlg, int idLb, int chars) ;
void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, bool bRepaint, bool checkBorders = false) ;
void FitWindowInWindow (HWND hRelativeTo, HWND hTarget) ;
int splitfn (char *filename, char *path, char *name, char *ext) ;
void splitpath (char *filename, char *path, char *name) ;
bool process_toggles (WPARAM wParam) ;
void set_toggles (void) ;
void load_tool_menu (char *iniFilename) ;
char *parse_tool_command (char *command) ;
char *get_elapsed_time (int seconds) ;
void initialise_statusbar (bool isMaxiMiniMode) ;
void calculate_statusbar (void) ;
void paint_statusbar (int nSection) ;
void extract_ini_sections (char *filename, HWND hwnd) ;
void extract_ini_sections_ex (char *filename, HWND hwnd) ;
int select_combo_item_ex (HWND hwnd, char *s) ;
void paint_rendering_signal (int which_one) ;
char *get_full_name (char *s) ;
bool PovInvalidateRect (HWND hWnd, CONST RECT *lpRect, bool bErase) ;
int load_editors (char *iniFilename) ;
bool TaskBarAddIcon (HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip) ;
bool TaskBarModifyIcon (HWND hwnd, UINT uID, LPSTR lpszTip) ;
bool TaskBarDeleteIcon (HWND hwnd, UINT uID) ;
bool TestAccessAllowed (const char *Filename, unsigned int FileType, bool IsWrite) ;
char *clean (char *s) ;
bool fileExists (char *filename) ;
bool dirExists (char *filename) ;
bool GetDontShowAgain (char *Name, char *lpszFilename) ;
void PutDontShowAgain (char *Name, bool dontShow, char *lpszFilename) ;

// file PVFILES.C

INT_PTR CALLBACK PovLegalDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
char *save_demo_file (char *s1, char *s2) ;
void save_povlegal (void) ;

// file PVMENU.C

bool PVEnableMenuItem (UINT idItem, UINT state) ;
bool PVCheckMenuItem (UINT idItem, UINT state) ;
bool PVCheckMenuRadioItem (UINT idFirst, UINT idLast, UINT idItem) ;
bool PVModifyMenu (UINT idItem, UINT flags, UINT idNewItem, LPCSTR lpNewItem) ;
bool PVDeleteMenuItem (UINT idItem) ;
void init_menus (void) ;
void setup_menus (bool have_editor) ;
void clear_menu (HMENU hMenu) ;
void build_main_menu (HMENU hMenu, bool have_editor) ;
void build_editor_menu (HMENU hMenu) ;
void set_newuser_menus (bool hide) ;

// file PVTEXT.C

void write_wrapped_text (HDC hdc, RECT *rect, char *text) ;
void tip_of_the_day (HDC hdc, RECT *rect, char *text) ;
void paint_statusbar (int nSection) ;
void say_status_message (int section, char *message) ;
void handle_menu_select (WPARAM wParam, LPARAM lParam) ;
char *clean_str (const char *s) ;
HWND create_toolbar (HWND hwndParent) ;
HWND create_tabbed_window (HWND hwndParent) ;
void initialise_tabbed_window (HWND hwnd) ;
unsigned add_window_to_tab (HWND hwnd, void *editor, char *s) ;
void resize_windows (unsigned left, unsigned top, unsigned width, unsigned height) ;
unsigned get_tab_index (HWND hwnd, void *editor) ;
char *preparse_commandline (char *s) ;
char *preparse_instance_commandline (char *s) ;
int need_hscroll (void) ;
HWND create_rebar (HWND hwndParent) ;
HWND CreateStatusbar (HWND hwndParent) ;
void ResizeStatusBar (HWND hwnd) ;

// file PVBITMAP.C

HDIB      FAR  BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
HDIB      FAR  ChangeBitmapFormat (HBITMAP  hBitmap,
                                   WORD     wBitCount,
                                   DWORD    dwCompression,
                                   HPALETTE hPal);
HDIB      FAR  ChangeDIBFormat (HDIB hDIB, WORD wBitCount,
                                DWORD dwCompression);
HBITMAP   FAR  CopyScreenToBitmap (LPRECT);
HDIB      FAR  CopyScreenToDIB (LPRECT);
HBITMAP   FAR  CopyWindowToBitmap (HWND, WORD);
HDIB      FAR  CopyWindowToDIB (HWND, WORD);
HPALETTE  FAR  CreateDIBPalette (HDIB hDIB);
HDIB      FAR  CreateDIB(DWORD, DWORD, WORD);
WORD      FAR  DestroyDIB (HDIB);
void      FAR  DIBError (int ErrNo);
DWORD     FAR  DIBHeight (LPSTR lpDIB);
WORD      FAR  DIBNumColors (LPSTR lpDIB);
HBITMAP   FAR  DIBToBitmap (HDIB hDIB, HPALETTE hPal);
DWORD     FAR  DIBWidth (LPSTR lpDIB);
LPSTR     FAR  FindDIBBits (LPSTR lpDIB);
HPALETTE  FAR  GetSystemPalette (void);
HDIB      FAR  LoadDIB (LPSTR);
bool      FAR  PaintBitmap (HDC, LPRECT, HBITMAP, LPRECT, HPALETTE);
bool      FAR  PaintDIB (HDC, LPRECT, HDIB, LPRECT, HPALETTE);
int       FAR  PalEntriesOnDevice (HDC hDC);
WORD      FAR  PaletteSize (LPSTR lpDIB);
WORD      FAR  PrintDIB (HDIB, WORD, WORD, WORD, LPSTR);
WORD      FAR  PrintScreen (LPRECT, WORD, WORD, WORD, LPSTR);
WORD      FAR  PrintWindow (HWND, WORD, WORD, WORD, WORD, LPSTR);
WORD      FAR  SaveDIB (HDIB, LPSTR);
HANDLE         AllocRoomForDIB(BITMAPINFOHEADER bi, HBITMAP hBitmap);
HBITMAP        lpDIBToBitmap(void *lpDIBHdr, HPALETTE hPal);
HBITMAP        lpDIBToBitmapAndPalette(void *lpDIBHdr);

#ifdef DECLARE_TABLES

// Default windows compatible halftone palette. This includes the default
// Windows system colors in the first 10 and last 10 entries in the
// palette.

RGBQUAD halftonePal [256] =
{
  {0x00,0x00,0x00,0}, {0xA8,0x00,0x00,0}, {0x00,0xA8,0x00,0}, {0xA8,0xA8,0x00,0},
  {0x00,0x00,0xA8,0}, {0xA8,0x00,0xA8,0}, {0x00,0x54,0xA8,0}, {0xA8,0xA8,0xA8,0},
  {0x54,0x54,0x54,0}, {0xFC,0x54,0x54,0}, {0x54,0xFC,0x54,0}, {0xFC,0xFC,0x54,0},
  {0x54,0x54,0xFC,0}, {0xFC,0x54,0xFC,0}, {0x54,0xFC,0xFC,0}, {0xFC,0xFC,0xFC,0},
  {0x00,0x00,0x00,0}, {0x14,0x14,0x14,0}, {0x20,0x20,0x20,0}, {0x2C,0x2C,0x2C,0},
  {0x00,0x00,0x00,0}, {0x00,0x00,0x33,0}, {0x00,0x00,0x66,0}, {0x00,0x00,0x99,0},
  {0x00,0x00,0xCC,0}, {0x00,0x00,0xFF,0}, {0x00,0x33,0x00,0}, {0x00,0x33,0x33,0},
  {0x00,0x33,0x66,0}, {0x00,0x33,0x99,0}, {0x00,0x33,0xCC,0}, {0x00,0x33,0xFF,0},
  {0x00,0x66,0x00,0}, {0x00,0x66,0x33,0}, {0x00,0x66,0x66,0}, {0x00,0x66,0x99,0},
  {0x00,0x66,0xCC,0}, {0x00,0x66,0xFF,0}, {0x00,0x99,0x00,0}, {0x00,0x99,0x33,0},
  {0x00,0x99,0x66,0}, {0x00,0x99,0x99,0}, {0x00,0x99,0xCC,0}, {0x00,0x99,0xFF,0},
  {0x00,0xCC,0x00,0}, {0x00,0xCC,0x33,0}, {0x00,0xCC,0x66,0}, {0x00,0xCC,0x99,0},
  {0x00,0xCC,0xCC,0}, {0x00,0xCC,0xFF,0}, {0x00,0xFF,0x00,0}, {0x00,0xFF,0x00,0},
  {0x00,0xFF,0x66,0}, {0x00,0xFF,0x99,0}, {0x00,0xFF,0xCC,0}, {0x00,0xFF,0xFF,0},
  {0x33,0x00,0x00,0}, {0x33,0x00,0x33,0}, {0x33,0x00,0x66,0}, {0x33,0x00,0x99,0},
  {0x33,0x00,0xCC,0}, {0x33,0x00,0xFF,0}, {0x33,0x33,0x00,0}, {0x33,0x33,0x33,0},
  {0x33,0x33,0x66,0}, {0x33,0x33,0x99,0}, {0x33,0x33,0xCC,0}, {0x33,0x33,0xFF,0},
  {0x33,0x66,0x00,0}, {0x33,0x66,0x33,0}, {0x33,0x66,0x66,0}, {0x33,0x66,0x99,0},
  {0x33,0x66,0xCC,0}, {0x33,0x66,0xFF,0}, {0x33,0x99,0x00,0}, {0x33,0x99,0x33,0},
  {0x33,0x99,0x66,0}, {0x33,0x99,0x99,0}, {0x33,0x99,0xCC,0}, {0x33,0x99,0xFF,0},
  {0x33,0xCC,0x00,0}, {0x33,0xCC,0x33,0}, {0x33,0xCC,0x66,0}, {0x33,0xCC,0x99,0},
  {0x33,0xCC,0xCC,0}, {0x33,0xCC,0xFF,0}, {0x00,0xFF,0x00,0}, {0x33,0xFF,0x33,0},
  {0x33,0xFF,0x66,0}, {0x33,0xFF,0x99,0}, {0x33,0xFF,0xCC,0}, {0x33,0xFF,0xFF,0},
  {0x66,0x00,0x00,0}, {0x66,0x00,0x33,0}, {0x66,0x00,0x66,0}, {0x66,0x00,0x99,0},
  {0x66,0x00,0xCC,0}, {0x66,0x00,0xFF,0}, {0x66,0x33,0x00,0}, {0x66,0x33,0x33,0},
  {0x66,0x33,0x66,0}, {0x66,0x33,0x99,0}, {0x66,0x33,0xCC,0}, {0x66,0x33,0xFF,0},
  {0x66,0x66,0x00,0}, {0x66,0x66,0x33,0}, {0x66,0x66,0x66,0}, {0x66,0x66,0x99,0},
  {0x66,0x66,0xCC,0}, {0x66,0x66,0xFF,0}, {0x66,0x99,0x00,0}, {0x66,0x99,0x33,0},
  {0x66,0x99,0x66,0}, {0x66,0x99,0x99,0}, {0x66,0x99,0xCC,0}, {0x66,0x99,0xFF,0},
  {0x66,0xCC,0x00,0}, {0x66,0xCC,0x33,0}, {0x66,0xCC,0x66,0}, {0x66,0xCC,0x99,0},
  {0x66,0xCC,0xCC,0}, {0x66,0xCC,0xFF,0}, {0x66,0xFF,0x00,0}, {0x66,0xFF,0x33,0},
  {0x66,0xFF,0x66,0}, {0x66,0xFF,0x99,0}, {0x66,0xFF,0xCC,0}, {0x66,0xFF,0xFF,0},
  {0x99,0x00,0x00,0}, {0x99,0x00,0x33,0}, {0x99,0x00,0x66,0}, {0x99,0x00,0x99,0},
  {0x99,0x00,0xCC,0}, {0x99,0x00,0xFF,0}, {0x99,0x33,0x00,0}, {0x99,0x33,0x33,0},
  {0x99,0x33,0x66,0}, {0x99,0x33,0x99,0}, {0x99,0x33,0xCC,0}, {0x99,0x33,0xFF,0},
  {0x99,0x66,0x00,0}, {0x99,0x66,0x33,0}, {0x99,0x66,0x66,0}, {0x99,0x66,0x99,0},
  {0x99,0x66,0xCC,0}, {0x99,0x66,0xFF,0}, {0x99,0x99,0x00,0}, {0x99,0x99,0x33,0},
  {0x99,0x99,0x66,0}, {0x99,0x99,0x99,0}, {0x99,0x99,0xCC,0}, {0x99,0x99,0xFF,0},
  {0x99,0xCC,0x00,0}, {0x99,0xCC,0x33,0}, {0x99,0xCC,0x66,0}, {0x99,0xCC,0x99,0},
  {0x99,0xCC,0xCC,0}, {0x99,0xCC,0xFF,0}, {0x99,0xFF,0x00,0}, {0x99,0xFF,0x33,0},
  {0x99,0xFF,0x66,0}, {0x99,0xFF,0x99,0}, {0x99,0xFF,0xCC,0}, {0x99,0xFF,0xFF,0},
  {0xCC,0x00,0x00,0}, {0xCC,0x00,0x33,0}, {0xCC,0x00,0x66,0}, {0xCC,0x00,0x99,0},
  {0xCC,0x00,0xCC,0}, {0xCC,0x00,0xFF,0}, {0xCC,0x33,0x00,0}, {0xCC,0x33,0x33,0},
  {0xCC,0x33,0x66,0}, {0xCC,0x33,0x99,0}, {0xCC,0x33,0xCC,0}, {0xCC,0x33,0xFF,0},
  {0xCC,0x66,0x00,0}, {0xCC,0x66,0x33,0}, {0xCC,0x66,0x66,0}, {0xCC,0x66,0x99,0},
  {0xCC,0x66,0xCC,0}, {0xCC,0x66,0xFF,0}, {0xCC,0x99,0x00,0}, {0xCC,0x99,0x33,0},
  {0xCC,0x99,0x66,0}, {0xCC,0x99,0x99,0}, {0xCC,0x99,0xCC,0}, {0xCC,0x99,0xFF,0},
  {0xCC,0xCC,0x00,0}, {0xCC,0xCC,0x33,0}, {0xCC,0xCC,0x66,0}, {0xCC,0xCC,0x99,0},
  {0xCC,0xCC,0xCC,0}, {0xCC,0xCC,0xFF,0}, {0xCC,0xFF,0x00,0}, {0xCC,0xFF,0x33,0},
  {0xCC,0xFF,0x66,0}, {0xCC,0xFF,0x99,0}, {0xCC,0xFF,0xCC,0}, {0xCC,0xFF,0xFF,0},
  {0xFF,0x00,0x00,0}, {0xFF,0x00,0x00,0}, {0xFF,0x00,0x66,0}, {0xFF,0x00,0x99,0},
  {0xFF,0x00,0xCC,0}, {0xFF,0x00,0xFF,0}, {0xFF,0x00,0x00,0}, {0xFF,0x33,0x33,0},
  {0xFF,0x33,0x66,0}, {0xFF,0x33,0x99,0}, {0xFF,0x33,0xCC,0}, {0xFF,0x33,0xFF,0},
  {0xFF,0x66,0x00,0}, {0xFF,0x66,0x33,0}, {0xFF,0x66,0x66,0}, {0xFF,0x66,0x99,0},
  {0xFF,0x66,0xCC,0}, {0xFF,0x66,0xFF,0}, {0xFF,0x99,0x00,0}, {0xFF,0x99,0x33,0},
  {0xFF,0x99,0x66,0}, {0xFF,0x99,0x99,0}, {0xFF,0x99,0xCC,0}, {0xFF,0x99,0xFF,0},
  {0xFF,0xCC,0x00,0}, {0xFF,0xCC,0x33,0}, {0xFF,0xCC,0x66,0}, {0xFF,0xCC,0x99,0},
  {0xFF,0xCC,0xCC,0}, {0xFF,0xCC,0xFF,0}, {0xFF,0xFF,0x00,0}, {0xFF,0xFF,0x33,0},
  {0xFF,0xFF,0x66,0}, {0xFF,0xFF,0x99,0}, {0xFF,0xFF,0xCC,0}, {0xFF,0xFF,0xFF,0},
  {0x2C,0x40,0x40,0}, {0x2C,0x40,0x3C,0}, {0x2C,0x40,0x34,0}, {0x2C,0x40,0x30,0},
  {0x2C,0x40,0x2C,0}, {0x30,0x40,0x2C,0}, {0x34,0x40,0x2C,0}, {0x3C,0x40,0x2C,0},
  {0x40,0x40,0x2C,0}, {0x40,0x3C,0x2C,0}, {0x40,0x34,0x2C,0}, {0x40,0x30,0x2C,0},
  {0x54,0x54,0x54,0}, {0xFC,0x54,0x54,0}, {0x54,0xFC,0x54,0}, {0xFC,0xFC,0x54,0},
  {0x54,0x54,0xFC,0}, {0xFC,0x54,0xFC,0}, {0x54,0xFC,0xFC,0}, {0xFC,0xFC,0xFC,0}
} ;

// Division lookup tables.  These tables compute 0-255 divided by 51 and
// modulo 51.  These tables could approximate gamma correction.

uchar div51 [256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5
} ;

uchar mod51 [256] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
  7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0
} ;

// Multiplication lookup tables. These compute 0-5 times 6 and 36.

uchar mul6 [6] = {0, 6, 12, 18, 24, 30} ;
uchar mul36 [6] = {0, 36, 72, 108, 144, 180} ;

// Ordered 8x8 dither matrix for 8 bit to 2.6 bit halftones.

uchar dither8x8 [64] =
{
   0, 38,  9, 47,  2, 40, 11, 50,
  25, 12, 35, 22, 27, 15, 37, 24,
   6, 44,  3, 41,  8, 47,  5, 43,
  31, 19, 28, 15, 34, 21, 31, 18,
   1, 39, 11, 49,  0, 39, 10, 48,
  27, 14, 36, 23, 26, 13, 35, 23,
   7, 46,  4, 43,  7, 45,  3, 42,
  33, 20, 30, 17, 32, 19, 29, 16,
} ;

#endif // #if DECLARE_TABLES

#endif // PVENGINE_H_INCLUDED
