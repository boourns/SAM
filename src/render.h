#ifndef RENDER_H
#define RENDER_H

void Render(int *bufferpos, char *buffer);
void SetMouthThroat(unsigned char mouth, unsigned char throat);

void ProcessFrames(unsigned char mem48, int *bufferpos, char *buffer);
void RenderSample(unsigned char *mem66, unsigned char consonantFlag, unsigned char mem49, int *bufferpos, char *buffer);
unsigned char CreateTransitions();

#define PHONEME_PERIOD (1)
#define PHONEME_QUESTION (2)

#define RISING_INFLECTION (1)
#define FALLING_INFLECTION (255)

#endif
