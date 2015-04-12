/****************************************************************************
 *                   pvtips.h
 *
 * This file contains the text used for the 'tip of the day' dialog.
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
 * $File: //depot/povray/3.5/windows/pvtips.h $
 * $Revision: #8 $
 * $Change: 2933 $
 * $DateTime: 2004/07/03 13:06:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

char *tips [] =
{
  "Many frequently asked questions refer to things already in the documentation "
  "and even these tip dialogs. Be sure to read the help file (press F1 after "
  "dismissing this dialog) and to read all of these tips before asking a question.",

  "Many people ask how to do things that are clearly visible in the menus. They "
  "just never looked. Spending a few minutes exploring all the menu options could "
  "save you some frustration later on!",

  "By default, POV-Ray for Windows will not allow you to start multiple copies "
  "of itself. This seems to be the behaviour that most people want. However, "
  "it is not always desirable. To change this, deselect 'Options|Keep Single Instance'",

  "A quick way to switch focus from the render window to the editor is to just "
  "hit ESCAPE. This is most useful just after a render. Additionally, hitting "
  "ESCAPE while the message window is displayed at the bottom of an edit window "
  "will hide the messages.",

  "If you find that the render window gets in your way a lot, explore the various "
  "render window options under &Options|Render &Window. These include some that "
  "will prevent the render window grabbing focus when it is first displayed.",

  "Version 3.5 introduces some important time-saving features to the built-in editor "
  "including keyword expansion and keyword listing. See the documentation for more "
  "information.",

  "By default, if you double-click on a file using the Windows Explorer while "
  "POV-Ray is already loaded, the file will be loaded into the current instance. You can "
  "change this to start the file in a new copy of POV-Ray by clearing '&Options|Keep Single Instance'.",

  "You can open a file for editing or rendering from Windows Explorer by using the "
  "right mouse button when you click on the file name. This feature even works with "
  "POV-Ray's own internal file browse dialogs. For example, dismiss this tip dialog "
  "press ALT-S, choose a .POV or .INI file, press the right mouse button and see "
  "what happens.",

  "By default, POV-Ray's rendering speed under Windows will be slower than DOS "
  "because it tries to be 'nice' to other programs by not hogging the CPU. If "
  "time is an issue (or you want to do a benchmark), you need to set the priority "
  "of the renderer from the '&Render|Render &Priority' menu. Also note that you "
  "will make a render slightly more efficient by closing the render window or, "
  "preferably, not turning display on at all.",

  "Regarding the message window, you can select a plain background, or a different "
  "bitmap for the tiled background, from the &Options menu. A shortcut for the bitmap "
  "file browse dialog box is ALT-P. POV-Ray for Windows comes with a number of tileable "
  "backgrounds for you to use.",

  "You can access a number of commonly-used functions by pressing the right "
  "mouse button while over a window. You will get a different pop-up menu for "
  "the message window, the edit window, and the render window.",

  "New users often ask \"how do I save my image?\", not realising that it is written "
  "to disk automatically during the render. A good read of the documentation usually "
  "cures this problem.",

  "You can cause POV-Ray to output all image files to a single directory by "
  "specifying it using Output_File_Name. E.g., to output all image files to "
  "the directory c:\\images, use Output_File_Name=c:\\images\\ in either an INI "
  "file, or on the command-line. Unless you explicitly specify an output file, "
  "the output file will be automatically placed in that directory.",

  "You can change the default color and font for the message display from the "
  "&Options|Message Window menu.",

  "You can close the mini-message window (that is displayed below the editor window "
  "during a render or after an error) just by hitting ESCAPE.",

  "If you are using POV-Ray's own editor, POV-Edit, you can press F1 when "
  "the cursor is on a word to do a help-file lookup for that word.",

  "That while the default output file type is uncompressed BMP, POV-Ray for Windows "
  "can also write TGA (+ft), compressed TGA (+fc), BMP (+fs), PNG (+fn) and PPM (+fp).",

  "You can Drag'N'Drop .POV and .INI files onto POV-Ray, and POV will either render "
  "them or open them for editing (depending on the setting of the menu '&Options|&Drag'N'Drop Destination'.",

  "You can supply command-line parameters to POV-Ray for Windows in exactly the "
  "same way as for the Unix version. If any parameters are on the command-line "
  "when POV-Ray starts, it will fire up the rendering engine straight away. "
  "This allows you to set up icons for commonly-used tasks you may perform. "
  "Read the help file section on command-line parameters for more information.",

  "That it is also legal to supply INI file parameters on the command line ? "
  "For example, instead of \"+ifilename.pov\" you can say \"Input_File_Name="
  "filename.pov\". This is actually essential if you are using long filenames "
  "that contain spaces, as the conventional \"+i\" form does not work with those "
  "types of filenames.",

  "That you can customize the &Tools menu ? You can add your own entries (up to 32 "
  "in fact) that can execute any external program. POV-Ray for Windows allows you to "
  "specify parameters, such as '%r', which expands to the last rendered file.",

  NULL
} ;

