#ifndef __SOUNDENGINE_H__
#define __SOUNDENGINE_H__

enum Mode {
  MODE_PHRASE,
  MODE_WORD,
  MODE_PARTIAL,
  MODE_SCAN
}

class SoundEngine {

public:
  SoundEngine() {
  //  playingSam = 0;
    mode = MODE_PHRASE;
  }

  ~SoundEngine() { }

  void Init();

  // fill with sound
  void Process(unsigned int count, char *buffer);

  void SetPatch(unsigned int count, char **partials);
  void SetMode(Mode m);
  void SetGate(bool g);
  //void SetPitch(unsigned short p);

  void Reset();

private:
  //SAM sam[2];
  //unsigned char playingSam;
  SAM sam;

  bool playing;
  bool gate;
  unsigned int position;

//  unsigned short pitch;

  unsigned int patchCount;
  char **patchPartials;

  Mode mode;
}

#endif
