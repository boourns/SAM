#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "sam/reciter.h"
#include "sam/sam.h"
#include "sam/debug.h"

#ifdef USESDL
#include <SDL.h>
#include <SDL_audio.h>
#endif

// contains the final soundbuffer
int bufferpos;
char *buffer;

#define MAX_BUFFER (20 * 22050)

void WriteWav(char* filename, char* buffer, int bufferlength)
{
	FILE *file = fopen(filename, "wb");
	if (file == NULL) return;
	//RIFF header
	fwrite("RIFF", 4, 1,file);
	unsigned int filesize=bufferlength + 12 + 16 + 8 - 8;
	fwrite(&filesize, 4, 1, file);
	fwrite("WAVE", 4, 1, file);

	//format chunk
	fwrite("fmt ", 4, 1, file);
	unsigned int fmtlength = 16;
	fwrite(&fmtlength, 4, 1, file);
	unsigned short int format=1; //PCM
	fwrite(&format, 2, 1, file);
	unsigned short int channels=1;
	fwrite(&channels, 2, 1, file);
	unsigned int samplerate = 22050;
	fwrite(&samplerate, 4, 1, file);
	fwrite(&samplerate, 4, 1, file); // bytes/second
	unsigned short int blockalign = 1;
	fwrite(&blockalign, 2, 1, file);
	unsigned short int bitspersample=8;
	fwrite(&bitspersample, 2, 1, file);

	//data chunk
	fwrite("data", 4, 1, file);
	fwrite(&bufferlength, 4, 1, file);
	fwrite(buffer, bufferlength, 1, file);

	fclose(file);
}

void PrintUsage()
{
	printf("usage: sam [options] Word1 Word2 ....\n");
	printf("options\n");
	printf("	-phonetic 		enters phonetic mode. (see below)\n");
	printf("	-pitch number		set pitch value (default=64)\n");
	printf("	-speed number		set speed value (default=72)\n");
	printf("	-throat number		set throat value (default=128)\n");
	printf("	-mouth number		set mouth value (default=128)\n");
	printf("	-wav filename		output to wav instead of libsdl\n");
	printf("	-sing			special treatment of pitch\n");
	printf("	-debug			print additional debug messages\n");
	printf("\n");


	printf("     VOWELS                            VOICED CONSONANTS	\n");
	printf("IY           f(ee)t                    R        red		\n");
	printf("IH           p(i)n                     L        allow		\n");
	printf("EH           beg                       W        away		\n");
	printf("AE           Sam                       W        whale		\n");
	printf("AA           pot                       Y        you		\n");
	printf("AH           b(u)dget                  M        Sam		\n");
	printf("AO           t(al)k                    N        man		\n");
	printf("OH           cone                      NX       so(ng)		\n");
	printf("UH           book                      B        bad		\n");
	printf("UX           l(oo)t                    D        dog		\n");
	printf("ER           bird                      G        again		\n");
	printf("AX           gall(o)n                  J        judge		\n");
	printf("IX           dig(i)t                   Z        zoo		\n");
	printf("				       ZH       plea(s)ure	\n");
	printf("   DIPHTHONGS                          V        seven		\n");
	printf("EY           m(a)de                    DH       (th)en		\n");
	printf("AY           h(igh)						\n");
	printf("OY           boy						\n");
	printf("AW           h(ow)                     UNVOICED CONSONANTS	\n");
	printf("OW           slow                      S         Sam		\n");
	printf("UW           crew                      Sh        fish		\n");
	printf("                                       F         fish		\n");
	printf("                                       TH        thin		\n");
	printf(" SPECIAL PHONEMES                      P         poke		\n");
	printf("UL           sett(le) (=AXL)           T         talk		\n");
	printf("UM           astron(omy) (=AXM)        K         cake		\n");
	printf("UN           functi(on) (=AXN)         CH        speech		\n");
	printf("Q            kitt-en (glottal stop)    /H        a(h)ead	\n");
}

