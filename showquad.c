/*
   showquad.c
   by Gisli R. Hjaltason
   
   Display quadtree pictures in an X window.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define NeedFunctionPrototypes 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BUFSIZE 200

/* globals and defaults for print settings */

double pictscale = 4;
int linewidth = 1, fontsize = 8, showtext = 0;

char *progname;

Display *mydisplay;
Window mywindow;
Pixmap mypixmap;
GC mygc, mypixgc;
Font font;

/* prototypes */

void showfile(FILE *);
int convnumarg(char *);
double convfloatnumarg(char *);
void printerr(char *, ...);

void startpicture(int lx, int ly);
void endpicture();
void setlinedash(int black, int white);
void drawline(float x1, float y1, float x2, float y2);
void drawrect(float x1, float y1, float x2, float y2);
void drawdot(float x, float y, int r);
void drawstr(char *c, int len, float x, float y);

void eventloop(void);

main(int argc, char *argv[]) {
  FILE *inp = 0;
  XSizeHints myhint;
  int myscreen;
  unsigned long myforeground, mybackground;
  int i;
  char fontname[100];
  
  progname = *argv;	
  for (i = 1; i < argc && *argv[i] == '-'; ++i) {
    if (strlen(argv[i]) != 2)
      printerr("unknown option: %s", argv[i]);
    else if (argc - i < 1)
      printerr("missing argument: %s", argv[i]);
    else
      switch (argv[i][1]) {
      case 'f':
	fontsize = convnumarg(argv[++i]);
	break;
      case 'l':
	linewidth = convnumarg(argv[++i]);
	break;
      case 's':
	pictscale = convfloatnumarg(argv[++i]);
	break;
      case 't':
	showtext = 1;
	break;
      default:
	printerr("unknown option: %s", argv[i]);
	break;
      }
  }
  
  mydisplay = XOpenDisplay("");
  if (mydisplay == NULL) {
    printf("Error: Can't open display: %s\n", XDisplayName(fontname));
    printf("(check that DISPLAY environment variable is set and xhost has "
	   "been executed)\n");
    return 1;
  }
  myscreen = DefaultScreen(mydisplay);

  mybackground = WhitePixel(mydisplay, myscreen);
  myforeground = BlackPixel(mydisplay, myscreen);
  
  myhint.x = 200; myhint.y = 300;
  myhint.width = 350; myhint.height = 250;
  myhint.flags = PPosition | PSize;
  
  mywindow = XCreateSimpleWindow(mydisplay, DefaultRootWindow(mydisplay),
			 myhint.x, myhint.y, myhint.width, myhint.height,
			 5, myforeground, mybackground);
  XSetStandardProperties(mydisplay, mywindow, progname, progname,
			 None, argv, argc, &myhint);

  mygc = XCreateGC(mydisplay, mywindow, 0, 0);
  XSetBackground(mydisplay, mygc, mybackground);
  XSetForeground(mydisplay, mygc, myforeground);
	
  XSelectInput(mydisplay, mywindow,
	       ButtonPressMask | ExposureMask);

  sprintf(fontname, "-*-courier-medium-r-*-*-%d-*-*-*-*-*-*-*", fontsize);
  font = XLoadFont(mydisplay, fontname);
  
  if (i < argc)
    for (; i < argc; ++i) {
      if ((inp = fopen(argv[i], "r")) == NULL) {
	fprintf(stderr, "File cannot be opened: %s\n", argv[i]);
	exit(1);
      }
      else
	showfile(inp);
    }
  else
    showfile(stdin);
  
  XFreeGC(mydisplay, mygc);
  XDestroyWindow(mydisplay, mywindow);
  XCloseDisplay(mydisplay);
  return 0;
}

/* Show file contents. */

void showfile(FILE *inp) {
  char inbuf[BUFSIZE+1], c, name[30];
  float lx, ly, x, y, x1, y1, x2, y2;
  int r, black, white;
  static pictno = 0;
  
  while (fgets(inbuf, BUFSIZE, inp) != NULL) {
    if (sscanf(inbuf, "$$$$ SP(%f,%f)", &lx, &ly) == 2) {
      if (showtext)
	printf("\n#### DISPLAY %d ####\n\n", ++pictno);
      startpicture(lx, ly);
      
      while (fgets(inbuf, BUFSIZE, inp) != NULL)
	if (strncmp(inbuf, "EP", 2) == 0)
	  break;
	else if (sscanf(inbuf, "LD(%d,%d)", &black, &white) == 2)
	  setlinedash(black, white);
	else if (sscanf(inbuf, "DL(%f,%f,%f,%f)", &x1, &y1, &x2, &y2) == 4)
	  drawline(x1, y1+1, x2, y2+1);
	else if (sscanf(inbuf, "DR(%f,%f,%f,%f)", &x1, &y1, &x2, &y2) == 4)
	  drawrect(x1, y1+1, x2, y2+1);
	else if (sscanf(inbuf, "DD(%f,%f,%d)", &x, &y, &r) == 3)
	  drawdot(x, y+1, r);
	else if (sscanf(inbuf, "DC(%c,%f,%f)", &c, &x, &y) == 3)
	  drawstr(&c, 1, x, y+1);
	else if (sscanf(inbuf, "DN(%[^,],%f,%f)", name, &x, &y) == 3)
	  drawstr(name, strlen(name), x, y+1);
	else
	  fprintf(stderr, "Illegal drawing command: %s", inbuf);
      
      eventloop();
    }
    else if (showtext)
      printf("%s", inbuf);
  }
}

