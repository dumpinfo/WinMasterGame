#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"


//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Simple textured cube" );
BOOL   g_bAppUseZBuffer    = TRUE;    // Create/use a z-buffering
BOOL   g_bAppUseBackBuffer = TRUE;    // Create/use a back buffer




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl     = NULL;
LPDIRECT3DLIGHT     g_pLight              = NULL;

D3DVERTEX* g_pvRenderVertices = NULL;            //object's vertices
DWORD      g_dwNumVertices;


#define NUM_CUBE_VERTICES (4*6)
#define NUM_CUBE_INDICES  (6*6)
D3DVERTEX          g_pVertices[NUM_CUBE_VERTICES];
WORD               g_pIndices[NUM_CUBE_INDICES];

//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
VOID    RotateVertexInX( FLOAT, DWORD, D3DVERTEX*, D3DVERTEX* );

//-----------------------------------------------------------------------------
// Name: CreateCube()
// Desc: Sets up the vertices for a cube.
//-----------------------------------------------------------------------------
HRESULT CreateCube( D3DVERTEX* pVertices, WORD* pIndices )
{
    // Define the normals for the cube
    D3DVECTOR n0( 0.0f, 0.0f,-1.0f ); // Front face
    D3DVECTOR n1( 0.0f, 0.0f, 1.0f ); // Back face
    D3DVECTOR n2( 0.0f,-1.0f, 0.0f ); // Top face
    D3DVECTOR n3( 0.0f, 1.0f, 0.0f ); // Bottom face
    D3DVECTOR n4(-1.0f, 0.0f, 0.0f ); // Right face
    D3DVECTOR n5( 1.0f, 0.0f, 0.0f ); // Left face

    // Set up the vertices for the cube. Note: to prevent tiling problems,
    // the u/v coords are knocked slightly inwards.

    // Front face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n0, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n0, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n0, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n0, 0.01f, 0.01f );

    // Back face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n1, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n1, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n1, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n1, 0.01f, 0.99f );

    // Top face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n2, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n2, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n2, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n2, 0.01f, 0.01f );

    // Bottom face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n3, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n3, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n3, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n3, 0.99f, 0.99f );

    // Right face
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f,-1.0f), n4, 0.01f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f, 1.0f, 1.0f), n4, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f, 1.0f), n4, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR( 1.0f,-1.0f,-1.0f), n4, 0.01f, 0.01f );

    // Left face
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f,-1.0f), n5, 0.99f, 0.99f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f,-1.0f), n5, 0.99f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f,-1.0f, 1.0f), n5, 0.01f, 0.01f );
    *pVertices++ = D3DVERTEX( D3DVECTOR(-1.0f, 1.0f, 1.0f), n5, 0.01f, 0.99f );

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	// Generate the cube data
    CreateCube( g_pVertices, g_pIndices );
    
	RotateVertexInX( (FLOAT)(g_PI/2), g_dwNumVertices, g_pvRenderVertices,
                     g_pvRenderVertices );

    // Create textures
    D3DTextr_CreateTexture( "Stitches.bmp" );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
	
    // Compute the bend and rotate angles for this frame
    static FLOAT fTime = 0.0f;
    FLOAT fRotateAngle = (FLOAT)( fTime / 7 );
    fTime += .05f;

    // Setup the world spin matrix
    D3DMATRIX matWorldSpin;
    D3DUtil_SetRotateYMatrix( matWorldSpin, fRotateAngle );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorldSpin );

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
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                      g_pVertices, NUM_CUBE_VERTICES,
                                      g_pIndices,  NUM_CUBE_INDICES, 0 );

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

    // Get a ptr to the ID3D object to create materials and/or lights. Note:
    // the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    HRESULT           hr;
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    
    // Create and set up the object material
    if( FAILED( hr = pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    mtrl.power = 40.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0x40404040 );

    D3DTextr_RestoreAllTextures( pd3dDevice );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("Stitches.bmp") );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0, 0, -6.5 );
    D3DVECTOR vLookatPt = D3DVECTOR( 0, 0,   0  );
    D3DVECTOR vUpVec    = D3DVECTOR( 0, 1,   0  );
    D3DMATRIX matWorld, matView, matProj;

    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );

    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, 1 );

    // Set up the light
    if( FAILED( hr = pD3D->CreateLight( &g_pLight, NULL ) ) )
        return E_FAIL;
    
    D3DLIGHT light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0, 0.0, -12.0 );
    light.dcvColor.r = D3DVAL(0.9);
    light.dcvColor.g = D3DVAL(0.9);
    light.dcvColor.b = D3DVAL(0.9);
    light.dvAttenuation0 = 1.0f;
    g_pLight->SetLight( &light );
    pvViewport->AddLight( g_pLight );

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

    SAFE_RELEASE( g_pLight );
    SAFE_RELEASE( g_pmtrlObjectMtrl );
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
    
    SAFE_DELETE( g_pvRenderVertices );

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

//-----------------------------------------------------------------------------
// Name: RotateVertexInX()
// Desc: Rotates an array of vertices by an amount theta about the x-axis.
//-----------------------------------------------------------------------------
VOID RotateVertexInX( FLOAT fTheta, DWORD dwCount,
                      D3DVERTEX* pvInVertices, D3DVERTEX* pvOutVertices )
{
    FLOAT fSin = (FLOAT)sin(fTheta); 
    FLOAT fCos = (FLOAT)cos(fTheta);
    
    for( DWORD i=0; i<dwCount; i++ )
    {
        FLOAT y = pvInVertices[i].y;
        FLOAT z = pvInVertices[i].z;
        pvOutVertices[i].y = fCos*y + fSin*z;
        pvOutVertices[i].z = -fSin*y + fCos*z;

        FLOAT ny = pvInVertices[i].ny;
        FLOAT nz = pvInVertices[i].nz;
        pvOutVertices[i].ny = fCos*ny + fSin*nz;
        pvOutVertices[i].nz = -fSin*ny + fCos*nz;
    }
}
