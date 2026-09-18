/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#include "Game.h"
#include <stdio.h>

int
main (int argc, char** argv)
{
	Game * game = new Game();
	game->run();
	return EXIT_SUCCESS;
}

