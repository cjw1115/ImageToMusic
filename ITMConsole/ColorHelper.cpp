#include "ColorHelper.h"

float MIN(float v1, float v2)
{
	if (v1 > v2)
		return v2;
	else
		return v1;
}
float MAX(float v1, float v2)
{
	if (v1 > v2)
		return v1;
	else
		return v2;
}

///R:0-255 B:0-255 G:0-255
///h:0-360 s:0-1 v:0-1
void RGBtoHSV(unsigned char r, unsigned char g, unsigned char b, float * h, float * s, float * v)
{
	float fr, fg, fb;
	float max, min;
	float delta;
	float temp;
	fr = r / 255.0;
	fg = g / 255.0;
	fb = b / 255.0;

	max = MAX(fr, MAX(fg, fb));
	min = MIN(fr, MIN(fg, fb));
	delta = max - min;

	if (0 == delta)
	{
		*h = 0;
		*s = 0;
		*v = max;
		return;
	}
		
	//V
	*v = max;

	//S
	if (0 == max)
		*s = 0;
	else
		*s = delta / max;
	//H

	if (max == fr)
	{
		*h = ((fg - fb) / delta) * 60;
	}
	else if (max == fg)
	{
		temp = ((fb - fr) / delta);
		*h = 60 * (temp + 2);
	}
	else if (max == fb)
	{
		temp = ((fr - fg) / delta);
		*h = 60 * (temp + 4);
	}
	if (*h < 0)
		*h+=360;
}

void RGBtoGray(unsigned char  r, unsigned char g, unsigned char b, unsigned char * gray) 
{
	*gray = (77 * r + 151 * g + 28 * b) >> 8;
}