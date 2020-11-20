
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
// variables used to watch, one of each type

char			c	=0;
UCHAR			uc	=0;
short			s	=0;
USHORT			us	=0;
int				i	=0;
unsigned int	ui	=0;
char			string[]="Game Developer";
float			f	=(float)0.0;
int				*ptr= (int *)&i;

mono_print debug;	// create an instance of a debugger port

// clear the debugging display
debug.clear();

// first create watches, note that we simply send the address of each variable cast to void *
	
debug.add_watch("char c",(void *)&c,MONO_WATCH_CHAR					,0,0,16);	
debug.add_watch("unsigned char uc",(void *)&uc,MONO_WATCH_UCHAR		,0,1,16);
debug.add_watch("short s",(void *)&s,MONO_WATCH_SHORT				,0,2,16);
debug.add_watch("unsigned short us",(void *)&us,MONO_WATCH_USHORT	,0,3,16);
debug.add_watch("int i",(void *)&i,MONO_WATCH_INT					,0,4,16);
debug.add_watch("unsigned int ui",(void *)&ui,MONO_WATCH_UINT		,0,5,16);
debug.add_watch("string ASCII",(void *)string,MONO_WATCH_STRING		,0,6,16);
debug.add_watch("string HEX",(void *)string,MONO_WATCH_STRING_HEX	,0,7,16);
debug.add_watch("float f",(void *)&f,MONO_WATCH_FLOAT				,0,8,16);
debug.add_watch("ptr",(void *)&ptr,MONO_WATCH_PTR					,0,9,16);

// enter main loop

while(!kbhit())
	{
	// update the watch display with current data 	
	debug.update_watches();

	// update watch variables each cycle

	c++;
	uc++;
	s++;
	us++;
	i++;
	ui++;
	f=f+(float)0.1;
	ptr++;
	
	// wait a sec
	Sleep(200);

	} // while

} // end main
