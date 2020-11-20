// TMAPDEMO.CPP - DirectX 5.0 Texture Mapping Demo
// Compile as Win32 Application along with TMAPPER.CPP, DDRAW.LIB, and WINMM.LIB
// Press ESC to exit, you may have to hold it!

// INCLUDES ///////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // make sure certain headers are included correctly

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream.h>  // include important C/C++ stuff
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
#include "tmapper.h"   // need the class defs
#include <ddraw.h>     // include directdraw

// DEFINES ////////////////////////////////////////////////////////////////////

#define WINDOW_CLASS_NAME "WINDOW_CLASS" // this is the name of the window class

// defines for screen parameters
#define SCREEN_WIDTH     640         // the width of the viewing surface
#define SCREEN_HEIGHT    480         // the height of the viewing surface
#define SCREEN_BPP       8           // the bits per pixel
#define MAX_COLORS       256         // the maximum number of colors

// graphics defines
#define BITMAP_ID            0x4D42 // universal id for a bitmap

// TYPES /////////////////////////////////////////////////////////////////////

// container structure for bitmaps .BMP file
typedef struct BITMAP_FILE_TAG
        {
        BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
        BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
        PALETTEENTRY     palette[256];      // we will store the palette here
        UCHAR            *buffer;           // this is a pointer to the data

        } BITMAP_FILE, *BITMAP_FILE_PTR;

// MACROS /////////////////////////////////////////////////////////////////////

// this builds a 16 bit color value
#define _RGB16BIT(r,g,b) ((b%32) + ((g%32) << 5) + ((r%32) << 10))

// these query the keyboard in real-time
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// PROTOTYPES ////////////////////////////////////////////////////////////////

int DD_Init(HWND hwnd);
int DD_Shutdown(void);

void Game_Init(void);
void Game_Main(void);
void Game_Shutdown(void);

// bitmap file functions
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename);
int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);
int Set_Palette(LPPALETTEENTRY set_palette);
int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height);
void Draw_Textured_Triangle(void *vface,UCHAR *dest_buffer,int mem_pitch);

// DIRECTDRAW GLOBALS ////////////////////////////////////////////////////////

LPDIRECTDRAW         lpdd         = NULL;		// dd object
LPDIRECTDRAWSURFACE  lpddsprimary = NULL;		// dd primary surface
LPDIRECTDRAWPALETTE  lpddpal      = NULL;		// a pointer to the created dd palette
PALETTEENTRY         palette[256];		        // holds the shadow palette entries
DDSURFACEDESC        ddsd;						// a direct draw surface description struct
DDSCAPS              ddscaps;					// a direct draw surface capabilities struct
HRESULT              ddrval;					// result back from dd calls
HWND                 main_window_handle = NULL; // used to store the window handle
UCHAR                *video_buffer = NULL;      // pointer to video ram
int                  mem_pitch;                 // horizontal memory pitch

BITMAP_FILE          bitmap;                    // used to hold bitmap

// these defined the general clipping rectangle
int poly_clip_min_x = 0,
    poly_clip_max_x = SCREEN_WIDTH-1,
    poly_clip_min_y = 0,
    poly_clip_max_y = SCREEN_HEIGHT-1;  

// GAME GLOBALS GO HERE ///////////////////////////////////////////////////// 

// DIRECT X FUNCTIONS /////////////////////////////////////////////////////////

int DD_Init(HWND hwnd)
{
// this function is responsible for initializing direct draw, it creates a
// primary surface 

int  index;     // looping index

// now that the windows portion is complete, start up direct draw
if (DirectDrawCreate(NULL,&lpdd,NULL)!=DD_OK)
   {
   // shutdown any other dd objects and kill window
   DD_Shutdown();
   return(0);
   } // end if

// now set the coop level to exclusive and set for full screen and mode x
if (lpdd->SetCooperativeLevel(hwnd, DDSCL_NORMAL)!=DD_OK)
   {
   // shutdown any other dd objects and kill window
   DD_Shutdown();
   return(0);
   } // end if

// now set the coop level to exclusive and set for full screen and mode x
if (lpdd->SetCooperativeLevel(hwnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE |
                              DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX)!=DD_OK)
   {
   // shutdown any other dd objects and kill window
   DD_Shutdown();
   return(0);
   } // end if

// now set the display mode
if (lpdd->SetDisplayMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP)!=DD_OK)
   {
   // shutdown any other dd objects and kill window
   DD_Shutdown();
   return(0);
   } // end if

