/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#ifndef _ALTARSANDDOORS_H_
#define _ALTARSANDDOORS_H_

#define FULL     0
#define WAITING  1
#define CHARGING 2
#define EMPTY    3

#define CLOSED   0
#define OPENING  1
#define OPEN     2

typedef struct {
	int x;
	int y;
	int state;
	int special;
} Altar;

typedef struct {
	int x;
	int y;
	Altar* altar;
} Horn;

typedef struct {
	int x;
	int y;
	int state;
} Door;

#endif

