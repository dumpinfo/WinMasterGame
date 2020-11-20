#include <d3drmwin.h>

#define SAFE_RELEASE(x) if (x != NULL) {x->Release(); x = NULL;}
#define MSG(str) MessageBox( NULL, str, "Application Message", MB_OK )

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
LPDIRECT3DRM3 lpD3DRM;
LPDIRECT3DRMFRAME3 lpScene;


//-----------------------------------------------------------------------------
// Name: AddLight
// Desc: Adds lights to a scene
//-----------------------------------------------------------------------------
BOOL AddLight(LPDIRECT3DRMFRAME3 frame, D3DRMLIGHTTYPE ltLightType, LPDIRECT3DRMLIGHT lplpD3DRMLight, D3DVALUE vRed, D3DVALUE vGreen, D3DVALUE vBlue, D3DVALUE pX, D3DVALUE pY, D3DVALUE pZ)
{

	if( FAILED(lpD3DRM->CreateFrame(lpScene, &frame)))
		return FALSE;
  
	if( FAILED(frame->SetPosition(lpScene, pX, pY, pZ)))
		return FALSE:

   if( FAILED(lpD3DRM->CreateLightRGB(ltLightType, vRed, vGreen, vBlue, &lplpD3DRMLight)))
		return FALSE;

   if( FAILED(frame->AddLight(lplpD3DRMLight)))
		return FALSE;
	
   lplpD3DRMLight->Release();
	frame->Release();

return TRUE;
}


//-----------------------------------------------------------------------------
// Name: BuildScene()
// Desc: Initializes the scene.
//-----------------------------------------------------------------------------
BOOL BuildScene( LPDIRECT3DRM3 pD3DRM,
				 LPDIRECT3DRMDEVICE3 pDevice, LPDIRECT3DRMVIEWPORT2,
				 LPDIRECT3DRMFRAME3 pScene, LPDIRECT3DRMFRAME3 pCamera )
{
    LPDIRECT3DRMFRAME3       pLightsFrame = NULL;
    LPDIRECT3DRMMESHBUILDER3 pMeshBuilder = NULL;
    LPDIRECT3DRMFRAME3       pMeshFrame   = NULL;
    LPDIRECT3DRMLIGHT       pLight1      = NULL;
    LPDIRECT3DRMLIGHT       pLight2      = NULL;
	
	lpD3DRM = pD3DRM;
	lpScene = pScene;

	AddLight(pLightsFrame, D3DRMLIGHT_AMBIENT, pLight1, 0.5f, 0.5f, 0.5f, D3DVALUE(17.0), D3DVALUE(17.0), D3DVALUE(-17.0));

    // Load mesh file
    if( FAILED( pD3DRM->CreateMeshBuilder( &pMeshBuilder ) ) )
		goto generic_error;
    if( FAILED( pMeshBuilder->Load( "cube.x", NULL, D3DRMLOAD_FROMFILE,
		                            NULL, NULL ) ) )
	{
        MSG("Failed to load cube.x\n" );
		goto ret_with_error;
    }

    // Create a frame for the mesh within the scene
    if( FAILED( pD3DRM->CreateFrame( pScene, &pMeshFrame ) ) )
		goto generic_error;

    // Add the loaded mesh into the frame
    if( FAILED( pMeshFrame->AddVisual( (LPDIRECT3DRMVISUAL)pMeshBuilder ) ) )
		goto generic_error;

    // Set up the frames position, orientation and rotation
    pCamera->SetPosition( pScene, 0.0f, 1.0f, -5.0f );
    pCamera->SetOrientation( pScene, 0.0f, -0.2f, 1.0f, 0.0f, 1.0f, 0.0f );
    pMeshFrame->SetRotation( pScene, D3DVAL(0), D3DVAL(1), D3DVAL(1), D3DVAL(0.02) );

    SAFE_RELEASE( pMeshFrame );
    SAFE_RELEASE( pLightsFrame );
    SAFE_RELEASE( pMeshBuilder );
    SAFE_RELEASE( pLight1 );
    SAFE_RELEASE( pLight2 );
    return TRUE;

generic_error:
    MSG("A failure occured while building the scene.\n");
ret_with_error:
    SAFE_RELEASE( pMeshFrame );
    SAFE_RELEASE( pLightsFrame );
    SAFE_RELEASE( pMeshBuilder );
    SAFE_RELEASE( pLight1 );
    SAFE_RELEASE( pLight2 );
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: OverrideDefualts()
// Desc: Let's the sample customize default behavior.
//-----------------------------------------------------------------------------
VOID OverrideDefaults( BOOL* pbNoTextures, BOOL* pbResizingDisabled, 
					   BOOL* pbConstRenderQuality, CHAR** pstrName )
{
	(*pbNoTextures) = TRUE;
    (*pstrName)     = "Light Demo";
}