// Create the primary surface
ddsd.dwSize            = sizeof(ddsd);
ddsd.dwFlags           = DDSD_CAPS;
ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;

if (lpdd->CreateSurface(&ddsd,&lpddsprimary,NULL)!=DD_OK)
   {
   // shutdown any other dd objects and kill window
   DD_Shutdown();
   return(0);
   } // end if

// create the palette and attach it to the primary surface

// clear all the palette entries to RGB 0,0,0
memset(palette,0,256*sizeof(PALETTEENTRY));

// set all of the flags to the correct value
for (index=0; index<256; index++)
	{
	// create the GRAY/RED/GREEN/BLUE palette, 64 shades of each
	if ((index / 64)==0) 
		palette[index].peRed = palette[index].peGreen = palette[index].peBlue  = index*4;
    else
	if ((index / 64)==1) 
		palette[index].peRed  = (index%64)*4;
	else
	if ((index / 64)==2) 
		palette[index].peGreen = (index%64)*4;
	else
	if ((index / 64)==3) 
		palette[index].peBlue  = (index%64)*4;

	// set the no collapse flag
	palette[index].peFlags = PC_NOCOLLAPSE;
	
	} // end for index

// now create the palette object, note that it is a member of the dd object itself
if (lpdd->CreatePalette((DDPCAPS_8BIT | DDPCAPS_INITIALIZE),palette,&lpddpal,NULL)!=DD_OK)
	{
	// shutdown any other dd objects and kill window
	DD_Shutdown();
	return(0);
	} // end if

// now attach the palette to the primary surface
lpddsprimary->SetPalette(lpddpal);

// return success if we got this far
return(1);

} // end DD_Init

///////////////////////////////////////////////////////////////////////////////

int DD_Shutdown(void)
{
// this function tests for dd components that have been created and releases
// them back to the operating system

// test if the dd object exists
if (lpdd)
   {
   // test if there is a primary surface
   if (lpddsprimary)
      {
      // release the memory and set pointer to NULL
      lpddsprimary->Release();
      lpddsprimary = NULL;
      } // end if

    // now release the dd object itself
    lpdd->Release();
    lpdd = NULL;

    // return success
    return(1);

    } // end if
else
    return(0);

} // end DD_Shutdown

// GENERAL GRAPHICS HERE /////////////////////////////////////////////////////

int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
{
// this function opens a bitmap file and loads the data into bitmap

int file_handle,  // the file handle
    index;        // looping index

UCHAR *temp_buffer = NULL; // used to convert 24 bit images to 16 bit
OFSTRUCT file_data;        // the file data information

// open the file if it exists
if ((file_handle = OpenFile(filename,&file_data,OF_READ))==-1)
   return(0);

// now load the bitmap file header
_lread(file_handle, &bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER));

// test if this is a bitmap file
if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
   {
   // close the file
   _lclose(file_handle);

   // return error
   return(0);
   } // end if

// now we know this is a bitmap, so read in all the sections

// first the bitmap infoheader

// now load the bitmap file header
_lread(file_handle, &bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER));

// now load the color palette if there is one
if (bitmap->bitmapinfoheader.biBitCount == 8)
   {
   _lread(file_handle, &bitmap->palette,256*sizeof(PALETTEENTRY));

   // now set all the flags in the palette correctly and fix the reversed 
   // BGR RGBQUAD data format
   for (index=0; index < 256; index++)
       {
       // reverse the red and green fields
       int temp_color = bitmap->palette[index].peRed;
       bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
       bitmap->palette[index].peBlue = temp_color;
       
       // always set the flags word to this
       bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
       } // end for index

    } // end if

// finally the image data itself
_lseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

// now read in the image, if the image is 8 or 16 bit then simply read it
// but if its 24 bit then read it into a temporary area and then convert
// it to a 16 bit image

if (bitmap->bitmapinfoheader.biBitCount==8 || bitmap->bitmapinfoheader.biBitCount==16)
   {
   // delete the last image if there was one
   if (bitmap->buffer)
       free(bitmap->buffer);

   // allocate the memory for the image
   if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
      {
      // close the file
      _lclose(file_handle);

      // return error
      return(0);
      } // end if

   // now read it in
   _lread(file_handle,bitmap->buffer,bitmap->bitmapinfoheader.biSizeImage);

   } // end if
