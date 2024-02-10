/*=============================================================================
author        : Walter Schreppers
filename      : game_of_life.cpp
created       : 09/2/2024
modified      : 09/2/2024
version       :
description   : Game of life
=============================================================================*/

#include <ctime>
#include <iostream>
#include <numeric>
#include <vector>
// #define _USE_MATH_DEFINES
#include <math.h>

#include "screen.h"
#include "board.h"


int main(int argc, char **argv) {
  Screen screen(800, 600, "Game Of Life", false);
  SDL_ShowCursor(SDL_DISABLE);

  Board game_board(134, 100); // 200, 200 is also nice but we'll need to load seed files then...
  int speed_delay = 60; // higher is slower, (16 or lower is around max 60fps)

  long evolutions = 1;
  long prev_ticks = 0;

  while (screen.opened()) {
    screen.handle_events();
    SDL_Delay(20);
    if (SDL_GetTicks() - prev_ticks < speed_delay) continue;

    //screen.clear(0, 0, 80);
    screen.clear();
    game_board.draw(&screen);

    game_board.evolve(); 

    // this step keeps it interesting. however when loading some seed files we can
    // have automatic glider guns etc making this not needed most likely.
    if(evolutions++ % 30 == 0) game_board.seed();
    prev_ticks = SDL_GetTicks();

    screen.draw();
  }

  return 0;
}
