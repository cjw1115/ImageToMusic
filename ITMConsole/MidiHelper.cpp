#include "MidiHelper.h"


void CreateMidiTrack(MidiTrack * midiTrack, uchar tone[], uchar delta[], int length, int tempo, uchar channel)
{
	int i = 0, j = 0;
	int k = 0, n = 0, temp = 0;
	int trackLength = 0;

	//循环中重复length次，每次至少3字节
	int dataLength = 3 * length;
	MidiTrack *midi = midiTrack;
	midi->mid = new uchar[dataLength];

	//音轨块标志MTrk
	midi->MTrk[0] = 0x4d;
	midi->MTrk[1] = 0x54;
	midi->MTrk[2] = 0x72;
	midi->MTrk[3] = 0x6b;

	//事件开始
	//时延为0
	midi->start[0] = 0x00;
	//为通道定义音色
	midi->start[1] = 0xC0 + channel;
	//音色编号
	midi->start[2] = 0x01;

	//第一个音符的时延
	midi->start[3] = 0x00;
	//打开通道
	midi->start[4] = 0x90 + channel;             //通道1音符打开

	for (i = 0; i < length; i++) {
		//音符
		midi->mid[n++] = tone[i];
		//响度，固定值
		midi->mid[n++] = 0x64;

		//时延，由于tempo范围为120-200，所言delay最大值为500，只需要两字节
		int delay = 0;
		delay = 4.0 / delta[i] * tempo;
		if (delay > 128)
		{
			//动态字节处理
			int d1 = delay / 128;
			int d2 = delay % 128;
			midi->mid[n++] = 0x80 + d1;
			midi->mid[n++] = d2;
			//增加了一个字节，需要重新申请空间
			midi->mid = (uchar*)realloc(midi->mid, ++dataLength);
		}
		else
		{
			midi->mid[n++] = delay;
		}
	}

	//轨道结束表示
	midi->end[0] = 0xff;
	midi->end[1] = 0x2f;
	midi->end[2] = 0x00;

	//计算音轨长度
	trackLength = 5 + dataLength + 3;
	//长度值在四个字节上分开表示，依旧是大端模式
	midi->tracksLength[0] = trackLength >> 24;
	midi->tracksLength[1] = trackLength >> 16;
	midi->tracksLength[2] = trackLength >> 8;
	midi->tracksLength[3] = trackLength >> 0;
}


void CreateMidi(float tones[][14],float speeds[][14],int length,int tempo,const char * filePath)
{
	//创建文件，并返回文件指针
	FILE* midiFile = fopen(filePath, "wb");

	MIDI midi;
	//文件头块标志MThd
	midi.MIDid[0] = 0x4d;          
	midi.MIDid[1] = 0x54;
	midi.MIDid[2] = 0x68;
	midi.MIDid[3] = 0x64;

	//文件头块长度，固定为6
	midi.headLength[0] = 0x00;
	midi.headLength[1] = 0x00;
	midi.headLength[2] = 0x00;
	midi.headLength[3] = 0x06;

	//同步多音轨 0x01
	midi.format[0] = 0x00;         
	midi.format[1] = 0x01;

	//音轨数目 0x0e;
	midi.tracks[0] = 0x00;         
	midi.tracks[1] = 0x0e;

	midi.ticks[0] = tempo>>8;
	//每个4分音符delta-time节奏数
	midi.ticks[1] = tempo;     

	//MIDI头部数据，一次写入文件
	fwrite(midi.MIDid, 4, 1, midiFile);
	fwrite(midi.headLength, 4, 1, midiFile);
	fwrite(midi.format, 2, 1, midiFile);
	fwrite(midi.tracks, 2, 1, midiFile);
	fwrite(midi.ticks, 2, 1, midiFile);

	//循环创建14个轨道
	for (int i = 0; i < 14; i++)
	{
		uchar *tonesBuffer = new uchar[length];
		uchar *speedsBuffer = new uchar[length];
		for (int j = 0; j < length; j++)
		{
			tonesBuffer[j] = (uchar)tones[j][i];
			//speeds中第一个元素值未定义，故用第二个代替
			if (0==j)
			{
				speedsBuffer[0] = speeds[1][i];
			}
			else 
			{
				speedsBuffer[j] = speeds[j][i];
			}
			
		}
		//申明MIDI轨道
		MidiTrack track;
		//不同通道10和通道11，该通道为打击乐专用通道
		if (i >= 9)
		{
			CreateMidiTrack(&track, tonesBuffer, speedsBuffer, length, tempo, i+2);
		}
		else
		{
			CreateMidiTrack(&track, tonesBuffer, speedsBuffer, length, tempo, i);
		}
		midi.trackData[i] = track;

		//MIDI轨道数据写入文件
		fwrite(midi.trackData[i].MTrk, 4, 1, midiFile);
		fwrite(midi.trackData[i].tracksLength, 4, 1, midiFile);
		fwrite(midi.trackData[i].start, 5, 1, midiFile);
		
		//长度计算
		int dataLength = (midi.trackData[i].tracksLength[0] << 24) +
			(midi.trackData[i].tracksLength[1] << 16) +
			(midi.trackData[i].tracksLength[2] << 8) +
			midi.trackData[i].tracksLength[3];
		fwrite(midi.trackData[i].mid, dataLength - 5-3,1, midiFile);

		fwrite(midi.trackData[i].end, 3, 1, midiFile);
	}
	//关闭文件流
	fclose(midiFile);
}