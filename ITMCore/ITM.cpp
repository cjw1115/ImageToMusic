#include "ColorHelper.h"

#include "MidiHelper.h"
#include <string.h>
#include <stdlib.h>

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef long LONG;

typedef struct{
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

typedef struct {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
} RGBQUAD;

typedef struct {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

int CalcAVG(float * buffer, int length)
{
	float sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += buffer[i];
	}
	return sum / length;
}
void PrintChannels(float buffer[][14],int width,int height)
{
	FILE * pfile = fopen("channels.txt", "w+");

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			fprintf(pfile, "%4.2f    ", buffer[j][i]);
		}
		fprintf(pfile, "\r\n");
	}
	fclose(pfile);
}
void PrintHue(float * buffer,int width,int height)
{
	FILE * pfile=fopen("hue.txt", "w+");
	
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			fprintf(pfile, "%4.2f  ", buffer[j*width + i]);
		}
		fprintf(pfile,"\r\n");
	}
	fclose(pfile);
}
void PrintRGB(BYTE* buffer,int width,int height)
{
	FILE * pfile = fopen("RGB.txt", "w+");

	int byteCount= ((((width*24) + 31) / 32) * 4);
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < byteCount; i=i+3)
		{
			fprintf(pfile, "(%3d,%3d,%3d)  ", buffer[j*byteCount + i], buffer[j*byteCount + i+1], buffer[j*byteCount + i+2]);
		}
		fprintf(pfile, "\r\n");
	}
	fclose(pfile);
}
//template <typename T>
//void ClearBuffer(T*buffer, T value, int length)
//{
//	for (int i = 0; i < length; i++)
//	{
//		buffer[i] = value;
//	}
//}

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

