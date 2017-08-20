

#include "render.h"

extern unsigned char speed;

// From RenderTabs.h
extern unsigned char multtable[];
extern unsigned char sinus[];
extern unsigned char rectangle[];

// From render.c
extern unsigned char pitches[256];
extern unsigned char sampledConsonantFlag[256]; // tab44800
extern unsigned char amplitude1[256];
extern unsigned char amplitude2[256];
extern unsigned char amplitude3[256];
extern unsigned char frequency1[256];
extern unsigned char frequency2[256];
extern unsigned char frequency3[256];

extern void Output(int index, unsigned char A, int *bufferpos, char *buffer);

static void CombineGlottalAndFormants(unsigned char phase1, unsigned char phase2, unsigned char phase3, unsigned char Y, int *bufferpos, char *buffer)
{
  unsigned int tmp;

  tmp   = multtable[sinus[phase1]     | amplitude1[Y]];
  tmp  += multtable[sinus[phase2]     | amplitude2[Y]];
  tmp  += tmp > 255 ? 1 : 0; // if addition above overflows, we for some reason add one;
  tmp  += multtable[rectangle[phase3] | amplitude3[Y]];
  tmp  += 136;
  tmp >>= 4; // Scale down to 0..15 range of C64 audio.

  Output(0, tmp & 0xf, bufferpos, buffer);
}

unsigned char ProcessFrame(unsigned char Y, unsigned char mem48, int *bufferpos, char *buffer);

// PROCESS THE FRAMES
//
// In traditional vocal synthesis, the glottal pulse drives filters, which
// are attenuated to the frequencies of the formants.
//
// SAM generates these formants directly with sin and rectangular waves.
// To simulate them being driven by the glottal pulse, the waveforms are
// reset at the beginning of each glottal pulse.
//

unsigned char speedcounter = 72;
unsigned char phase1 = 0;
unsigned char phase2 = 0;
unsigned char phase3 = 0;
unsigned char mem66;
unsigned char glottal_pulse;
unsigned char mem38;

void ProcessFrames(unsigned char mem48, int *bufferpos, char *buffer)
{
  char *tiny_buffer = (char *) malloc(50000);
  int tiny_bufferpos = 0;

  unsigned char Y = 0;
  glottal_pulse = pitches[0];
  mem38 = glottal_pulse - (glottal_pulse >> 2); // mem44 * 0.75

  while(mem48) {
    unsigned char absorbed = ProcessFrame(Y, mem48, &tiny_bufferpos, &tiny_buffer[0]);

    while (tiny_bufferpos / 50 > 20) {
      // consume a sound byte, leaving 5 in the window.
      for (int k = 0; k < 5; k++) {
        buffer[*bufferpos/50] = tiny_buffer[k];
        *bufferpos = *bufferpos + 50;
      }
      for (int k = 5; k < (tiny_bufferpos/50) + 5; k++) {
        tiny_buffer[k-5] = tiny_buffer[k];
      }
      tiny_bufferpos -= 250;
    }
    Y += absorbed;
    mem48 -= absorbed;
  }
}

unsigned char ProcessFrame(unsigned char Y, unsigned char mem48, int *bufferpos, char *buffer)
{
    unsigned char flags = sampledConsonantFlag[Y];
    unsigned char absorbed = 0;

    // unvoiced sampled phoneme?
    if(flags & 248) {
      RenderSample(&mem66, flags, Y, bufferpos, buffer);
      // skip ahead two in the phoneme buffer
      speedcounter = speed;
      absorbed = 2;
    } else {
      CombineGlottalAndFormants(phase1, phase2, phase3, Y, bufferpos, buffer);

      speedcounter--;
      if (speedcounter == 0) {
        absorbed = 1;

        if(mem48 == 1) {
          return absorbed;
        }
        speedcounter = speed;
      }

      --glottal_pulse;

      if(glottal_pulse != 0) {
        // not finished with a glottal pulse

        --mem38;
        // within the first 75% of the glottal pulse?
        // is the count non-zero and the sampled flag is zero?
        if((mem38 != 0) || (flags == 0)) {
          // reset the phase of the formants to match the pulse
          phase1 += frequency1[Y + absorbed];
          phase2 += frequency2[Y + absorbed];
          phase3 += frequency3[Y + absorbed];
          return absorbed;
        }

        // voiced sampled phonemes interleave the sample with the
        // glottal pulse. The sample flag is non-zero, so render
        // the sample for the phoneme.
        RenderSample(&mem66, flags, Y + absorbed, bufferpos, buffer);
      }
    }

    glottal_pulse = pitches[Y + absorbed];
    mem38 = glottal_pulse - (glottal_pulse>>2); // mem44 * 0.75

    // reset the formant wave generators to keep them in
    // sync with the glottal pulse
    phase1 = 0;
    phase2 = 0;
    phase3 = 0;

    return absorbed;
}
