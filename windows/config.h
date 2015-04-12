/****************************************************************************
 *                   config.h
 *
 * This file contains the Windows specific defines.
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
 * $File: //depot/povray/3.5/windows/config.h $
 * $Revision: #63 $
 * $Change: 4789 $
 * $DateTime: 2009/05/23 09:21:29 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

// stop Window's objidl.h from being pulled in (which dupes IStream)
#define WIN32_LEAN_AND_MEAN

//#define WIN_MEM_TRACKING

// some params are of type COLOUR, which is not yet defined (it's in frame.h).
// this allows us to substitute _COLOUR for it within this file.
typedef float _COLOUR [5] ;

#define FILE_NAME_LENGTH          _MAX_PATH
#define POV_NAME_MAX              _MAX_FNAME

#define IFF_SWITCH_CAST (long)

#ifdef _WIN64
  #define POVRAY_PLATFORM_NAME "win64"
#else
  #define POVRAY_PLATFORM_NAME "win32"
#endif

// NOTE NOTE NOTE - do NOT define PRECISION_TIMER_AVAILABLE unless you know
// what you are doing. and have read the comments in ZTIMER/READ.ME. You
// will need a working ZTimer library if this is switched on. The only thing
// you lose from not having it is the ability to do scene profiling.

#if defined(__MINGW32__)                    /* MinGW GCC */

  #include <stdio.h>    /* fseek and the like */
  #include <algorithm>  /* min and max below */
  using std::min;
  using std::max;

  // Note that this section is supposed to be compiled with MinGW 3.2 with GCC 3.2.3
  // Old names (without underscores) are turned off there.

  #if __MINGW32_MAJOR_VERSION == 3
    #define COMPILER_MAJOR_VER              "3"
  #else
    #error "Adjust config.h to your compiler!"
  #endif

  #if __MINGW32_MINOR_VERSION == 1
    #define COMPILER_MINOR_VER              "1"
  #else
    #error "Adjust config.h to your compiler!"
  #endif

  #if  __GNUC__ == 3
    #define GCC_MAJOR_VER                   "3"
  #else
    #error "Adjust config.h to your compiler!"
  #endif

  #if __GNUC_MINOR__ == 2
    #define GCC_MINOR_VER                   "2"
  #else
    #error "Adjust config.h to your compiler!"
  #endif

  #if __GNUC_PATCHLEVEL__ == 3
    #define GCC_PATCH_VER                   "3"
  #else
    #error "Adjust config.h to your compiler!"
  #endif

  #define COMPILER_VER                      ".mingw-" COMPILER_MAJOR_VER \
                                            "." COMPILER_MINOR_VER \
                                            "(gcc-" GCC_MAJOR_VER \
                                            "." GCC_MINOR_VER \
                                            "." GCC_PATCH_VER ")"

  #include <sys/stat.h>                     /* deliver stat structure/functions */
  #define stat                              _stat
  #define S_ISDIR(m)                        _S_ISDIR(m)
  #define S_ISREG(m)                        _S_ISREG(m)

  #include <setjmp.h>                       /* deliver jmp_buf */

  #include <math.h>                         /* deliver NAN */

  #include <unistd.h>                       /* deliver _unlink definition */
  #define DELETE_FILE(name)                 _unlink(name)
  #define getcwd                            _getcwd

  #include <windef.h>                       /* deliver BLOB from windef.h to dlgs.h */
  #include <dlgs.h>                         /* deliver rad2 macro from dlgs.h */
  #include <ctype.h>                        /* deliver toupper etc. */
  #ifdef rad2                               /* undefine rad2 and leave name for field in blob.h */
    #undef rad2
  #endif
  #define NEED_INVHYP 1                     /* deliver asinh, acosh, atanh */

  #ifndef MAX
    #define MAX(a,b) ((a>b)?a:b)
  #endif
  #ifndef MIN
    #define MIN(a,b) ((a>b)?b:a)
  #endif

  #ifndef fcloseall
    #define fcloseall                       _fcloseall
  #endif

  #define POV_NAMESPACE                     pov
  #define BEGIN_POV_NAMESPACE               namespace POV_NAMESPACE { using namespace std;
  #define END_POV_NAMESPACE                 }
  #define USING_POV_NAMESPACE               using namespace POV_NAMESPACE;

  #define POV_FRONTEND_NAMESPACE            pov_frontend
  #define BEGIN_POV_FRONTEND_NAMESPACE      namespace POV_FRONTEND_NAMESPACE { using namespace std;
  #define END_POV_FRONTEND_NAMESPACE        }
  #define USING_POV_FRONTEND_NAMESPACE      using namespace POV_FRONTEND_NAMESPACE;

  #define POV_BASE_NAMESPACE                pov_base
  #define BEGIN_POV_BASE_NAMESPACE          namespace POV_BASE_NAMESPACE { using namespace std;
  #define END_POV_BASE_NAMESPACE            }
  #define USING_POV_BASE_NAMESPACE          using namespace POV_BASE_NAMESPACE;

  #define POV_TRY                           if(true)
  #define POV_EXCEPT(x)                     if(false)
  #ifndef GetExceptionCode
    #define GetExceptionCode()              0
  #endif

  #define strnicmp                          _strnicmp
  #define stricmp                           _stricmp
  #define strlwr                            _strlwr
  #define strupr                            _strupr
  #define strdup                            _strdup

