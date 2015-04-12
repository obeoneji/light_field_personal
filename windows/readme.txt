------------------------------------------------------------------------------
$Id: //depot/povray/3.5/windows/readme.txt#4 $
------------------------------------------------------------------------------

NOTE: Use of the POV-Ray source code is governed by the conditions laid
out in distribution-license.txt and source-license.txt. Be sure you read
these files (which are included in this source archive, and also linked
from http://www.povray.org/povlegal.html) before doing anything with this
source.

In summary, if you are only going to be compiling this source for your
own education, and will not be distributing the resulting EXE or DLL's,
then basically you're OK.

Distributing any binaries, or modified source, requires that you comply with 
both the above licences, and the users of the distributed files must agree to
comply with POVLEGAL.DOC (see http://www.povray.org/povlegal.html).

Use of our source code in any other project (other than a fully-functional 
custom version of POV-Ray) is prohibited. In other words you may make your
own derivitive version, but you may not remove functionality or incorporate
the source code into another program.

The POV-Team provides no assistance with compiling POV-Ray. The POV-Ray
news server at http://news.povray.org/ and news://news.povray.org/ provides 
the best means for support with relation to this subject. We recommend you 
visit the povray.programming group there.

------------------------------------------------------------------------------
    SO YOU WANT TO COMPILE IT NOW ??? - QUICK START FOR THE IMPATIENT
------------------------------------------------------------------------------

Open windows\vc.net\povray.sln in VC.NET (2003 or later).

Select 'povray' as the start-up project. Do NOT try to compile the default
project (cmedit). It won't do anything (and is not meant to). It's a utility
project there for the convenience of our developers.

Edit the file source\optout.h and customize DISTRIBUTION_MESSAGE. Then hit F7
and wait. Note that if you do not do the above first you will get compile errors.

------------------------------------------------------------------------------
The following section only matters if you want to work with the editor DLL's.
------------------------------------------------------------------------------

You may want to turn on the DEVELOPMENT macro near the top of pvengine.cpp to
modify the search path for DLL's (it's the way we test without having to copy
the DLL's to the POVWIN install dir). You probably won't want to do this if you
don't intend to build the editor DLL's.

If you want to compile the CodeMax editor, build the 'codemax' project. The
output of this will be placed in windows\codemax. pvengine will find this if
(a) you compiled it with the DEVELOPMENT macro turned on, and (b) you have a
copy of cmedit.dll in the codemax directory. Otherwise you'll need to copy
it to either windows\vc.net\bin or your POVWIN 3.6 binary install dir. If you
have not built cmedit.dll (using C++Builder), just copy the one supplied with
the binary distribution of POV-Ray v3.6.0 into the codemax directory, so that
you can more easily test and debug whatever work you are doing on codemax itself.

If you want to build cmedit.dll you need C++Builder, plus you'll need to build
and install the codemax component in C++Builder before you build cmedit. (see
the windows\codemax\component dir).

The next release of the POVWIN source code will contain a replacement cmedit
DLL implementation that is coded in C++ to the Win32 API. C++Builder will not
be needed and all the above will be a lot less hassle.

------------------------------------------------------------------------------
                               FULL DETAILS
------------------------------------------------------------------------------

This archive contains all the code needed to compile the Windows version
of POV-Ray v3.6 (excluding any compiler or operating-specific header files
and libraries that are supplied by Microsoft). You will need either Visual
Studio .NET, or the Intel C++ compiler v8. Other versions of these compilers
may work but have not been tested. Other compilers (e.g. watcom, borland) may
or may not work, we haven't tested these either. There are some untested
makefiles in the windows\makefiles directory for your convenience, but don't
expect these to work without some tweaking of the files and/or source code.

NOTE: you must have already installed the POV-Ray for Windows v3.6 binary
files from the povray website to be able to use the resulting EXE. This source 
on its own won't be a lot of use to you (unless you only want to study it).

The directory tree is as follows:

+--libraries ............... Third-party support libraries
¦  +--jpeg ................... JPEG library
¦  +--png .................... PNG library
¦  +--tiff ................... TIFF library
¦  +--zlib ................... ZLIB libary (needed for PNG support)
+--source .................. Generic POV-Ray core renderer code.
¦  +--base ................... Generic platform base code.
¦  +--frontend ............... Generic platform frontend code.
+--windows ................. Windows-specific platform support code
   +--bitmaps ................ BMP files used as windows resources
   +--console ................ Support files for console-mode builds
   +--codemax ................ Editor wrapper source code files
   ¦  +--component ............. C++Builder v3 source for codemax component
   ¦  +--source ................ Actual source code for editor itself
   +--guiext ................. Sample source code for POVWIN GUI Extension
   +--icons .................. Icon resources
   +--makefiles .............. Untested makefiles for various compilers
   +--vc.net ..................Visual Studio.NET solution files
   +--ztimer ................. Precision timer library

The vc.net workspace contains the following projects -

  00 README
    A utility project that references this file.

  cmedit
    A utility project that contains the codemax editor wrapper files.
            NOTE: YOU CANNOT COMPILE THIS PROJECT FROM VC.NET.
    The project is included as a convenient way for the POV-Ray developers to
    get access to these files while editing other parts of the POV-Ray source.

    To compile this source code you need Borland's C++Builder, version 3 (or
    possibly later versions). You also need to build and install the cmpkg
    component to C++Builder first.

  cmpkg
    A utility project that contains the codemax C++Builder interface source.
              NOTE: YOU CANNOT COMPILE THIS PROJECT FROM VC.NET.
    The project is included as a convenient way for the POV-Ray developers to
    get access to these files while editing other parts of the POV-Ray source.

    To compile this source code you need Borland's C++Builder, version 3 (or
    possibly later versions).

  codemax
    This is the source code for Winmain's Codemax editor custom control, with
    Nathan Lewis's codelist addition and modifications made by the POV-Team.

  guidemo
    This project builds a demonstation GUIEXT (gui extension) for POVWIN. Note
    that it will not compile as-is; you need to modify guiext.c in all places
    indicated (where there are #error directives). You don't need to compile
    this unless you want to make a GUIEXT (most people don't). GUIEXT's are
    special DLL's that allow other programs to communicate with POV-Ray for
    Windows and obtain output from it. (For example, the popular Moray modeller
    uses this technique ; see http://www.stmuc.com/moray/).

  jpeg
    The JPEG library source. You don't need to compile this manually.

  libpng
    The PNG library source. You don't need to compile this manually.

  povray
    The main POVWIN project. Building this will automatically build the jpeg,
    libpng, tiff and zlib projects. You don't need to build any other projects
    as the compiled DLL's needed for POVWIN will have been installed when you
    installed the normal POV-Ray for Windows v3.6 binaries.

  tiff
    The TIFF library source. You don't need to compile this manually.

  zlib
    The ZLIB library source. You don't need to compile this manually.

All you need to do to get a working POV-Ray for Windows binary is to edit the
file source\optout.h to define your name and email address in the distribution 
message (search for DISTRIBUTION_MESSAGE). Then, activate the POVRAY project 
and hit F7. You should end up with the file windows\vc.net\bin\pvengine.exe.
If it complains about missing DLL's when you run it, it's probably because you
weren't paying attention when we told you that you needed to install POV-Ray
v3.6 first :-). (If you have in fact installed it ensure it is the same version
as the source code you have compiled, as the DLL names will change between v3.60
and v3.61).

ABOUT THE EDITOR
----------------

The editor used in POV-Ray for Windows is a custom control called CodeMax. It
provides the core editing functionality that you see in the edit windows. 
However, there is a lot more to the editor than that, as we will explain.

The custom control (project codemax in the workspace) produces a DLL called 
POVCMAX2.DLL. This DLL is loaded by another DLL, CMEDIT.DLL. CMEDIT.DLL is 
produced from the source code in the windows\codemax subdirectory, using 
Borland's C++Builder compiler, version 3 (or possibly later versions, though 
this has not been tested). POVCMAX2.DLL is never loaded directly by PVENGINE
itself.

CMEDIT contains the support code needed to turn the custom control in POVCMAX2 
into a fully-featured editor, as well as adding some extra functionality. 

If you want to change the editor core code, all you need to do is to modify  
and recompile POVCMAX2. If you need to modify the way the wrapper code or 
enhancements work, then you need to rebuild CMEDIT, and for that you need 
Borland's compiler installed.

The CMEDIT project uses a custom package to encapsulate the codemax DLL into 
a C++Builder run-time component. (Note that this is NOT a visual component, 
you won't see it on your palette). The code for this package is in the 
windows\codemax\component subdirectory. You must build this and install the 
resulting package into C++Builder before compining CMEDIT. Instructions as to 
how to do this are beyond the scope of this document, see the C++Builder 
manuals.

If all this sounds complicated, well, it kind of is, but it's not ;)

You really don't need to worry about any of the above unless you want to do
editor hacking ; the DLL's are provided with the POV-Ray for Windows binaries
are sufficient, and therefore you don't need to compile this stuff at all if 
you don't want to. And if all you want to do is compile the codemax DLL itself
then you don't need C++Builder.

Be aware that if you do compile new DLL's, you'll need to put them somewhere
useful for them to be loaded. The best place is the <installdir>\bin directory 
of the POV-Ray for Windows binary install (make sure you back up the original 
DLL's first).

If you define the macro DEVELOPMENT in windows\pvengine.cpp (just search for 
this, it's commented out by default, and is near the top of the file), the 
resulting PVENGINE.EXE will use a different search strategy for the editor 
DLL's. Just look at the source for details.

THANKS
------

The POV-Team would like to thank Perforce software for supplying the turbo-
studly revision control system we used in developing POV-Ray v3.6. It saved
us numerous headaches. Also thanks to Wise Solutions for supplying the copy
of the Wise installation system we used in making the setup program, and of
course to Winmain software for supplying us with the editor that has been
included in POVWIN since version 3.1 (CodeMax was at that time a commercial
product ; it is now freeware, thus allowing us to include its source with
POVWIN since v3.5). Finally thanks to Intel Corporation for the compilers
we use to generate our release compiles.
