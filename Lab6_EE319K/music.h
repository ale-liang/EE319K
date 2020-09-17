//music.h
#include <stdint.h>
#include "Sound.h"
//timer0A is 32 bits
//counts down at 80 MHz
// 12.5 *2^32 = 53 seconds


void Music_Init(void);
void Music_Play(void);
void Music_Stop(void);