#elif defined(__WATCOMC__)                  /* Watcom C/C++ C32 */

  #define COMPILER_VER                      ".watcom"

  #include <ctype.h>

  #define POV_NAMESPACE
  #define BEGIN_POV_NAMESPACE
  #define END_POV_NAMESPACE
  #define USING_POV_NAMESPACE

  #define POV_FRONTEND_NAMESPACE
  #define BEGIN_POV_FRONTEND_NAMESPACE
  #define END_POV_FRONTEND_NAMESPACE
  #define USING_POV_FRONTEND_NAMESPACE

  #define POV_BASE_NAMESPACE
  #define BEGIN_POV_BASE_NAMESPACE
  #define END_POV_BASE_NAMESPACE
  #define USING_POV_BASE_NAMESPACE

  // include for unlink
  #include <io.h>
  #include <direct.h>

  #define TB_SETIMAGELIST                   (WM_USER + 48)

  #define POV_TRY                           try
  #define POV_EXCEPT(x)                     catch(...)

#elif defined(__BORLANDC__)                 /* Borland C/C++ */

  #include <setjmp.h>
  #include <dir.h>
  #include <stdio.h>

  #if __BORLANDC__ >= 0x551 && __BORLANDC__ < 0x552
    #define COMPILER_VER_STR              "5.51"
  #else
    #error "Adjust config.h to your compiler!"
  #endif

  #define COMPILER_VER                    ".borland-" COMPILER_VER_STR

  #define IFF_SWITCH_CAST                   (long)

  #include <search.h>
  #define QSORT_FUNCT_PARM                  const void *
  #define QSORT_FUNCT_RET                   int CDECL
  #define QSORT(a,b,c,d)                    qsort((void *) a, (size_t) b, (size_t) c, (int (_USERENTRY *) (const void *, const void *)) d)

  #define CLASSLIB_DEFS_H
  #define MSDOS
  #define CLASSLIB_DEFS_H

  using namespace std;

  #define POV_LONG __int64
  #define POVMSLong __int64
  #define SetPOVMSLong(v,h,l) *v = (((((POVMSLong)(h)) & 0x00000000ffffffff) << 32) | (((POVMSLong)(l)) & 0x00000000ffffffff))
  #define GetPOVMSLong(h,l,v) *h = ((v) >> 32) & 0x00000000ffffffff; *l = (v) & 0x00000000ffffffff
  #define POVMSLongToCDouble(x) double(x)

  // turn on asinh,acosh,atanh
  #define NEED_INVHYP 1

  #define POV_NAMESPACE
  #define BEGIN_POV_NAMESPACE
  #define END_POV_NAMESPACE
  #define USING_POV_NAMESPACE

  #define POV_FRONTEND_NAMESPACE
  #define BEGIN_POV_FRONTEND_NAMESPACE
  #define END_POV_FRONTEND_NAMESPACE
  #define USING_POV_FRONTEND_NAMESPACE

  #define POV_TRY                           __try
  #define POV_EXCEPT(x)                     __except(x)

