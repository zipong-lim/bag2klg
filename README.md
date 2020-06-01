# bag2klg
Quick and dirty implementation for .bag to .klg conversion.

#### requirements
> openGL    
> librealsense2    
> GLUT    

#### tested on ubuntu 18.04
> 1) mkdir build    
> 2) cd build    
> 3) cmake ..   
> 4) add line to end of ./build/CMakeFiles/bag2klg.dir/link.txt   
> **-lGL -lGLU -lglut**   
> 5) make   
> 6) put .bag file in ./build    
> 7) run ./bag2klg    
> 8) output.klg will be located in ./build    

#### TODO
> currently using frame number as timestamp    
> unable to get timestamp directly from frames using librealsense
