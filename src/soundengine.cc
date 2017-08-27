#include "soundengine.h"

char *testPatch[3] = {"it", "doesn't", "matter"}

void SoundEngine::Init()
{
  SetPatch(3, testPatch);
}

void SoundEngine::Process(unsigned int count, char *buffer)
{
  switch(mode) {
  case MODE_PHRASE:
    if (!playing && gate) {
      // just triggered
      playing = true;
      position = 0;
      sam.SetInput(patchPartials[position]);
    }
    break;
  }
}
