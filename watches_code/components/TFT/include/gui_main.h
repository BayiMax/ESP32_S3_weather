#ifndef gui_main_h
#define gui_main_h

#define Black_HZ 5000
#define LEDC_OUTPUT_IO 40

void gui_task(void *pvParameter);

void Screen_Black_Light_Init(void);
void Screen_Black_light(uint8_t light);
#endif /* gui_main_h */
