#include <stdio.h>
#include <stdlib.h>

#include "vocalist.h"
#include "wordlist.h"

void Vocalist::SetBank(unsigned char b) {
  bank = b;
}

void Vocalist::SetWord(unsigned char w) {
  word = w;
}

void Vocalist::FillBuffer(char *output, int len) {
  if (!playing && trigger) {
    playing = true;

    sam.LoadNextWord(
      &data[wordpos[bank][word]],
      &data[wordpos[bank][word] + wordlen[bank][word]],
      &data[wordpos[bank][word] + (wordlen[bank][word] << 1)],
      wordlen[bank][word]
    );
    sam.InitFrameProcessor();
    sam.PrepareFrames();
  }

  int written = 0;
  if (playing) {
    written = sam.FillBufferFromFrame(len, &output[0]);
  }
  if (written < len) {
    playing = false;
    word = (word + 1) % NUM_WORDS;
    sam.SetPitch(rand() % 200);
  }
  for (int i = written; i < len; i++) {
    output[i] = 0x80;
  }
}

void Vocalist::Trigger(bool trig) {
  trigger = trig;
}
