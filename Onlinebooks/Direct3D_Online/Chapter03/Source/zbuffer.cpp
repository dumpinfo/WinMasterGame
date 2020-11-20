//-------------------------------------------------------------------
// DEFINES AND INCLUDES
//-------------------------------------------------------------------
#define STRICT
#define D3D_OVERLOADS
#include <math.h>
#include <d3d.h>

#define NUM_OBJECTS 4

//-------------------------------------------------------------------
// GLOBALS
//-------------------------------------------------------------------
D3DMATRIX           g_matLocal[NUM_OBJECTS];
LPDIRECT3DMATERIAL3 g_pmtrlObjectMtrl = NULL;
D3DVERTEX           g_pvTriangleVertices[6];




//-----------------------------------------------------------------------------
// App_InitDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
	// Data for the geometry of the triangle.
	D3DVECTOR p1( 0.0f, 3.0f, 0.0f );
	D3DVECTOR p2( 3.0f,-3.0f, 0.0f );
	D3DVECTOR p3(-3.0f,-3.0f, 0.0f );
	D3DVECTOR vNormal( 0.0f, 0.0f, 1.0f );
	
	// Initialize the 3 vertices for the front of the triangle
	g_pvTriangleVertices[0] = D3DVERTEX( p1, vNormal, 0.0f, 0.0f );
	g_pvTriangleVertices[1] = D3DVERTEX( p2, vNormal, 0.0f, 0.0f );
	g_pvTriangleVertices[2] = D3DVERTEX( p3, vNormal, 0.0f, 0.0f );
    
	// Initialize the 3 vertices for the back of the triangle
	g_pvTriangleVertices[3] = D3DVERTEX( p1, -vNormal, 0.0f, 0.0f );
	g_pvTriangleVertices[4] = D3DVERTEX( p3, -vNormal, 0.0f, 0.0f );
	g_pvTriangleVertices[5] = D3DVERTEX( p2, -vNormal, 0.0f, 0.0f );
    
	// Get a ptr to the ID3D object to create materials or lights.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    // Create the object material.
    if( FAILED( pD3D->CreateMaterial( &g_pmtrlObjectMtrl, NULL ) ) )
        return E_FAIL;

	// Set the object material as yellow.
    D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL) );
    mtrl.dwSize       = sizeof(D3DMATERIAL);
    mtrl.dcvAmbient.r = 1.0f;
    mtrl.dcvAmbient.g = 1.0f;
    mtrl.dcvAmbient.b = 1.0f;
    g_pmtrlObjectMtrl->SetMaterial( &mtrl );

	// Put the object material into effect.
    g_pmtrlObjectMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_MATERIAL, hmtrl );

	// The ambient lighting value is another state to set.
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,  0xffffffff );

    // Set the transform matrices.
    D3DMATRIX mat;
	mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
	mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
	mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
	mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
	
	// The world matrix controls the position and orientation of the polygons
	// in world space. We'll use it later to spin the triangle.
	D3DMATRIX matWorld = mat;
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

	// The view matrix defines the position and orientation of the camera.
	// Here, we are just moving it back along the z-axis by 10 units.
	D3DMATRIX matView = mat;
	matView._43 = 10.0f;
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

	// The projection matrix defines how the 3D scene is "projected" onto the
	// 2D render target (the backbuffer surface).
	D3DMATRIX matProj = mat;
	matProj._11 =  2.0f;
	matProj._22 =  2.0f;
	matProj._34 =  1.0f;
	matProj._43 = -1.0f;
	matProj._44 =  0.0f;
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	return S_OK;
}

//-----------------------------------------------------------------------------
// App_FrameMove
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{

	// For each object, set up a local rotation matrix to be applied right
	// before rendering the object's polygons.
	for( int i=0; i<NUM_OBJECTS; i++ )
	{
		ZeroMemory( &g_matLocal[i], sizeof(D3DMATRIX) );
		g_matLocal[i]._11 = (FLOAT)cos( fTimeKey + (3.14159*i)/NUM_OBJECTS );
		g_matLocal[i]._33 = (FLOAT)cos( fTimeKey + (3.14159*i)/NUM_OBJECTS );
		g_matLocal[i]._13 = (FLOAT)sin( fTimeKey + (3.14159*i)/NUM_OBJECTS );
		g_matLocal[i]._31 = (FLOAT)sin( fTimeKey + (3.14159*i)/NUM_OBJECTS );
		g_matLocal[i]._22 = g_matLocal[i]._44 = 1.0f;
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// App_Render()
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewportRect )
{
    // Clear the viewport and z buffer to a blue color.
    pvViewport->Clear2( 1UL, prcViewportRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		                0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( FAILED( pd3dDevice->BeginScene() ) )
		return E_FAIL;

	// Enable z-buffering.
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );

	// Draw all the objects.
	for( int i=0; i<NUM_OBJECTS; i++ )
	{
		// Alternate the color of every other object
		DWORD dwColor = ( i%2 ) ? 0x0000ff00 : 0x00ffff00;
		pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT, dwColor );

		// Set the local matrix for the object
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &g_matLocal[i] );

		// Draw the object.
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
			                       g_pvTriangleVertices, 6, NULL );
	}

    // End the scene.
    pd3dDevice->EndScene();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the device is being deleted, this function deletes any
//       device dependant objects.
//-----------------------------------------------------------------------------
VOID App_DeleteDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice, 
                              LPDIRECT3DVIEWPORT3 pvViewport )
{
	// Release the material that was created earlier.
    if( g_pmtrlObjectMtrl )
		g_pmtrlObjectMtrl->Release();
	g_pmtrlObjectMtrl = NULL;
}




