// TMAPPER.CPP - The texture mapping function
// compile this and link it to the TMAPDEMO.CPP to
// make a complete executable
// Affine Texture Mapper, for 256 color linear
// memory 64x64 pixel texture mapping of 3 vertex polys.
// there's an example at the bottom!

#include <iostream.h> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>

#include "tmapper.h" // include all the header info and classes

// GLOBALS /////////////////////////////////////////////////////////////////

// the viewport to clip to, can be smaller than the screen
extern int poly_clip_min_x,
           poly_clip_max_x,
           poly_clip_min_y,
           poly_clip_max_y;

// FUNCTIONS ////////////////////////////////////////////////////////////////


void Draw_Textured_Triangle(void *vface,          // ptr to face
                            UCHAR *dest_buffer,   // pointer to video buffer
                            int mem_pitch)        // bytes per line, 320, 640 etc.
{
// this function draws a textured triangle

FACE3D_PTR face; // working face

int v0=0,
    v1=1,
	v2=2,
	temp=0,
	tri_type = TRI_TYPE_NONE,
	irestart = INTERP_LHS;

int dx,dy,dyl,dyr,      // general deltas
    u,v,
    du,dv,
    xi,yi,              // the current interpolated x,y
	ui,vi,              // the current interpolated u,v
	index_x,index_y,    // looping vars
	x,y,                // hold general x,y
	xstart,
	xend,
	ystart,
	yrestart,
	yend,
	xl,                 
	dxdyl,              
	xr,
	dxdyr,             
	dudyl,    
	ul,
	dvdyl,   
	vl,
	dudyr,
	ur,
	dvdyr,
	vr;

int x0,y0,tu0,tv0,    // cached vertices
	x1,y1,tu1,tv1,
	x2,y2,tu2,tv2;

UCHAR *screen_ptr  = NULL,
	  *screen_line = NULL,
	  *textmap     = NULL;

// convert void ptr to face
face = (FACE3D_PTR)vface;

// extract texture map
textmap = face->texture;

// first trivial clipping rejection tests 
if (((face->tlist[0].y < poly_clip_min_y)  && 
	 (face->tlist[1].y < poly_clip_min_y)  &&
	 (face->tlist[2].y < poly_clip_min_y)) ||

	((face->tlist[0].y > poly_clip_max_y)  && 
	 (face->tlist[1].y > poly_clip_max_y)  &&
	 (face->tlist[2].y > poly_clip_max_y)) ||

	((face->tlist[0].x < poly_clip_min_x)  && 
	 (face->tlist[1].x < poly_clip_min_x)  &&
	 (face->tlist[2].x < poly_clip_min_x)) ||

	((face->tlist[0].x > poly_clip_max_x)  && 
	 (face->tlist[1].x > poly_clip_max_x)  &&
	 (face->tlist[2].x > poly_clip_max_x)))
   return;

// degenerate triangle
if ( ((face->tlist[0].x==face->tlist[1].x) && (face->tlist[1].x==face->tlist[2].x)) ||
	 ((face->tlist[0].y==face->tlist[1].y) && (face->tlist[1].y==face->tlist[2].y)))
   return;

// sort vertices
if (face->tlist[v1].y < face->tlist[v0].y) 
	{SWAP(v0,v1,temp);} 

if (face->tlist[v2].y < face->tlist[v0].y) 
	{SWAP(v0,v2,temp);}

if (face->tlist[v2].y < face->tlist[v1].y) 
	{SWAP(v1,v2,temp);}

// now test for trivial flat sided cases
if (face->tlist[v0].y==face->tlist[v1].y)
	{ 
	// set triangle type
	tri_type = TRI_TYPE_FLAT_TOP;

	// sort vertices left to right
	if (face->tlist[v1].x < face->tlist[v0].x) 
		{SWAP(v0,v1,temp);}

	} // end if
else
// now test for trivial flat sided cases
if (face->tlist[v1].y==face->tlist[v2].y)
	{ 
	// set triangle type
	tri_type = TRI_TYPE_FLAT_BOTTOM;

	// sort vertices left to right
	if (face->tlist[v2].x < face->tlist[v1].x) 
		{SWAP(v1,v2,temp);}
	
	} // end if
else
	{
	// must be a general triangle
	tri_type = TRI_TYPE_GENERAL;

	} // end else

// extract vertices for processing, now that we have order
x0  = face->tlist[v0].x;
y0  = face->tlist[v0].y;
tu0 = face->tlist[v0].u;
tv0 = face->tlist[v0].v;

x1  = face->tlist[v1].x;
y1  = face->tlist[v1].y;
tu1 = face->tlist[v1].u;
tv1 = face->tlist[v1].v;

x2  = face->tlist[v2].x;
y2  = face->tlist[v2].y;
tu2 = face->tlist[v2].u;
tv2 = face->tlist[v2].v;

// set interpolation restart value
yrestart = y1;

// what kind of triangle
if (tri_type & TRI_TYPE_FLAT_MASK)
	{

	if (tri_type == TRI_TYPE_FLAT_TOP)
	{
	// compute all deltas
	dy = (y2 - y0);

	dxdyl = ((x2 - x0)   << FIXP16_SHIFT)/dy;
	dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;  
	dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;    

	dxdyr = ((x2 - x1)   << FIXP16_SHIFT)/dy;
	dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;  
	dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;   

	// test for y clipping
	if (y0 < poly_clip_min_y)
		{
		// compute overclip
		dy = (poly_clip_min_y - y0);

		// computer new LHS starting values
		xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
		ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
		vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);

		// compute new RHS starting values
		xr = dxdyr*dy + (x1  << FIXP16_SHIFT);
		ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
		vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);

		// compute new starting y
		ystart = poly_clip_min_y;

		} // end if
	else
		{
		// no clipping

		// set starting values
		xl = (x0 << FIXP16_SHIFT);
		xr = (x1 << FIXP16_SHIFT);

		ul = (tu0 << FIXP16_SHIFT);
		vl = (tv0 << FIXP16_SHIFT);

		ur = (tu1 << FIXP16_SHIFT);
		vr = (tv1 << FIXP16_SHIFT);

		// set starting y
		ystart = y0;

		} // end else

	} // end if flat top
	else
	{
	// must be flat bottom

	// compute all deltas
	dy = (y1 - y0);

	dxdyl = ((x1 - x0)   << FIXP16_SHIFT)/dy;
	dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;  
	dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;    

	dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dy;
	dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;  
	dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;   

	// test for y clipping
	if (y0 < poly_clip_min_y)
		{
		// compute overclip
		dy = (poly_clip_min_y - y0);

		// computer new LHS starting values
		xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
		ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
		vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);

		// compute new RHS starting values
		xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
		ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
		vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);

		// compute new starting y
		ystart = poly_clip_min_y;

		} // end if
	else
		{
		// no clipping

		// set starting values
		xl = (x0 << FIXP16_SHIFT);
		xr = (x0 << FIXP16_SHIFT);

		ul = (tu0 << FIXP16_SHIFT);
		vl = (tv0 << FIXP16_SHIFT);

		ur = (tu0 << FIXP16_SHIFT);
		vr = (tv0 << FIXP16_SHIFT);

		// set starting y
		ystart = y0;

		} // end else	

	} // end else flat bottom

	// test for bottom clip, always
	if ((yend = y2) > poly_clip_max_y)
		yend = poly_clip_max_y;

    // test for horizontal clipping
	if ((x0 < poly_clip_min_x) || (x0 > poly_clip_max_x) ||
		(x1 < poly_clip_min_x) || (x1 > poly_clip_max_x) ||
		(x2 < poly_clip_min_x) || (x2 > poly_clip_max_x))
	{
    // clip version

	// point screen ptr to starting line
	screen_ptr = dest_buffer + (ystart * mem_pitch);

	for (yi = ystart; yi<=yend; yi++)
		{
		// compute span endpoints
		xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

		// compute starting points for u,v interpolants
		ui = ul + FIXP16_ROUND_UP;
		vi = vl + FIXP16_ROUND_UP;
		
		// compute u,v interpolants
		if ((dx = (xend - xstart))>0)
			{
			du = (ur - ul)/dx;
			dv = (vr - vl)/dx;
			} // end if
		else
			{
			du = (ur - ul);
			dv = (vr - vl);
			} // end else

		///////////////////////////////////////////////////////////////////////

		// test for x clipping, LHS
		if (xstart < poly_clip_min_x)
			{
			// compute x overlap
			dx = poly_clip_min_x - xstart;

			// slide interpolants over
			ui+=dx*du;
			vi+=dx*dv;
			
			// reset vars
			xstart = poly_clip_min_x;

			} // end if
		
		// test for x clipping RHS
		if (xend > poly_clip_max_x)
			xend = poly_clip_max_x;

		///////////////////////////////////////////////////////////////////////

		// draw span
		for (xi=xstart; xi<=xend; xi++)
			{
			// write textel
                        screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) +
                        ((vi >> FIXP16_SHIFT) << 6)];
			
			// interpolate u,v
			ui+=du;
			vi+=dv;
			} // end for xi

		// interpolate u,v,x along right and left edge
		xl+=dxdyl;
		ul+=dudyl;
		vl+=dvdyl;
	
		xr+=dxdyr;
		ur+=dudyr;
		vr+=dvdyr;
 
		// advance screen ptr
		screen_ptr+=mem_pitch;

		} // end for y

	} // end if clip
	else
	{
	// non-clip version

	// point screen ptr to starting line
	screen_ptr = dest_buffer + (ystart * mem_pitch);

	for (yi = ystart; yi<=yend; yi++)
		{
		// compute span endpoints
		xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		
		// compute starting points for u,v interpolants
		ui = ul + FIXP16_ROUND_UP;
		vi = vl + FIXP16_ROUND_UP;
	
		// compute u,v interpolants
		if ((dx = (xend - xstart))>0)
			{
			du = (ur - ul)/dx;
			dv = (vr - vl)/dx;
			} // end if
		else
			{
			du = (ur - ul);
			dv = (vr - vl);
			} // end else

		// draw span
		for (xi=xstart; xi<=xend; xi++)
			{
			// write textel
                        screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) +
                        ((vi >> FIXP16_SHIFT) << 6)];
			
			// interpolate u,v
			ui+=du;
			vi+=dv;
			} // end for xi

		// interpolate u,v,x along right and left edge
		xl+=dxdyl;
		ul+=dudyl;
		vl+=dvdyl;
	
		xr+=dxdyr;
		ur+=dudyr;
		vr+=dvdyr;

		// advance screen ptr
		screen_ptr+=mem_pitch;

		} // end for y

	} // end if non-clipped

	} // end if