#elif defined(_MSC_VER)

  #include <setjmp.h>
  #include "ztimer.h"
  #include "direct.h"

  #pragma warning(disable : 4018)	/* signed/unsigned mismatch */
  #pragma warning(disable : 4305)	/* truncation from 'type1' to 'type2' (mostly double to float) */
  #pragma warning(disable : 4244)	/* possible loss of data (converting ints to shorts) */

  #ifdef __INTEL_COMPILER
    #if __INTEL_COMPILER >= 500 && __INTEL_COMPILER < 600
      #define COMPILER_VER                      ".icl5"
    #elif __INTEL_COMPILER >= 600 && __INTEL_COMPILER < 700
      #define COMPILER_VER                      ".icl6"
    #elif __INTEL_COMPILER >= 700 && __INTEL_COMPILER < 800
      #define COMPILER_VER                      ".icl7"
    #elif __INTEL_COMPILER >= 800 && __INTEL_COMPILER < 900
      #define COMPILER_VER                      ".icl8"
    #elif __INTEL_COMPILER >= 900 && __INTEL_COMPILER < 1000
      #define COMPILER_VER                      ".icl9"
    #elif __INTEL_COMPILER >= 1000 && __INTEL_COMPILER < 1100
      #define COMPILER_VER                      ".icl10"
    #elif __INTEL_COMPILER >= 1100 && __INTEL_COMPILER < 1200
      #define COMPILER_VER                      ".icl11"
    #else
      #define COMPILER_VER                      ".icl"
    #endif
    #define COMPILER_NAME                       "Intel C++ Compiler"
    #define COMPILER_VERSION                    __INTEL_COMPILER
    #pragma warning(disable : 1899) // 'multi-character literal'
  #else
    #if _MSC_VER >= 1100 && _MSC_VER < 1200
      #define COMPILER_VER                      ".msvc5"
    #elif _MSC_VER >= 1200 && _MSC_VER < 1300
      #define COMPILER_VER                      ".msvc6"
    #elif _MSC_VER >= 1300 && _MSC_VER < 1400
      #define COMPILER_VER                      ".msvc7"
    #elif _MSC_VER >= 1400 && _MSC_VER < 1500
      #define COMPILER_VER                      ".msvc8"
    #elif _MSC_VER >= 1500 && _MSC_VER < 1600
      #if SSE2 == 1
        #define COMPILER_VER                    ".msvc9-sse2"
      #else
        #define COMPILER_VER                    ".msvc9"
      #endif
    #else
      #define COMPILER_VER                      ".msvc"
    #endif
    #define COMPILER_NAME                       "Microsoft Visual C++"
    #define COMPILER_VERSION                    _MSC_VER

    #if defined (_WIN64)
      #define ALIGN16                           __declspec(align(16))
    #endif
  #endif

  #define IFF_SWITCH_CAST                     (long)
  #define QSORT_FUNCT_PARM                    const void *
  #define QSORT_FUNCT_RET                     int CDECL
  #define PRECISION_TIMER_AVAILABLE           1
  #define PRECISION_TIMER_INIT
  #define PRECISION_TIMER_START               LZTimerOn() ;
  #define PRECISION_TIMER_STOP                LZTimerOff() ;
  #define PRECISION_TIMER_COUNT               LZTimerCount() ;
  #define MSDOS
  #define QSORT(a,b,c,d)                      qsort((void *) a, (size_t) b, (size_t) c, (int (__cdecl*) (const void *, const void *)) d)
  #define getcwd _getcwd
  #define POV_LONG                            __int64
  #define POVMSLong                           __int64
  #define POV_TRY                             __try
  #define POV_EXCEPT(x)                       __except(x)
  #define SetPOVMSLong(v,h,l)                 *v = (((((POVMSLong)(h)) & 0x00000000ffffffff) << 32) | (((POVMSLong)(l)) & 0x00000000ffffffff))
  #define GetPOVMSLong(h,l,v)                 *h = ((v) >> 32) & 0x00000000ffffffff; *l = (v) & 0x00000000ffffffff
  #define POVMSLongToCDouble(x)               double(x)

  #define POV_NAMESPACE                       pov
  #define BEGIN_POV_NAMESPACE                 namespace POV_NAMESPACE { using namespace std;
  #define END_POV_NAMESPACE                   }
  #define USING_POV_NAMESPACE                 using namespace POV_NAMESPACE;

  // NB _heapmin () only works on NT.
  #define HEAPSHRINK                          _heapmin () ;

  #define NEED_INVHYP

  #define RTR_SHOW

  #define IStream                             POVIStream
  #define OStream                             POVOStream

  #define S_ISDIR(m)                        ((m & S_IFDIR) != 0)
  #define S_ISREG(m)                        ((m & S_IFREG) != 0)

  #if (_MSC_VER < 1300) && !defined __INTEL_COMPILER
    template<class _Ty> inline const _Ty& max(const _Ty& _X, const _Ty& _Y) {return (_X < _Y ? _Y : _X); }
    template<class _Ty> inline const _Ty& min(const _Ty& _X, const _Ty& _Y) {return (_Y < _X ? _Y : _X); }
  #elif _MSC_VER == 1400 && defined _WIN64
    inline const int& max(const int& _X, const int& _Y) {return (_X < _Y ? _Y : _X); }
    inline const int& min(const int& _X, const int& _Y) {return (_Y < _X ? _Y : _X); }
  #else
    inline double fabs(unsigned char val) { return (fabs ((double) val)) ; }
  #endif
