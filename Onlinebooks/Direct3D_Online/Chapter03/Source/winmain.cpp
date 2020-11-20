//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <d3d.h>
#include "resource.h"

//-----------------------------------------------------------------------------
// Variables for the DirectX part of the sample
//-----------------------------------------------------------------------------
static LPDIRECTDRAW         g_pDD1           = NULL;
static LPDIRECTDRAW4        g_pDD4           = NULL;
static LPDIRECTDRAWSURFACE4 g_pddsPrimary    = NULL;
static LPDIRECTDRAWSURFACE4 g_pddsBackBuffer = NULL;
static LPDIRECTDRAWSURFACE4 g_pddsZBuffer    = NULL;
static LPDIRECT3D3          g_pD3D           = NULL;
static LPDIRECT3DDEVICE3    g_pd3dDevice     = NULL;
static LPDIRECT3DVIEWPORT3  g_pvViewport     = NULL;
static RECT                 g_rcScreenRect;
static RECT                 g_rcViewportRect;

//-----------------------------------------------------------------------------
// Variables for the DirectX part of the sample
//-----------------------------------------------------------------------------
static BOOL g_bActive  = FALSE; // Whether the app is active (not minimized)
static BOOL g_bReady   = FALSE; // Whether the app is ready to render frames

//-----------------------------------------------------------------------------
// Local function-prototypes
//-----------------------------------------------------------------------------
HRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
HRESULT CreateEverything( HWND );
HRESULT Initialize3DEnvironment( HWND, GUID*, const GUID* );
HRESULT Cleanup3DEnvironment();
HRESULT Render3DEnvironment();
VOID    OnMove( INT, INT );
HRESULT ShowFrame();
HRESULT RestoreSurfaces();

//-----------------------------------------------------------------------------
// External function prototypes
//-----------------------------------------------------------------------------

VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT App_FrameMove( LPDIRECT3DDEVICE3, FLOAT );
HRESULT App_Render( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3, D3DRECT* );

//-----------------------------------------------------------------------------
// WinMain
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	// Register the window class
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                          LoadIcon( hInst, MAKEINTRESOURCE(IDI_MAIN_ICON)),
                          LoadCursor(NULL, IDC_ARROW), 
                          (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          TEXT("Direct3D IM Sample") };
    RegisterClass( &wndClass );

    // Create our main window
    HWND hWnd = CreateWindow( TEXT("Direct3D IM Sample"),
		                      TEXT("Direct3D IM Sample"),
                              WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                              CW_USEDEFAULT, 300, 300, 0L, 0L, hInst, 0L );
    ShowWindow( hWnd, SW_SHOWNORMAL );
    UpdateWindow( hWnd );

    // Load keyboard accelerators
    HACCEL hAccel = LoadAccelerators( hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

	// Initialize the DirectDraw and Direct3D stuff
	if( FAILED( CreateEverything( hWnd ) ) )
        return 0;

    // recieve and process Windows messages.
	BOOL bGotMsg;
	MSG  msg;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	g_bReady = TRUE;

    while( WM_QUIT != msg.message  )
    {
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage()
		if( g_bActive )
			bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		else
			bGotMsg = GetMessage( &msg, NULL, 0U, 0U );

		if( bGotMsg )
        {
			// Translate and dispatch the message
            if( 0 == TranslateAccelerator( hWnd, hAccel, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
        }
		else
		{
			// Render a frame during idle time (no messages are waiting)
			if( g_bActive && g_bReady )
				Render3DEnvironment();
		}
    }
	return msg.wParam;
}

//-----------------------------------------------------------------------------
// WndProc()
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_PAINT:
			// If we get WM_PAINT messages, it usually means our window was
			// covered up, so we need to refresh it by re-showing the contents
			// of the current frame.
			ShowFrame();
            break;

        case WM_MOVE:
			// Move messages need to be tracked to update the screen rects
			// used for blitting the backbuffer to the primary.
            if( g_bActive && g_bReady )
				OnMove( (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) );
            break;

        case WM_SIZE:
            // Check to see if we are losing or gaining our window. Set the
			// active flag to match.
            if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
                g_bActive = FALSE;
			else g_bActive = TRUE;

            // A new window size will require a new backbuffer size. The
			// easiest way to achieve this is to release and re-create
			// everything. Note: if the window gets too big, we may run out
			// of video memory and need to exit. This simple app exits
			// without displaying error messages, but a real app would behave
			// itself much better.
            if( g_bActive && g_bReady )
			{
				g_bReady = FALSE;

				// Cleanup the environment and recreate everything
				if( FAILED( CreateEverything( hWnd) ) )
					DestroyWindow( hWnd );
				
				g_bReady = TRUE;
			}
            break;

		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimum size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;

        case WM_CLOSE:
            DestroyWindow( hWnd );
            return 0;
        
        case WM_DESTROY:
            Cleanup3DEnvironment();
            PostQuitMessage(0);
            return 0L;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// CreateEverything()
//-----------------------------------------------------------------------------
HRESULT CreateEverything( HWND hWnd )
{
	// Cleanup any objects that might've been created before
	if( FAILED( Cleanup3DEnvironment() ) )
		return E_FAIL;
				
	// Create the D3D environment, at first, trying the HAL
	if( SUCCEEDED( Initialize3DEnvironment( hWnd, NULL,
		                                    &IID_IDirect3DHALDevice ) ) )
		return S_OK;

	// Else, cleanup objects potentially created during the failed
	// initialization attempt.
	Cleanup3DEnvironment();

	if( SUCCEEDED( Initialize3DEnvironment( hWnd, NULL,
					                        &IID_IDirect3DRGBDevice ) ) )
		return S_OK;
	
	// Else, return failure. This simple tutorial will exit ungracefully.
	return E_FAIL;
}

//-----------------------------------------------------------------------------
// EnumZBufferCallback()
//-----------------------------------------------------------------------------
static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired )
{
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

        // Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
    }

    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}

