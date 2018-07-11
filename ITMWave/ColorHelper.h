#pragma once
float MIN(float v1, float v2);
float MAX(float v1, float v2);
void RGBtoHSV(unsigned char r, unsigned char g, unsigned char b, float *h, float *s, float *v);
void RGBtoGray(unsigned char  r, unsigned char g, unsigned char b, unsigned char * gray);