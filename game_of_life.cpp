/*=============================================================================
author        : Walter Schreppers
filename      : game_of_life.cpp
created       : 09/2/2024
modified      : 09/2/2024
version       :
description   : Game of life
=============================================================================*/

#include "screen.h"
#include <ctime>
#include <iostream>
#include <numeric>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

typedef std::vector<std::vector<int> > matrix;

class Board {
  public:
    Board(int width, int height){
      board = new matrix();
      next_board = new matrix();
      resize(width, height);
      seed();
    }

    ~Board() {
      delete board;
      delete next_board;
    }

    void resize(int width, int height){
      board->resize(width);
      next_board->resize(width);
      for(int col=0; col < width; col++){
        (*board)[col].resize(height);
        (*next_board)[col].resize(height);
      }

      for(int i=0; i < board->size(); i++) {
        for( int j=0; j < (*board)[i].size(); j++){
          (*board)[i][j] = 0;
          (*next_board)[i][j] = 0;
        }
      }
    }

    void seed() {
      // initial nice seed found by playing around ;)
      // blinker pattern as seed
      (*board)[25][14] = 1;
      (*board)[25][15] = 1;
      (*board)[25][16] = 1;

     
      // glider
      (*board)[1][12] = 1;
      (*board)[2][13] = 1;
      (*board)[0][14] = 1;
      (*board)[1][14] = 1;
      (*board)[2][14] = 1;


      // glider
      (*board)[10][2] = 1;
      (*board)[12][3] = 1;
      (*board)[10][4] = 1;
      (*board)[11][4] = 1;
      (*board)[12][4] = 1;


      // glider
      (*board)[31][32] = 1;
      (*board)[32][33] = 1;
      (*board)[30][34] = 1;
      (*board)[31][34] = 1;
      (*board)[32][34] = 1;


      // blinker
      (*board)[65][74] = 1;
      (*board)[65][75] = 1;
      (*board)[65][76] = 1;
    }

    int getPos(int x, int y){
      // do bounds check and return 0 when out of bounds
      if (x < 0 || y < 0) return 0;
      if (x >= (*board).size() || y >= (*board)[0].size() ) return 0;

      return (*board)[x][y];
    }

    int getLiveNeighbours(int x, int y){
      int count = 0;
      
      // count all cells around x,y
      count += getPos(x-1, y-1);
      count += getPos(x, y-1);
      count += getPos(x+1, y-1);
      count += getPos(x-1, y);
      count += getPos(x+1, y);
      count += getPos(x-1, y+1);
      count += getPos(x, y+1);
      count += getPos(x+1, y+1);

      return count;
    }

    void evolve(){
      // calculate next generation (we store results in new board to not mess up the old one during checks)
      for(int i=0; i < (*board).size(); i++){
        for( int j=0; j < (*board)[i].size(); j++){
          int neighbours = getLiveNeighbours(i,j);
          (*next_board)[i][j] = 0;
          
          if((*board)[i][j] == 1) { //alive
            if (neighbours < 2 || neighbours > 3){
              // 1. Living cells die if they have fewer than two (underpop) or more than three living 
              // neighbors (overpopulation).
              (*next_board)[i][j] = 0;
            }
            else {
              // 2. Living cells stay alive if they have two or three living neighbors.
              (*next_board)[i][j] = 1;
            }
          }
          else { // Dead cell
            // 3. Dead cells with exactly three living neighbors become alive (reproduction).
            if (neighbours == 3) (*next_board)[i][j] = 1;
          }
        }
      }

      // swap boards is faster than a copy here ;)
      matrix* swap = board;
      board = next_board;
      next_board = swap;
    }

    void draw(Screen *scr){
      scr->setColor(245,225,0);  

      float bw = (int)(scr->width / board->size());
      float bh = (int)(scr->height / (*board)[0].size()) - 0.2;

      // normally just take min bw, bhw... we know bh is smaller
      // we get square block here
      bw = bh;

      // add some padding top+left
      int xoff = 110;
      int yoff = 10;

      for (int x=0; x < (*board).size(); x++) {
        for (int y=0; y < (*board)[x].size(); y++) {
          if ((*board)[x][y] > 0) {
            scr->fill_rectangle(xoff + (x * bw), yoff + (y * bh), bw-1, bh-1);
          }
        }
      }

      // draw border around cells
      // scr->setColor(70, 60, 0);
      // scr->rectangle(xoff, yoff-2, bw * (*board).size(), bh * (*board)[0].size());
      // scr->rectangle(xoff-1, yoff-3, bw * (*board).size()+2, bh * (*board)[0].size()+2);
    }

    matrix *board;
    std::vector<std::vector<int> > *next_board;
};



int main(int argc, char **argv) {
  Screen screen(800, 600, "Game Of Life", false);
  SDL_ShowCursor(SDL_DISABLE);

  Board game_board(100, 100);
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
    if(evolutions++ % 20 == 0) game_board.seed();
    prev_ticks = SDL_GetTicks();

    screen.draw();
  }

  return 0;
}
