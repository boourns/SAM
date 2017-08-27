#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "sam/reciter.h"
#include "sam/sam.h"
#include "sam/debug.h"

#include "vocalist.h"

#ifdef USESDL
#include <SDL.h>
#include <SDL_audio.h>
#endif

// void WriteWav(char* filename, char* buffer, int bufferlength)
// {
// 	FILE *file = fopen(filename, "wb");
// 	if (file == NULL) return;
// 	//RIFF header
// 	fwrite("RIFF", 4, 1,file);
// 	unsigned int filesize=bufferlength + 12 + 16 + 8 - 8;
// 	fwrite(&filesize, 4, 1, file);
// 	fwrite("WAVE", 4, 1, file);
//
// 	//format chunk
// 	fwrite("fmt ", 4, 1, file);
// 	unsigned int fmtlength = 16;
// 	fwrite(&fmtlength, 4, 1, file);
// 	unsigned short int format=1; //PCM
// 	fwrite(&format, 2, 1, file);
// 	unsigned short int channels=1;
// 	fwrite(&channels, 2, 1, file);
// 	unsigned int samplerate = 22050;
// 	fwrite(&samplerate, 4, 1, file);
// 	fwrite(&samplerate, 4, 1, file); // bytes/second
// 	unsigned short int blockalign = 1;
// 	fwrite(&blockalign, 2, 1, file);
// 	unsigned short int bitspersample=8;
// 	fwrite(&bitspersample, 2, 1, file);
//
// 	//data chunk
// 	fwrite("data", 4, 1, file);
// 	fwrite(&bufferlength, 4, 1, file);
// 	fwrite(buffer, bufferlength, 1, file);
//
// 	fclose(file);
// }

Vocalist *vocalist;

int pos = 0;
void MixAudio(void *unused, Uint8 *stream, int len)
{
	vocalist->FillBuffer((char *) stream, len);
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

	SDL_Event event;
	int quit = 0;

	while (!quit)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_a) {
					vocalist->Trigger(true);
					break;
				}
			}
		}
		SDL_Delay(20);
	}

	SDL_CloseAudio();
}

extern int debug;

int main(int argc, char **argv)
{
	vocalist = new Vocalist();

	if ( SDL_Init(SDL_INIT_AUDIO) < 0 )
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	OutputSound();

	return 0;
}
