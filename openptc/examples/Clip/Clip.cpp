//
// Clip example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <math.h>
#include <stdlib.h>


inline int random(int max)
{
    // random number
    return rand() % max;
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create console
        Console console;
        
        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);
 
        // open the console
        console.open("Clip example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);
        
        // calculate clip coordinates
        const int x1 = console.width() / 4;
        const int y1 = console.height() / 4;
        const int x2 = console.width() - x1;
        const int y2 = console.height() - y1;

        // setup clip area
        Area area(x1,y1,x2,y2);

        // set clip area
        console.clip(area);

        // loop until a key is pressed
        while (!console.key())
        {
            // lock surface pixels
            int32 *pixels = (int32*) surface.lock();

            // get surface dimensions
            const int width  = surface.width();
            const int height = surface.height();

            // draw random pixels
            for (int i=0; i<100; i++)
            {
                // get random position
                const int x = random(width);
                const int y = random(height);

                // get random color
                const int r = random(256);
                const int g = random(256);
                const int b = random(256);

                // draw color [r,g,b] at position [x,y]
                pixels[x+y*width] = (r<<16) + (g<<8) + b;
            }
        
            // unlock surface
            surface.unlock();

            // copy to console
            surface.copy(console);
            
            // update console
            console.update();
        }

        // close console
        console.close();
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }

    // exit
    return 0;
}
