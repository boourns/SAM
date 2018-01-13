#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../sam/reciter.h"
#include "../sam/sam.h"
#include "../sam/debug.h"

void PrintUsage()
{
	// TODO
}

int validOffsetLen;
unsigned char validOffset[255];

SAM *sam;

void calculateValidOffsets() {
	// calculate valid offset positions
    validOffsetLen = 0;
    int i = 0;
    while (i < sam->framesRemaining) {
      validOffset[validOffsetLen++] = i;

      if (sam->sampledConsonantFlag[i] & 248) {
        i += 2;
      } else {
        i += 1;
      }
    }
}

extern int debug;

void printArray(const char *name, unsigned char *target, int len) {
	printf("\"%s\": [", name);
	bool first = true;
	int i = 0;

	while(i < len)
	{
		if (!first) {
			printf(", ");
		}
		if (first) {
			first = false;
		}
		printf("%i", target[i]);
		i++;
	}
	printf("]");
}

int main(int argc, char **argv)
{
	int i;
	int phonetic = 0;

	char input[256];

	for(i=0; i<256; i++) input[i] = 0;

	if (argc <= 1)
	{
		PrintUsage();
		return 1;
	}

	sam = new SAM();

	i = 1;
	while(i < argc)
	{
		if (argv[i][0] != '-')
		{
			strcat(input, argv[i]);
			strcat(input, " ");
		} else {
			// impl options like this
			// if (strcmp(&argv[i][1], "p")==0)
			// {
			// 	wavfilename = argv[i+1];
			// 	i++;
			// }
		}

		i++;
	} //while

	for(i=0; input[i] != 0; i++)
	input[i] = toupper((int)input[i]);

	if (!phonetic)
	{
		strcat(input, "[");
		if (!TextToPhonemes(input)) return 1;
		if (debug)
		printf("phonetic input: %s\n", input);
	} else strcat(input, "\x9b");

	sam->SetInput(input);
	if (!sam->PreparePhonemes())
	{
		PrintUsage();
		return 1;
	}

	sam->LoadNextWord(&sam->phonemeindex[0], &sam->phonemeLength[0], &sam->stress[0], 255);

	sam->InitFrameProcessor();
    sam->PrepareFrames();
    calculateValidOffsets();

    // frequency1
    // frequency2
    // frequency3
    // pitches
    // amplitude1[256];
    // unsigned char amplitude2[256];
    // unsigned char amplitude3[256];
    // unsigned char sampledConsonantFlag[256];

	printf("{\n");
	printArray("frequency1", &sam->frequency1[0], sam->framesRemaining);
	printf(",\n");
	printArray("frequency2", &sam->frequency2[0], sam->framesRemaining);
	printf(",\n");
	printArray("frequency3", &sam->frequency3[0], sam->framesRemaining);
	printf(",\n");
	printArray("pitches", &sam->pitches[0], sam->framesRemaining);
	printf(",\n");
	printArray("amplitude1", &sam->amplitude1[0], sam->framesRemaining);
	printf(",\n");
	printArray("amplitude2", &sam->amplitude2[0], sam->framesRemaining);
	printf(",\n");
	printArray("amplitude3", &sam->amplitude3[0], sam->framesRemaining);
	printf(",\n");
	printArray("sampledConsonantFlag", &sam->sampledConsonantFlag[0], sam->framesRemaining);
	printf(",\n");
	printArray("validOffset", &validOffset[0], validOffsetLen);
	printf("\n}\n");

	return 0;
}
