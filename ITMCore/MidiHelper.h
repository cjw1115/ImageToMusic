#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char uchar;

typedef struct
{
	uchar MTrk[4];			//音轨块标志MTrk
	uchar tracksLength[4];  //音轨块长度
	uchar start[5];         //音色定义00、C0、xx;第一个时延和通道开
	uchar* mid;             //音轨块实际数据
	uchar end[3];           //音轨块结束标志0xff、0x2f、0x00
}MidiTrack;

typedef struct               //MIDI文件
{
	uchar MIDid[4];          //文件头块标志MThd,4个字节
	uchar headLength[4];     //头部文件长度，4个字节
	uchar format[2];         //格式，2个字节
	uchar tracks[2];         //音轨数目,2个字节
	uchar ticks[2];          //基本时间格式类型，每个四分音符的tick数，2个字节
	MidiTrack  trackData[14];//实际音轨
} MIDI;


///用于创建MIDI轨道
///midiTrack：代表MIDI轨道，是一个指针变量	tone:代表音符数组	delta:代表时延数组
///length:音符数组长度	tempo:节拍数或是一个四分音符的TICK数目	channel:通道编号
void CreateMidiTrack(MidiTrack * midiTrack, uchar tone[], uchar delta[], int length, int tempo, uchar channel);
///用于创建MIDI文件
///tones:代表转换图片得到的二维音符数组	speeds:代表转换图片得到的二维时延数组
///length:代表二维数组的第一维长度	tempo:节拍数或是一个四分音符的TICK数目 filePath:输出MIDI文件的路径
void CreateMidi(float tones[][14], float speeds[][14], int length, int tempo,char * filePath);