#include "MidiHelper.h"


void CreateMidiTrack(MidiTrack * midiTrack, uchar tone[], uchar delta[], int length, int tempo, uchar channel)
{
	int i = 0, j = 0;
	int k = 0, n = 0, temp = 0;
	int trackLength = 0;

	//ѭ�����ظ�length�Σ�ÿ������3�ֽ�
	int dataLength = 3 * length;
	MidiTrack *midi = midiTrack;
	midi->mid = new uchar[dataLength];

	//������־MTrk
	midi->MTrk[0] = 0x4d;
	midi->MTrk[1] = 0x54;
	midi->MTrk[2] = 0x72;
	midi->MTrk[3] = 0x6b;

	//�¼���ʼ
	//ʱ��Ϊ0
	midi->start[0] = 0x00;
	//Ϊͨ��������ɫ
	midi->start[1] = 0xC0 + channel;
	//��ɫ���
	midi->start[2] = 0x01;

	//��һ��������ʱ��
	midi->start[3] = 0x00;
	//��ͨ��
	midi->start[4] = 0x90 + channel;             //ͨ��1������

	for (i = 0; i < length; i++) {
		//����
		midi->mid[n++] = tone[i];
		//��ȣ��̶�ֵ
		midi->mid[n++] = 0x64;

		//ʱ�ӣ�����tempo��ΧΪ120-200������delay���ֵΪ500��ֻ��Ҫ���ֽ�
		int delay = 0;
		delay = 4.0 / delta[i] * tempo;
		if (delay > 128)
		{
			//��̬�ֽڴ���
			int d1 = delay / 128;
			int d2 = delay % 128;
			midi->mid[n++] = 0x80 + d1;
			midi->mid[n++] = d2;
			//������һ���ֽڣ���Ҫ��������ռ�
			midi->mid = (uchar*)realloc(midi->mid, ++dataLength);
		}
		else
		{
			midi->mid[n++] = delay;
		}
	}

	//���������ʾ
	midi->end[0] = 0xff;
	midi->end[1] = 0x2f;
	midi->end[2] = 0x00;

	//�������쳤��
	trackLength = 5 + dataLength + 3;
	//����ֵ���ĸ��ֽ��Ϸֿ���ʾ�������Ǵ��ģʽ
	midi->tracksLength[0] = trackLength >> 24;
	midi->tracksLength[1] = trackLength >> 16;
	midi->tracksLength[2] = trackLength >> 8;
	midi->tracksLength[3] = trackLength >> 0;
}


void CreateMidi(float tones[][14],float speeds[][14],int length,int tempo,const char * filePath)
{
	//�����ļ����������ļ�ָ��
	FILE* midiFile = fopen(filePath, "wb");

	MIDI midi;
	//�ļ�ͷ���־MThd
	midi.MIDid[0] = 0x4d;          
	midi.MIDid[1] = 0x54;
	midi.MIDid[2] = 0x68;
	midi.MIDid[3] = 0x64;

	//�ļ�ͷ�鳤�ȣ��̶�Ϊ6
	midi.headLength[0] = 0x00;
	midi.headLength[1] = 0x00;
	midi.headLength[2] = 0x00;
	midi.headLength[3] = 0x06;

	//ͬ�������� 0x01
	midi.format[0] = 0x00;         
	midi.format[1] = 0x01;

	//������Ŀ 0x0e;
	midi.tracks[0] = 0x00;         
	midi.tracks[1] = 0x0e;

	midi.ticks[0] = tempo>>8;
	//ÿ��4������delta-time������
	midi.ticks[1] = tempo;     

	//MIDIͷ�����ݣ�һ��д���ļ�
	fwrite(midi.MIDid, 4, 1, midiFile);
	fwrite(midi.headLength, 4, 1, midiFile);
	fwrite(midi.format, 2, 1, midiFile);
	fwrite(midi.tracks, 2, 1, midiFile);
	fwrite(midi.ticks, 2, 1, midiFile);

	//ѭ������14�����
	for (int i = 0; i < 14; i++)
	{
		uchar *tonesBuffer = new uchar[length];
		uchar *speedsBuffer = new uchar[length];
		for (int j = 0; j < length; j++)
		{
			tonesBuffer[j] = (uchar)tones[j][i];
			//speeds�е�һ��Ԫ��ֵδ���壬���õڶ�������
			if (0==j)
			{
				speedsBuffer[0] = speeds[1][i];
			}
			else 
			{
				speedsBuffer[j] = speeds[j][i];
			}
			
		}
		//����MIDI���
		MidiTrack track;
		//��ͬͨ��10��ͨ��11����ͨ��Ϊ�����ר��ͨ��
		if (i >= 9)
		{
			CreateMidiTrack(&track, tonesBuffer, speedsBuffer, length, tempo, i+2);
		}
		else
		{
			CreateMidiTrack(&track, tonesBuffer, speedsBuffer, length, tempo, i);
		}
		midi.trackData[i] = track;

		//MIDI�������д���ļ�
		fwrite(midi.trackData[i].MTrk, 4, 1, midiFile);
		fwrite(midi.trackData[i].tracksLength, 4, 1, midiFile);
		fwrite(midi.trackData[i].start, 5, 1, midiFile);
		
		//���ȼ���
		int dataLength = (midi.trackData[i].tracksLength[0] << 24) +
			(midi.trackData[i].tracksLength[1] << 16) +
			(midi.trackData[i].tracksLength[2] << 8) +
			midi.trackData[i].tracksLength[3];
		fwrite(midi.trackData[i].mid, dataLength - 5-3,1, midiFile);

		fwrite(midi.trackData[i].end, 3, 1, midiFile);
	}
	//�ر��ļ���
	fclose(midiFile);
}