else
   {
   // this must be a 24 bit image, load it in and convert it to 16 bit
//   printf("\nconverting 24 bit image...");

   // allocate temporary buffer
   if (!(temp_buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
      {
      // close the file
      _lclose(file_handle);

      // return error
      return(0);
      } // end if
   
   // allocate final 16 bit storage buffer
   if (!(bitmap->buffer=(UCHAR *)malloc(2*bitmap->bitmapinfoheader.biWidth * 
                                        bitmap->bitmapinfoheader.biHeight)))
      {
      // close the file
      _lclose(file_handle);

      // release working buffer
      free(temp_buffer);

      // return error
      return(0);
      } // end if

   // now read it in
   _lread(file_handle,temp_buffer,bitmap->bitmapinfoheader.biSizeImage);

   // now convert each 24 bit RGB value into a 16 bit value
   for (index=0; index<bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
       {
       // extract RGB components (in BGR order), note the scaling
       UCHAR blue  = (temp_buffer[index*3 + 0] >> 3),
             green = (temp_buffer[index*3 + 1] >> 3),
             red   = (temp_buffer[index*3 + 2] >> 3);

       // build up 16 bit color word
       USHORT color = _RGB16BIT(red,green,blue);

       // write color to buffer
       ((USHORT *)bitmap->buffer)[index] = color;

       } // end for index

   // finally write out the correct number of bits
   bitmap->bitmapinfoheader.biBitCount=16;

   } // end if


// close the file
_lclose(file_handle);

// flip the bitmap
Flip_Bitmap(bitmap->buffer, 
            bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8), 
            bitmap->bitmapinfoheader.biHeight);

// return success
return(1);

} // end Load_Bitmap_File

///////////////////////////////////////////////////////////

int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
// this function releases all memory associated with "bitmap"
if (bitmap->buffer)
   {
   // release memory
   free(bitmap->buffer);

   // reset pointer
   bitmap->buffer = NULL;

   } // end if

// return success
return(1);

} // end Unload_Bitmap_File

////////////////////////////////////////////////////////////////////////////////

int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
{
// this function is used to flip upside down .BMP images

UCHAR *buffer; // used to perform the image processing
int index;     // looping index

// allocate the temporary buffer
if (!(buffer = (UCHAR *)malloc(bytes_per_line*height)))
   return(0);

// copy image to work area
memcpy(buffer,image,bytes_per_line*height);

// flip vertically
for (index=0; index < height; index++)
    memcpy(&image[((height-1) - index)*bytes_per_line],
           &buffer[index*bytes_per_line], bytes_per_line);

// release the memory
free(buffer);

// return success
return(1);

} // end Flip_Bitmap

///////////////////////////////////////////////////////////////////////////////

int Set_Palette(LPPALETTEENTRY set_palette)
{
// this function writes the sent palette

// first save the new palette in shadow
memcpy(palette, set_palette,256*sizeof(PALETTEENTRY));

// now set the new palette
lpddpal->SetEntries(0,0,256,palette);

// return success
return(1);
} // end Set_Palette


// WINDOWS CALLBACK FUNCTION //////////////////////////////////////////////////		             

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
// this is the main message handler of the system

HDC			hdc;		// handle to graphics context
PAINTSTRUCT ps;			// used to hold the paint info

// what is the message?

switch(msg)
	{	
	case WM_CREATE:
		{
		// do windows inits here
		return(0);
		} break;

	case WM_PAINT:
		{
		// this message occurs when your window needs repainting
		hdc = BeginPaint(hwnd,&ps);	 
		EndPaint(hdc,&ps);
		
		return(0);
   		} break;

	case WM_DESTROY:
		{
		// this message is sent when your window is destroyed
		PostQuitMessage(0);
		return(0);
		} break;

	default:break;

    } // end switch

// let windows process any messages that we didn't take care of 
return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
WNDCLASSEX		winclass;	// this holds the windows class info
HWND			hwnd;		// this holds the handle of our new window
MSG				msg;		// this holds a generic message

// first fill in the window class stucture

winclass.cbSize			= sizeof(WNDCLASSEX);
winclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
winclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
winclass.hbrBackground	= GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= NULL;
winclass.lpszClassName	= WINDOW_CLASS_NAME;

// register the window class
if (!RegisterClassEx(&winclass))
	return(0);

