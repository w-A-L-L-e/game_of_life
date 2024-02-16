/*=============================================================================
author        : Walter Schreppers
filename      : board.h
created       : 10/2/2024
modified      : 
version       : 
copyright     : Walter Schreppers
bugreport(log): 
=============================================================================*/

#ifndef BOARD_H
#define BOARD_H

#include "screen.h"

typedef std::vector< std::vector<int> > matrix;

class Board {

  public:
    //constructor & destructor
    //==========================
    Board(int width, int height);
    ~Board();

    //public members
    //==============
    void resize(int width, int height);
    void seed();
    void seed_file(const char* filename, int xpos=0, int ypos=0);
    int getPos(int x, int y);
    int getLiveNeighbours(int x, int y);
    void evolve();
    void draw(Screen *scr);

  private:
    //private members:
    //================

    //private locals:
    //===============
    matrix *board;
    std::vector<std::vector<int> > *next_board;

}; //end of class Board

#endif

