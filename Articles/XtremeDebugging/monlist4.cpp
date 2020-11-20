
// INCLUDES /////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // make sure certain headers are included correctly
#include <windows.h>         // include the standard windows stuff
#include <windowsx.h>        // include the 32 bit stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 
#include <io.h>
#include "mono.h"

// MAIN //////////////////////////////////////////////////////////////////////////

void main(void)
{
mono_print debug;	// create an instance of a debugger port

int x,y;			// used to get the cursor position
char buffer[80];	// used to build up strings

// clear the debugging display
debug.clear();

// demo of simple methods

// demo of draw
debug.draw("This is the draw method",0,0,MONO_BRIGHT);
Sleep(2000);

// demo of set_cursor
debug.set_cursor(10,10);
debug.print("Using the print method after setting the cursor position to 10,10");
Sleep(2000);

// demo of set_style
debug.set_style(MONO_DARK);
debug.print("\nUsing the print method again with a differnt style");
Sleep(2000);

// demo of get_cursor
debug.get_cursor(x,y);
sprintf(buffer,"\nCursor at (%d,%d)",x,y);
debug.print(buffer);
Sleep(2000);

// demo of disable method
debug.disable();
debug.print("\nYou can't see this.");
Sleep(2000);

// demo of enable method
debug.enable();
debug.print("\n");
debug.print("\n");
debug.print("\nNow you can see me, but not for long...\n");
Sleep(2000);

// demo of set_style
debug.set_style(MONO_BRIGHT);
debug.print("\nJust changed style to bright!");
Sleep(2000);

// demo of clear
debug.clear();
Sleep(2000);

// demo of scrolling
while(!kbhit())
	{
	debug.print("hit any key to exit          ");
	Sleep(50);
	} // end while

// demo of clear
debug.clear();

} // end main