#else
  #warning Using unknown compiler configuration
#endif

#define POVMS_Sys_Queue_Type                POVMS_Sys_QueueNode_Default *
#define POVMS_Sys_QueueToAddress(q)         POVMS_Sys_QueueToAddress_Default(q)
#define POVMS_Sys_AddressToQueue(a)         POVMS_Sys_AddressToQueue_Default(a)
#define POVMS_Sys_QueueOpen()               POVMS_Sys_QueueOpen_Default()
#define POVMS_Sys_QueueClose(q)             POVMS_Sys_QueueClose_Default(q)
#define POVMS_Sys_QueueReceive(q,l,b)       POVMS_Sys_QueueReceive_Default(q, l, b)
#define POVMS_Sys_QueueSend(q, p, l)        WIN_POVMS_Sys_QueueSend(q, p, l)

#ifndef MAX
  #define MAX(a,b) ((a>b)?a:b)
#endif
#ifndef MIN
  #define MIN(a,b) ((a>b)?b:a)
#endif

#ifndef HEAPSHRINK
#define HEAPSHRINK
#endif

#ifndef NAN
#define NAN (10E100)
#endif

void Fix_Watcom_Bug(char *s);

// this is for other compilers that don't use ztimer.h
#ifndef __GENDEFS
  #define __GENDEFS
  typedef unsigned char   uchar;
  typedef unsigned short  ushort;
  typedef unsigned int    uint;
  typedef unsigned long   ulong;
#endif

#ifndef COMPILER_VER
  #define COMPILER_VER ".win_unsupported"
#endif

#include <stdarg.h>
#include <stdlib.h>

#define MEM_RECLAIM    // Enables memory reclaimation

#ifdef NDEBUG
  #define MEM_STATS      1
