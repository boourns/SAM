#ifndef __VOCALIST_H__
#define __VOCALIST_H__

#include "sam/sam.h"

#define MODE_NORMAL 0
#define MODE_CRAZY 1
#define MODE_REALLY_CRAZY 2

class Vocalist {
public:
  Vocalist() {
    playing = false;
    bank = 0;
    word = 0;
    risingEdge = 0;
    mode = MODE_NORMAL;
    sam.InitFrameProcessor();
    sam.EnableSingmode();
  }

  ~Vocalist() { }

  void FillBuffer(char *output, int len);
  void Trigger(bool trig);
  void SetBank(unsigned char b);
  void SetWord(unsigned char b);

  void SetMode(int m) { mode = m; }
  void SetPitch(unsigned char pitch) { sam.SetPitch(pitch); }
  void SetMouth(unsigned char mouth) { sam.SetMouth(mouth); }
  void SetThroat(unsigned char throat) { sam.SetThroat(throat); }
  void SetSpeed(unsigned char speed) { sam.SetSpeed(speed); }

private:

  void Load();
  void LoadRando();

  SAM sam;
  int mode;
  unsigned char bank;
  unsigned char word;
  bool trigger;
  bool risingEdge;
  bool playing;
  unsigned char rando[24];
};

#endif
