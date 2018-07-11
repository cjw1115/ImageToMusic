#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char uchar;

typedef struct
{
	uchar MTrk[4];                  //音轨块标志MTrk
	uchar tracksLength[4];               //音轨块长度
	uchar start[5];                 //音色定义00 C0 01，音色大钢琴，编号001；最开始的dt和开音00 90
	uchar* mid;             //音轨块实际数据
	uchar end[3];                   //音轨块结束标志 ff 2f 00
}MidiTrack;

typedef struct            //MIDI文件
{
	uchar MIDid[4];                 //文件头块标志MThd,4个字节
	uchar headLength[4];               //头部文件长度，4个字节
	uchar format[2];                //格式，2个字节
	uchar tracks[2];                //音轨数目,2个字节
	uchar ticks[2];                 //基本时间格式类型，每个四分音符的tick数，2个字节
	MidiTrack  trackData[14];
} MIDI;



void CreateMidiTrack(MidiTrack * midiTrack, uchar tone[], uchar delta[], int length, int tempo, uchar channel);
void CreateMidi(float tones[][14], float speeds[][14], int length, int tempo,const char *);