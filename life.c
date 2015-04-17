#include "toolbox.h"
#include <stdbool.h>

#define REG_VCOUNT *(volatile u16*)0x04000006

void vid_vsync()
{
  while(REG_VCOUNT >= 160);   // wait till VDraw
  while(REG_VCOUNT < 160);    // wait till VBlank
}

#define LIFE_ROWS 40
#define LIFE_COLS 60

int main()
{
  REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

  bool life1[LIFE_ROWS][LIFE_COLS];
  bool life2[LIFE_ROWS][LIFE_COLS];

  for (int r = 0; r < LIFE_ROWS; r++) {
    for (int c = 0; c < LIFE_COLS; c++) {
      life1[r][c] = c & (r * 2);
    }
  }

  bool toggle = true;
  while (1) {
    vid_vsync();
    for (int r = 0; r < LIFE_ROWS; r++) {
      for (int c = 0; c < LIFE_COLS; c++) {
        COLOR color = (toggle ? life1 : life2)[r][c] ? RGB15(0, 0, 31) : RGB15(31, 31, 31);
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
            m3_plot(c * 4 + i, r * 4 + j, color);
          }
        }
      }
    }
    for (int r = 0; r < LIFE_ROWS; r++) {
      for (int c = 0; c < LIFE_COLS; c++) {
        int r_p1 = r == LIFE_ROWS - 1 ? 0 : r + 1;
        int r_m1 = r == 0 ? LIFE_ROWS - 1 : r - 1;
        int c_p1 = c == LIFE_COLS - 1 ? 0 : c + 1;
        int c_m1 = c == 0 ? LIFE_COLS - 1 : c - 1;
        int neighbors
          = (toggle ? life1 : life2)[r   ][c_p1]
          + (toggle ? life1 : life2)[r   ][c_m1]
          + (toggle ? life1 : life2)[r_p1][c   ]
          + (toggle ? life1 : life2)[r_m1][c   ]
          + (toggle ? life1 : life2)[r_p1][c_p1]
          + (toggle ? life1 : life2)[r_p1][c_m1]
          + (toggle ? life1 : life2)[r_m1][c_p1]
          + (toggle ? life1 : life2)[r_m1][c_m1]
          ;
        if ((toggle ? life1 : life2)[r][c]) {
          (toggle ? life2 : life1)[r][c] = neighbors == 2 || neighbors == 3;
        }
        else {
          (toggle ? life2 : life1)[r][c] = neighbors == 3;
        }
      }
    }
    toggle = !toggle;
  }

  return 0;
}
