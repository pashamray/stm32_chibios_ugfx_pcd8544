/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "stdlib.h"
#include "string.h"

#include "ch.h"
#include "hal.h"

#include "gfx.h"

#define SCB_DEMCR (*(volatile unsigned *)0xE000EDFC)
#define CPU_RESET_CYCLECOUNTER do { SCB_DEMCR = SCB_DEMCR | 0x01000000; \
DWT_CYCCNT = 0; \
DWT_CTRL = DWT_CTRL | 1 ; } while(0)

static int uitoa(unsigned int value, char * buf, int max) {
    int n = 0;
    int i = 0;
    unsigned int tmp = 0;

    if (!buf)
        return -3;

    if (2 > max)
        return -4;

    i=1;
    tmp = value;
    if (0 > tmp) {
        tmp *= -1;
        i++;
    }
    for (;;) {
        tmp /= 10;
        if (0 >= tmp)
            break;
        i++;
    }
    if (i >= max) {
        buf[0] = '?';
        buf[1] = 0x0;
        return 2;
    }

    n = i;
    tmp = value;
    if (0 > tmp) {
        tmp *= -1;
    }
    buf[i--] = 0x0;
    for (;;) {
        buf[i--] = (tmp % 10) + '0';
        tmp /= 10;
        if (0 >= tmp) {
            break;
        }
    }
    if (-1 != i) {
        buf[i--] = '-';
    }

    return n;
}

void benchmark(void) {

  uint32_t i, pixels, ms, pps;
  char pps_str[25];
  coord_t height, width, rx, ry, rcx, rcy;
  color_t random_color;
  font_t font;

  //gdispSetOrientation(GDISP_ROTATE_90);

  width = gdispGetWidth();
  height = gdispGetHeight();
  font = gdispOpenFont("Archangelsk Regular 12");

  gdispDrawStringBox(0,  0, width, 11, "ChibiOS/GFX", font, White, justifyCenter);
  gdispDrawStringBox(0, 12, width, 11, "Benchmark", font, White, justifyCenter);
  gdispDrawStringBox(0, 25, width, 11, "5000 random", font, White, justifyCenter);
  gdispDrawStringBox(0, 38, width, 11, " rectangles", font, White, justifyCenter);
  
  gfxSleepMilliseconds(3000);
  
  /* seed for the rand() */
  srand(DWT_CYCCNT);
  pixels = 0;

  CPU_RESET_CYCLECOUNTER;

  for (i = 0; i < 5000; i++) {
    random_color = (rand() % 2);
    rx  = (rand() % (width   - 10));
    ry  = (rand() % (height  - 10));
    rcx = (rand() % ((width  - rx) -10)) + 10;
    rcy = (rand() % ((height - ry) -10)) + 10;

    gdispFillArea(rx, ry, rcx, rcy, random_color);
    pixels += (rcx + 1) * (rcy + 1);
  }

  ms = DWT_CYCCNT / 168000;
  pps = (float) pixels / ((float) ms / 1000.0f);

  memset (pps_str, 0, sizeof(pps_str));
  uitoa(pps, pps_str, sizeof(pps_str));
  strcat(pps_str, " px/s");

  gdispClear(Black);
  gdispDrawStringBox(0,  0, width, 11, "ChibiOS/GFX", font, White, justifyCenter);
  gdispDrawStringBox(0, 13, width, 11, "Benchmark", font, White, justifyCenter);
  gdispDrawStringBox(0, 26, width, 11, pps_str, font, White, justifyCenter);
  //gdispDrawString(20, height/2, pps_str, font, White);
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /* Initialize and clear the display */
  gfxInit();

  benchmark();

  // Wait forever
  while(TRUE) {
    gfxSleepMilliseconds(500);
  }
}
