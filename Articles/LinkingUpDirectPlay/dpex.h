#define NAME "dpex"
#define TITLE "DirectPlay Example"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <mmsystem.h>

#include "resource.h"
//#include "ddutil.h"
//#include "ddtools.h"
//#include "utility.h"
//#include "sprites.h"
#include "dptools.h"
//#include "gameutil.h"

// Externals


// Keyboard commands

#define KEY_THRUST 0x0001 // Thrust with the "5" key
#define KEY_LEFT   0x0004 // Turn left with left arrow
#define KEY_RIGHT  0x0008 // Turn right with right arrow
#define KEY_FIRE   0x0020 // Fire with space bar

#define SYNCKEYS   0x000D

// Game constants

#define SCREEN_X    639
#define SCREEN_Y    479
#define SHIP_X      ( SCREEN_X - 32 )
#define SHIP_Y      ( SCREEN_Y - 32 )
#define SHOT_X      ( SCREEN_X - 3 )
#define SHOT_Y      ( SCREEN_Y - 3 )

#define SAMPLE_RATE		25		// 40 frames per second (ms)
#define SHOTLIFE		2000	// lifetime of shote (ms)
#define SHOTFREQ		500		// time between shots (ms)
#define DISABLEDTIME	4000	// time disabled after hit
#define SYNC_INTERVAL	1000	// time between sync messages
#define INPUT_RATE		25		// 40 times per second (ms)



// Prototypes

//BOOL DDInit( void );


