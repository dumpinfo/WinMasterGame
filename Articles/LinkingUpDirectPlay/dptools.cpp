/*
Semion S. Bezrukov 5-31-99
e-mail: deltree@rocketmail.com
This file is a compilation of Direct Play routines for the management
and setup of DirectPlay object and player. The majority of this code comes
from 'Inside Direct X' by Bradley Bargen and Peter Donnelly. There, that's
all the legal stuff.
To get this to work with your program, simply include 'dptools.h' in
your main .h file and initialize DirectPlay like so: 

  InitializeDirectPlay(hinstance);

where hinstance is the instance of your application.
To deinitialize DirectPlay, simply use:

	UninitializeDirectPlay();

just before you quit the application. You must have dplay.lib and 
dplayx.lib specified in your library compile path or you'll get a ton
of link errors.
To see how to send and receive data simply look to the SendFireMessage()
and EvaluateGameMessage().
SendFireMessage() simply fills the FIREMSG struct and then sends it using
another general function.
EvaluateGameMessage(), casts the received struct to a general struct
to figure out the exact type and then recasts it to the specific struct
so that the data can be extracted.
Good luck,
    SIM.
*/


#define WIN32_LEAN_AND_MEAN
#define IDIRECTPLAY2_OR_GREATER
#define INITGUID


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <objbase.h>	// GUID functions


#include "dplay.h"
#include "dplobby.h"
#include "dptools.h"
#include "resource.h"
#include "dpex.h"



int receive_count;			//a global variable that holds the recieved
							//data. When the main program asks for it, a
							//function simply returns it. I chose this
							//approach for it's ease.


typedef struct
{
	GUID	guidSP;
	LPVOID 	lpConnection;
	DWORD	dwConnectionSize;
}CONNECTIONINFO;

typedef CONNECTIONINFO* LPCONNECTIONINFO;


GUID ZCAR_GUID = {
    0x949a21e2,
    0x6179,
    0x11cf,
    {0x95, 0x4f, 0x00, 0xaa, 0x00, 0x6c, 0x26, 0x58}
};


#define TIMER_ID        1
#define TIMER_RATE      2000			// milliseconds


LPDIRECTPLAY3       lpDP				= NULL;
LPDIRECTPLAYLOBBY2	lpDPLobby2			= NULL;
DPSESSIONDESC2      dpDesc;
LPDPSESSIONDESC2    lpdpDesc			= NULL;
DPCAPS              dpCaps;
HANDLE              hPlayerEvent		= NULL;
HANDLE              hKillEvent			= NULL;
HANDLE              hReceiveThread		= NULL;
DWORD               idReceiveThread		= 0;
LPVOID              lpReceiveBuffer		= NULL;
DWORD               dwReceiveBufferSize = 0;
bool				bEnumerating		= FALSE;
bool				bUserCancel			= TRUE;

HOSTMSG				MsgHost;
SYNCMSG				MsgSync;
FIREMSG				MsgFire;
CONTROLMSG			MsgControl;

BOOL		g_bHost	= FALSE;
char		g_szPlayerName[31];		// our name
DPID		g_dpPlayerID;			// our id

HANDLE                  g_hInstance;
HWND                	g_hwnd;





void InitMessageBuffers( void )
{
    MsgHost.byType		= MSG_HOST;
	MsgSync.byType		= MSG_SYNC;
	MsgFire.byType		= MSG_FIRE;
	MsgControl.byType	= MSG_CONTROL;
}


BOOL FAR PASCAL EnumPlayer( DPID pidID,
                            DWORD dwPlayerType, LPCDPNAME lpName, 
                            DWORD dwFlags, LPVOID lpContext)
{
    HWND hWnd = ( HWND ) lpContext;

    SendMessage( hWnd, LB_ADDSTRING, 0, 
                    ( LPARAM ) lpName->lpszShortNameA );

    return( TRUE );
}


