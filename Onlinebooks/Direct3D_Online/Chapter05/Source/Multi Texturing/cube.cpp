#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Multi Texturing Sample" );
BOOL   g_bAppUseZBuffer    = TRUE;    // Create/use a z-buffering
BOOL   g_bAppUseBackBuffer = TRUE;    // Create/use a back buffer


struct MTVERTEX
{
    D3DVALUE x, y, z;
    D3DVALUE tuBase, tvBase;
    D3DVALUE tuLightMap, tvLightMap;
};

#define FILL_MTVERTEX( v, ax, ay, az, atu1, atv1, atu2, atv2 )  \
{   v.x = ax; v.y = ay; v.z = az; \
    v.tuBase     = atu1; v.tvBase     = atv1; \
    v.tuLightMap = atu2; v.tvLightMap = atv2; \
}

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

LPDIRECT3DTEXTURE2 g_ptexWallTexture   = NULL; // Textures for the app
LPDIRECT3DTEXTURE2 g_ptexLightMap      = NULL;

MTVERTEX           g_avWallVertices[10];

//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
VOID    RotateVertexInX( FLOAT, DWORD, D3DVERTEX*, D3DVERTEX* );

//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	// Generate the cube data
//    CreateFlatWall();

	// Declare the vertices which define the room's walls, floor, and ceiling
    D3DVECTOR vNorm(0,1,0);

    FILL_MTVERTEX( g_avWallVertices[ 0], -5,-5, 5, 0.0f, 1.0f, 0.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 1], -5, 5, 5, 0.0f, 0.0f, 0.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 2],  5,-5, 5, 1.0f, 1.0f, 1.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 3],  5, 5, 5, 1.0f, 0.0f, 1.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 4],  5,-5,-5, 2.0f, 1.0f, 2.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 5],  5, 5,-5, 2.0f, 0.0f, 2.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 6], -5,-5,-5, 3.0f, 1.0f, 3.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 7], -5, 5,-5, 3.0f, 0.0f, 3.0, 0.0f );
    FILL_MTVERTEX( g_avWallVertices[ 8], -5,-5, 5, 4.0f, 1.0f, 4.0, 1.0f );
    FILL_MTVERTEX( g_avWallVertices[ 9], -5, 5, 5, 4.0f, 0.0f, 4.0, 0.0f );


    // Create textures
    D3DTextr_CreateTexture( "wall.bmp", 0 );
	D3DTextr_CreateTexture( "spotlite.bmp", 1 );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{

    return S_OK;

}

//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewRect )
{
    //Clear the viewport
    pvViewport->Clear2( 1UL, prcViewRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                        0x000000ff, 1.0f, 0L );

    // Begin the scene 
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {

        pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
        pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );


		pd3dDevice->SetTexture( 0, g_ptexWallTexture);
		pd3dDevice->SetTexture( 1, g_ptexLightMap );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_XYZ|D3DFVF_TEX2,
                                   g_avWallVertices, 10, NULL );


        // End the scene.
        pd3dDevice->EndScene();
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
    // Check parameters
    if( NULL==pd3dDevice || NULL==pvViewport )
        return E_INVALIDARG;

	    // Load the textures to the device
    D3DTextr_RestoreAllTextures( pd3dDevice );

    if( NULL == ( g_ptexWallTexture = D3DTextr_GetTexture( "wall.bmp" ) ) )
        return E_FAIL;
    if( NULL == ( g_ptexLightMap = D3DTextr_GetTexture( "spotlite.bmp" ) ) )
        return E_FAIL;

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, -3.0 );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1,   0  );
    D3DMATRIX matWorld, matView, matProj;

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0xffffffff );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
VOID App_DeleteDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice, 
                              LPDIRECT3DVIEWPORT3 pvViewport)
{
    D3DTextr_InvalidateAllTextures();
}

//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3 pd3dDevice, 
                          LPDIRECT3DVIEWPORT3 pvViewport)
{
    App_DeleteDeviceObjects( pd3dDevice, pvViewport );

    return S_OK;
}

//----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//----------------------------------------------------------------------------
HRESULT App_RestoreSurfaces()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice( DDCAPS* pddDriverCaps,
                           D3DDEVICEDESC* pd3dDeviceDesc )
{
    return S_OK;
}