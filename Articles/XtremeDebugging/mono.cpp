// MONO.CPP - C++ METHOD IMPLEMENTATION FOR MONOCHROME PRINTING ENGINE ///////////

// INCLUDES //////////////////////////////////////////////////////////////////////

#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include "mono.h" // include the monochrome header 

// CLASS METHODS IMPLEMENTATION //////////////////////////////////////////////////

mono_print::mono_print(void)
{
// the constructor simply initializes the system and sets the cursor to the upper
// left hand corner

cx = cy			= 0;					// position cursor at (0,0)
style			= MONO_BRIGHT;			// set style to bright text
output_enable	= 1;					// enable the output to mono monitor
mono_video		= (USHORT *)0xB0000;	// pointer to the monochrome video buffer
num_watches		= 0;					// set number of watches to 0

} // end mono_print

//////////////////////////////////////////////////////////////////////////////////
	
void mono_print::print(char *string)
{
// this function is similar to printf in that it will scroll, wrap, and can interpret
// newlines, note: we probably could have used the draw function, but the logic needed to control
// it from this function would be as long as copying the draw function as changing it

USHORT char_attr,   // the total character attribute
       char_part,   // the ascii part of the character low byte
       attr_part;   // the color part of the character high byte

int index;				// looping index
	
// only print if gate is enabled

if (!output_enable) return;

// enter main loop and print each character

for (index=0; index<(int)strlen(string); index++)
    {
    // extract the character and attribute

    char_part = (USHORT)string[index];
    attr_part = ((USHORT)style) << 8;

	// merge character and attribute

    char_attr = (char_part | attr_part);

	// test if this is a control character?
	// for now only test \n = 0x0A

	if (char_part==0x0A)
		{
		// reset cursor to left edge
		cx=0;

		// advance cursor down a line and test for scroll
		if (++cy>=25)
			{
			scroll(1);
			cy=24;
 			} // end if
			
		} // end if
	else
		{
		// display character

		mono_video[cy*(MONO_BYTES_PER_LINE/2) + cx] = char_attr;

		// update cursor position

		if (++cx>=MONO_COLUMNS)
			{
			cx=0;
			// test for vertical scroll
			if (++cy>=25)
				{
				scroll(1);
				cy=24;
	 			} // end if
			} // end if
		} // end else

    } // end for index

} // end print
	
//////////////////////////////////////////////////////////////////////////////////

void mono_print::draw(char *string,int x,int y,int style)
{
// this function is lower level that print, it simply prints the sent string at
// the sent position and color and doesn't update anything
// note that the function has simple clipping

USHORT char_attr,   // the total character attribute
       char_part,   // the ascii part of the character low byte
       attr_part;   // the color part of the character high byte

int index,				// looping index
	length,				// length of sent string
	offset=0;			// used in clipping algorithm

char temp_string[256];	// holds working copy of string

// only print if gate is enabled

if (!output_enable) return;

// do trivial rejections first

length = strlen(string);

if (y<0 || y>(MONO_ROWS-1) || x>(MONO_COLUMNS-1) || (x<=-length)) return;

// make working copy of string

strcpy(temp_string,string);

// now test if string is partially clipped on X axis

if (x<0) // test left extent
	{
	// set offset into string
	offset = -x;

	// reset x
	x=0;

	} // end if

// note that we test both cases, since the string may be longer than the width of display

if (x+length>MONO_COLUMNS) // test right extent
	length = MONO_COLUMNS-x;

// enter main loop and print each character

for (index=0; index<length; index++)
    {
    // extract the character and attribute

    char_part = (USHORT)temp_string[index+offset];
    attr_part = ((USHORT)style) << 8;

    // merge character and attribute

    char_attr = (char_part | attr_part);

    // display character

    mono_video[y*(MONO_BYTES_PER_LINE/2)  + x+index] = char_attr;

    } // end for index

} // end draw

//////////////////////////////////////////////////////////////////////////////////

void mono_print::set_cursor(int x, int y)		
{
// this function sets the position of the printing cursor

// check if x position is valid

if (x<0) cx=0;
else
if (x>=MONO_COLUMNS) cx=MONO_COLUMNS-1;
else
	cx = x;

// check if y position is valid

if (y<0) cy=0;
else
if (y>=MONO_ROWS) cy=MONO_ROWS-1;
else
	cy = y;

} // end set_cursor

//////////////////////////////////////////////////////////////////////////////////
	
void mono_print::get_cursor(int &x, int &y)		
{
// this function retrieves the position of the cursor
x = cx;
y = cy;

} // end get_cursor

//////////////////////////////////////////////////////////////////////////////////

void mono_print::set_style(int new_style)		
{
// this function sets the printing style

// make sure the style is somewhat reasonable

if (style<0 || style>255)
	style = MONO_BRIGHT;
else
	style = new_style;

} // end set_style

//////////////////////////////////////////////////////////////////////////////////

void mono_print::enable()						
{
// this function sets the output enable gate so that output is sent to the display
output_enable = 1;

} // end enable

//////////////////////////////////////////////////////////////////////////////////