BOOL WINAPI EnumConnection( LPCGUID lpguidSP,
                            LPVOID lpConnection, DWORD dwSize,
                            LPDPNAME lpName, DWORD dwFlags, 
                            LPVOID lpContext )
{
    LONG				iIndex;
    HWND hWnd			= ( HWND ) lpContext;
    LPVOID				lpOurConnection = NULL;
	LPCONNECTIONINFO	lpConnectionInfo = NULL;
    LPDIRECTPLAY3		lpDPTemp;

    // Check to see if a connection can be initialized
    if FAILED( CoCreateInstance( CLSID_DirectPlay,
                NULL, CLSCTX_ALL, IID_IDirectPlay3A,
                ( LPVOID* ) &lpDPTemp ) )
    {
        return( FALSE );
    }

    if FAILED( lpDPTemp->InitializeConnection( lpConnection, 0 ) )
    {
        lpDPTemp->Release();
        return( TRUE );
    }
    lpDPTemp->Release();

    // If it was initialized, add it to the list box
    iIndex = SendMessage( hWnd, CB_ADDSTRING, 0, 
                            (LPARAM) lpName->lpszShortNameA );

    // If it got added to the list box, create a copy of the connection
    // info and store a pointer to it in the list box.
    if ( iIndex != LB_ERR )
    {
		lpConnectionInfo = 
			(LPCONNECTIONINFO)malloc( sizeof( CONNECTIONINFO ) );

		memcpy( &lpConnectionInfo->guidSP, lpguidSP, sizeof( GUID ) );

		lpConnectionInfo->lpConnection = malloc( dwSize );
		memcpy( lpConnectionInfo->lpConnection, lpConnection, dwSize );

		lpConnectionInfo->dwConnectionSize = dwSize;

        SendMessage( hWnd, CB_SETITEMDATA, iIndex, 
                            ( LPARAM ) lpConnectionInfo );
    }
    else 
    {
        return FALSE;
    }

    return( TRUE );
}


BOOL WINAPI EnumSession( LPDPSESSIONDESC2 lpDPGameDesc, LPDWORD lpdwTimeOut,
                            DWORD dwFlags, LPVOID lpContext )
{
    LONG iIndex;
    HWND hWnd = ( HWND ) lpContext;
    LPGUID lpGuid;

    // First check and see if the enumeration timed out.  If so, we
    // could reset it, but we'll just canel it instead.

    if( dwFlags & DPESC_TIMEDOUT )
    {
        return FALSE;
    }

    iIndex = SendMessage( hWnd, LB_ADDSTRING, 0, 
                            (LPARAM) lpDPGameDesc->lpszSessionName );

    // If it got added to the list box, create a copy of the GUID
    // and store a pointer to it in the list box.

    if ( iIndex != LB_ERR )
    {
        lpGuid = ( LPGUID ) malloc( sizeof( GUID ) );
        if ( !lpGuid ) return FALSE;

        *lpGuid = lpDPGameDesc->guidInstance;

        SendMessage( hWnd, LB_SETITEMDATA, iIndex, 
                                ( LPARAM ) lpGuid );
    }

    return(TRUE);
}


LRESULT CALLBACK DlgProcEnumPlayers( HWND hWnd, UINT message, 
                                        WPARAM wParam, LPARAM lParam )
{
    int iIndex;
    LPGUID lpGuid;

    switch( message )
    {
        case WM_INITDIALOG:

            iIndex = SendDlgItemMessage( GetParent( hWnd ), IDC_SESSIONS, 
                                    LB_GETCURSEL, 0, 0L );
            lpGuid = ( LPGUID )SendDlgItemMessage( GetParent( hWnd ), 
                        IDC_SESSIONS, LB_GETITEMDATA, iIndex, 0 );

            if FAILED( lpDP->EnumPlayers( lpGuid, 
                            ( LPDPENUMPLAYERSCALLBACK2 )EnumPlayer,
                            ( LPVOID )GetDlgItem( hWnd, IDC_PLAYERS ),
                            DPENUMPLAYERS_SESSION ) )
            {
                MessageBox( hWnd, "Error enumerating players.", 
                                        "Error", MB_OK );
                EndDialog( hWnd, FALSE );
            }

            return TRUE;

        case WM_COMMAND:         
            switch( LOWORD( wParam ) )
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog( hWnd, FALSE );
                    return TRUE;
            }

    }
    return FALSE; 
}


