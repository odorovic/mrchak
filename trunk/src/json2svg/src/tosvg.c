#include <stdio.h>
#include <stdlib.h>

#include "json.h"

//SVG files are not properly aligned, use this to fix it
static int refX, refY;
static int scale = 15; //Always set manually
static char *netname = NULL; 
static uint32_t n = 0;
static value *netlists[65535];
static bool hasNet = false;

void baseAttrsCallback(value *top);

//used only to figure out offests of images
void submodulesCallback(value *top)
{
	object obj;
	array arr;
//	for (arr = top->val.arr; arr; arr = arr->next)
	for (arr = top->val.arr; arr; arr = NULL) //XXX: temp workaround
		for (obj = arr->val->val.obj; obj; obj = obj->next)
			if (!strcmp(obj->pair->str, "\"point\"")){
				refX = obj->pair->val->val.arr->val->val.num;
				refY = obj->pair->val->val.arr->next->val->val.num;
			}

	//XXX: handcoded workaround
	refX -= 3000;
	refY -= 6000;
}

//used to figure out what name of netlist
//There are several cases:
//Buffer is full and we hit a name : print it
//Buffer is empty and we hit a name : store a name
void netnameCallback(value *top)
{
	if (top->type != VAL_STRING) //Junk, ignore
		return ;

	if (!hasNet){ //buffer is full, print it
		uint32_t i;
		netname = top->val.str;
		hasNet = true; //This is so baseAttrsCallback works
		for (i = 0; i < n; ++i)
			baseAttrsCallback (netlists[i]);
		
		hasNet = false;
		n = 0;
	} else {
		hasNet = true;
		netname = top->val.str;
	}
}

