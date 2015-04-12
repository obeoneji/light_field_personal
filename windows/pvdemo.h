/****************************************************************************
 *                   pvdemo.h
 *
 * This file contains the SDL source code to the standard demo scene as a C
 * array.
 *
 * Author: Christopher J. Cason.
 * Scene author: Fabien Mosen.
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
 * $File: //depot/povray/3.5/windows/pvdemo.h $
 * $Revision: #5 $
 * $Change: 2933 $
 * $DateTime: 2004/07/03 13:06:53 $
 * $Author: chrisc $
 * $Log$
 *****************************************************************************/

char *povdemo_scene [] =
{
  "// Persistence Of Vision raytracer version 3.5 sample file.",
  "// Copyright 2001 Fabien Mosen",
  "",
  "#declare White   = rgb 1;",
  "#declare Gray60 = White*0.60;",
  "#declare Gray80 = White*0.80;",
  "#declare Orange = color red 1 green 0.5 blue 0.0;",
  "",
  "#declare Povray_Logo_Prism =",
  "merge {",
  "   cylinder {-z, z, 1 translate 2*y}",
  "   difference {",
  "      prism {-0.9999, 0.9999, 4, <-1,2>, <1,2>, <0,-4>, <-1,2> rotate -90*x}",
  "      cylinder {-z, z, 1.4 scale <1,0.9,1.1> translate 1.9*y}",
  "   }",
  "   difference {",
  "      cylinder {-z, z, 1 scale <2.6, 2.2, 1>}",
  "      cylinder {-z, z, 1 scale <2.22, 1.72, 1.1> translate <-0.43, 0, 0>}",
  "      rotate z*30 translate 2*y",
  "   }",
  "   rotate <0, 0, -25>",
  "   translate <-0.5,-0.35,0>",
  "   scale <1/4,1/4,1/8>",
  "}",
  "",
  "camera {location <20,40,28> direction 2*z look_at <0,2,0>}",
  "",
  "light_source {<-140,200,300> rgb <1.0, 1.0, 0.95>*1.5}",
  "light_source {<140,200,-300> rgb <0.9, 0.9, 1.00>*0.8 shadowless}",
  "",
  "#declare r1 = seed(0);",
  "",
  "//----------------------- THE TABLE",
  "#declare Pig_1 =",
  "pigment {",
  "   gradient z",
  "   color_map {",
  "      [0.00, rgb <0.01, 0.59, 0.81>]",
  "      [0.70, rgb <0.01, 0.59, 0.81>]",
  "      [0.70, rgb <0.98, 0.98, 0.87>]",
  "      [1.00, rgb <0.98, 0.98, 0.87>]",
  "   }",
  "   frequency 4",
  "}",
  "",
  "#declare Pig_2 =",
  "pigment {",
  "   bozo",
  "   color_map {",
  "      [0.00, rgb <0.35, 0.58, 0.88>*1.0]",
  "      [0.25, rgb <0.35, 0.58, 0.88>*1.1]",
  "      [0.50, rgb <0.35, 0.58, 0.88>*0.9]",
  "      [0.75, rgb <0.35, 0.58, 0.88>*1.0]",
  "      [1.00, rgb <0.35, 0.58, 0.88>*0.8]",
  "   }",
  "   scale 0.1",
  "}",
  "",
  "#declare Nappe =",
  "cylinder {0,y*-1,50",
  "   texture {",
  "      pigment {",
  "         gradient x",
  "         pigment_map {",
  "            [0.0, Pig_1]",
  "            [0.5, Pig_1]",
  "            [0.5, Pig_2]",
  "            [1.0, Pig_2]",
  "         }",
  "         warp {turbulence .05 octaves 2}",
  "      }",
  "      normal {quilted 0.6 scale 0.025 warp {turbulence 0.05 octaves 2}}",
  "      scale 5",
  "      translate 10",
  "   }",
  "}",
  "",
  "object {Nappe}",
  "",
  "//----------------------- BISCUITS",
  "#declare Tex_Biscuit =",
  "texture {",
  "   pigment {color rgb <0.98, 0.83, 0.58>}",
  "   normal {dents 1.2 scale 0.01}",
  "   finish {phong 0 brilliance 0.7}",
  "}",
  "",
  "#declare Base_Biscuit =",
  "union {",
  "   blob {",
  "      threshold 0.7",
  "      #declare I = 0;",
  "      #while (I < 359)",
  "         sphere {<4,0,0>, 1+rand(r1)*0.1, 1 rotate y*I}",
  "         #declare I = I+(360/28);",
  "      #end",
  "   }",
  "   cylinder {<0,0,0>, <0,0.5,0>, 4}",
  "   texture {Tex_Biscuit}",
  "}",
  "",
  "#declare Chocolate =",
  "union {",
  "   difference {",
  "      cone {<0,0,0>, 4.2, <0,0.4,0>, 4}",
  "      cone {<0,0.1,0>, 3.6, <0,0.401,0>, 3.75}",
  "   }",
  "   torus {",
  "      3.55, 0.1",
  "      translate y*0.2",
  "      clipped_by {torus {3.55+0.1, 0.1 translate y*0.1}}",
  "   }",
  "   union {",
  "      #declare I = -4;",
  "      #while (I < 4)",
  "         cylinder {<-4,0.1,I>,<4,0.1,I>, 0.05}",
  "         #declare I = I+0.5;",
  "      #end",
  "      clipped_by {cone {<0,0,0>, 4.2, <0,0.4,0>, 4}}",
  "   }",
  "   torus {3.96, 0.04 translate y*0.4}",
  "   torus {3.79, 0.04 translate y*0.4}",
  "}",
  "",
  "#declare LogoFun =",
  "object{Povray_Logo_Prism rotate x*90 scale 2.2 translate -0.3*z}",
  "",
  "#declare ProjLogo =",
  "blob {",
  "   threshold 0.6",
  "   #declare I = 0;",
  "   #while (I < 1)",
  "      #declare Pos = <-2+rand(r1)*4, 0, -2+rand(r1)*4>;",
  "      #if (inside(LogoFun,Pos))",
  "         sphere {Pos, 0.08, 1}",
  "      #end",
  "      #declare I = I+0.0002;",
  "   #end",
  "}",
  "",
  "#declare Black_Chocolate =",
  "texture {",
  "   pigment {color rgb <0.24, 0.10, 0.03>}",
  "   normal {wrinkles 0.2}",
  "   finish {specular 0.3}",
  "   }",
  "#declare Milk_Chocolate =",
  "texture {",
  "   pigment {color rgb <0.48, 0.26, 0.13>}",
  "   normal {wrinkles 0.2}",
  "   finish {specular 0.2}",
  "}",
  "#declare White_Chocolate =",
  "texture {",
  "   pigment {color rgb <0.96, 0.95, 0.75>}",
  "   normal {wrinkles 0.2}",
  "   finish {ambient 0.3 specular 0.01}",
  "}",
  "",
  "#declare Icing = texture {",
  "   pigment {rgbf <0.95, 0.95, 1.00, 0.1>*1.2}",
  "   normal {bumps 0.1}",
  "}",
  "",
  "#declare Biscuit_1 =",
  "union {",
  "   object {Base_Biscuit}",
  "   object {Chocolate translate y*0.5 texture {Black_Chocolate}}",
  "   disc {",
  "      <0,0.101,0>, y, 3.6",
  "      translate y*0.5",
  "      texture {Black_Chocolate} normal {bumps 0.3 scale 0.05}",
  "   }",
  "   object {ProjLogo scale 1.5 rotate y*-90 translate <-0.4,0.6,0.5> texture {Icing}}",
  "   translate y*0.5",
  "}",
  "#declare Biscuit_2 =",
  "union {",
  "   object {Base_Biscuit}",
  "   object {Chocolate translate y*0.5 texture {Milk_Chocolate}}",
  "   disc {",
  "      <0,0.101,0>, y, 3.6",
  "      translate y*0.5",
  "      texture {Milk_Chocolate} normal {bumps 0.3 scale 0.05}",
  "   }",
  "   object {ProjLogo scale 1.5 rotate y*-90 translate <-0.4,0.6,0.5> texture {White_Chocolate}}",
  "   translate y*0.5",
  "}",
  "#declare Biscuit_3 =",
  "union {",
  "   object {Base_Biscuit}",
  "   object {Chocolate translate y*0.5 texture {White_Chocolate}}",
  "   disc {",
  "      <0,0.101,0>, y, 3.6",
  "      translate y*0.5",
  "      texture {White_Chocolate} normal {bumps 0.3 scale 0.05}",
  "   }",
  "   object {ProjLogo scale 1.5 rotate y*-90 translate <-0.4,0.6,0.5> texture {Milk_Chocolate}}",
  "   translate y*0.5",
  "}",
  "",
  "object {Biscuit_2 rotate y*-80 translate <-3.5,0,2>}",
  "object {Biscuit_1 rotate y*-120 translate <3.5,0,-4>}",
  "object {Biscuit_3 rotate x*-4 translate <8.5,0.9,0>}",
  "",
  "#macro SevenBiscuits (Bisc,Num)",
  "   union {",
  "      #declare I = 0;",
  "      #while (I < Num)",
  "         object {Bisc translate x*9 rotate y*60*I}",
  "         #declare I = I+1;",
  "      #end",
  "      object {Bisc}",
  "   }",
  "#end",
  "",
  "//----------------------- CRUMBS",
  "#declare Fun_Sphere = function {x*x + y*y +z*z}",
  "#declare f_noise3d = function { internal(76) }",
  "",
  "#declare Crumb =",
  "isosurface {",
  "   function {Fun_Sphere(x,y,z) + f_noise3d(x*2,y*2,z*2)*1}",
  "   threshold 1",
  "   max_gradient 3.9",
  "   accuracy 0.01",
  "   contained_by {box {-1,1}}",
  "   scale 0.5",
  "}",
  "",
  "#declare r1 = seed(0);",
  "",
  "#declare I = 0;",
  "#while (I < 1)",
  "   object {",
  "      Crumb",
  "      rotate rand(r1)*360",
  "      scale 0.2+rand(r1)*0.3",
  "      translate <rand(r1)*10,0,rand(r1)*10>",
  "      texture {Tex_Biscuit}",
  "   }",
  "   object {",
  "      Crumb",
  "      rotate rand(r1)*360",
  "      scale 0.1+rand(r1)*0.15",
  "      translate <rand(r1)*10,0,rand(r1)*10>",
  "      texture {Tex_Biscuit}",
  "   }",
  "   #declare I = I+0.03;",
  "#end",
  "",
  "//----------------------- METAL BOX",
  "#declare Pig3 =",
  "pigment {",
  "   gradient y",
  "   color_map {",
  "      [0, rgb <0.1, 0.5, 0.7>]",
  "      [1, rgb <0.7, 0.6, 0.4>]",
  "   }",
  "   scale 0.5",
  "}",
  "",
  "#declare Pig4 =",
  "pigment {",
  "   crackle",
  "   color_map {",
  "      [0, rgb <1.0, 0.5, 0.6>]",
  "      [1, rgb <0.0, 0.0, 0.0>]",
  "   }",
  "   scale 0.2",
  "}",
  "",
  "#declare MetalBoxPig =",
  "pigment {",
  "   radial frequency 60",
  "   pigment_map {",
  "      [0.0, Pig3]",
  "      [0.5, Pig3]",
  "      [0.5, Pig4]",
  "      [1.0, Pig4]",
  "   }",
  "}",
  "",
  "#declare BiscuitBox =",
  "union {",
  "   difference {",
  "      cylinder {<0,0,0>, <0,5,0>, 14}",
  "      cylinder {<0,0.1,0>, <0,5.1,0>, 13.9}",
  "   }",
  "   torus {14, 0.1 translate y*5}",
  "   torus {14, 0.1 translate y*0.1}",
  "   torus {14, 0.1 scale <1,2,1> translate y*4}",
  "   cylinder {",
  "      <0,0.3,0>,<0,3.5,0>, 14.01 open",
  "      pigment {MetalBoxPig}",
  "      finish {phong 0.8 reflection {0.01, 0.15}}",
  "   }",
  "   pigment {Gray60}",
  "   finish {phong 0.8 metallic reflection {0.5, 0.7}}",
  "}",
  "",
  "union {",
  "   object {BiscuitBox}",
  "   object {SevenBiscuits (Biscuit_1,6)}",
  "   object {SevenBiscuits (Biscuit_3,6) rotate y*30 translate y*1}",
  "   object {SevenBiscuits (Biscuit_2,5) rotate y*0 translate y*2}",
  "   rotate y*-75 translate <-18,0,-12>",
  "}",
  "",
  "//-----------------------CUP OF TEA",
  "#declare TeaCup =",
  "union {",
  "   difference {",
  "      cylinder {<0,1.2,0>, <0,6,0>, 4.2}",
  "      cylinder {<0,1,0>, <0,6.2,0>, 3.8}",
  "   }",
  "",
  "   difference {",
  "      cylinder {<0,0.2,0>, <0,2.5,0>, 4}",
  "      torus {2.8, 1 translate y*2.5}",
  "      torus {4, 1 translate y*0}",
  "      cylinder {<0,1.5,0>, <0,2.6,0>, 2.8}",
  "   }",
  "",
  "   difference {",
  "      #declare LiquidLevel = 5;",
  "      cylinder {<0,1.4,0>, <0,LiquidLevel,0>, 4}",
  "      torus {3.6, 0.2 translate y*LiquidLevel}",
  "      cylinder {<0,LiquidLevel-0.2,0>,<0,LiquidLevel+0.3,0>,3.6}",
  "      pigment {Orange*0.8 filter 0.6}",
  "      finish {phong 0.7 reflection 0.15}",
  "      normal {bumps 0.05 scale 1}",
  "   }",
  "",
  "   torus {4.0, 0.2 translate y*6.0}",
  "   torus {4.0, 0.2 translate y*1.2}",
  "   torus {2.8, 0.2 translate y*0.2}",
  "",
  "   union {",
  "      difference {",
  "         cylinder {<0.2,0,0>,<-0.2,0,0>,0.5}",
  "         torus {0.5, 0.2 rotate z*90 translate x*0.2}",
  "         translate y*1.25",
  "      }",
  "      difference {",
  "         cylinder {<0.2,0,0>,<-0.2,0,0>,0.5}",
  "         torus {0.5, 0.2 rotate z*90 translate x*0.2}",
  "         translate y*-1.25",
  "      }",
  "      torus {1.25, 0.3 rotate x*90 clipped_by {plane {x,0 inverse}} translate x*0.8}",
  "      cylinder {<0,-1.25,0>,<0.8,-1.25,0>,0.3}",
  "      cylinder {<0,1.25,0>,<0.8,1.25,0>,0.3}",
  "      scale <1,1,1.5> translate <4.2,4,0> rotate y*-90",
  "   }",
  "",
  "   pigment {White}",
  "   normal {bumps 0.05 scale 3}",
  "   finish {phong 0.8 reflection 0.1}",
  "}",
  "",
  "object {TeaCup rotate y*50 translate <4,0,-14>}",
  NULL
} ;

char *povdemo_ini [] =
{
  "Width=320",
  "Height=240",
  "Pause_when_Done=off",
  "Bounding_Threshold=3",
  "Output_to_File=Off",
  "Pause_When_Done=Off",
  "Test_Abort=Off",
  "Verbose=Off",
  "Display=On",
  "Antialias=Off",
  NULL
} ;