#else
  #define MEM_STATS      2 // Enables extended status
  #define MEM_TAG          // Enables memory tag debugging
  #define MEM_TRACE        // Enables garbage collection and memory tracing

  #define MEM_PREFILL
  #define MEM_GUARD
#endif

/* For now, don't allow POV to track C++ new and deletes */
#define	POV_USE_NATIVE_CPP_MEM
#define POV_CPP_MEM_HAS_NOTHROW_SUPPORT 0

#define RED_RAW_FILE_EXTENSION    ".r8"  /* PICLAB's "RAW" file format */
#define GREEN_RAW_FILE_EXTENSION  ".g8"  /* PICLAB's "RAW" file format */
#define BLUE_RAW_FILE_EXTENSION   ".b8"  /* PICLAB's "RAW" file format */
#define PLATFORM_CONSTANT         1.622
#define FILENAME_SEPARATOR        '\\'
#define DRIVE_SEPARATOR           ':'
#define DEFAULT_OUTPUT_FORMAT     's'
#define POV_IS1                   ".bmp"
#define POV_IS2                   ".BMP"
#define POV_NUM_ECHO_LINES        2
#define POV_ERROR_MARKER_TEXT     " <----ERROR\n"

void WIN_Startup(void);
void WIN_Finish(void);
void WIN_Cooperate(int n);
int  WIN_Display_Init(int width, int height);
void WIN_Display_Plot(int x, int y, int r, int g, int b, int a);
void WIN_Display_Plot_Rect(int x1, int x2, int y1, int y2, int r, int g, int b, int a);
void WIN_Pre_Pixel (int x, int y, _COLOUR colour);
void WIN_Post_Pixel (int x, int y, _COLOUR colour);
void WIN_Display_Finished(void);
void WIN_Display_Close(void);
void WIN_LoadRerun(void);
void WIN_SaveRerun(void);
void WIN_PrintOtherCredits (void) ;
void WIN_Write_Line (_COLOUR *line, int y) ;
void WIN_Assign_Pixel (int x, int y, _COLOUR colour) ;
void WIN_Assert (const char *message, const char *filename, int line) ;
void WIN_Debug_Log (unsigned int from, const char *msg) ;
int WIN_System(char *s);
int WIN_Povray (int argc, char **argv) ;
int WIN_Allow_File_Write (const char *Filename, const unsigned int FileType) ;
int WIN_Allow_File_Read (const char *Filename, const unsigned int FileType) ;
int WIN_Shellout (int) ;
void *WIN_GetOutputContext (void) ;
void *WIN_pov_malloc(size_t size, char *file, int line, char *msg) ;
void *WIN_pov_calloc(size_t nitems, size_t size, char *file, int line, char *msg) ;
void *WIN_pov_realloc(void *ptr, size_t size, char *file, int line, char *msg) ;
void WIN_pov_free(void *ptr, char *file, int line) ;
void WIN_mem_init() ;
void WIN_mem_release_all() ;
char *WIN_pov_strdup(char *str) ;
int WIN_POVMS_Sys_QueueSend(struct POVMS_Sys_QueueNode_Default *q, void *p, int l) ;

#define POVRAY_COOPERATE_GLOBAL
#define POVRAY_BEGIN_COOPERATE    if (setjmp (gWinSetJmpEnv) == 0) {
#define POVRAY_END_COOPERATE      }
#define	EXIT_POVRAY(n)            povray_return_code = n ; longjmp (gWinSetJmpEnv, 1) ;

#define STARTUP_POVRAY            WIN_Startup();
#define FINISH_POVRAY             /* do nothing */
#define COOPERATE_0               WIN_Cooperate(0);
#define COOPERATE_1               WIN_Cooperate(1);
#define COOPERATE_2               WIN_Cooperate(2);
#define TEST_ABORT                WIN_Cooperate(0);

