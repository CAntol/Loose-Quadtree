#include "drawing_c.h"

/*
	drawing.c
	
	A C/C++ implementation of drawing primitives.
*/




/*	Starts a new picture. This routine must be called before any
	other drawing routines are called. The size of the picture
	is assumed to be lx * ly, with the origin (0,0) in the lower
	left corner. You can draw outside of the boundary of the
	picture area, but it may not be displayed or printed.
	Output: $$$$ SP(lx,ly) */

void StartPicture(double lx, double ly)
{
		printf("$$$$ SP(%.2lf,%.2lf)\n", lx, ly);
}

/*	Marks the end of the current picture. After this call, no
	drawing routine other than StartPicture may be called.
	Output: EP */

void EndPicture(void)
{
	printf("EP\n");
}

/*	Sets the dash of subsequently drawn lines and rectangles.
	If a line is dashed, there are pieces of the line that are
	drawn, and pieces that are not drawn. The length of the
	drawn pieces is determined by black, and the length of the
	pieces that are not drawn is determined by white. If white
	is 0, the line is not dashed. The unit of the parameters
	depends on the display program (same as line width, unless
	otherwise defined).
	Output: LD(black,white) */

void SetLineDash(int black, int white)
{
	printf("LD(%d,%d)\n", black, white);
}

/*	Draws a line with end points (x1, y1) and (x2, y2). The current
	line dash is used.
	Output: DL(x1,y2,x2,y2) */

void DrawLine(double x1, double y1, double x2, double y2)
{
	printf("DL(%.2lf,%.2lf,%.2lf,%.2lf)\n", x1, y1, x2, y2);
}

/*	Draws a rectangle with top left corner at (x1, y1) and bottom
	right corner at (x2, y2). The current line dash is used.
	Output: DR(x1,y2,x2,y2) */

void DrawRect( double x1, double y1, double x2, double y2)
{
	printf("DR(%.2lf,%.2lf,%.2lf,%.2lf)\n", x1, y1, x2, y2);
}

/*	A dot centered at (x, y) with radius r is drawn. The unit of the
	radius is the line width, unless the output program defines a
	dash width.
	Output: DD(x,y,r) */

void DrawDot(double x, double y, int r)
{
	printf("DD(%.2lf,%.2lf,%d)\n", x, y, r);
}

/*	Draws a character, with the left side and base line of coordinate
	x and y, respectively. Do not make assumptions about the width of
	characters.
	Output: DC(c) */

void DrawChar(char c, double x, double y)
{
	printf("DC(%c,%.2lf,%.2lf)\n", c, x, y);
}

/*	Draws a name, with the left side and base line of the
	leftmost character of coordinate x and y, respectively. Do
	not make assumptions about the width of characters. The
	type nameptr is assumed to be a pointer to a record with
	fields LEN and STR, as in the data definition that you
	will be given. If you store your names differently, you
	need to change this routine.
	Output: DN(n) */

void DrawName(char *n, double x, double y)
{
	printf("DN(%s,%.2lf,%.2lf)\n", n, x, y);
}

