/****************************************************************************
 *                  config.h
 *
 * This file contains the Windows Console specific defines.
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
 * $File: $
 * $Revision: $
 * $Change: $
 * $DateTime: $
 * $Author: $
 * $Log$
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <math.h>

#ifdef __MINGW32__                                             // MinGW specific

    #include <algorithm>

    // Note that this section is supposed to be compiled with MinGW 3.2 with GCC 3.2.3
    // Old names (without underscores) are turned off there.

    #if __MINGW32_MAJOR_VERSION == 3
        #define COMPILER_MAJOR_VER                             "3"
    #else
        #error "Adjust config.h to your compiler!"
    #endif

    #if __MINGW32_MINOR_VERSION == 1
        #define COMPILER_MINOR_VER                             "1"
    #else
        #error "Adjust config.h to your compiler!"
    #endif

    #if  __GNUC__ == 3
        #define GCC_MAJOR_VER                                  "3"
    #else
        #error "Adjust config.h to your compiler!"
    #endif

    #if __GNUC_MINOR__ == 2
        #define GCC_MINOR_VER                                  "2"
    #else
        #error "Adjust config.h to your compiler!"
    #endif

    #if __GNUC_PATCHLEVEL__ == 3
        #define GCC_PATCH_VER                                  "3"
    #else
        #error "Adjust config.h to your compiler!"
    #endif

    #define COMPILER_VER                                       ".mingw-" COMPILER_MAJOR_VER \
                                                               "." COMPILER_MINOR_VER \
                                                               "(gcc-" GCC_MAJOR_VER \
                                                               "." GCC_MINOR_VER \
                                                               "." GCC_PATCH_VER ")"


    using namespace std;

    // deliver unlink definition
    #include <unistd.h>

    // cause rad2 to be defined
    #include <windef.h>
    #include <dlgs.h>
    #ifdef rad2
        // undefine it and leave name free for field in blob.h
        #undef rad2
    #endif

    #define IStream                                            MinGW_IStream
    #include <objbase.h>
    #undef  IStream
    #define IStream                                            POV_IStream

    #define POV_NAMESPACE                                      pov
    #define BEGIN_POV_NAMESPACE                                namespace POV_NAMESPACE { using namespace std;
    #define END_POV_NAMESPACE                                  }
    #define USING_POV_NAMESPACE                                using namespace POV_NAMESPACE;

    #define POV_FRONTEND_NAMESPACE                             pov_frontend
    #define BEGIN_POV_FRONTEND_NAMESPACE                       namespace POV_FRONTEND_NAMESPACE { using namespace std;
    #define END_POV_FRONTEND_NAMESPACE                         }
    #define USING_POV_FRONTEND_NAMESPACE                       using namespace POV_FRONTEND_NAMESPACE;

    #define POV_BASE_NAMESPACE                                 pov_base
    #define BEGIN_POV_BASE_NAMESPACE                           namespace POV_BASE_NAMESPACE { using namespace std;
    #define END_POV_BASE_NAMESPACE                             }
    #define USING_POV_BASE_NAMESPACE                           using namespace POV_BASE_NAMESPACE;

    #define POV_CPP_MEM_HAS_NEW_INCLUDE                        1
    #define POV_CPP_MEM_HAS_PLACEMENT_FORMS                    1
    #define POV_CPP_MEM_HAS_NOTHROW_SUPPORT                    1

    // turn on asinh,acosh,atanh
    #define NEED_INVHYP                                        1

    #define DELETE_FILE(name)                                  _unlink(name)
    #define getcwd                                             _getcwd

#elif __BORLANDC__                                             // Borland specific

    #if __BORLANDC__ >= 0x551 && __BORLANDC__ < 0x552
        #define COMPILER_VER_STR                               "5.51"
    #else
        #error "Adjust config.h to your compiler!"
    #endif

    #define COMPILER_VER                                       ".borland-" COMPILER_VER_STR

    #include <dir.h>

    #define POV_LONG                                           __int64
    #define POVMSLong                                          __int64
    #define SetPOVMSLong(v,h,l)                                *v = (((((POVMSLong)(h)) & 0x00000000ffffffff) << 32) | (((POVMSLong)(l)) & 0x00000000ffffffff))
    #define GetPOVMSLong(h,l,v)                                *h = ((v) >> 32) & 0x00000000ffffffff; *l = (v) & 0x00000000ffffffff
    #define POVMSLongToCDouble(x)                              double(x)

    #define IStream                                            MinGW_IStream
    #include <objbase.h>
    #undef  IStream
    #define IStream                                            POV_IStream

    #define POV_NAMESPACE
    #define BEGIN_POV_NAMESPACE
    #define END_POV_NAMESPACE
    #define USING_POV_NAMESPACE

    #define POV_FRONTEND_NAMESPACE
    #define BEGIN_POV_FRONTEND_NAMESPACE
    #define END_POV_FRONTEND_NAMESPACE
    #define USING_POV_FRONTEND_NAMESPACE

    #define POV_CPP_MEM_HAS_NEW_INCLUDE                        1
    #define POV_CPP_MEM_HAS_PLACEMENT_FORMS                    0
    #define POV_CPP_MEM_HAS_NOTHROW_SUPPORT                    0

    // turn on asinh,acosh,atanh
    #define NEED_INVHYP                                        1

    #include <objidl.h>
    #undef BLOB
    #define BLOB POV_BLOB

#elif __WATCOMC__                                              // Watcom specific

    #define COMPILER_VER                                       ".watcom"
    // include for unlink
    #include <stdlib.h>
    #include <io.h>
    #include <direct.h>

    #define POV_LONG                                           __int64
    #define POVMSLong                                          __int64
    #define SetPOVMSLong(v,h,l)                                *v = (((((POVMSLong)(h)) & 0x00000000ffffffff) << 32) | (((POVMSLong)(l)) & 0x00000000ffffffff))
    #define GetPOVMSLong(h,l,v)                                *h = ((v) >> 32) & 0x00000000ffffffff; *l = (v) & 0x00000000ffffffff
    #define POVMSLongToCDouble(x)                              double(x)

    #define POV_NAMESPACE                                      pov
    #define BEGIN_POV_NAMESPACE                                namespace POV_NAMESPACE {
    #define END_POV_NAMESPACE                                  }
    #define USING_POV_NAMESPACE                                using namespace POV_NAMESPACE;

    #define POV_FRONTEND_NAMESPACE
    #define BEGIN_POV_FRONTEND_NAMESPACE
    #define END_POV_FRONTEND_NAMESPACE
    #define USING_POV_FRONTEND_NAMESPACE

    #define POV_BASE_NAMESPACE
    #define BEGIN_POV_BASE_NAMESPACE
    #define END_POV_BASE_NAMESPACE
    #define USING_POV_BASE_NAMESPACE

    #define POV_CPP_MEM_HAS_NEW_INCLUDE                        1
    #define POV_CPP_MEM_HAS_PLACEMENT_FORMS                    0
    #define POV_CPP_MEM_HAS_NOTHROW_SUPPORT                    0

    // turn on asinh,acosh,atanh
    #define NEED_INVHYP                                        1

#elif __DJGPP__

    #define COMPILER_VER                                       "djgpp(gcc)"
    using namespace std;

    // deliver unlink definition
    #include <unistd.h>

    #define POV_NAMESPACE                                      pov
    #define BEGIN_POV_NAMESPACE                                namespace POV_NAMESPACE { using namespace std;
    #define END_POV_NAMESPACE                                  }
    #define USING_POV_NAMESPACE                                using namespace POV_NAMESPACE;

    #define POV_CPP_MEM_HAS_NEW_INCLUDE                        1
    #define POV_CPP_MEM_HAS_PLACEMENT_FORMS                    0
    #define POV_CPP_MEM_HAS_NOTHROW_SUPPORT                    0

#endif

#define EPSILON                                                1.0e-7 // 1.0e-10

#define POVRAY_PLATFORM_NAME				                           "POV-Ray for consoles"

#define MEM_RECLAIM							                               1
#define MEM_STATS							                                 1

#define READ_ENV_VAR
#define PROCESS_POVRAY_INI

#define POVRAY_COOPERATE_GLOBAL jmp_buf globalSetJmpEnv;
#define POVRAY_BEGIN_COOPERATE	if(setjmp(globalSetJmpEnv) == 0) {
#define POVRAY_END_COOPERATE	}
#define	EXIT_POVRAY(n)			longjmp(globalSetJmpEnv, 1);

#endif // CONFIG_H
