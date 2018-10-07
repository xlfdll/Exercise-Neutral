#define _CRT_SECURE_NO_WARNINGS

#ifndef DUMPWAVEINFO_H
#define DUMPWAVEINFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MAX_PATH
#define _MAX_PATH (255)
#endif // !_MAX_PATH

#pragma pack(push,1) // Disable alignments by compiler
typedef struct tagSWaveFileHeader
{
	char			hdrRiff[4];	// "RIFF"
	unsigned int	sizeOfFile;	// File size - 8 ("RIFF" + sizeOfFile)
	char			hdrWave[4];	// "WAVE"
} SWaveFileHeader;

typedef struct tagChunk
{
	unsigned char	hdrFmtData[4];	// "fmt" or "data"
	unsigned int	sizeOfFmtData;	// sizeof(PCMWAVEFORMAT) or wave data size
} tChunk;

typedef struct tagWaveFormatPCM
{
	unsigned short	formatTag;		// 1 = WAVE_FORMAT_PCM
	unsigned short	channels;		// # of channels
	unsigned int	samplesRate;	// Sampling rate
	unsigned int	bytesPerSec;	// samplesPerSec * channels * (bitsPerSample / 8)
	unsigned short	blockAlign;		// (bitsPerSample / 8) * channels
	unsigned short	bitsPerSample;
} tWaveFormatPCM;
#pragma pack(pop)

#define TAG_RIFF "RIFF"
#define TAG_WAVE "WAVE"
#define TAG_FMT "fmt "
#define TAG_DATA "data"

static int readfmtChunk(FILE * fp, tWaveFormatPCM * waveFormatPCM);

int readWaveHeader(
	const char * waveFileName,
	unsigned int * sampleRate,
	unsigned short * bitsPerSample,
	long * posOfData,
	long * sizeOfData);

static int dump8BitWave(FILE * fp, long sizeOfData);
static int dump16BitWave(FILE * fp, long sizeOfData);

static int dumpWave(
	FILE * fp,
	long posOfData,
	long sizeOfData,
	short bytesPerChannel);

int dumpWaveData(
	const char * waveFileName,
	unsigned short bitsPerSample,
	long posOfData,
	long sizeOfData
);

#endif