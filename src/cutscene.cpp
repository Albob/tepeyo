#include "cutscene.h"
#include "StreamedSound.h"

#define END 0
#define SECONDS(n) ((n)*60)

struct CutSceneElement {
	const char * filename;
	int start; // in frames
	int end; // in frames
};

struct CutScene {
	CutSceneElement * top;
	CutSceneElement * bottom;
	const char * sound;
	int length;
};

static CutSceneElement intro_top[] = {
	{ "pix/Intro1Couleur.bg", SECONDS(0), SECONDS(6) },
	{ "pix/Intro3Couleur.bg", SECONDS(10)-16, SECONDS(16) },
	{ "pix/Intro5Couleur.bg", SECONDS(20)-16, SECONDS(26) },
	{ "pix/Intro7.bg", SECONDS(33)-16, SECONDS(40) },
	{ END, END, END },
};

static CutSceneElement intro_bottom[] = {
	{ "pix/Intro2Couleur.bg", SECONDS(5)-16, SECONDS(11) },
	{ "pix/Intro4Couleur.bg", SECONDS(15)-16, SECONDS(21) },
	{ "pix/Intro6Couleur.bg", SECONDS(25)-16, SECONDS(31) },
	{ "pix/Intro8.bg", SECONDS(32)-16, SECONDS(40) },
	{ END, END, END },
};

static CutSceneElement outro_top[] = {
	{ "pix/Fin1.bg", SECONDS(0), SECONDS(6) },
	{ "pix/Fin3.bg", SECONDS(6)-16, SECONDS(12) },
	{ "pix/Fin5.bg", SECONDS(12), SECONDS(18) },
	{ "pix/Fin7.bg", SECONDS(18), SECONDS(24) },
	{ "pix/Fin9.bg", SECONDS(24), SECONDS(30) },
	{ "pix/Credit1.bg", SECONDS(33), SECONDS(39) },
	{ "pix/Credit3.bg", SECONDS(39), SECONDS(45) },
	{ END, END, END },
};

static CutSceneElement outro_bottom[] = {
	{ "pix/Fin2.bg", SECONDS(3)-16, SECONDS(6) },
	{ "pix/Fin4.bg", SECONDS(9)-16, SECONDS(12) },
	{ "pix/Fin6.bg", SECONDS(12), SECONDS(18) },
	{ "pix/Fin8.bg", SECONDS(18), SECONDS(24) },
	{ "pix/Fin10.bg", SECONDS(27)-16, SECONDS(32) },
	{ "pix/Credit2.bg", SECONDS(33), SECONDS(39) },
	{ "pix/Credit4.bg", SECONDS(39), SECONDS(45) },
	{ END, END, END },
};

static CutScene cutscenes[] = {
	{ intro_top, intro_bottom, "/tepeyo/snd/theme_02.snd", SECONDS(41)-22 },
	{ outro_top, outro_bottom, "/tepeyo/snd/theme_06.snd", SECONDS(46) },
};

void playCutScene(Engine * engine, int id, bool stopMusic)
{
	CutSceneElement * top = cutscenes[id].top;
	CutSceneElement * bottom = cutscenes[id].bottom;
	int time = 0;
	Background * bgTop = engine->getBackground(0, 0),
			   * bgBottom = engine->getBackground(0, 1);

	getStreamedSound()->load(cutscenes[id].sound);
	//getStreamedSound()->setVolume(0);
	getStreamedSound()->setVolume(127);
	getStreamedSound()->fadein();
	getStreamedSound()->play();

	engine->resetBackgrounds(0);
	engine->resetBackgrounds(1);

	engine->setBrightness(0, 0);
	engine->setBrightness(1, 0);

	if ( top->filename != END ) {
		if ( bgTop->load(top->filename) )
			bgTop->setVisibility(true);
		else
			bgTop->setVisibility(false);
	}
	if ( bottom->filename != END ) {
		if ( bgBottom->load(bottom->filename) )
			bgBottom->setVisibility(true);
		else
			bgBottom->setVisibility(false);
	}

	while ( time < cutscenes[id].length && ! (keysHeld() & KEY_START) ) {
		engine->waitForDisplay();

		engine->update();

		if ( top->filename != END ) {
			if ( time > top->end ) {
				top++;
				if ( top->filename != END ) {
					if ( bgTop->load(top->filename, true) )
						bgTop->setVisibility(true);
					else
						bgTop->setVisibility(false);
				}
			} else {
				if ( time > top->start && time < top->end-16 )
					engine->fade(0, 16);
				else
					engine->fade(0, 0);
			}
		}

		if ( bottom->filename != END ) {
			if ( time > bottom->end ) {
				bottom++;
				if ( bottom->filename != END ) {
					if ( bgBottom->load(bottom->filename, true) )
						bgBottom->setVisibility(true);
					else
						bgBottom->setVisibility(false);
				}
			} else {
				if ( time > bottom->start && time < bottom->end-16 )
					engine->fade(1, 16);
				else
					engine->fade(1, 0);
			}
		}

		time++;
	}

	if ( stopMusic )
		getStreamedSound()->fadeout();
	engine->fade(0, 0);
	engine->fade(1, 0);
	for ( int i = 0 ; i < 16 ; i++ ) {
		engine->waitForDisplay();
		engine->update();
	}
}