SAM *sam;

#ifdef USESDL
int pos = 0;
void MixAudio(void *unused, Uint8 *stream, int len)
{
	int i;
	if (pos >= bufferpos) return;
	if ((bufferpos-pos) < len) len = (bufferpos-pos);
	for(i=0; i<len; i++)
	{
		stream[i] = buffer[pos];
		pos++;
	}
}

void OutputSound()
{
	SDL_AudioSpec fmt;

	fmt.freq = 22050;
	fmt.format = AUDIO_U8;
	fmt.channels = 1;
	fmt.samples = 2048;
	fmt.callback = MixAudio;
	fmt.userdata = NULL;

	/* Open the audio device and start playing sound! */
	if ( SDL_OpenAudio(&fmt, NULL) < 0 )
	{
		printf("Unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_PauseAudio(0);
	//SDL_Delay((bufferpos)/7);

	while (pos < bufferpos)
	{
		SDL_Delay(100);
	}

	SDL_CloseAudio();
}

#else

void OutputSound() {}

#endif

extern int debug;

int main(int argc, char **argv)
{
	int i;
	int phonetic = 0;

	char* wavfilename = NULL;
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
		} else
		{
			if (strcmp(&argv[i][1], "wav")==0)
			{
				wavfilename = argv[i+1];
				i++;
			} else
			if (strcmp(&argv[i][1], "sing")==0)
			{
				sam->EnableSingmode();
			} else
			if (strcmp(&argv[i][1], "phonetic")==0)
			{
				phonetic = 1;
			} else
			if (strcmp(&argv[i][1], "debug")==0)
			{
				debug = 1;
			} else
			if (strcmp(&argv[i][1], "pitch")==0)
			{
				sam->SetPitch(atoi(argv[i+1]));
				i++;
			} else
			if (strcmp(&argv[i][1], "speed")==0)
			{
				sam->SetSpeed(atoi(argv[i+1]));
				i++;
			} else
			if (strcmp(&argv[i][1], "mouth")==0)
			{
				sam->SetMouth(atoi(argv[i+1]));
				i++;
			} else
			if (strcmp(&argv[i][1], "throat")==0)
			{
				sam->SetThroat(atoi(argv[i+1]));
				i++;
			} else
			{
				PrintUsage();
				return 1;
			}
		}

		i++;
	} //while

	for(i=0; input[i] != 0; i++)
	input[i] = toupper((int)input[i]);

	if (debug)
	{
		if (phonetic) printf("phonetic input: %s\n", input);
		else printf("text input: %s\n", input);
	}

	if (!phonetic)
	{
		strcat(input, "[");
		if (!TextToPhonemes(input)) return 1;
		if (debug)
		printf("phonetic input: %s\n", input);
	} else strcat(input, "\x9b");

	#ifdef USESDL
	if ( SDL_Init(SDL_INIT_AUDIO) < 0 )
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);
	#endif

	sam->SetInput(input);
	if (!sam->PreparePhonemes())
	{
		PrintUsage();
		return 1;
	}

	buffer = (char *) malloc(MAX_BUFFER);
	bufferpos = 0;
	int written = 0;
	bool finished = false;

	do {
		printf("loading next frame\n");
		finished = sam->LoadNextWord(sam->phonemeindex, sam->phonemeLength, sam->stress, 255);
		sam->PrepareFrames();
		do {
			written = sam->FillBufferFromFrame(100, &buffer[bufferpos]);
			printf("written %d, outer bufferpos %d\n", written, bufferpos);
			bufferpos += written;
		} while (written == 100 && bufferpos < MAX_BUFFER-100);
	} while (!finished && bufferpos < MAX_BUFFER-100);

	if (wavfilename != NULL)
	WriteWav(wavfilename, buffer, bufferpos);
	else
	OutputSound();

	return 0;
}
