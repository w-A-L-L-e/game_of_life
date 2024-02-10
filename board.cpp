/*=============================================================================
author        : Walter Schreppers
filename      : board.cpp
created       : 10/2/2024
modified      : 
version       : 
copyright     : Walter Schreppers
bugreport(log): 
=============================================================================*/

#include "board.h"
#include <fstream>
#include <string>
#include <iostream>
// #include <sstream>

/*-----------------------------------------------------------------------------
name        : Board
description : constructor
parameters  : 
return      : 
exceptions  : 
algorithm   : trivial
-----------------------------------------------------------------------------*/
Board::Board(int width, int height){
  board = new matrix();
  next_board = new matrix();
  resize(width, height);
}


/*-----------------------------------------------------------------------------
name        : ~Board
description : destructor
parameters  : 
return      : 
exceptions  : 
algorithm   : trivial
-----------------------------------------------------------------------------*/
Board::~Board(){
  delete board;
  delete next_board;

}


/*-----------------------------------------------------------------------------
name        : resize
description : resize board matrix
parameters  : width, height
return      : 
exceptions  : 
algorithm   : trivial
-----------------------------------------------------------------------------*/
void Board::resize(int width, int height){
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

/*-----------------------------------------------------------------------------
name        : seed
description : seed initial state of board fields with some live cells
              we add some blinkers and gliders to start the world board
parameters  : 
return      : 
exceptions  : 
algorithm   : trivial, TODO: allow loading of seed files etc...
-----------------------------------------------------------------------------*/
void Board::seed() {
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


/*-----------------------------------------------------------------------------
name        : seed_file 
description : get cell value and given x,y if out of bounds return 0 (dead)
parameters  : char* filename
return      : 
exceptions  : 
algorithm   : trivial
-----------------------------------------------------------------------------*/
void Board::seed_file(const char* filename, int xpos, int ypos) {
  std::ifstream cfile(filename);

  std::string line;
  int rows = 0;
  int cols = 0;

  while (std::getline(cfile, line)) {
    if (line.size() > 0 && line[0] == '!') continue;  // skip comment line


    rows ++;
    if ((line.size()+xpos) > cols) cols = line.size() + xpos;

    // std::cout << rows << " : " << cols << " " << line << std::endl;

    // make sure cells fit on board
    if ( cols > board->size() ) board->resize(cols);
    if ( rows + ypos > board->size() ) board->resize(rows + ypos);

    for (int pos = 0; pos < line.size(); pos++) {
      if (line[pos]== 'O') (*board)[xpos+pos][rows+ypos] = 1;
      else  (*board)[xpos+pos][rows+ypos] = 0;
    }
  }
}


/*-----------------------------------------------------------------------------
name        : getPos
description : get cell value and given x,y if out of bounds return 0 (dead)
parameters  : 
return      : 
exceptions  : 
algorithm   : trivial
-----------------------------------------------------------------------------*/
int Board::getPos(int x, int y){
  // do bounds check and return 0 when out of bounds
  if (x < 0 || y < 0) return 0;
  if (x >= (*board).size() || y >= (*board)[0].size() ) return 0;

  return (*board)[x][y];
}


/*-----------------------------------------------------------------------------
name        : getLiveNeighbours 
description : look at all surrounding cells and count alive neighbours
parameters  : 
return      : count of neighbours alive
exceptions  : 
algorithm   : trivial
-----------------------------------------------------------------------------*/
int Board::getLiveNeighbours(int x, int y){
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


/*-----------------------------------------------------------------------------
name        : evolve
description : evolve to next generation using 3 easy rules given in algorithm
parameters  : 
return      : 
exceptions  : 
algorithm   : 1. Living cells die if they have fewer than wo (underpopulation)
                 or more than three living neighbors (overpopulation).

              2. Living cells stay alive if they have two or 
                 three living neighbors.

              3. Dead cells with exactly three living neighbors become 
                 alive (reproduction).
-----------------------------------------------------------------------------*/
void Board::evolve(){
  for(int i=0; i < (*board).size(); i++){
    for( int j=0; j < (*board)[i].size(); j++){
      int neighbours = getLiveNeighbours(i,j);
      (*next_board)[i][j] = 0;
      
      if((*board)[i][j] == 1) {  // alive (1)
        if (neighbours < 2 || neighbours > 3){
          (*next_board)[i][j] = 0;
        }
        else {  // alive (2)
          (*next_board)[i][j] = 1;
        }
      }
      else {    // Dead cell (3)
        if (neighbours == 3) (*next_board)[i][j] = 1;
      }
    }
  }

  // swap boards is faster than a copy here ;)
  matrix* swap = board;
  board = next_board;
  next_board = swap;
}


/*-----------------------------------------------------------------------------
name        : draw
description : draw game of life board
parameters  : 
return      : 
exceptions  : 
algorithm   : draw state of board to sdl screen by using some scaling and offsets
-----------------------------------------------------------------------------*/
void Board::draw(Screen *scr){
  scr->setColor(245,225,0);  

  float bw = (int)(scr->width / board->size());
  float bh = (int)(scr->height / (*board)[0].size()) - 0.2;

  // normally just take min bw, bhw... we know bh is smaller
  // we get square block here
  bw = bh;

  // add some padding top+left
  int xoff = 13; // 110 offset for 100x100 and 200x200;
  int yoff = 16;

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

