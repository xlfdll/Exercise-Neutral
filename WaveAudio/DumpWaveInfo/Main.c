#include "DumpWaveInfo.h"

void showUsage(void)
{
	printf("Usage: DumpWaveInfo <WAVE filename>\n");
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

	if (readWaveHeader(argv[1], &sampleRate, &bitsPerSample, &posOfData, &sizeOfData) != 0)
	{
		return EXIT_FAILURE;
	}

	if (dumpWaveData(argv[1], bitsPerSample, posOfData, sizeOfData) != 0)
	{
		return EXIT_FAILURE;
	}

	printf("Done.\n");

	return EXIT_SUCCESS;
}