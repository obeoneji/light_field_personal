#include "colors.inc"
#include "camera-context.inc" //common file containing object definitions for the camera demos


           
        
// Persistence of Vision Ray Tracer Scene Description File
// File: ?.pov
// Vers: 3.6
// Desc: Checkered Floor Example
// Date: mm/dd/yy
// Auth: ?
//

#version 3.6;

#include "colors.inc"

global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

// ----------------------------------------

camera {
        light_field_slabs    
        location <5,5,5>    
        camera_num_in_height 1
        camera_num_in_width 8
        slabs_length 1
        slabs_length_ex 1
        slabs_height 0
        slabs_height_ex 0
        } 

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [0.7 rgb <0.0,0.1,0.8>]
    }
  }
}

light_source {
  <0, 0, 0>            // light's position (translated below)
  color rgb <1, 1, 1>  // light's color
  translate <-30, 30, -30>
}

// ----------------------------------------

plane {               // checkered floor
  z, 1
  texture
  {
    pigment {
      checker
      color rgb 1
      color green 1
      scale 5
    }
    finish{
      diffuse 0.8
      ambient 0.1
    }
  }
}     


// An infinite planar surface
// plane {<A, B, C>, D } where: A*x + B*y + C*z = D
plane {
  y, -1 // <X Y Z> unit surface normal, vector points "away from surface"
  -100.0 // distance from the origin in the direction of the surface normal
  texture
  {
    pigment {
      checker
      color rgb 1
      color red 1
      scale 10
    }
    finish{
      diffuse 0.8
      ambient 0.1
    }
  }
}  


plane {
  x, 1 // <X Y Z> unit surface normal, vector points "away from surface"
  -100.0 // distance from the origin in the direction of the surface normal
  texture
  {
    pigment {
      checker
      color rgb 1
      color blue 1
      scale 20
    }
    finish{
      diffuse 0.8
      ambient 0.1
    }
  }
}   
          
          
          // create a regular point light source
light_source {
  0*x                  // light's position (translated below)
  color rgb <1,1,1>    // light's color
  translate <1, 1, 1>
}