void mono_print::disable()						
{
// this function is used to disable the output gate to the monitor
output_enable = 0;

} // end disable

//////////////////////////////////////////////////////////////////////////////////

void mono_print::scroll(int num_lines)			
{
// this function scrolls the display upward the requested number of lines
// only print if gate is enabled
// note that mono_video is a USHORT pointer!

if (!output_enable) return;

// the display is 25 lines long, all we need to do is move the last 24 up one
// line and blank out the last line

while (num_lines-->0)
	{
	// scroll the last 24 lines up, use memmove since dest & source overlap
	memmove((void *)mono_video, (void *)(mono_video+MONO_BYTES_PER_LINE/2),24*MONO_BYTES_PER_LINE);

	// now blank out the last line
	memset((void *)(mono_video+24*MONO_BYTES_PER_LINE/2),0,MONO_BYTES_PER_LINE);

	} // end while

} // end scroll

//////////////////////////////////////////////////////////////////////////////////

void mono_print::clear()
{
// this function clears the monchrome display

// only print if gate is enabled
if (!output_enable) return;

// clear the display
memset((void *)mono_video,0,25*MONO_BYTES_PER_LINE);

//reset the cursor
cx = cy	= 0;		

} // end clear

//////////////////////////////////////////////////////////////////////////////////

void mono_print::delete_watches(void)
{
// this deletes all the watches

num_watches = 0;
memset(watch, 0, sizeof(mono_watch)*MONO_MAX_WATCHES);

} // end delete_watches
	
//////////////////////////////////////////////////////////////////////////////////

int mono_print::add_watch(char *name,	// name or symbol for watch, up to 15 chars
						  void *addr,	// address of variable to watch
						  int type,		// type of variable
					  	  int x,		// position of watch on display
						  int y,
						  int field_width) // size of ouput field, for blank padding
{
// this function creates a watch variable and enters it into the watch list

if (num_watches>=MONO_MAX_WATCHES)
	return(-1);

// insert this watch at current location

strcpy(watch[num_watches].name, name);
watch[num_watches].data			= addr;
watch[num_watches].type			= type;
watch[num_watches].x			= x;
watch[num_watches].y			= y;
watch[num_watches].field_width	= field_width;

// increment number of watches

num_watches++;

// return the position of this watch
return(num_watches-1);

} // end add_watch

//////////////////////////////////////////////////////////////////////////////////

void mono_print::update_watches(void)
{
// this function updates the visual display with the current values of all
// the watches

int index;				// looping var
char temp_string[256];	// temporary working string

// only print if gate is enabled
if (!output_enable) return;

// loop thru all the watch variables and display them

for (index=0; index<num_watches; index++)
	{
	// first generate display string based on type of variable being watched
	
	switch (watch[index].type)
		{
		case MONO_WATCH_CHAR:
			{
			sprintf(temp_string,"%s=%c",watch[index].name,*((char *)(watch[index].data)));  
			} break;

		case MONO_WATCH_UCHAR:
			{
			sprintf(temp_string,"%s=%u",watch[index].name,*((UCHAR *)(watch[index].data)));  
			} break;

		case MONO_WATCH_SHORT:			
			{
			sprintf(temp_string,"%s=%d",watch[index].name,*((short *)(watch[index].data)));  
			} break;

		case MONO_WATCH_USHORT:			
			{
			sprintf(temp_string,"%s=%u",watch[index].name,*((USHORT *)(watch[index].data)));  
			} break;

		case MONO_WATCH_INT:			
			{
			sprintf(temp_string,"%s=%d",watch[index].name,*((int *)(watch[index].data)));  
			} break;

		case MONO_WATCH_UINT:				
			{
			sprintf(temp_string,"%s=%u",watch[index].name,*((unsigned int *)(watch[index].data)));  
			} break;

		case MONO_WATCH_STRING:
			{
			sprintf(temp_string,"%s=%s",watch[index].name,(char *)(watch[index].data));  
			} break;
	
		case MONO_WATCH_STRING_HEX:
			{
			sprintf(temp_string,"%s=",watch[index].name);  
			
			// merge in hex values for characters
			for (int bc=0; bc<(int)strlen((char *)watch[index].data); bc++)
				{
				char hexvalue[8];
				sprintf(hexvalue,"%X,",*(bc+(UCHAR *)watch[index].data));
				strcat(temp_string,hexvalue);
				} // end for bc

			} break;

		case MONO_WATCH_FLOAT:			
			{
			sprintf(temp_string,"%s=%f",watch[index].name,*((float *)(watch[index].data)));  
			} break;

		case MONO_WATCH_PTR:			
			{
			sprintf(temp_string,"%s=%X",watch[index].name,*((unsigned int *)watch[index].data));  
			} break;

		default:break;

		} // end switch
	
	// fill in tail of string with blanks
	strncat(temp_string,"                                                                ",
			watch[index].field_width - (strlen(temp_string) - strlen(watch[index].name) - 1));

	// print out the watch
	draw(temp_string, watch[index].x, watch[index].y, MONO_BRIGHT);

	} // end for index

} // end update_watches

//////////////////////////////////////////////////////////////////////////////////