BOOL CreateGamePlayer( void )
{	
	DPNAME dpPlayerName;
	
	ZeroMemory( &dpPlayerName, 
                sizeof( dpPlayerName ) );
    dpPlayerName.dwSize = sizeof( DPNAME );
    dpPlayerName.lpszShortNameA = g_szPlayerName;

	if FAILED( lpDP->CreatePlayer( &g_dpPlayerID, &dpPlayerName,
							hPlayerEvent, NULL, 0, 0 ) )
	{
		OutputDebugString( 
			"CreateGamePlayer: Player create failed!\n" );
		return FALSE;
	}

	OutputDebugString( 
		"CreateGamePlayer: Player created.\n" );
	return TRUE;
}


BOOL CALLBACK DlgProcDPStart( HWND hWnd, UINT message, WPARAM wParam, 
                           LPARAM lParam )
{	
	long                i;
	HRESULT				hr;
	LPDIRECTPLAYLOBBY	lpDPLobby	= NULL;
	LPDIRECTPLAY2		lpDP2		= NULL;
    LPVOID              lpHeap, lpConnection;
	LPDPLCONNECTION		lpLConnection;
	LPCONNECTIONINFO	lpConnectionInfo;
    DWORD               dwSessions;
    LPDELETEITEMSTRUCT  lpdis;
    char                szSessionName[30];
    LPGUID              lpGuid;

	DPCOMPOUNDADDRESSELEMENT	addressChunks[2];
	DWORD						dwChunkCount;
	DWORD						dwSize = 0;

    switch( message )
    {
        case WM_INITDIALOG:

            if FAILED( CoCreateInstance( CLSID_DirectPlay,
                        NULL, CLSCTX_ALL, IID_IDirectPlay3A,
                        ( LPVOID* ) &lpDP ) )
            {
                MessageBox( hWnd, "Error creating DirectPlay object.", 
                                "Error", MB_OK );
                EndDialog( hWnd, -1 );
            }

			if FAILED( DirectPlayLobbyCreate( NULL, &lpDPLobby, 
												NULL, NULL, 0 ) )
			{
                MessageBox( hWnd, "Error creating DirectPlayLobby object.", 
                                "Error", MB_OK );
                EndDialog( hWnd, -1 );
			}
			// DirectPlayLobbyCreate gives us an IID_IDirectPlayLobby
			// interface. Query for the newer *ANSI* interface.
			else if FAILED( lpDPLobby->QueryInterface( 
						IID_IDirectPlayLobby2A, (LPVOID*) &lpDPLobby2 ) )
			{
				MessageBox( hWnd, "Error getting Lobby interface.", 
                            "Error", MB_OK );
				EndDialog( hWnd, -1 );
			}

			// This version of the interface is no longer needed
			lpDPLobby->Release();

			// Check for lobby launch

			hr = lpDPLobby2->GetConnectionSettings( 0, NULL, &dwSize );

			if ( hr != DPERR_NOTLOBBIED )

			{
				OutputDebugString("Attempting Lobby Launch...\n");

				// We were lobbied. Get the connection settings.

				lpLConnection = (LPDPLCONNECTION) malloc( dwSize );

				lpDPLobby2->GetConnectionSettings( 0,
											lpLConnection, &dwSize );

				// Set the session up the way we want it.

				lpLConnection->lpSessionDesc->dwFlags = DPSESSION_MIGRATEHOST |
														DPSESSION_KEEPALIVE;
				lpLConnection->lpSessionDesc->dwMaxPlayers = 4;
				lpDPLobby2->SetConnectionSettings( 0, 0, lpLConnection );
				
				// Connect to the session.
				lpDPLobby2->Connect( 0, &lpDP2, NULL );

				lpDP2->QueryInterface( IID_IDirectPlay3A, (LPVOID*)&lpDP );

				lpDP2->Release();

				EndDialog( hWnd, 0 );
			}

			// Fill the connections drop down list box
            SendDlgItemMessage( hWnd, IDC_CONNECTIONS,
                                CB_ADDSTRING, 0, 
                                (LPARAM) "<<Select a Connection>>" );

            if FAILED( lpDP->EnumConnections( NULL, 
                        ( LPDPENUMCONNECTIONSCALLBACK )EnumConnection,
                        ( LPVOID )GetDlgItem( hWnd, IDC_CONNECTIONS ),
                        0 ) )
            {
                MessageBox( hWnd, "Couldn't enumerate connections.",
                                "Error", MB_OK );
                EndDialog( hWnd, -1 );
            }
            else
            {
                SendDlgItemMessage( hWnd, IDC_CONNECTIONS, 
                    CB_SETCURSEL, 0, 0L );
            }

            // Start a timer for async session list
            SetTimer( hWnd, TIMER_ID, TIMER_RATE, NULL );

            return TRUE;

        case WM_DELETEITEM:
            // Delete the memory allocated when the list and combo
            // boxes are filled.

            lpdis = ( LPDELETEITEMSTRUCT ) lParam;

			if ( wParam == IDC_CONNECTIONS )
			{
				lpConnectionInfo = ( LPCONNECTIONINFO )lpdis->itemData;
				if ( lpConnectionInfo )
				{
					if ( lpConnectionInfo->lpConnection )
						free( lpConnectionInfo->lpConnection );
					free( lpConnectionInfo );
				}
			}
			else
			{
				lpHeap = ( LPVOID )lpdis->itemData;
				if ( lpHeap ) 
					free( lpHeap );
            }

            return TRUE;

        case WM_COMMAND:         
            switch( LOWORD( wParam ) )
            {
                case IDC_CONNECTIONS:
                    switch (HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                            // Release the existing DP object so we can 
                            // reinitialize it with the selected connection.

                            if ( lpDP ) lpDP->Release();

                            if FAILED( CoCreateInstance( CLSID_DirectPlay,
                                NULL, CLSCTX_ALL, IID_IDirectPlay3A,
                                ( LPVOID* ) &lpDP) )
                            {
                                MessageBox( hWnd, 
                                    "Error creating DirectPlay object.",
                                    "Error", MB_OK );
                                return TRUE;
                            }
                            
                            // Get the currently selected connection from 
                            // the list box.
                            i = SendDlgItemMessage( hWnd, IDC_CONNECTIONS, 
                                                        CB_GETCURSEL,
                                                        0, 0L );
                            lpConnectionInfo = (LPCONNECTIONINFO)
														SendDlgItemMessage(
                                                        hWnd, IDC_CONNECTIONS, 
                                                        CB_GETITEMDATA,
                                                        i, 0 );
							
							lpConnection = lpConnectionInfo->lpConnection;

							// If the connection uses the TCP/IP service 
							// provider, fill in chunks for the provider
							// and TCP/IP address.
							if ( IsEqualGUID( lpConnectionInfo->guidSP, 
														DPSPGUID_TCPIP ) )
							{
								dwChunkCount = 0;

								addressChunks[ dwChunkCount ].guidDataType = 
														DPAID_ServiceProvider;
								addressChunks[ dwChunkCount ].dwDataSize = 
															sizeof(GUID);
								addressChunks[ dwChunkCount ].lpData = 
													(LPVOID) &DPSPGUID_TCPIP;
								dwChunkCount++;

								// An empty address will trigger a
								// local enumeration.
								addressChunks[dwChunkCount].guidDataType = 
																	DPAID_INet;
								addressChunks[dwChunkCount].dwDataSize = 1;
								addressChunks[dwChunkCount].lpData = "";
								dwChunkCount++;

								// Find out how much space we'll need for
								// the connection data and allocate.
								lpDPLobby2->CreateCompoundAddress(
													addressChunks, 
													dwChunkCount, 
													NULL, &dwSize );

								lpConnection = malloc( dwSize );

								// Create the connection buffer.
								if FAILED( lpDPLobby2->CreateCompoundAddress(
														addressChunks, 
														dwChunkCount,
														lpConnection, 
														&dwSize ) )
								{
									OutputDebugString(
										"Couldn't create compound address.\n");
								}

							}

                            if FAILED( lpDP->InitializeConnection( 
														lpConnection, 0 ) )
                            {
                                MessageBox( hWnd, 
                                    "Error initializing DirectPlay object.",
                                    "Error", MB_OK );
                                EnableWindow( GetDlgItem( hWnd, IDCREATE ),
                                                FALSE );
                                EnableWindow( GetDlgItem( hWnd, IDPLAYERS ),
                                                FALSE );
                                EnableWindow( GetDlgItem( hWnd, IDJOIN ),
                                                FALSE );
                            }
                            else
                            {
                                EnableWindow( GetDlgItem( hWnd, IDCREATE ),
                                                TRUE );
								bUserCancel = FALSE;
                            }
                            return TRUE;
                    }
                    return TRUE;


                case IDPLAYERS:
                    DialogBox( (HINSTANCE)g_hInstance, MAKEINTRESOURCE( IDD_PLAYERS ), hWnd,
                                ( DLGPROC )DlgProcEnumPlayers );
                    return TRUE;

                case IDJOIN:
					OutputDebugString("*********Joining!\n");
                    i = SendDlgItemMessage( hWnd, IDC_SESSIONS, 
                                LB_GETCURSEL, 0, 0L );
                    lpGuid = ( LPGUID )SendDlgItemMessage( hWnd, 
                                IDC_SESSIONS, LB_GETITEMDATA, i, 0 );

                    ZeroMemory( &dpDesc, sizeof( dpDesc ) );
                    dpDesc.dwSize = sizeof( dpDesc );
                    dpDesc.guidInstance = *lpGuid;
                    dpDesc.guidApplication = ZCAR_GUID;

                    if FAILED( lpDP->Open( &dpDesc, DPOPEN_JOIN ) )
                    {         
                        MessageBox( hWnd, "Could not join session.", 
                                        "Error", MB_OK );
                        EndDialog( hWnd, -1 );
                    }
                    else
                    {
						EndDialog( hWnd, 0 );
                    }
                    return TRUE;

                case IDCREATE:
                    // Create the session with the supplied parameters
                    GetDlgItemText( hWnd, IDC_SESSIONNAME, 
                        szSessionName, 30 );

                    ZeroMemory( &dpDesc, sizeof( dpDesc ) );
                    dpDesc.dwSize = sizeof( dpDesc );
                    dpDesc.guidApplication = ZCAR_GUID;
                    dpDesc.dwFlags = DPSESSION_MIGRATEHOST |
                                        DPSESSION_KEEPALIVE;
					dpDesc.dwMaxPlayers = 4;
                    dpDesc.lpszSessionNameA = szSessionName;

                    if FAILED( lpDP->Open( &dpDesc, DPOPEN_CREATE ) )
                    {         
                        MessageBox( hWnd, "Could not create session.", 
                                        "Error", MB_OK );
                        EndDialog( hWnd, -1 );
                    }
                    else
                    {
						EndDialog( hWnd, 0 );
                    }
                    return TRUE;

                case IDCANCEL:
                    EndDialog( hWnd, -2 );
                    return TRUE;
            }
            break;

        case WM_TIMER:
			if ( bEnumerating || bUserCancel ) return TRUE;

            // Display the current session list if
            // there is a valid DPlay object

            SendDlgItemMessage(hWnd, IDC_SESSIONS, 
                                LB_RESETCONTENT, 0, 0L);

            ZeroMemory( &dpDesc, sizeof( dpDesc ) );
            dpDesc.dwSize = sizeof( dpDesc );
            dpDesc.guidApplication = ZCAR_GUID;

            dwSessions = DPENUMSESSIONS_AVAILABLE | 
                            DPENUMSESSIONS_ASYNC;

			bEnumerating = TRUE;

            // Start the enumeration
            hr = lpDP->EnumSessions( &dpDesc, 0, 
					( LPDPENUMSESSIONSCALLBACK2 )EnumSession, 
					( LPVOID ) GetDlgItem( hWnd, IDC_SESSIONS ), 
					dwSessions );

			if ( hr == DPERR_USERCANCEL ) {
				bUserCancel = TRUE;
			}

			bEnumerating = FALSE;

            // Adjust the user interface accordingly if there
            // are sessions available
            if ( SendDlgItemMessage( hWnd, IDC_SESSIONS, 
                                        LB_GETCOUNT, 0, 0 ) > 0 )
            {
                EnableWindow( GetDlgItem( hWnd, IDPLAYERS ),
                    TRUE );
                EnableWindow( GetDlgItem( hWnd, IDJOIN ),
                    TRUE );
                SendDlgItemMessage( hWnd, IDC_SESSIONS,
                                        LB_SETCURSEL, 0, 0 );
            }
            else
            {
                EnableWindow( GetDlgItem( hWnd, IDPLAYERS ),
                    FALSE );
                EnableWindow( GetDlgItem( hWnd, IDJOIN ),
                    FALSE );
            }
            return TRUE;
                    
        case WM_DESTROY:
			// Save the player name for later
            GetDlgItemText( hWnd, IDC_PLAYER,
                            g_szPlayerName, 30 );

            KillTimer( hWnd, TIMER_ID );
            break;

    }
    return FALSE; 
}



