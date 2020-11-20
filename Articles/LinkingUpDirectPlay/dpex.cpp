#include "dpex.h"

#define WINDOW_CLASS_NAME "WINCLASS1"
#define WINDOW_WIDTH  400
#define WINDOW_HEIGHT 100

char					buffer[80];					//to hold the print messages
BOOL					g_bFullScreen = FALSE;
BOOL					g_bAllowWindowed = TRUE;
BOOL                    g_bReInitialize = FALSE;
int						send_count=0;				//counter we'll be sending
int						received_count=0;			//store counter we recieve
int						length = 0;					//of message



long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
    case WM_ACTIVATEAPP:
        break;

    case WM_CREATE:
        break;

    case WM_PAINT:
       break;

    case WM_DESTROY:
         PostQuitMessage(0);		
       return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}



// WINMAIN ////////////////////////////////////////////////
int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{

WNDCLASS winclass;	// this will hold the class we create
HWND	 hwnd;		// generic window handle
MSG		 msg;		// generic message
HDC      hdc;       // generic dc


winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                          CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= NULL;
winclass.lpszClassName	= WINDOW_CLASS_NAME;

// register the window class
if (!RegisterClass(&winclass))
	return(0);


//initialize Direct Play
if (!(InitializeDirectPlay(hinstance)))
   return(0);


// create the window
if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, 
						  "Direct Play Send/Receive Demo",	
						  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					 	  0,0,	 
						  WINDOW_WIDTH, 
                          WINDOW_HEIGHT,
						  NULL,	   
						  NULL,	   
						  hinstance,
						  NULL)))
return(0);

	while(1)
	{
    	if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
    	{
            if (msg.message == WM_QUIT)
              break;
        	
        	TranslateMessage(&msg);
        	DispatchMessage(&msg);
    	}
		else
		{	
			
		    hdc = GetDC(hwnd);				//to output text
 
											//display counters 
			length = sprintf(buffer,"SENT counter: %d   ", send_count++);
			TextOut(hdc,0,0,buffer,length);
			length = sprintf(buffer,"RECIEVED : %d   ", received_count);
			TextOut(hdc,260,0,buffer,length);

			if (send_count > 30)
			  send_count = 0;

			ReleaseDC(hwnd,hdc);			//finish text operations

			SendFireMessage(send_count);	//send the counter
			received_count = ReceiveFireMessage();  //get the received count

			Sleep(200);						//wait a sec

		}						
	}
   	
	UninitializeDirectPlay();				//uninitialize DP
    return(msg.wParam);						//quit to windows like so
}
