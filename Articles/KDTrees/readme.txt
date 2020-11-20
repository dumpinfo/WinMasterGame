Hi, my name is Ivan Pocina and I am demonstrating how to build a kD tree for a given 3D world.
Here, we have 4 .3d example files to play around with, and yes I know they are not really big
and exciting .3d files.  But this doesn't matter because this is just a DEMO!!!

Anyway, my demo has the following features:

	-standard file I/O
	-rendering options:  point, wireframe, solid, textured

	-You can do some simple (and I mean simple) 3D editing.  Like any 3D editor, you can
	 move around and the controls for doing this is the following:
		
		Using the keyboard:
		
		-left/right arrows:	yaw left/right
		-up/down arrows:	translate forward/backward
		-page up/down:		pitch up/down
		-END:			level the view at eye-level
		-INSERT/DELETE:		translate upward/downward
		-< and >:		translate sideways left/right
		-; and ':		roll left/right

		Using the mouse:

		-translating in a XZ plane:	hold down RIGHT mouse button and drag
		-rotating (yaw/pitch combo):	hold down LEFT mouse button and drag

	-Now, if you want to actually do some editing, do the following:
		
		When the cross hair is over a polygon, it will turn yellow.  When this
		happens, hold down the LEFT mouse button and drag either in the left or
		right direction.  You will see that the polygon is dragged along its normal
		force.  However, once the cross hair leaves a poly (and none are highlighted),
		then the function of the LEFT mouse button changes back to yawing left or
		right.  Still, a bug exists here such that sometimes when your cross hair is
		OBVIOUSLY over a poly, the poly is NOT highlighted!  This will prevent you from
		doing any editing and leave you frustrated.  Therefore, you can ONLY edit a
		polygon only when it is actually highlighted.

	-If you want to build a kD tree for a given .3d file, select "Options | build kD tree"
	 and you will see the ACTUAL tree being built.  When it's done, it will generate a
	 .kd file.

That's about it.  If you have any questions or comments, you can contact me via phone at
(718) - 456 - 9641.  If I'm not there, you can try to reach me at (718) - 972 - 4466.  If
all else fails, email me at ipocina@aol.com.