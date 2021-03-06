#include <tonc.h>
#include <stdbool.h>

#define LIFE_ROWS 40
#define LIFE_COLS 60

int main()
{
  // Load palette
  pal_bg_mem[0] = CLR_WHITE;
  pal_bg_mem[1] = CLR_BLACK;
  // Load tiles into CBB 0
  // Tile index 0bABCD means if ABCD are set, fill in quadrant: AB
  //                                                            CD
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 4; j++) {
      tile_mem[0][i].data[j] = ((i & 1) ? 0x1111 : 0) + ((i & 2) ? 0x11110000 : 0);
    }
    for (int j = 4; j < 8; j++) {
      tile_mem[0][i].data[j] = ((i & 4) ? 0x1111 : 0) + ((i & 8) ? 0x11110000 : 0);
    }
  }
  // Load map into SBB 30
  for (int i = 0; i < 32 * 32; i++) se_mem[30][i] = 0;

  // set up BG0 for a 4bpp 32x32t map, using charblock 0 and screenblock 30
  REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

  // These will contain only 1s and 0s, no other number.
  bool life1[LIFE_ROWS][LIFE_COLS];
  bool life2[LIFE_ROWS][LIFE_COLS];

  for   (int r = 0; r < LIFE_ROWS; r++) {
    for (int c = 0; c < LIFE_COLS; c++) {
      life1[r][c] = 0;
    }
  }
  // The fabled R-pentomino
  life1[10][11] = 1;
  life1[10][12] = 1;
  life1[11][10] = 1;
  life1[11][11] = 1;
  life1[12][11] = 1;

  // If true, life1 contains the current state. Otherwise life2 does.
  bool toggle = true;

  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  while (1) {
    VBlankIntrWait();
    for   (int r = 0; r < LIFE_ROWS; r += 2) {
      for (int c = 0; c < LIFE_COLS; c += 2) {
        se_mem[30][(r * 32 + c) / 2]
          = ((toggle ? life1 : life2)[r    ][c    ] * 1)
          | ((toggle ? life1 : life2)[r    ][c + 1] * 2)
          | ((toggle ? life1 : life2)[r + 1][c    ] * 4)
          | ((toggle ? life1 : life2)[r + 1][c + 1] * 8)
          ;
      }
    }
    for   (int r = 0; r < LIFE_ROWS; r++) {
      for (int c = 0; c < LIFE_COLS; c++) {
        // row/column indices wrap around
        int r_p1 = r == LIFE_ROWS - 1 ? 0             : r + 1;
        int r_m1 = r == 0             ? LIFE_ROWS - 1 : r - 1;
        int c_p1 = c == LIFE_COLS - 1 ? 0             : c + 1;
        int c_m1 = c == 0             ? LIFE_COLS - 1 : c - 1;
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
