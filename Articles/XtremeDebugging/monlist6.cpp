
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
int done	=	0;					// exit flag
int ship_x	=	MONO_COLUMNS/2;		// initial position of debugger ship

mono_print debug;					// create an instance of a debugger port

// clear the debugging display
debug.clear();

// show instructions
debug.print("Controls:\n\n<a> to move left\n<s> to move right.\n\nTo exit press <ESC>.");
Sleep(5000);

// add variable to debugger display
debug.add_watch("ship_x",&ship_x,MONO_WATCH_INT,0,0,8);

// loop until user hits keys other that 'a' and 's'
while(!done)
	{
	// draw display
	debug.draw(".",rand()%MONO_COLUMNS,24,((rand()%2) ? MONO_BRIGHT : MONO_DARK));
	debug.scroll(1);

	// draw ship
	debug.draw(" |-()-| ",ship_x,0,MONO_BRIGHT);

	// move ship
	if (kbhit())
		{
		switch(getch())
			{
			case 'a':if ((ship_x-=2)<0) ship_x=0; break;
			case 's':if ((ship_x+=2)>MONO_COLUMNS-7) ship_x=MONO_COLUMNS-7; break;
			
			default: done=1; break;

			} // end switch
		} // end if

	// update debug display
	debug.update_watches();
	
	// wait a sec
	Sleep(100);

	} // end while

// clear the debugging display
debug.clear();

} // end main
