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

   lpD3DRM->CreateFrame(lpScene, &frame);

   frame->SetOrientation(lpScene,
        D3DVALUE(1.0), D3DVALUE(-4.0), D3DVALUE(1.0),
        D3DVALUE(0.0), D3DVALUE(1.0), D3DVALUE(1.0));
  
	frame->SetPosition(lpScene, pX, pY, pZ);

   lpD3DRM->CreateLightRGB(ltLightType, vRed, vGreen, vBlue, &lplpD3DRMLight);

   frame->AddLight(lplpD3DRMLight);
	
   lplpD3DRMLight->Release();
	frame->Release();

return TRUE;
}


//-----------------------------------------------------------------------------
// Name: BuildScene()
// Desc: Initializes the scene.
//-----------------------------------------------------------------------------
BOOL BuildScene( LPDIRECT3DRM3 pD3DRM, 
				 LPDIRECT3DRMDEVICE3 dev, LPDIRECT3DRMVIEWPORT2,
	             LPDIRECT3DRMFRAME3 scene, LPDIRECT3DRMFRAME3 camera)
{
    D3DRMRENDERQUALITY quality = D3DRMRENDER_FLAT;
    LPDIRECT3DRMFRAME3 lights = NULL;
    LPDIRECT3DRMMESHBUILDER3 teapot_builder = NULL;
    LPDIRECT3DRMMESH teapot_mesh = NULL;
    LPDIRECT3DRMSHADOW2 teapot_shadow = NULL;
    LPDIRECT3DRMFRAME3 teapot = NULL;
    LPDIRECT3DRMLIGHT shadow_light = NULL;
    LPDIRECT3DRMLIGHT l2 = NULL;

    if (FAILED( dev->SetQuality( quality)))
		goto generic_error;
    if (FAILED( scene->SetSceneBackgroundRGB( D3DVAL(1), D3DVAL(1),
                                         D3DVAL(1))))
		goto generic_error;

    // initialize the lights in the scene
    if (FAILED( pD3DRM->CreateFrame( scene, &lights)))
		goto generic_error;
    if (FAILED( lights->SetPosition( scene, D3DVAL(2), D3DVAL(5),
                                -D3DVAL(10))))
		goto generic_error;
    if (FAILED( pD3DRM->CreateLightRGB( D3DRMLIGHT_POINT, D3DVAL(0.9),
                                  D3DVAL(0.8), D3DVAL(0.7), &shadow_light)))
		goto generic_error;
    if (FAILED( lights->AddLight( shadow_light)))
		goto generic_error;

    if (FAILED( pD3DRM->CreateLightRGB( D3DRMLIGHT_AMBIENT, D3DVAL(0.1),
                                  D3DVAL(0.1), D3DVAL(0.1), &l2)))
		goto generic_error;
    if (FAILED( scene->AddLight( l2)))
		goto generic_error;

    // load mesh file
    if (FAILED( pD3DRM->CreateMeshBuilder( &teapot_builder)))
		goto generic_error;
    if( FAILED( teapot_builder->Load( "teapot.x", NULL,
    				D3DRMLOAD_FROMFILE, NULL, NULL) ) )
    {
		MSG("Failed to load teapot.x.\n");
		goto ret_with_error;
    }
    if (FAILED( teapot_builder->CreateMesh( &teapot_mesh)))
		goto generic_error;
    SAFE_RELEASE(teapot_builder);

    // create a teapot frame within the scene
    if (FAILED( pD3DRM->CreateFrame( scene, &teapot)))
		goto generic_error;
    if (FAILED( pD3DRM->CreateShadow( teapot_mesh, shadow_light, 
                                D3DVAL(0), D3DVAL(-3), D3DVAL(0), D3DVAL(0),
                                D3DVAL(1), D3DVAL(0), &teapot_shadow)))
		goto generic_error;

    // add the loaded mesh into the frame
    if (FAILED( teapot->AddVisual( teapot_mesh)))
		goto generic_error;
    if (FAILED( teapot->AddVisual( teapot_shadow)))
		goto generic_error;

    // set up the frames position, orientation and rotation

    if (FAILED( camera->SetPosition( scene, D3DVAL(0), D3DVAL(0), -D3DVAL(10))))
		goto generic_error;
    if (FAILED( camera->SetOrientation( scene, D3DVAL(0), D3DVAL(0), D3DVAL(1), D3DVAL(0),
                                   D3DVAL(1), D3DVAL(0))))
		goto generic_error;
    if (FAILED( teapot->SetRotation( scene, D3DVAL(0), D3DVAL(1), D3DVAL(1),
                                D3DVAL(0.02))))
		goto generic_error;

    SAFE_RELEASE(lights);
    SAFE_RELEASE(teapot_mesh);
    SAFE_RELEASE(teapot_shadow);
    SAFE_RELEASE(teapot);
    SAFE_RELEASE(shadow_light);
    SAFE_RELEASE(l2);
    return TRUE;
generic_error:
    MSG("A failure occurred while building the scene.\n");
ret_with_error:
    SAFE_RELEASE(lights);
    SAFE_RELEASE(teapot_builder);
    SAFE_RELEASE(teapot_mesh);
    SAFE_RELEASE(teapot_shadow);
    SAFE_RELEASE(teapot);
    SAFE_RELEASE(shadow_light);
    SAFE_RELEASE(l2);
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
    (*pstrName)     = "Color Demo";
}