#ifndef __VOCALIST_H__
#define __VOCALIST_H__

#include "sam/sam.h"

class Vocalist {
public:
  Vocalist() {
    playing = false;
    bank = 0;
    word = 0;
    trigger = true;
    sam.InitFrameProcessor();
    sam.EnableSingmode();
  }

  ~Vocalist() { }

  void FillBuffer(char *output, int len);
  void Trigger(bool trig);
  void SetBank(unsigned char b);
  void SetWord(unsigned char b);
  void SetPitch(unsigned char pitch) { sam.SetPitch(pitch); }
  void SetMouth(unsigned char mouth) { sam.SetMouth(mouth); }
  void SetThroat(unsigned char throat) { sam.SetThroat(throat); }
  void SetSpeed(unsigned char speed) { sam.SetSpeed(speed); }

private:

  void Load();
  
  SAM sam;
  unsigned char bank;
  unsigned char word;
  bool trigger;
  bool playing;
};

#endif
