// Assignment 4

#include <iostream>
#include "draw.h"

using namespace std;

int main (int argc, char** argv)
{
    Draw dr = Draw::set(argc, argv);
    dr.run();
    
    return 0;
} // main