else
if (tri_type==TRI_TYPE_GENERAL)
	{

	// first test for bottom clip, always
	if ((yend = y2) > poly_clip_max_y)
		yend = poly_clip_max_y;

	// pre-test y clipping status
	if (y1 < poly_clip_min_y)
		{
		// compute all deltas
		// LHS
		dyl = (y2 - y1);

		dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
		dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
		dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    

		// RHS
		dyr = (y2 - y0);	

		dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
		dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
		dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
		
		// compute overclip
		dyr = (poly_clip_min_y - y0);
		dyl = (poly_clip_min_y - y1);

		// computer new LHS starting values
		xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
		ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
		vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);

		// compute new RHS starting values
		xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
		ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
		vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);

		// compute new starting y
		ystart = poly_clip_min_y;

		// test if we need swap to keep rendering left to right
		if (dxdyr > dxdyl)
			{
			SWAP(dxdyl,dxdyr,temp);
			SWAP(dudyl,dudyr,temp);
			SWAP(dvdyl,dvdyr,temp);
			SWAP(xl,xr,temp);
			SWAP(ul,ur,temp);
			SWAP(vl,vr,temp);
			SWAP(x1,x2,temp);
			SWAP(y1,y2,temp);
			SWAP(tu1,tu2,temp);
			SWAP(tv1,tv2,temp);
		
			// set interpolation restart
			irestart = INTERP_RHS;

			} // end if

		} // end if
	else
	if (y0 < poly_clip_min_y)
		{
		// compute all deltas
		// LHS
		dyl = (y1 - y0);

		dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
		dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
		dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    

		// RHS
		dyr = (y2 - y0);	

		dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
		dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
		dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
		
		// compute overclip
		dy = (poly_clip_min_y - y0);

		// computer new LHS starting values
		xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
		ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
		vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);

		// compute new RHS starting values
		xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
		ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
		vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);

		// compute new starting y
		ystart = poly_clip_min_y;

		// test if we need swap to keep rendering left to right
		if (dxdyr < dxdyl)
			{
			SWAP(dxdyl,dxdyr,temp);
			SWAP(dudyl,dudyr,temp);
			SWAP(dvdyl,dvdyr,temp);
			SWAP(xl,xr,temp);
			SWAP(ul,ur,temp);
			SWAP(vl,vr,temp);
			SWAP(x1,x2,temp);
			SWAP(y1,y2,temp);
			SWAP(tu1,tu2,temp);
			SWAP(tv1,tv2,temp);
		
			// set interpolation restart
			irestart = INTERP_RHS;

			} // end if

		} // end if
	else
		{
		// no initial y clipping
	
		// compute all deltas
		// LHS
		dyl = (y1 - y0);

		dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
		dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
		dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    

		// RHS
		dyr = (y2 - y0);	

		dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
		dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
		dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		

		// no clipping y

		// set starting values
		xl = (x0 << FIXP16_SHIFT);
		xr = (x0 << FIXP16_SHIFT);

		ul = (tu0 << FIXP16_SHIFT);
		vl = (tv0 << FIXP16_SHIFT);

		ur = (tu0 << FIXP16_SHIFT);
		vr = (tv0 << FIXP16_SHIFT);

		// set starting y
		ystart = y0;

		// test if we need swap to keep rendering left to right
		if (dxdyr < dxdyl)
			{
			SWAP(dxdyl,dxdyr,temp);
			SWAP(dudyl,dudyr,temp);
			SWAP(dvdyl,dvdyr,temp);
			SWAP(xl,xr,temp);
			SWAP(ul,ur,temp);
			SWAP(vl,vr,temp);
			SWAP(x1,x2,temp);
			SWAP(y1,y2,temp);
			SWAP(tu1,tu2,temp);
			SWAP(tv1,tv2,temp);
		
			// set interpolation restart
			irestart = INTERP_RHS;

			} // end if

		} // end else


    // test for horizontal clipping
	if ((x0 < poly_clip_min_x) || (x0 > poly_clip_max_x) ||
		(x1 < poly_clip_min_x) || (x1 > poly_clip_max_x) ||
		(x2 < poly_clip_min_x) || (x2 > poly_clip_max_x))
	{
    // clip version
	// x clipping	

	// point screen ptr to starting line
	screen_ptr = dest_buffer + (ystart * mem_pitch);

	for (yi = ystart; yi<=yend; yi++)
		{
		// compute span endpoints
		xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		
		// compute starting points for u,v interpolants
		ui = ul + FIXP16_ROUND_UP;
		vi = vl + FIXP16_ROUND_UP;
	
		// compute u,v interpolants
		if ((dx = (xend - xstart))>0)
			{
			du = (ur - ul)/dx;
			dv = (vr - vl)/dx;
			} // end if
		else
			{
			du = (ur - ul);
			dv = (vr - vl);
			} // end else

		///////////////////////////////////////////////////////////////////////

		// test for x clipping, LHS
		if (xstart < poly_clip_min_x)
			{
			// compute x overlap
			dx = poly_clip_min_x - xstart;

			// slide interpolants over
			ui+=dx*du;
			vi+=dx*dv;
			
			// set x to left clip edge
			xstart = poly_clip_min_x;

			} // end if
		
		// test for x clipping RHS
		if (xend > poly_clip_max_x)
			xend = poly_clip_max_x;

		///////////////////////////////////////////////////////////////////////

		// draw span
		for (xi=xstart; xi<=xend; xi++)
			{
			// write textel
            screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << 6)];
			
			// interpolate u,v
			ui+=du;
			vi+=dv;
			} // end for xi

		// interpolate u,v,x along right and left edge
		xl+=dxdyl;
		ul+=dudyl;
		vl+=dvdyl;
	
		xr+=dxdyr;
		ur+=dudyr;
		vr+=dvdyr;

		// advance screen ptr
		screen_ptr+=mem_pitch;

		// test for yi hitting second region, if so change interpolant
		if (yi==yrestart)
			{

		// test interpolation side change flag

			if (irestart == INTERP_LHS)
			{
			// LHS
			dyl = (y2 - y1);	

			dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		

			// set starting values
			xl = (x1  << FIXP16_SHIFT);
			ul = (tu1 << FIXP16_SHIFT);
			vl = (tv1 << FIXP16_SHIFT);

			// interpolate down on LHS to even up
			xl+=dxdyl;
			ul+=dudyl;
			vl+=dvdyl;
			} // end if
			else
			{
			// RHS
			dyr = (y1 - y2);	

			dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		

			// set starting values
			xr = (x2  << FIXP16_SHIFT);
			ur = (tu2 << FIXP16_SHIFT);
			vr = (tv2 << FIXP16_SHIFT);

			// interpolate down on RHS to even up
			xr+=dxdyr;
			ur+=dudyr;
			vr+=dvdyr;
		
			} // end else


			} // end if

		} // end for y

	} // end if
	else
	{
	// no x clipping
	// point screen ptr to starting line
	screen_ptr = dest_buffer + (ystart * mem_pitch);

	for (yi = ystart; yi<=yend; yi++)
		{
		// compute span endpoints
		xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
		
		// compute starting points for u,v interpolants
		ui = ul + FIXP16_ROUND_UP;
		vi = vl + FIXP16_ROUND_UP;
	
		// compute u,v interpolants
		if ((dx = (xend - xstart))>0)
			{
			du = (ur - ul)/dx;
			dv = (vr - vl)/dx;
			} // end if
		else
			{
			du = (ur - ul);
			dv = (vr - vl);
			} // end else

		// draw span
		for (xi=xstart; xi<=xend; xi++)
			{
			// write textel
            screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << 6)];
			
			// interpolate u,v
			ui+=du;
			vi+=dv;
			} // end for xi

		// interpolate u,v,x along right and left edge
		xl+=dxdyl;
		ul+=dudyl;
		vl+=dvdyl;
	
		xr+=dxdyr;
		ur+=dudyr;
		vr+=dvdyr;

		// advance screen ptr
		screen_ptr+=mem_pitch;

		// test for yi hitting second region, if so change interpolant
		if (yi==yrestart)
			{
			// test interpolation side change flag

			if (irestart == INTERP_LHS)
			{
			// LHS
			dyl = (y2 - y1);	

			dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		

			// set starting values
			xl = (x1  << FIXP16_SHIFT);
			ul = (tu1 << FIXP16_SHIFT);
			vl = (tv1 << FIXP16_SHIFT);

			// interpolate down on LHS to even up
			xl+=dxdyl;
			ul+=dudyl;
			vl+=dvdyl;
			} // end if
			else
			{
			// RHS
			dyr = (y1 - y2);	

			dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		

			// set starting values
			xr = (x2  << FIXP16_SHIFT);
			ur = (tu2 << FIXP16_SHIFT);
			vr = (tv2 << FIXP16_SHIFT);

			// interpolate down on RHS to even up
			xr+=dxdyr;
			ur+=dudyr;
			vr+=dvdyr;
		
			} // end else

			} // end if

		} // end for y

	} // end else	

	} // end if

} // end Draw_Textured_Triangle

/******************************* EXAMPLE ***********************************

This is the setup:

To create a single triangle to be rendered consisting of the 3
points (in CC order I think):

(x0,y0,u0,v0)
(x1,y1,u1,v1)
(x2,y2,u2,v2)

64x64 texture at address texture_buffer in row major form

FACE3D face; // the triangle object

// set up the vertices
face.tlist[0].x = x0;
face.tlist[0].y = y0;
face.tlist[0].u = u0;
face.tlist[0].v = v0;

face.tlist[1].x = x1;
face.tlist[1].y = y1;
face.tlist[1].u = u1;
face.tlist[1].v = v1;

face.tlist[2].x = x2;
face.tlist[2].y = y2;
face.tlist[2].u = u2;
face.tlist[2].v = v2;

// assign the texture to your 64x64 texture map buffer
face.texture = texture_buffer;

// set the clipping coords of the desired 2D viewport
poly_clip_min_x = 0; 
poly_clip_max_x = 0;
poly_clip_min_y = SCREEN_WIDTH-1;
poly_clip_max_y = SCREEN_HEIGHT-1

// then draw the triangle to the rendering buffer
Draw_Textured_Triangle((void *)&face, double_buffer, memory_pitch);

***************************************************************************/
