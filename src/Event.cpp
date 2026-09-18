#include "Event.h"
#include "BufferedSound.h"

using namespace Tepeyo;

bool playEvent(Game * game, int x, int y, char * img)
{
	Engine * engine = game->getEngine();
	Engine::log("playEvent(%s)", img);

	int12 X = sub12(fp12(x), game->getHero()->getX()),
		  Y = sub12(fp12(y), game->getHero()->getY());

	// If in event area
	if ( round12(distance12(X, Y)) > 80 )
		return false;

	// Fade screen
	engine->fade(0, 0);
	engine->fade(1, 0);
	while ( engine->getBrightness(0) != 0 ) {
		engine->waitForDisplay();
		engine->update();
	}
	// Load image
	engine->resetBackgrounds(0);
	if ( engine->getBackground(0, 0)->load(img) )
		engine->getBackground(0, 0)->setVisibility(true);
	// Disable level bgs and sprites
	engine->getBackground(1, 0)->setVisibility(false);
	engine->getBackground(2, 0)->setVisibility(false);
	engine->getBackground(3, 0)->setVisibility(false);
	engine->topSpritesOff();
	// Play sound
	BufferedSoundPlay(SND_AFFICHE);
	// Swap screens
	lcdSwap();
	// Display for 5 seconds
	engine->fade(0, 16);
	engine->fade(1, 16);
	for ( int i = 0 ; i < 5 * 60 ; i++ ) {
		engine->waitForDisplay();
		engine->update();
	}
	// Fade again
	engine->fade(0, 0);
	engine->fade(1, 0);
	while ( engine->getBrightness(0) != 0 ) {
		engine->waitForDisplay();
		engine->update();
	}
	// Re swap screens, Reload BGs and fade again
	lcdSwap();
	engine->waitForDisplay();
	engine->update();
	game->getLevel()->reloadTopBGs();
	engine->topSpritesOn();
	engine->fade(0, 16);
	engine->fade(1, 16);
	while ( engine->getBrightness(0) != 16 ) {
		engine->waitForDisplay();
		engine->update();
	}

	return true;
}