void EvaluateGameMessage( LPGENERICMSG lpGeneric, DPID pidFrom )
{   

    switch( lpGeneric->byType )
    {
        case MSG_HOST:
			// If we're getting this message, we must be a new
			// player. Get the slot from the slot number, fill it.
			OutputDebugString("Processing welcome message.\n");
			LPHOSTMSG       lpHost;
			lpHost = (LPHOSTMSG) lpGeneric;
			
            break;

		case MSG_FIRE:
			OutputDebugString("Getting fire!\n");
			LPFIREMSG	lpFire;
			lpFire = (LPFIREMSG) lpGeneric;

			receive_count = lpFire->sddata;

			break;

		case MSG_SYNC:
			LPSYNCMSG	lpSync;
			lpSync = (LPSYNCMSG) lpGeneric;

			
			break;

		case MSG_CONTROL:
			LPCONTROLMSG	lpControl;
			lpControl = (LPCONTROLMSG) lpGeneric;
			break;
    }  
}



int ReceiveFireMessage()
{
    return(receive_count);
}




void EvaluateSystemMessage( DPMSG_GENERIC *pGeneric, HWND hWnd )
{    

//	int ass;
    switch( pGeneric->dwType )
    {
        // The message comes to us cast as DPMSG_GENERIC.  We'll examine
        // dwType to determine the type of message, then cast to the new
        // type and evaluate the rest of the message.

        case DPSYS_CREATEPLAYERORGROUP:
        {
            DPMSG_CREATEPLAYERORGROUP *pMsg;
            pMsg = (DPMSG_CREATEPLAYERORGROUP *) pGeneric;

			OutputDebugString("A player has joined.\n");

			if ( g_bHost )
			{
			//	MsgHost.bySlot = FindPlayerSlot();
				SendGameMessage( (LPGENERICMSG)&MsgHost, pMsg->dpId );
				OutputDebugString( "Sending welcome message.\n" );
			}

            break;
        }

        case DPSYS_DESTROYPLAYERORGROUP:
        {
            DPMSG_DESTROYPLAYERORGROUP *pMsg;
            pMsg = (DPMSG_DESTROYPLAYERORGROUP *) pGeneric;

			OutputDebugString("A player has left.\n");

			// Remove the player from the game
        }

        case DPSYS_HOST:
        {
			g_bHost = TRUE;
			OutputDebugString( "This machine is now the session host.\n" );
            break;
        }       
    }      
}


