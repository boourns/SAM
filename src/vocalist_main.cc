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

#include "RtMidi.h"

#define NUM_PITCHES 32
#define MIDI_BASE 48

unsigned char pitches[] = {
 115, 108, 103, 98, 94, 88, 82, 78, 74, 70, 66, 62,
 58, 55, 52, 49, 46, 44, 42, 39, 37, 35, 33, 31,
 29, 28, 26, 25, 23, 22, 21, 20
};

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

void UILoop(RtMidiIn *midi)
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

	double stamp;
	std::vector<unsigned char> message;
	int nBytes;

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

		do {
      stamp = midi->getMessage( &message );
      nBytes = message.size();
  		if (nBytes > 0) {
  			// note on
  			if ((int)message[0] == 144) {
  				unsigned char pitch = (unsigned char)message[1];
  				if (pitch < MIDI_BASE) {
  					pitch = pitches[0];
  				} else if (pitch > MIDI_BASE + NUM_PITCHES) {
  					pitch = MIDI_BASE + NUM_PITCHES;
  				} else {
  					pitch = pitches[pitch-MIDI_BASE];
  				}
  				vocalist->SetPitch(pitch);
  				vocalist->Trigger(true);
  			}
  			else if ((int) message[0] == 128) {
  				vocalist->Trigger(false);
  			} else if ((int) message[0] == 0xB0) {
          unsigned char value = (unsigned char) message[2];
          switch((int) message[1]) {
          case 10:
            vocalist->SetSpeed(value);
            break;
          case 11:
            vocalist->SetMouth(value);
            break;
          case 12:
            vocalist->SetThroat(value);
            break;
          case 13:
            if (value < 40) {
              vocalist->SetMode(MODE_NORMAL);
            } else if (value < 80) {
              vocalist->SetMode(MODE_CRAZY);
            } else {
              vocalist->SetMode(MODE_REALLY_CRAZY);
            }
          }

        }
  		}
    } while (nBytes > 0);

    // for ( i=0; i<nBytes; i++ )
    //   std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
    // if ( nBytes > 0 )
    //   std::cout << "stamp = " << stamp << std::endl;

		SDL_Delay(20);
	}

	SDL_CloseAudio();
}

extern int debug;

RtMidiIn *initMidi() {

	// Check inputs.
	RtMidiIn *midiin = 0;
	// RtMidiIn constructor
	try {
		midiin = new RtMidiIn();
	}
	catch (RtMidiError &error) {
		// Handle the exception here
		error.printMessage();
	}

  if (midiin->getPortCount() == 0) {
		printf("No MIDI input ports found.\n");
		printf("If using OSX, open Audio MIDI Setup, double click 'IAC Driver', and check 'device is online'.\n");
		delete midiin;
		return 0;
	}

	midiin->openPort(0);
	// ignore sysex, timing, and active sensing messages.
  midiin->ignoreTypes( true, true, true );

	return midiin;
}

int main(int argc, char **argv)
{
	vocalist = new Vocalist();

	if ( SDL_Init(SDL_INIT_AUDIO) < 0 )
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	RtMidiIn *midi = initMidi();

	if (midi != 0) {
		UILoop(midi);
	}

	delete midi;
	return 0;
}
