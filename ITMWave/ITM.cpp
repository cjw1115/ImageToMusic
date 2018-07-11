#include "ColorHelper.h"

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include <cstring>

using namespace std;

#define PI 3.1415926

#define T 160

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef long LONG;

int CalcAVG(float * buffer, int length)
{
	float sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += buffer[i];
	}
	return sum / length;
}


void PrintChannels(INT16* buffer, int length)
{
	FILE * pfile = fopen("Buffer.txt", "w+");

	for (int j = 0; j < length; j++)
	{
		for (int i = 0; i < T; i++)
		{
			fprintf(pfile, "%4d    ", buffer[j*T+i]);
		}
		fprintf(pfile, "\r\n");
	}
	fclose(pfile);
}

void ClearBufferFloat(float *buffer, float value, int length)
{
	for (int i = 0; i < length; i++)
	{
		buffer[i] = value;
	}
}
void ClearBufferInt(int *buffer, int value, int length)
{
	for (int i = 0; i < length; i++)
	{
		buffer[i] = value;
	}
}

int main(int argc, char** argv)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	RGBQUAD *pColorTable;
	BYTE *img;
	BYTE *grayImg;

	FILE* pfin;
	if (argc > 1)
	{
		pfin=fopen(argv[1], "r+b");
	}
	else
	{
		printf("no input image");
		return 0;
	}

	//读位图文件头
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
	//读位图信息头
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);

	if (infoHeader.biBitCount == 24)
	{
		//计算位图每行字节数
		int lineBytes = ((((infoHeader.biWidth*infoHeader.biBitCount) + 31) / 32) * 4);
		infoHeader.biSizeImage = lineBytes * infoHeader.biHeight;

		//创建像素数组
		img = new BYTE[infoHeader.biSizeImage];
		memset(img, 0, infoHeader.biSizeImage);
		//从位图文件读取位图像素数据
		fread(img, sizeof(BYTE), infoHeader.biSizeImage, pfin);
		fclose(pfin);

		int pixelCount = infoHeader.biWidth*infoHeader.biHeight;
		int *afqBuffer;
		afqBuffer = new int[pixelCount*3];
		memset(afqBuffer, 0, pixelCount*3);

		BYTE red, green, blue;
		int k = 0;
		for (int j = infoHeader.biHeight-1; j >=0; j--)
		{
			for (int i = 0; i < infoHeader.biWidth * 3; i += 3)
			{
				//B--G--R
				blue = img[j*lineBytes + i];
				green = img[j*lineBytes + i + 1];
				red = img[j*lineBytes + i + 2];

				float h, s, v;
				RGBtoHSV(red, green, blue, &h, &s, &v);
				afqBuffer[k++] = h / 360 * (32767 + 32678) - 32767;
				afqBuffer[k++] = s*(3400-300)+300;
				afqBuffer[k++] = v * 2*PI;
			}

		}
		
		INT16* waveBuffer;

		int waveBufferIndex = 0;
		waveBuffer = new INT16[infoHeader.biWidth*T];
		memset(waveBuffer, sizeof(int), infoHeader.biWidth*T);

		for (int j = 0; j < infoHeader.biWidth*3; j+=3)
		{
			for (int k = 0; k < 160; k++)
			{
				int sum = 0;
				for (int i = 0; i < infoHeader.biHeight; i++)
				{
					int a = afqBuffer[i*infoHeader.biWidth * 3 + j + 0];
					int f = afqBuffer[i*infoHeader.biWidth * 3 + j + 1];
					int q = afqBuffer[i*infoHeader.biWidth * 3 + j + 2];

					sum += a * sin(2 * PI*f*k + q);
				}
				int avg = sum / infoHeader.biHeight;
				waveBuffer[waveBufferIndex++] = avg;
			}
		}

		PrintChannels(waveBuffer, infoHeader.biWidth);

		FILE *waveFile;
		if (argc > 1)
		{	
			string path = argv[1] + string(".wav");
			waveFile=fopen(path.c_str(), "wb");
		}
		else 
		{
			waveFile=fopen("created.wav", "wb");
		}
		fwrite(waveBuffer, sizeof(INT16), infoHeader.biWidth*T, waveFile);
		fclose(waveFile);

		delete waveBuffer;
	}
}