void ReceiveMessages( HWND hWnd )
{
    DPID            fromID;
    DPID            toID;
    DWORD           nBytes;
    DPMSG_GENERIC   *pGeneric;
	LPGENERICMSG	pGameMsg;
    HRESULT         dprval;


    // Don't let Receive work use the global value directly,
    // as it changes it.
    nBytes = dwReceiveBufferSize;

    while( TRUE )

    {   
		dprval = lpDP->Receive( &fromID, &toID,
							DPRECEIVE_ALL, lpReceiveBuffer, &nBytes);


		if ( dprval == DPERR_BUFFERTOOSMALL )
		// The recieve buffer size must be adjusted.
		{
			if ( lpReceiveBuffer == NULL)
			{
				// We haven't allocated any buffer yet -- do it.
				lpReceiveBuffer = malloc( nBytes );
				if ( lpReceiveBuffer == NULL ) {
					OutputDebugString( "Couldn't allocate memory.\n" );
					return;
				}
			}
			else
			{
				// The buffer's been allocated, but it's too small so
				// it must be enlarged.
				free( lpReceiveBuffer );
				lpReceiveBuffer = malloc( nBytes );
				if ( lpReceiveBuffer == NULL ) {
					OutputDebugString( "Couldn't allocate memory.\n" );
					return;
				}
			}
			// Update our global to the new buffer size.
			dwReceiveBufferSize = nBytes;
		}
		else if ( dprval == DP_OK )
		// A message was successfully retrieved.
		{	
			if ( fromID == DPID_SYSMSG )
			{
				pGeneric = (DPMSG_GENERIC *) lpReceiveBuffer;
				OutputDebugString( "Processing system message.\n" );
				EvaluateSystemMessage ( pGeneric, hWnd );
			}
			else
			{
				pGameMsg = (LPGENERICMSG) lpReceiveBuffer;
				OutputDebugString("Processing game message.\n");
				EvaluateGameMessage( pGameMsg, fromID );
			}
		}
		else
		{
			return;
		}
    }
}