//We presume that all stuff we need are withing finite compile time know path from node
void baseAttrsCallback(value *top)
{
	//add decoding of colors
	char *colors[10] = {
	"stroke=\"black\"", "stroke=\"yellow\"", "stroke=\"purple\"", "stroke=\"orange\"", 
	"stroke=\"red\"", "stroke=\"green\"", "stroke=\"blue\"", "stroke=\"cyan\"", 
	"stroke=\"pink\"", "stroke=\"magenta\""}; 

	//only add to end
	char *keywords[] = {
	"\"angle\"", "\"num_lines\"", "\"show_name_value\"", "\"point\"", "\"color\"",
	"\"text\"", "\"visibility\"", "\"key\"", "\"size\"", "\"type\"",
	"\"alignment\"", "\"attr\"", "\"line\"", "\"angle1\"", "\"angle2\"",
	"\"basename\"", "\"capstyle\"", "\"dashlength\"", "\"dashspace\"", "\"dashstyle\"", 
	"\"filltype\"", "\"fillwidth\"", "\"mirror\"", "\"pintype\"", "\"pitch1\"",
	"\"pitch2\"", "\"radius\"", "\"selectable\"", "\"wh\"",	"\"whichend\"", 
	"\"width\"" };
	int nkeywords = sizeof(keywords) / sizeof (void *);

	char *strings[nkeywords]; //union of all possible fields
	int ints[nkeywords]; //same as above
	
	char types[] = {'T', 'N', 'U', 'C', 'v', 'L', 'G', 'B', 'V', 'A', 'P', 'H', 'F'};
	int ntypes = sizeof(types) / (sizeof(char));
	
	bool needed[nkeywords][ntypes];

	object obj = top->val.obj;
	int i, matched=-1;
	char type = -1;

	//first find type
	for (obj = top->val.obj; obj; obj=obj->next)
		if (!strcmp(obj->pair->str, "\"type\""))
			type = obj->pair->val->val.str[1];

	//now find point, everything has a point
	int pointX, pointY;
	for (obj = top->val.obj; obj; obj=obj->next)
		if (!strcmp(obj->pair->str, "\"point\"")){
			pointX = obj->pair->val->val.arr->val->val.num;
			pointY = obj->pair->val->val.arr->next->val->val.num;
		}

	pointX = (pointX - refX) / scale;
	pointY = (pointY - refY) / scale;

	//now find a line if it exists 
	int pointX1, pointX2, pointY1, pointY2;
	for (obj = top->val.obj; obj; obj=obj->next)
		if (!strcmp(obj->pair->str, "\"line\"")){
			pointX1 = obj->pair->val->val.arr->val->val.arr->val->val.num;
			pointY1 = obj->pair->val->val.arr->val->val.arr->next->val->val.num;
			pointX2 = obj->pair->val->val.arr->next->val->val.arr->val->val.num;
			pointY2 = obj->pair->val->val.arr->next->val->val.arr->next->val->val.num;
	}

	pointX1 = (pointX1 - refX) / scale;
	pointX2 = (pointX2 - refX) / scale;
	pointY1 = (pointY1 - refY) / scale;
	pointY2 = (pointY2 - refY) / scale;

	//now find a line if it exists 
	int width, heigth;
	for (obj = top->val.obj; obj; obj=obj->next)
		if (!strcmp(obj->pair->str, "\"wh\"")){
		width = obj->pair->val->val.arr->val->val.num;
		heigth = obj->pair->val->val.arr->next->val->val.num;
	}

	width /= scale;
	heigth /= scale;

	//get members - will fail for some silently
	for (obj = top->val.obj; obj; obj=obj->next) {
		matched = -1;
		for (i = 0; i < nkeywords; ++i)
			if (!strcmp(keywords[i], obj->pair->str))
				matched = i;
		
		if (-1 == matched) {
		       	printf ("unmatched str %s\n", obj->pair->str);
		} else { //presume it is a string, fix later if needed, hope it works on 32bit
			strings[matched] = obj->pair->val->val.str;
			ints[matched] = obj->pair->val->val.num;
		}
	}

	//this is incorrect for eg point (which is array of integers)
	switch (type){
		case 'T' : 
			printf ("\t<text x=\"%d\" y=\"%d\">%s</text>\n",pointX, pointY, strings[5]);
			break;
		case 'P' : // same as U //TODO: fixme
		case 'L' : // same as U //TODO: fixme
		case 'U' : // XXX: BUS 
			printf ("\t<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" %s stroke-width=\"4\"/>\n",
					pointX1, pointY1, pointX2, pointY2, colors[ints[4]]);
			break;
		case 'N' : // net
			if (!hasNet){
				netlists[n++] = top;
				break;
			}
			printf ("\t<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" %s netname=%s stroke-width=\"4\"/>\n",
					pointX1, pointY1, pointX2, pointY2, colors[ints[4]], netname);
			break;
		case 'V' : 
			printf ("\t<circle cx=\"%d\" cy=\"%d\" %s r=\"%d\" fill=\"none\" stroke-width=\"4\" />\n",
					pointX, pointY, colors[ints[4]], ints[26]/scale);
			break;
		case 'B' : 
			printf ("\t<rect x=\"%d\" y=\"%d\" %s width=\"%d\" heigth=\"%d\" "
					"fill=\"none\" stroke-width=\"4\" />\n",
					pointX, pointY, colors[ints[4]], width, heigth);
			break;
		case 'C' : fprintf (stderr, "Type %c not implemented yet\n", type); break;
		case 'v' : ;
		case 'G' : ;
		case 'A' : ;
		case 'H' : ;
		case 'F' : fprintf (stderr, "I didn't know i needed type %c \n", type); break;
		default : fprintf (stderr, "unkown type, exitting...\n"); exit(1);
	}
}

int work (value *jsonTree)
{
	objectCallback baseAttrs;
	baseAttrs.event = "\"base_attrs\"";
	baseAttrs.fn = baseAttrsCallback;
	addCallback (baseAttrs);

	objectCallback submodules;
	submodules.event = "\"submodules\"";
	submodules.fn = submodulesCallback;
	addCallback (submodules);

	objectCallback netnameCB;
	netnameCB.event = "\"netname\"";
	netnameCB.fn = netnameCallback;
	addCallback (netnameCB);

	printf ("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">");
	parseTree (jsonTree);	
	printf ("\n</svg>\n");
	return 0;
}