//-----------------------------------------------------------------------------
// Initialize3DEnvironment()
//-----------------------------------------------------------------------------
HRESULT Initialize3DEnvironment( HWND hWnd, GUID* pDriverGUID,
								 const GUID* pDeviceGUID )
{
	HRESULT hr;

	// Create the IDirectDraw interface. 
	hr = DirectDrawCreate( pDriverGUID, &g_pDD1, NULL );
	if( FAILED( hr ) )
		return hr;

	// Get a ptr to an IDirectDraw4 interface.
	hr = g_pDD1->QueryInterface( IID_IDirectDraw4, (VOID**)&g_pDD4 );
	if( FAILED( hr ) )
		return hr;

    // Set the Windows cooperative level.
    hr = g_pDD4->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
	if( FAILED( hr ) )
		return hr;

	// Initialize a surface description structure for the primary surface.
	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize         = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	// Create the primary surface.
	hr = g_pDD4->CreateSurface( &ddsd, &g_pddsPrimary, NULL );
	if( FAILED( hr ) )
		return hr;

	// Setup a surface description to create a backbuffer.
	ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

	// Set the dimensions of the backbuffer. Note that if our window changes
	// size, we need to destroy this surface and create a new one.
	GetClientRect( hWnd, &g_rcScreenRect );
	GetClientRect( hWnd, &g_rcViewportRect );
	ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.left );
	ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.right );
	ddsd.dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
	ddsd.dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;

	// Create the backbuffer.
	hr = g_pDD4->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL );
	if( FAILED( hr ) )
		return hr;

	// Create a clipper object which handles the clipping for cases when
	// the window is partially obscured by other windows. 
	// This is not needed for when running in fullscreen mode.
	LPDIRECTDRAWCLIPPER pcClipper;
	hr = g_pDD4->CreateClipper( 0, &pcClipper, NULL );
	if( FAILED( hr ) )
		return hr;

	// Associate the clipper with our window.
	pcClipper->SetHWnd( 0, hWnd );
	g_pddsPrimary->SetClipper( pcClipper );
	pcClipper->Release();

    // Query DirectDraw for access to Direct3D
    g_pDD4->QueryInterface( IID_IDirect3D3, (VOID**)&g_pD3D );
    if( FAILED( hr) )
		return hr;

	// call the EnumZBufferFormats function.
	DDPIXELFORMAT ddpfZBuffer;
	g_pD3D->EnumZBufferFormats( *pDeviceGUID, 
		                        EnumZBufferCallback, (VOID*)&ddpfZBuffer );

	// If we found a good zbuffer format, then the dwSize field will be
	// properly set during enumeration. Else, there is a problem and we will exit.
    if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
        return E_FAIL;

    // Setup the surface desc for the z-buffer.
    ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	ddsd.dwWidth        = g_rcScreenRect.right - g_rcScreenRect.left;
	ddsd.dwHeight       = g_rcScreenRect.bottom - g_rcScreenRect.top;
    memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

	// For hardware devices, the z-buffer should be in video memory. For
	// software devices, create the z-buffer in system memory
	if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice ) )
		ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    // Create and attach a z-buffer.
    if( FAILED( hr = g_pDD4->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) ) )
		return hr;

	// Attach the z-buffer to the back buffer.
    if( FAILED( hr = g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) ) )
		return hr;

	// Before creating the device, check that we are NOT in a palettized
	// display. That case will cause CreateDevice() to fail.
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	g_pDD4->GetDisplayMode( &ddsd );
	if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
		return DDERR_INVALIDMODE;

	// Create the D3D Device. The device is created off of our back buffer, which
	// becomes the render target for the newly created device.
    if( FAILED( hr = g_pD3D->CreateDevice( *pDeviceGUID, g_pddsBackBuffer,
                                           &g_pd3dDevice, NULL ) ) )
	{
		// This call could fail for many reasons. The most likely cause is
		// that we specifically requested a hardware device, without knowing
		// whether there is even a 3D card installed in the system. Another
		// possibility is the hardware is incompatible with the current display
		// mode.
		return hr;
	}

    // Set up the viewport data parameters
    D3DVIEWPORT2 vdData;
    ZeroMemory( &vdData, sizeof(D3DVIEWPORT2) );
    vdData.dwSize       = sizeof(D3DVIEWPORT2);
	vdData.dwWidth      = g_rcScreenRect.right - g_rcScreenRect.left;
	vdData.dwHeight     = g_rcScreenRect.bottom - g_rcScreenRect.top;
    vdData.dvClipX      = -1.0f;
    vdData.dvClipWidth  = 2.0f;
    vdData.dvClipY      = 1.0f;
    vdData.dvClipHeight = 2.0f;
    vdData.dvMaxZ       = 1.0f;

    // Create the viewport
    hr = g_pD3D->CreateViewport( &g_pvViewport, NULL );
	if( FAILED( hr ) )
		return hr;

    // Associate the viewport with the D3DDEVICE object
    g_pd3dDevice->AddViewport( g_pvViewport );

    // Set the parameters to the new viewport
    g_pvViewport->SetViewport2( &vdData );

    // Set the viewport as current for the device
    g_pd3dDevice->SetCurrentViewport( g_pvViewport );

	// Finish by setting up our scene
	return App_InitDeviceObjects( g_pd3dDevice, g_pvViewport );
}

