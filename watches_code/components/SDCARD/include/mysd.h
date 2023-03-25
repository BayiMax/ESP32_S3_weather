#ifndef mysd_h
#define mysd_h

#include "stdio.h"

bool SD_card_init(void);

typedef struct
{
  volatile bool SDinit_Flag;
} _SD_TYP;

extern _SD_TYP SDTyp;

#endif /* mysd_h */
