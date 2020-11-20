xof 0302txt 0064
Material Material1 
	{
	0.000000;0.000000;1.000000;1.000000;;  // R = 0.0, G = 0.0., B = 255.0
	0.000000;
        	0.000000;0.000;0.000000;;
        	0.000000;0.000;0.000000;;
        	TextureFilename 
		{
          		"Win95.bmp";
        		}
	}

Material Material2 
	{
	1.000;0.000000;0.000000;1.000000;;  // R = 255.0, G = 255.0., B = 255.0
	0.000000;
        	0.000000;0.000000;0.000000;;
        	0.000000;0.000000;0.000000;;
        	TextureFilename 
		{
          		"Win95.bmp";
        		}
	}

// Specify a rectangle mesh.  This has 8 vertices and 12 triangular faces.
// You can use optional data objects in the mesh to specify materials, 
// normals, and texture coordinates.

Mesh MyRectMesh 
	{
	8;  // Specify the 8 vertices of the rectangle
        	1.000000;2.000000;-2.000000;,    // Vertex 1
        	-1.000000;2.000000;-2.000000;,   // Vertex 2
        	-1.000000;2.000000;2.000000;,    // Vertex 3
        	1.000000;2.000000;2.000000;,     // Vertex 4
        	1.000000;-2.000000;-2.000000;,   // Vertex 5
        	-1.000000;-2.000000;-2.000000;,  // Vertex 6
        	-1.000000;-2.000000;2.000000;,   // Vertex 7
        	1.000000;-2.000000;2.000000;,    // Vertex 8

	12;		// It has 12 faces (2 triangles per side)
	3;0,1,2;,	// Each face has 3 vertices
	3;0,2,3;,	
	3;0,4,5;,	
	3;0,5,1;,	
	3;1,5,6;,	
	3;1,6,2;,	
	3;2,6,7;,	
	3;2,7,3;,	
	3;3,7,4;,	
	3;3,4,0;,	
	3;4,7,6;,	
	3;4,6,5;;


// Now we define the optinoal data

MeshMaterialList 
	{
        	2;
        	12;
        	0,
        	0,
        	0,
        	0,
        	0,
        	0,
        	0,
        	0,
        	1,
        	1,
        	1,
        	1,
        	1;;
        	{Material1}
        	{Material2}
	}

MeshTextureCoords 
	{
        	8;
        	0.000000;1.000000;      // Define the texture coords for each vertex
        	1.000000;1.000000;
        	0.000000;1.000000;
        	1.000000;1.000000;
        	0.000000;0.000000;
        	1.000000;0.000000;
        	0.000000;0.000000;        
        	1.000000;0.000000;;
     	}
   }


Frame MyRectFrame {
  FrameTransformMatrix {
    1.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 1.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 1.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 1.000000;;
  }
  // You can have the mesh inline, but here
  // we will use an object referenece
  {MyRectMesh}
}

AnimationSet MyAnimationSet {
 Animation MyAnimation  {
    {MyRectFrame}
    AnimationKey {
      2;			// The position keys
      9; 			// 7 keys
      100; 3; -50.000000, 0.000000, 0.000000;;,
      101; 3; -50.000000, 0.000000, 0.000000;;,
      102; 3; -50.000000, 0.000000, 0.000000;;,
      200; 3; -50.000000, 0.000000, 0.0000000;;,
      300; 3; -40.000000, 0.0000000, 0.000000;;,
      400; 3; -25.500000, 0.000000, 0.000000;;,
      500; 3; 0.000000, 0.000000, 0.000000;;,
      600; 3; 25.500000, 0.000000, 0.000000;;,
      700; 3; 50.000000, 0.0000000, 0.0000000;;,
      800; 3; 75.500000, 0.500000, 0.000000;;,
      1200; 3; 100.00000, 0.000000, 0.000000;;;
    }
  }
}
