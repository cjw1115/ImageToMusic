#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char uchar;

typedef struct
{
	uchar MTrk[4];                  //������־MTrk
	uchar tracksLength[4];               //����鳤��
	uchar start[5];                 //��ɫ����00 C0 01����ɫ����٣����001���ʼ��dt�Ϳ���00 90
	uchar* mid;             //�����ʵ������
	uchar end[3];                   //����������־ ff 2f 00
}MidiTrack;

typedef struct            //MIDI�ļ�
{
	uchar MIDid[4];                 //�ļ�ͷ���־MThd,4���ֽ�
	uchar headLength[4];               //ͷ���ļ����ȣ�4���ֽ�
	uchar format[2];                //��ʽ��2���ֽ�
	uchar tracks[2];                //������Ŀ,2���ֽ�
	uchar ticks[2];                 //����ʱ���ʽ���ͣ�ÿ���ķ�������tick����2���ֽ�
	MidiTrack  trackData[14];
} MIDI;



void CreateMidiTrack(MidiTrack * midiTrack, uchar tone[], uchar delta[], int length, int tempo, uchar channel);
void CreateMidi(float tones[][14], float speeds[][14], int length, int tempo,const char *);