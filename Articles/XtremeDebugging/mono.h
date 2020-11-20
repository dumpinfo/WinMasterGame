
// MONO.H - HEADER FOR MONOCHROME PRINTING ENGINE ///////////////////////////////

#ifndef MONOCHROME_H					// watch out for multiple inclusions
#define MONOCHROME_H

// colors and styles for printing

#define MONO_BLACK                0		
#define MONO_DARK                 7
#define MONO_DARK_UNDERLINE       1
#define MONO_BRIGHT               15
#define MONO_BRIGHT_UNDERLINE     9
#define MONO_BYTES_PER_LINE       160

// dimensions of monchrome display

#define MONO_ROWS                 25	
#define MONO_COLUMNS              80

// watch variable defines

#define	MONO_MAX_WATCHES			64	// maximum number of watches

#define MONO_WATCH_CHAR				0	// types of watches, needed to compute RTTI
#define MONO_WATCH_UCHAR			1
#define MONO_WATCH_SHORT			2
#define MONO_WATCH_USHORT			3
#define MONO_WATCH_INT				4
#define MONO_WATCH_UINT				5	
#define MONO_WATCH_STRING			6
#define MONO_WATCH_STRING_HEX		7
#define MONO_WATCH_FLOAT			8
#define MONO_WATCH_PTR              9

// TYPES //////////////////////////////////////////////////////////////////////

// some convienient types

typedef unsigned short USHORT;
typedef unsigned char  UCHAR;

// this structure is used to hold the parameters for a "watch" variable

typedef struct mono_watch_tag
{
char name[32];		// the name of the variable
void *data;			// pointer to the data to watch
int type;			// type of data being watched
int x,y;			// position to display watch
int field_width;	// the width you want cleared in watch display
					// this is so you don't get garbage 

} mono_watch, *_monowatch_ptr;

// CLASSES ////////////////////////////////////////////////////////////////////

class mono_print						// monochrome display class
{

public:

	mono_print(void);					// constructor 
	~mono_print() {}					// destructor does nothing for now
	
	void print(char *string);			// prints a string at the current cursor location 
										// and style, similar to printf supports scrolling etc.
	
	void draw(char *string,				// "draws" a string anywhere with sent style
		 int x,							// no scrolling or logic
		 int y,
		 int style); 

	void set_cursor(int x, int y);		// positions the cursor in the 80x25 matrix
	void get_cursor(int &x, int &y);	// retrieves the position of the cursor
	void set_style(int new_style);		// sets the style of output
	void enable();						// enables output
	void disable();						// disables output
	void scroll(int num_lines);			// scrolls the display from the bottom
	void clear();						// this function clears the display	

// methods for debugging watch display

	void delete_watches(void);			// deletes all watches
	
	int add_watch(char *name,			// ascii name of variable, up to 15 chars
				void *addr,				// address of variable to watch
				int type,				// type of variable
				int x,					// position of variable
				int y,
				int field_width);		// size of output field

	void update_watches(void);			// simply displays all the watches

private:

	int cx,cy;							// position of printing cursor on 80x25 matrix
	int style;							// style of output, dark, bright, underlined, etc.
	int output_enable;					// used to "gate" output to monitor
	USHORT *mono_video;					// pointer to the monochrome video buffer
	
	int num_watches;					// number of active watches	
	mono_watch watch[MONO_MAX_WATCHES];	// pre-allocate the watch structures to keep
										// code simple, IRL use a linked list
};

#endif