DWORD WINAPI ReceiveThread( LPVOID lpParameter )
{
    HWND        hWnd = (HWND) lpParameter;
    HANDLE      eventHandles[2];
    eventHandles[0] = hPlayerEvent;
    eventHandles[1] = hKillEvent;

    // Wait for either the player or kill event to fire.  If it
    // is the player event (WAIT_OBJECT_0), process the messages
    // and wait again.  If it's the kill event, shut down the
    // thread and exit

    while (WaitForMultipleObjects( 2, eventHandles, FALSE,
                INFINITE) == WAIT_OBJECT_0 )
    {
        OutputDebugString( "Thread awakened.\n" );
		ReceiveMessages( hWnd );
    }

    ExitThread( 0 );

    return ( 0 );
}


int StartDPSession( void )
{
	int rc;

	// Call the dialog for establishing a connection
	rc = DialogBox((HINSTANCE) g_hInstance, MAKEINTRESOURCE(IDD_CONNECT), 
                            g_hwnd, (DLGPROC)DlgProcDPStart );

	if ( !rc )
	{
		InitMessageBuffers();

		// Create an event that will be used to signal when
		// the player has messages
		hPlayerEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

		hKillEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

		hReceiveThread = CreateThread(NULL,
                                   0,           
                                   ReceiveThread,
                                   g_hwnd,
                                   0,           
                                   &idReceiveThread);

		 // Get the caps for the session
		ZeroMemory( &dpCaps, sizeof( dpCaps ) );
		dpCaps.dwSize = sizeof( dpCaps );
		lpDP->GetCaps( &dpCaps, 0 );

		// Are we the session host?
		if ( dpCaps.dwFlags & DPCAPS_ISHOST  )
		{
			g_bHost = TRUE;
			OutputDebugString( "We are the session host.\n" );
		}
	}
	return rc;
}


