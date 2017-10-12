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
  if (mode == MODE_NORMAL) {
    sam.LoadNextWord(
      &data[wordpos[bank][word]],
      &data[wordpos[bank][word] + wordlen[bank][word]],
      &data[wordpos[bank][word] + (wordlen[bank][word] << 1)],
      wordlen[bank][word]
    );
    sam.InitFrameProcessor();
    sam.PrepareFrames();
  } else {
    LoadRando();
  }
}

void Vocalist::LoadRando() {
  for (int i = 0; i < 24; i++) {
    rando[i] = rand() % 256;
  }
  sam.LoadNextWord(&rando[0], &rando[8], &rando[16], 8);
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
    if (mode == MODE_NORMAL) {
      playing = false;
      SetWord((word + 1) % NUM_WORDS);
    } else {
      LoadRando();
    }
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
