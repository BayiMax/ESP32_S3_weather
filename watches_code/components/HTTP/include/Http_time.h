#ifndef HTTP_TIME_H
#define HTTP_TIME_H

typedef struct
{
  char date[20];
  char time[20];
  volatile bool Time_Flag;
} _TIME_Data;
extern _TIME_Data Time_Data;

void Time_test_task(void *pvParameters);

#endif /* HTTP_TIME_H */