//-----------------------------------------------------------------------------
// Cleanup3DEnvironment()
//-----------------------------------------------------------------------------
HRESULT Cleanup3DEnvironment()
{
	// Cleanup any objects that we created for our app
	App_DeleteDeviceObjects( g_pd3dDevice, g_pvViewport );

	// Release the DDraw and D3D objects used by the app
    if( g_pvViewport )     g_pvViewport->Release();
    if( g_pddsZBuffer )    g_pddsZBuffer->Release();
	if( g_pD3D )           g_pD3D->Release();
	if( g_pddsBackBuffer ) g_pddsBackBuffer->Release();
	if( g_pddsPrimary )    g_pddsPrimary->Release();
	if( g_pDD4 )           g_pDD4->Release();

    // Do a safe check for releasing the D3DDEVICE.
    if( g_pd3dDevice )
        if( 0 < g_pd3dDevice->Release() )
			return E_FAIL;

	// Do a safe check for releasing DDRAW.
    if( g_pDD1 )
        if( 0 < g_pDD1->Release() )
			return E_FAIL;

    g_pvViewport     = NULL;
    g_pddsZBuffer    = NULL;
	g_pd3dDevice     = NULL;
	g_pD3D           = NULL;
	g_pddsBackBuffer = NULL;
	g_pddsPrimary    = NULL;
	g_pDD4           = NULL;
	g_pDD1           = NULL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Render3DEnvironment()
//-----------------------------------------------------------------------------
HRESULT Render3DEnvironment()
{
	// Call the app specific function to animate the scene
    App_FrameMove( g_pd3dDevice, ((FLOAT)clock())/CLOCKS_PER_SEC );

    // Call our app specific function to render the scene
    App_Render( g_pd3dDevice, g_pvViewport, (D3DRECT*)&g_rcViewportRect );
    
    // Show the frame on the primary surface. Note: this is the best place to
	// check for "lost" surfaces. Surfaces can be lost if something caused
	// them to temporary lose their video memory. "Lost" surfaces simply
	// need to be restored before continuing.
    if( DDERR_SURFACELOST == ShowFrame() )
		RestoreSurfaces();

    return S_OK;
}

//-----------------------------------------------------------------------------
// ShowFrame()
//-----------------------------------------------------------------------------
HRESULT ShowFrame()
{
	if( NULL == g_pddsPrimary )
		return E_FAIL;

    // We are in windowed mode, so perform a blit from the backbuffer to the
	// correct position on the primary surface
    return g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer, 
                               &g_rcViewportRect, DDBLT_WAIT, NULL );
}

//-----------------------------------------------------------------------------
// Name: RestoreSurfaces()
// Desc: Checks for lost surfaces and restores them if lost.
//-----------------------------------------------------------------------------
HRESULT RestoreSurfaces()
{
    // Check/restore the primary surface
    if( g_pddsPrimary )
        if( g_pddsPrimary->IsLost() )
            g_pddsPrimary->Restore();
    
    // Check/restore the back buffer
    if( g_pddsBackBuffer )
        if( g_pddsBackBuffer->IsLost() )
            g_pddsBackBuffer->Restore();

    // Check/restore the z-buffer
    if( g_pddsZBuffer )
        if( g_pddsZBuffer->IsLost() )
            g_pddsZBuffer->Restore();

    return S_OK;
}




//-----------------------------------------------------------------------------
// OnMove()
// Desc: Moves the screen rect for windowed renderers
//-----------------------------------------------------------------------------
VOID OnMove( INT x, INT y )
{
	DWORD dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
	DWORD dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
    SetRect( &g_rcScreenRect, x, y, x + dwWidth, y + dwHeight );
}