extern "C" void ImageToMusic(int pixels[], int length,int biWidht,int biHeight,char* midiFile)
{
	BITMAPINFOHEADER infoHeader;

	infoHeader.biWidth = biWidht;
	infoHeader.biHeight = biHeight;
	infoHeader.biBitCount = 24;

	//计算位图每行字节数
	int lineBytes = ((((infoHeader.biWidth*infoHeader.biBitCount) + 31) / 32) * 4);
	infoHeader.biSizeImage = lineBytes * infoHeader.biHeight;

	//从RGB转换为Hue，并将数组转换为代表色调信息的数组
	int pixelCount = infoHeader.biWidth*infoHeader.biHeight;
	float *hBuffer;
	hBuffer = new float[pixelCount];
	ClearBufferFloat(hBuffer, 0, pixelCount);

	BYTE red, green, blue;
	int k = 0;
	for (int j = 0; j < infoHeader.biHeight; j++)
	{
		for (int i = 0; i < infoHeader.biWidth; i ++)
		{
			//B--G--R
			blue = (pixels[j*infoHeader.biWidth + i] & 0xff)>>0;
			green = (pixels[j*infoHeader.biWidth + i] & 0xff00)>>8;
			red = (pixels[j*infoHeader.biWidth + i] & 0xff0000)>>16;

			float h, s, v;
			RGBtoHSV(red, green, blue, &h, &s, &v);
			hBuffer[k++] = h;
		}

	}

	//对原始像素在列上优化

	int rowHeight = infoHeader.biWidth / 14;
	int rowCount = (infoHeader.biHeight + rowHeight - 1) / rowHeight;

	float *newHBuffer = new float[rowCount*infoHeader.biWidth];

	for (int i = 0; i < infoHeader.biWidth; i++)
	{
		for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
		{
			float sum = 0;
			for (int j = 0; j < rowHeight; j++)
			{
				int realIndex = rowIndex * rowHeight + j;
				if (realIndex >= infoHeader.biHeight)
					break;
				sum += hBuffer[realIndex*infoHeader.biWidth + i];
			}
			newHBuffer[rowIndex*infoHeader.biWidth + i] = sum / rowHeight;
		}
	}
	delete hBuffer;
	hBuffer = newHBuffer;
	infoHeader.biHeight = rowCount;
	//创建二维数组，存储14列色调数据
	float(*channels)[14] = new float[rowCount][14];

	float maxHue = 0;
	float minHue = 360;
	float hue = 0;

	float maxDiff = 0;
	float minDiff = 360;
	float diff = 0;
	
	//理想的每列像素宽度
	int columnWidth = (infoHeader.biWidth + 13) / 14;

	int rowIndex = 0;
	for (int j = 0; j < infoHeader.biHeight; j++)
	{
		float rowBuffer[14];
		memset(rowBuffer, 0, 14);
		for (int i = 0; i < 14; i++)
		{
			int length = columnWidth;
			//去掉超出部分的长度
			if (i * columnWidth + length > infoHeader.biWidth)
			{
				length = infoHeader.biWidth - i * columnWidth;
			}
			rowBuffer[i] = CalcAVG(j*infoHeader.biWidth + hBuffer + i * columnWidth, length);
		}

		
		for (int k = 0; k < 14; k++)
		{
			float sum = 0;
			for (int i = 13 - k; i >= 0; i--)
			{
				sum += rowBuffer[i];
			}
			hue = sum / (14 - k);
			channels[j][13 - k] = hue;

			//统计明度再全局的最大值与最小值
			if (hue > maxHue)
			{
				maxHue = hue;
			}
			if (hue < minHue)
			{
				minHue = hue;
			}
			//统计明度再全局的最大值与最小值,因为是差值，所以忽略第一行
			if (j > 0)
			{
				//针对每一列，
				diff = channels[j][13 - k] - channels[j - 1][13 - k];
				if (diff > maxDiff)
				{
					maxDiff = diff;
				}
				if (diff < minDiff)
				{
					minDiff = diff;
				}
			}
		}
	}

	//开始计算音符，时延，节拍

	float(*tones)[14] = new float[infoHeader.biHeight][14];
	float(*speeds)[14] = new float[infoHeader.biHeight][14];
	for (int i = 0; i < 14; i++)
	{
		speeds[0][i] = 0;
	}
	float deltaHue = maxHue - minHue;
	float deltaDiff= maxDiff - minDiff;
	
	for (int i = 0; i < infoHeader.biHeight; i++)
	{
		for (int j = 0; j < 14; j++)
		{
			//计算音符
			if (deltaHue == 0)
				tones[i][j] = 0;
			else
			{
				//因为有128个可用音符
				tones[i][j] = (channels[i][j] - minHue) / deltaHue * 127;
			}

			//计算时延
			if (i > 0)
			{
				if (0 == deltaDiff)
					speeds[i][j] = 0;
				else
				{
					float tempDiff = (channels[i][j] - channels[i - 1][j]);
					if (tempDiff < 0)
						tempDiff = -tempDiff;
					//映射到11中音长
					speeds[i][j] = (tempDiff - minDiff) / deltaDiff * 10;
				}
			}

		}
	}

	//计算节拍
	int *speedSet = new int[361];
	ClearBufferInt(speedSet, 0, 361);
	float minSpeed = 361;
	float maxSpeed = 0;

	float f = 0;

	for (int i = 1; i < infoHeader.biHeight; i++)
	{
		for (int j = 1; j < 14; j++)
		{
			f = speeds[i][j] - speeds[i][j - 1];
			if (f < 0)
				f = -f;
			if (f > maxSpeed)
				maxSpeed = f;
			if (f < minSpeed)
				minSpeed = f;
			speedSet[(int)f] ++;
		}
	}
	int speedCount = 0;
	for (int i = 1; i < 361; i++)
	{
		if (speedSet[i] > speedCount)
		{
			speedCount = speedSet[i];
			f = i;
		}
	}

	//节拍数目,
	float tempo = 0;
	if (maxSpeed - minSpeed == 0)
	{
		tempo = 120;
	}
	else
	{
		tempo = (f - minSpeed) / (maxSpeed - minSpeed)*(200 - 120) + 120;
	}

	CreateMidi(tones, speeds, infoHeader.biHeight, tempo, midiFile);
}