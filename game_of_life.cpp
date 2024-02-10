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

void seed_guns(Board& game_board){
  game_board.seed_file("./examples/new_gun.cells");
  game_board.seed_file("./examples/glider_gun.cells", 20, 74);
}

int main(int argc, char **argv) {
  Screen screen(800, 600, "Game Of Life", false);
  SDL_ShowCursor(SDL_DISABLE);

  Board game_board(160, 120);
  int speed_delay = 60; // higher is slower, (16 or lower is around max 60fps)

  long evolutions = 1;
  long prev_ticks = 0;
 
  seed_guns(game_board);

  while (screen.opened()) {
    screen.handle_events();
    SDL_Delay(20);
    if (SDL_GetTicks() - prev_ticks < speed_delay) continue;
    prev_ticks = SDL_GetTicks();

    //screen.clear(0, 0, 80);
    screen.clear();
    game_board.draw(&screen);

    game_board.evolve(); 

    // periodically re-seed the guns in case they got broken by nearby explosions of cells ;)
    if(evolutions++ % 500 == 0) seed_guns(game_board);

    screen.draw();
  }

  return 0;
}
