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

void Vocalist::Load() {
  playing = false;
  sam.LoadNextWord(
    &data[wordpos[bank][word]],
    &data[wordpos[bank][word] + wordlen[bank][word]],
    &data[wordpos[bank][word] + (wordlen[bank][word] << 1)],
    wordlen[bank][word]
  );
  sam.InitFrameProcessor();
  sam.PrepareFrames();
}

void Vocalist::FillBuffer(char *output, int len) {
  if (risingEdge) {
    Load();
    risingEdge = false;
  }

  if (!playing && trigger) {
    playing = true;
  }

  int written = 0;
  if (playing) {
    written = sam.FillBufferFromFrame(len, &output[0]);
  }
  if (written < len) {
    playing = false;
    SetWord((word + 1) % NUM_WORDS);
    sam.SetThroat(rand() % 256);
    sam.SetMouth(rand() % 256);
    sam.SetSpeed(rand() % 256);
  }
  for (int i = written; i < len; i++) {
    output[i] = 0x80;
  }
}

void Vocalist::Trigger(bool trig) {
  if (!trigger && trig) {
    risingEdge = true;
  }
  trigger = trig;
}