BOOL ShutDownDPSession( void )
{
	// Destroy the local player
	if ( g_dpPlayerID )
	{
		lpDP->DestroyPlayer( g_dpPlayerID );
		g_dpPlayerID = 0;
	}

	// Shut down the recieve thread
	if ( hReceiveThread )
	{
		// Signal event to kill receive thread
		SetEvent( hKillEvent );

		// Wait for the thread to shut down
		WaitForSingleObject( hReceiveThread, INFINITE );

		CloseHandle( hReceiveThread); 
		hReceiveThread = NULL;
	}

	// Close the other events
	if ( hKillEvent ) 
	{
		CloseHandle( hKillEvent );
		hKillEvent = NULL;
	}
	if ( hPlayerEvent )
	{
		CloseHandle( hPlayerEvent );
		hPlayerEvent = NULL;
	}

	// Free the receive buffer.
	if ( lpReceiveBuffer )
	{
		free( lpReceiveBuffer );
		lpReceiveBuffer = NULL;
	}

	// Release the DirectPlay object and the Lobby object
	if ( lpDP )
	{
		OutputDebugString( "Releasing the DP object.\n" );
		lpDP->Release();
		lpDP = NULL;
	}
	if ( lpDPLobby2 )
	{
		OutputDebugString( "Releasing the lobby object.\n" );
		lpDPLobby2->Release();
		lpDPLobby2 = NULL;
	}
	return TRUE;
}