// create the window
if (!(hwnd = CreateWindowEx(0, 
							WINDOW_CLASS_NAME,					// class
							"Texture Mapping Demo!",			// title
							WS_OVERLAPPED | WS_VISIBLE,
					 		0,0,								// x,y
							320, // GetSystemMetrics(SM_CXSCREEN),
							240, // GetSystemMetrics(SM_CYSCREEN),				
							NULL,								// parent
							NULL,								// menu
							hinstance,							// instance
							NULL)))								// creation parms
return(0);

// hide the mouse cursor
ShowCursor(0);

// save the window handle
main_window_handle = hwnd;

// initialize direct draw
if (!DD_Init(hwnd))
   {
   DestroyWindow(hwnd);
   return(0);
   } // end if

// initialize game
Game_Init();

// enter main event loop
while(1)
	{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
		// test if this is a quit
        if (msg.message == WM_QUIT)
           break;
	
		// translate any accelerator keys
		TranslateMessage(&msg);

		// send the message to the window proc
		DispatchMessage(&msg);
		} // end if
	else
		{
		// do asynchronous processing here

		// acquire pointer to video ram, note it is always linear
		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		lpddsprimary->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);

        // get video pointer
		video_buffer = (UCHAR *)ddsd.lpSurface;

        // get video pitch, should be SCREEN_WIDTH, but possibly not!
        mem_pitch = (int)ddsd.lPitch;

		// call main logic module
		Game_Main();

		// release pointer to video ram
		lpddsprimary->Unlock(ddsd.lpSurface);
		
		} // end else

	} // end while

// shut down direct draw
DD_Shutdown();

// shutdown game
Game_Shutdown();

// return to Windows
return(msg.wParam);

} // end WinMain

// HERE ARE OUR GAME CONSOLE FUNCTIONS ///////////////////////////////////////////////////////

void Game_Init(void)
{
// do any initialization here

// load in the bitmap file here, 64x64 8 bit
Load_Bitmap_File(&bitmap, "texture.bmp");

// set new palette
Set_Palette(bitmap.palette);

} // end Game_Init

/////////////////////////////////////////////////////////////////////////////////////////////

void Game_Shutdown(void)
{
// cleanup and release all resources here

// release bitmap memory
Unload_Bitmap_File(&bitmap);

} // end Game_Shutdown

/////////////////////////////////////////////////////////////////////////////////////////////

void Game_Main(void)
{
// process your game logic and draw next frame
// this function must exit each frame and return back to windows
// also this function is responsible for controlling the frame rate
// therefore, if you want the game to run at 60fps, then the function should
// take 1/60th of a second before returning

// on entry video_buffer and mem_pitch are valid

// get input, note how keyboard is accessed with constants "VK_"
// these are defined in "winuser.h" are basically the scan codes
// for letters just use capital ASCII codes

FACE3D face; // the triangle object

// test for exit
if (KEY_DOWN(VK_ESCAPE))
	PostMessage(main_window_handle,WM_DESTROY,0,0); 

// draw 100 polys a frame
for (int polys=0; polys < 100; polys++)
{
// set up the vertices
face.tlist[0].x = rand()%SCREEN_WIDTH;
face.tlist[0].y = rand()%SCREEN_HEIGHT;
face.tlist[0].u = 0;
face.tlist[0].v = 0;

face.tlist[1].x = rand()%SCREEN_WIDTH;
face.tlist[1].y = rand()%SCREEN_HEIGHT;
face.tlist[1].u = 0;
face.tlist[1].v = 63;

face.tlist[2].x = rand()%SCREEN_WIDTH;
face.tlist[2].y = rand()%SCREEN_HEIGHT;
face.tlist[2].u = 63;
face.tlist[2].v = 63;

// assign the texture to your 64x64 texture map buffer
face.texture = bitmap.buffer;

// set the clipping coords of the desired 2D viewport
poly_clip_min_x = 0; 
poly_clip_max_x = SCREEN_WIDTH-1;
poly_clip_min_y = 0;
poly_clip_max_y = SCREEN_HEIGHT-1;

// then draw the triangle to the rendering buffer
Draw_Textured_Triangle((void *)&face, video_buffer, mem_pitch);

} // end for polys

// use sleep to slow system down to 60ish fps
Sleep(15);

// return and let main event loop work a bit

} // end Game_Main

