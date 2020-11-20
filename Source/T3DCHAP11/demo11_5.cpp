// DEMO11_5.CPP - Creates a single thread that prints simultaneously 
// while the Primary thread prints.

// INCLUDES ///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // make sure certain headers are included correctly

#include <windows.h>         // include the standard windows stuff
#include <windowsx.h>        // include the 32 bit stuff
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

// DEFINES ////////////////////////////////////////////////////////////////////////////////


// PROTOTYPES /////////////////////////////////////////////////////////////////////////////

DWORD WINAPI Printer_Thread(LPVOID data);

// GLOBALS ////////////////////////////////////////////////////////////////////////////////


// FUNCTIONS //////////////////////////////////////////////////////////////////////////////

DWORD WINAPI Printer_Thread(LPVOID data)
{
// this thread function simply prints out data 25 times with a slight delay

for (int index=0; index<25; index++)
	{
	printf("%d ",data); // output a single character
	Sleep(100);         // sleep a little to slow things down
	} // end for index


// just return the data sent to the thread function

return((DWORD)data);

} // end Printer_Thread

// MAIN //////////////////////////////////////////////////////////////////////////////////

void main(void)
{

HANDLE thread_handle;  // this is the handle to the thread
DWORD  thread_id;      // this is the id of the thread


// start with a blank line

printf("\nStarting threads...\n");

// create the thread, IRL we would check for errors

thread_handle = CreateThread(NULL,           // default security
		                     0,				 // default stack 
			   			     Printer_Thread, // use this thread function
							 (LPVOID)1,		 // user data sent to thread
							 0,				 // creation flags, 0=start now.
							 &thread_id);	 // send id back in this var

// now enter into printing loop, make sure this takes longer than thread,
// so thread finish's first

for (int index=0; index<50; index++)
	{
	printf("2 ");
	Sleep(100);

	} // end for index

// at this point the thread should be dead
CloseHandle(thread_handle);

// end with a blank line

printf("\nAll threads terminated.\n");

} // end main
