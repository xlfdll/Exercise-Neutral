#include "DumpWaveInfo.h"

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

int readWaveHeader(
	const char * waveFileName,
	unsigned int * sampleRate,
	unsigned short * bitsPerSample,
	long * posOfData,
	long * sizeOfData)
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

	if (strncmp(waveFileHeader.hdrRiff, TAG_RIFF, 4) != 0)
	{
		fprintf(stderr, "Wave file is not RIFF format.\n");

		fclose(fp);

		return -1;
	}

	if (strncmp(waveFileHeader.hdrWave, TAG_WAVE, 4) != 0)
	{
		fprintf(stderr, "Wave file does not contain WAVE tag.\n");

		fclose(fp);

		return -1;
	}

	while (fread(&chunk, sizeof(chunk), 1, fp) == 1)
	{
		if (strncmp(chunk.hdrFmtData, TAG_FMT, sizeof(chunk.hdrFmtData)) == 0)
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
		else if (strncmp(chunk.hdrFmtData, TAG_DATA, 4) == 0)
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

static int read8BitWaveData(FILE * fp, long sizeOfData)
{
	// 8 bit: 0 - 255, mute = 128
	unsigned char data[2];

	for (size_t i = 0; i < sizeOfData / sizeof(data); i++)
	{
		if (fread(data, sizeof(data), 1, fp) != 1)
		{
			return -1;
		}

		printf("%4hhu,\t%4hhu\n", data[0], data[1]);
	}

	return 0;
}

static int read16BitWaveData(FILE * fp, long sizeOfData)
{
	// 16 bit: -32768 - +32767, mute = 0
	short data[2];

	for (size_t i = 0; i < sizeOfData / sizeof(data); i++)
	{
		if (fread(data, sizeof(data), 1, fp) != 1)
		{
			return -1;
		}

		printf("%6hd,\t%6hd\n", data[0], data[1]);
	}

	return 0;
}

static int readWaveData(
	FILE * fp,
	long posOfData,
	long sizeOfData,
	short bytesPerChannel)
{
	fseek(fp, posOfData, SEEK_SET);

	switch (bytesPerChannel)
	{
	case 1:
		return read8BitWaveData(fp, sizeOfData);
	case 2:
		return read16BitWaveData(fp, sizeOfData);
	default:
		return -1;
	}
}

int dumpWaveData(
	const char * waveFileName,
	unsigned short bitsPerSample,
	long posOfData,
	long sizeOfData
)
{
	unsigned short bytesPerChannel = bitsPerSample / 8;
	FILE * fp;

	if ((fp = fopen(waveFileName, "rb")) == NULL)
	{
		fprintf(stderr, "Cannot open file: %s\n", waveFileName);

		return -1;
	}

	if (readWaveData(fp, posOfData, sizeOfData, bytesPerChannel) != 0)
	{
		fprintf(stderr, "Cannot read Wave audio contents\n");
		
		fclose(fp);

		return -1;
	}

	fclose(fp);

	return 0;
}