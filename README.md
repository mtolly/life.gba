A simple Game Boy Advance implementation of Conway's Game of Life.
Built for [devkitARM](http://devkitpro.org/) version 43.

The 240x160 screen is split into 60x40 tiles (each of which is 4x4 pixels).
Each square group of 2 tiles by 2 tiles is drawn with a background tile in `BG0`,
so there are 16 different background tiles.
