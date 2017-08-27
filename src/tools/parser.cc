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

SAM *sam;

extern int debug;

void printArray(const char *name, unsigned char *phonemeindex, unsigned char *target) {
	printf("\"%s\": [", name);
	bool first = true;
	int i = 0;

	while((phonemeindex[i] != 255) && (i < 255))
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

	printf("{\n");
	printArray("index", &sam->phonemeindex[0], &sam->phonemeindex[0]);
	printf(",\n");
	printArray("length", &sam->phonemeindex[0], &sam->phonemeLength[0]);
	printf(",\n");
	printArray("stress", &sam->phonemeindex[0], &sam->stress[0]);
	printf("\n}\n");

	return 0;
}