#define POV_DISPLAY_INIT(prc,w,h) WIN_Display_Init((w),(h));
#define POV_DISPLAY_CLOSE         WIN_Display_Close();
#define POV_DISPLAY_FINISHED      WIN_Display_Finished();
#define POV_DISPLAY_PLOT(prc,x,y,r,g,b,a) WIN_Display_Plot((x),(y),(r),(g),(b),(a));
#define POV_DISPLAY_PLOT_RECT(prc,x1,x2,y1,y2,r,g,b,a) WIN_Display_Plot_Rect((x1),(x2),(y1),(y2),(r),(g),(b),(a));
#define POV_PRE_PIXEL(x,y,c)      WIN_Pre_Pixel (x, y, c) ;
#define POV_POST_PIXEL(x,y,c)     WIN_Post_Pixel (x, y, c) ;
#define POV_SYSTEM(s)             WIN_System (s) ;
#define POV_WRITE_LINE(line,y)    WIN_Write_Line (line, y) ;
#define POV_ASSIGN_PIXEL(x,y,colour) WIN_Assign_Pixel (x, y, colour) ;
#define POV_SHELLOUT(type)        WIN_Shellout (type)

#define POV_ALLOW_FILE_WRITE(f,t) WIN_Allow_File_Write (f, t)
#define POV_ALLOW_FILE_READ(f,t)  WIN_Allow_File_Read (f, t)

#define POVMS_ASSERT_OUTPUT(s,f,l) WIN_Assert (s, f, l)

#define POV_BYTE_ORDER            'HHHH'

#define ALTMAIN
#define RENAME_FILE(orig,new)     rename(orig,new)
#define NEW_LINE_STRING           "\r\n"
#define TEXTSTREAM_CRLF
#define SYS_IMAGE_CLASS           BMP_Image
#define SYS_IMAGE_HEADER          "pvbmp.h"
#define READ_SYS_IMAGE(i,n)       Read_BMP_Image((i),(n))
#define GET_SYS_FILE_HANDLE       Get_BMP_File_Handle
#define SYS_DEF_EXT               ".bmp"

#define vsnprintf                 _vsnprintf

#define POVMSBool                 bool

#define PRINT_OTHER_CREDITS       WIN_PrintOtherCredits () ;

#define USE_LOCAL_POVMS_OUTPUT    0
#define FRONTEND_ADDRESS          WIN_GetOutputContext ()

/*
 VC6 math.h defines HUGE_VAL as +inf.  The bounding box routines
 seem to have problems with +/- inf that lead to page faults.
 Rather than fix the bbox routines, which shouldn't have to deal
 with infinities anyway, use the frame.h value for HUGE_VAL
 instead of the one in math.h   This depends on config.h being
 included after math.h, but that is the case in the current frame.h
 14 Jan 2001 RLP
*/
#undef HUGE_VAL

// this is necessary to fix a problem with VC++ seeking to the
// wrong location during #while loops. ("r" isn't enough).
#define READ_TXTFILE_STRING "rb"

// use a larger buffer for more efficient parsing
#define DEFAULT_ITEXTSTREAM_BUFFER_SIZE 65536

#ifdef WIN_MEM_TRACKING
  #define POV_MALLOC(size,msg)        WIN_pov_malloc ((size), __FILE__, __LINE__, (msg))
  #define POV_CALLOC(nitems,size,msg) WIN_pov_calloc ((nitems), (size), __FILE__, __LINE__, (msg))
  #define POV_REALLOC(ptr,size,msg)   WIN_pov_realloc ((ptr), (size), __FILE__, __LINE__, (msg))
  #define POV_FREE(ptr)               { WIN_pov_free ((void *)(ptr), __FILE__, __LINE__); (ptr) = NULL; }
  #define POV_MEM_INIT()              WIN_mem_init()
  #define POV_MEM_RELEASE_ALL()       WIN_mem_release_all()
  #define POV_STRDUP(str)             WIN_pov_strdup(str)
#endif

extern int povray_return_code ;
extern jmp_buf gWinSetJmpEnv ;

#endif
