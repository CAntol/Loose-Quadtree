/*
   printquad.c
   by Gisli R. Hjaltason

   Generate PostScript code for quadtree pictures.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BUFSIZE 200

/* globals and defaults for print settings */

float linewidth = .1, dashdotwidth = .5, pictscale = 2, fontscale = .5;
char *headerstr = NULL, datestr[26];

char *progname;

/* prototypes */

void printfile(FILE *);
void genpsheader();
void genpstrailer();
char *fixstring(char *);
float convnumarg(char *);
void printerr(char *, ...);

main(int argc, char *argv[]) {
  FILE *inp = 0;
  int i;

  progname = *argv;	
  for (i = 1; i < argc && *argv[i] == '-'; ++i) {
    if (strlen(argv[i]) != 2)
      printerr("unknown option: %s", argv[i]);
    else if (argc - i < 1)
      printerr("missing argument: %s", argv[i]);
    else
      switch (argv[i][1]) {
      case 'd':
	dashdotwidth = convnumarg(argv[++i]);
	break;
      case 'f':
	fontscale = convnumarg(argv[++i]);
	break;
      case 'h':
	headerstr = argv[++i];
	break;
      case 'l':
	linewidth = convnumarg(argv[++i]);
	break;
      case 's':
	pictscale = convnumarg(argv[++i]);
	break;
      default:
	printerr("unknown option: %s", argv[i]);
	break;
      }
  }

  if (headerstr == NULL) {
    time_t clock;

    time(&clock);
    strftime(datestr, 25, "%c", localtime(&clock));
    headerstr = datestr;
  }

  genpsheader();

  if (i < argc)
    for (; i < argc; ++i) {
      if ((inp = fopen(argv[i], "r")) == NULL) {
	fprintf(stderr, "File cannot be opened: %s\n", argv[i]);
	exit(1);
      }
      else
	printfile(inp);
    }
  else
    printfile(stdin);

  genpstrailer();

  return 0;
}

/* Print out file. */

void printfile(FILE *inp)
{
  char inbuf[BUFSIZE+1], c, name[30];
  float lx, ly, x, y, x1, y1, x2, y2;
  int r, black, white;

  while (fgets(inbuf, BUFSIZE, inp) != NULL) {
    if (sscanf(inbuf, "$$$$ SP(%f,%f)", &lx, &ly) == 2) {
      printf("%.2f %.2f startpict\n", lx, ly);

      while (fgets(inbuf, BUFSIZE, inp) != NULL)
	if (strncmp(inbuf, "EP", 2) == 0)
	  break;
	else if (sscanf(inbuf, "LD(%d,%d)", &black, &white) == 2)
	  printf("%d %d setlinedash\n", black, white);
	else if (sscanf(inbuf, "DL(%f,%f,%f,%f)", &x1, &y1, &x2, &y2) == 4)
	  printf("%.2f %.2f %.2f %.2f drawline\n", x1, y1, x2, y2);
	else if (sscanf(inbuf, "DR(%f,%f,%f,%f)", &x1, &y1, &x2, &y2) == 4)
	  printf("%.2f %.2f %.2f %.2f drawrect\n", x1, y1, x2, y2);
	else if (sscanf(inbuf, "DD(%f,%f,%d)", &x, &y, &r) == 3)
	  printf("%.2f %.2f %d drawdot\n", x, y, r);
	else if (sscanf(inbuf, "DC(%c,%f,%f)", &c, &x, &y) == 3)
	  printf("(%c) %.2f %.2f drawstr\n", c, x, y);
	else if (sscanf(inbuf, "DN(%[^,],%f,%f)", name, &x, &y) == 3)
	  printf("(%s) %.2f %.2f drawstr\n", name, x, y);
	else
	  fprintf(stderr, "Illegal drawing command: %s", inbuf);
      puts("endpict");
    }
    else {
      printf("(%s) drawtxt\n", fixstring(inbuf));
    }
  }
}

/* Generate PostScript header and setup */