int wx, wy;

#define MAPX(x) ((x)*pictscale)
#define MAPY(y) (wy-(y)*pictscale)
#define SWAP(x,y) { float t = x; x = y; y = t; }
#define ISWAP(x,y) { int t = x; x = y; y = t; }

void eventloop(void)
{
  XEvent myevent;
  
  while (True) {
    XNextEvent(mydisplay, &myevent);
    switch (myevent.type) {
    case Expose:
      XCopyPlane(mydisplay, mypixmap, mywindow, mygc,
		 myevent.xexpose.x, myevent.xexpose.y,
		 myevent.xexpose.width, myevent.xexpose.height,
		 myevent.xexpose.x, myevent.xexpose.y, 1);
      break;
    case MappingNotify:
      XRefreshKeyboardMapping(&myevent.xmapping);
      break;
    case ButtonPress:
      return;
      break;
    }
  }
}

void startpicture(int lx, int ly)
{
  wx = rint(lx * pictscale);
  wy = rint(ly * pictscale);
  
  XResizeWindow(mydisplay, mywindow, wx, wy);
  XMapRaised(mydisplay, mywindow);
  XClearArea(mydisplay, mywindow, 0, 0, wx, wy, True);
  
  mypixmap = XCreatePixmap(mydisplay, mywindow, wx, wy, 1);
  mypixgc = XCreateGC(mydisplay, mypixmap, 0, 0);
  
  XSetForeground(mydisplay, mypixgc, 1);
  XSetBackground(mydisplay, mypixgc, 0);
  XSetFunction(mydisplay, mypixgc, GXclear);
  XFillRectangle(mydisplay, mypixmap, mypixgc, 0, 0, wx, wy);
  XSetFunction(mydisplay, mypixgc, GXcopy);
  XSetLineAttributes(mydisplay, mypixgc, linewidth, LineSolid, CapRound,
		     JoinRound);
  XSetFont(mydisplay, mypixgc, font);
}

void endpicture()
{
  XFreePixmap(mydisplay, mypixmap);
  XFreeGC(mydisplay, mypixgc);
}

void setlinedash(int black, int white)
{
  char dl[2] = { black, white };
  
  if (white == 0)
    XSetLineAttributes(mydisplay, mypixgc, linewidth, LineSolid, CapRound,
		       JoinRound);
  else {
    XSetLineAttributes(mydisplay, mypixgc, linewidth, LineOnOffDash,
		       CapRound, JoinRound);
    XSetDashes(mydisplay, mypixgc, 0, dl, 2);
  }
}

void drawline(float x1, float y1, float x2, float y2)
{
  int swapx, swapy, ix1, iy1, ix2, iy2;
  
  swapx = x1 > x2;
  swapy = y1 > y2;
  if (swapx)
    SWAP(x1, x2);
  if (swapy)
    SWAP(y1, y2);
  ix1 = floor(MAPX(x1));
  ix2 = ceil(MAPX(x2));
  iy1 = floor(MAPY(y1));
  iy2 = ceil(MAPY(y2));
  if (swapx != swapy)
    ISWAP(ix1, ix2);
  XDrawLine(mydisplay, mypixmap, mypixgc, ix1, iy1, ix2, iy2);
}

void drawrect(float left, float top, float right, float bottom)
{
  if (top < bottom)
    SWAP(top, bottom);
  XDrawRectangle(mydisplay, mypixmap, mypixgc, floor(MAPX(left)),
		 floor(MAPY(top)), ceil(MAPX(right))-floor(MAPX(left)),
		 ceil(MAPY(bottom))-floor(MAPY(top)));
}

void drawdot(float x, float y, int r)
{
  XFillArc(mydisplay, mypixmap, mypixgc, floor(MAPX(x)-r), floor(MAPY(y)-r),
	   2*r, 2*r, 0, 360*64);
}

void drawstr(char *str, int len, float x, float y)
{
  XDrawString(mydisplay, mypixmap, mypixgc, floor(MAPX(x)), floor(MAPY(y)),
	      str, len);
}

/* Convert the string to number. */

int convnumarg(char *s)
{
  int n;
  
  if (sscanf(s, "%d", &n) != 1)
    printerr("invalid argument: %s", s);
  return n;
};

double convfloatnumarg(char *s)
{
  double f;
  
  if (sscanf(s, "%lf", &f) != 1)
    printerr("invalid argument: %s", s);
  return f;
};

void printerr(char *format, ...)
{
  va_list args;
  
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
  fprintf(stderr,
	  "syntax: %s [-f n][-l n][-s n][-t] [file... | < file]\n",
	  progname);
  exit(1);
}