void SendSyncMessage()

{

//MsgSync.x = vehicle->x;

SendGameMessage( (LPGENERICMSG)&MsgSync, DPID_ALLPLAYERS );

}


void SendFireMessage(int send_me)
{

	MsgFire.sddata = send_me;

	SendGameMessage( (LPGENERICMSG)&MsgFire, DPID_ALLPLAYERS );
}


void SendControlMessage( BYTE byControl )
{

	MsgControl.byState = byControl;
	SendGameMessage( (LPGENERICMSG)&MsgControl, DPID_ALLPLAYERS );
}


void SendGameMessage( LPGENERICMSG lpMsg, DPID idTo )
{
    int             nBytes;
	DWORD			dwFlags = 0;

    switch( lpMsg->byType )
    {
		case MSG_HOST:
			nBytes = sizeof( HOSTMSG );
			dwFlags = DPSEND_GUARANTEED;
			break;

		case MSG_SYNC:
			nBytes = sizeof( SYNCMSG );
			break;

		case MSG_FIRE:
			nBytes = sizeof( FIREMSG );
			dwFlags = DPSEND_GUARANTEED;
			break;

		case MSG_CONTROL:
			nBytes = sizeof( CONTROLMSG );
			break;

		default:
			return;
	}

	// Send the message buffer
	lpDP->Send( g_dpPlayerID, idTo, dwFlags, (LPVOID)lpMsg, nBytes);	
}




///new aggregate stuff, added by SIM(4-08-99)

BOOL InitializeDirectPlay( HANDLE hInstance)
{	
    CoInitialize( NULL );	
	int rc;
    g_hInstance = hInstance;

	// Attempt to initialize DirectPlay
	rc = StartDPSession();

	switch ( rc )
	{
		case -1:
			// Something failed when attempting DirectPlay init
			MessageBox( NULL, "DirectPlay Init FAILED try including the rc file", "ERROR", MB_OK );
			// Fall through
		case -2:
			// The user pressed the cancel button.
			OutputDebugString( "The user cancelled DPlay.\n" );
			return FALSE;
	}

    CreateGamePlayer();         
    return TRUE;
}


void UninitializeDirectPlay()
{	
	CoUninitialize();
	ShutDownDPSession();
//MessageBox(NULL, "uninit", "uninit", MB_OK);
}


