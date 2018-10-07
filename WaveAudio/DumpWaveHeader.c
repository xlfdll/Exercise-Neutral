#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
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

static int readfmtChunk(FILE * fp, tWaveFormatPCM * waveFormatPCM)
{
	if (fread(waveFormatPCM, sizeof(tWaveFormatPCM), 1, fp) != 1)
	{
		return -1;
	}

	printf("%32s%hu (1 = WAVE_FORMAT_PCM)\n", "Data format: ", waveFormatPCM->formatTag);
	printf("%32s%hu\n", "# of channels: ", waveFormatPCM->channels);
	printf("%32s%u Hz\n", "Sampling rate: ", waveFormatPCM->samplesRate);
	printf("%32s%u B/s (%.2f KB/s)\n", "Bytes per second: ",
		waveFormatPCM->bytesPerSec, ((double)waveFormatPCM->bytesPerSec / 1024));
	printf("%32s%hu B\n", "Block align: ", waveFormatPCM->blockAlign);
	printf("%32s%hu (bit/sample)\n", "Bits per sample: ", waveFormatPCM->bitsPerSample);
	printf("\n");

	return 0;
}

int readWaveHeader(const char * waveFileName, unsigned int * sampleRate, unsigned short * bitsPerSample, long * posOfData, long * sizeOfData)
{
	SWaveFileHeader waveFileHeader;
	tWaveFormatPCM waveFormatPCM;
	tChunk chunk;
	long fPos;
	FILE * fp;

	if ((fp = fopen(waveFileName, "rb")) == NULL)
	{
		fprintf(stderr, "Cannot open file: %s\n", waveFileName);

		return -1;
	}

	printf("\n%s:\n", waveFileName);

	for (size_t i = 0; i <= strlen(waveFileName) + 1; i++)
	{
		putchar('-');
	}

	printf("\n");

	if (fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp) != 1)
	{
		fprintf(stderr, "Cannot read wave file header at %ld.\n", ftell(fp));

		fclose(fp);

		return -1;
	}

	if (strncmp(waveFileHeader.hdrRiff, "RIFF", 4) != 0)
	{
		fprintf(stderr, "Wave file is not RIFF format.\n");

		fclose(fp);

		return -1;
	}

	if (strncmp(waveFileHeader.hdrWave, "WAVE", 4) != 0)
	{
		fprintf(stderr, "Wave file does not contain WAVE tag.\n");

		fclose(fp);

		return -1;
	}

	while (fread(&chunk, sizeof(chunk), 1, fp) == 1)
	{
		if (strncmp(chunk.hdrFmtData, "fmt ", sizeof(chunk.hdrFmtData)) == 0)
		{
			printf("The size of 'fmt' chunk: %u B\n", chunk.sizeOfFmtData);
			printf("\n");

			fPos = ftell(fp);

			if (readfmtChunk(fp, &waveFormatPCM) != 0)
			{
				return -1;
			}

			*sampleRate = waveFormatPCM.samplesRate;
			*bitsPerSample = waveFormatPCM.bitsPerSample;

			fseek(fp, fPos + chunk.sizeOfFmtData, SEEK_SET);
		}
		else if (strncmp(chunk.hdrFmtData, "data", 4) == 0)
		{
			*sizeOfData = chunk.sizeOfFmtData;

			printf("The size of 'data' chunk: %u B (%.2f MB)\n",
				chunk.sizeOfFmtData, ((double)chunk.sizeOfFmtData / 1024 / 1024));

			*posOfData = ftell(fp);

			fseek(fp, *sizeOfData + *posOfData - 4, SEEK_SET);

			break;
		}
		else
		{
			printf("The size of '%c%c%c%c' chunk: %u B\n",
				chunk.hdrFmtData[0],
				chunk.hdrFmtData[1],
				chunk.hdrFmtData[2],
				chunk.hdrFmtData[3],
				chunk.sizeOfFmtData);

			fseek(fp, ftell(fp) + chunk.sizeOfFmtData, SEEK_SET);
		}
	}

	fclose(fp);

	return 0;
}

void showUsage(void)
{
	printf("Usage: DumpWaveHeader <WAVE filename>\n");
}

int main(int argc, char * argv[])
{
	unsigned int sampleRate;
	unsigned short bitsPerSample;
	long posOfData, sizeOfData;

	if (argc != 2)
	{
		showUsage();

		return 0;
	}

	return readWaveHeader(argv[1], &sampleRate, &bitsPerSample, &posOfData, &sizeOfData);
}