void genpsheader(void)
{
  puts( "%!PS-Adobe-1.0\n"
	"save\n"
	"/printquad 30 dict def printquad\n"
	"begin\n\n"
	"% library routines and globals\n"
	"/prepnewpage { 32 36 translate\n"
	"	/vpos top def\n"
	"	gsave 0 top txtheight 2 div add moveto\n"
	"	/pgno pgno 1 add def\n"
	"	boldtxtfont setfont\n"
	"	pgno ( ) cvs show\n"
	"	40 0 rmoveto\n"
	"	headerstr show grestore } def\n"
	"/startpict { pictscale mul dup vpos gt { showpage prepnewpage } if\n"
	"	vpos exch sub txtheight sub /vpos exch def pop\n"
	"	gsave\n"
	"	0 vpos 2 add translate\n"
	"	pictscale pictscale scale\n"
	"	/pictlinewidth linewidth pictscale div def\n"
	"	/pictdashdotwidth dashdotwidth pictscale div def\n"
	"	pictlinewidth setlinewidth\n"
	"	txtfont fontscale pictscale div scalefont setfont } def\n"
	"/endpict { grestore } def\n"
	"/setlinedash { dup 0 eq\n"
	"	{ [] 0 setdash pop pop }\n"
	"	{ pictdashdotwidth mul exch pictdashdotwidth mul exch\n"
	"		2 array astore 0 setdash }\n"
	"	ifelse } def\n"
	"/drawline { newpath moveto lineto stroke } def\n"
	"/drawrect { newpath\n"
	"	2 copy moveto\n"
	"	3 index exch lineto\n"
	"	3 -1 roll 2 index lineto\n"
	"	exch lineto\n"
	"	closepath stroke } def\n"
	"/drawdot { newpath\n"
	"	pictdashdotwidth mul\n"
	"	2 index 2 index moveto\n"
	"	0 360 arc\n"
	"	fill } def\n"
	"/drawstr { moveto show } def\n"
	"/updatevpos { /vpos vpos txtheight sub def } def\n"
	"/drawtxt { updatevpos\n"
	"	vpos 0 lt { showpage prepnewpage updatevpos } if\n"
	"	0 vpos moveto show } def\n\n"
	"% setup\n\n"
	"/top 720 def\n"
	"/vpos 0 def\n"
	"/txtheight 11 def\n"
	"/pgno 0 def\n"
	"/boldtxtfont /Courier-Bold findfont 10 scalefont def\n"
	"/txtfont /Courier findfont 10 scalefont def\n");

  printf("/headerstr (%s) def\n", fixstring(headerstr));
  printf("/linewidth %.2f def\n", linewidth);
  printf("/dashdotwidth %.2f def\n", dashdotwidth);
  printf("/pictscale %.2f def\n", pictscale);
  printf("/fontscale %.2f def\n", fontscale);

  puts( "/pictlinewidth linewidth def\n"
	"/pictdashdotwidth dashdotwidth def\n\n"
	"txtfont setfont\n\n"
	"prepnewpage\n\n"
	"% printing stuff\n");
}

/* Generate PostScript trailer */

void genpstrailer(void)
{
  puts( "\n% trailer\n\n"
	"vpos top ne { showpage } if\n"
	"end\n"
	"restore");
}

/* Make string printable in PostScript (change parentheses) */

char *fixstring(char *s)
{
  static char cbuf[BUFSIZE*2]; /* prepare for the worst */
  char *t = cbuf, c = 'a';

  do {
    switch (*s) {
    case '\n':
      break;
    case '(':
    case ')':
    case '\\':
      *t++ = '\\';
      *t++ = *s;
      break;
    default:
      *t++ = *s;
      break;
    }
  } while (*s++);

  return cbuf;
}

/* Convert the string to number. */

float convnumarg(char *s)
{
  float f;

  if (sscanf(s, "%f", &f) != 1)
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
	  "syntax: %s [-d n][-f n][-h s][-l n][-s n] [file... | < file]\n",
	  progname);
  exit(1);
}
