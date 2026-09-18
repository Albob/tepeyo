/*
 * Tepeyollotl Engine
 * Enjmin 2008
 * @author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 */

#include "Engine.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "mt19937.h"
#include "../common/SoundSystem.h"
#include "StreamedSound.h"
#include "BufferedSound.h"

#ifdef _DEBUG
FILE * Engine::flog;
int Engine::logtime = 0;
bool Engine::logpending = 0;
#endif

#define ENABLE_CONSOLE 0

Engine::Engine()
{
  init();
  log("init() finished");

  //TODO classe shadow sprite
  //m_shadowSprites = new Sprite();
  for (int i = 0; i < MAX_SPRITES; i++)
    {
#ifdef _NDSBUILD_
      m_sprites[i] = new Sprite(SPRITE_GFX + (i * 64 * 64)); // 64x64 sized sprites
	  m_sprites16c[i] = new Sprite16c();
#else
	  m_sprites[i] = new Sprite(0); // TODO : PC Version
#endif
	  m_sprite_availability[i] = true;
	  m_sprite16c_availability[i] = true;
    }

  bgVramManagers[0] = new BGVramManager((u16 *) 0x06000000);
  bgVramManagers[1] = new BGVramManager((u16 *) 0x06200000);

  for ( int i = 0 ; i < MAX_SCREENS ; i++ )
	  for ( int j = 0 ; j < MAX_BG ; j++ )
		  m_backs[i][j] = new Background(i, j, bgVramManagers[i]);
}

/** Note to myself : ne servira que sur le PC */
Engine::~Engine()
{
	log("End of session");
}

void
Engine::init()
{
#ifdef _NDSBUILD_
  // Booting the NDS
  powerON(POWER_ALL_2D);
	
  videoSetMode(MODE_0_2D
	  | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
	  | DISPLAY_BG_EXT_PALETTE);
  videoSetModeSub(MODE_0_2D
	  | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
	  | DISPLAY_BG_EXT_PALETTE);
  lcdMainOnTop();

  // Activate all vram banks needed
  // Main sprites : 128K
  vramSetBankA(VRAM_A_MAIN_SPRITE_0x06400000);
  // Main BGs : 128K
  vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
  // Sub BGs : 128K
  vramSetBankC(VRAM_C_SUB_BG_0x06200000);
  // Sub sprites : 128K
  vramSetBankD(VRAM_D_SUB_SPRITE);
  // Main BG palettes : 32K
  vramSetBankF(VRAM_F_BG_EXT_PALETTE);
  vramSetBankG(VRAM_G_BG_EXT_PALETTE);
  // Sub BG palettes : 32K
  vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

  // Enable alpha blending
  BLEND_CR = BLEND_ALPHA
	  | BLEND_SRC_SPRITE
	  | BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BG2 | BLEND_DST_BG3 | BLEND_DST_BACKDROP;
  BLEND_AB = 0x0C04; // 25% blending
  SUB_BLEND_CR = BLEND_ALPHA
	  | BLEND_SRC_SPRITE
	  | BLEND_DST_BG0 | BLEND_DST_BG1 | BLEND_DST_BG2 | BLEND_DST_BG3 | BLEND_DST_BACKDROP;
  SUB_BLEND_AB = 0x0C04; // 25% blending

#if ENABLE_CONSOLE
  // Console
  BG0_CR = BG_MAP_BASE(0) | BG_TILE_BASE(1);
  BG_PALETTE[255] = RGB15(31, 31, 31);
  consoleInitDefault((u16 *) BG_MAP_RAM(0), (u16 *) BG_TILE_RAM(1), 16);
#endif

  irqInit();
  irqEnable(IRQ_VBLANK);

  if ( fatInitDefault() == 0 ) {
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    BG0_CR = BG_MAP_BASE(0) | BG_TILE_BASE(1);
    BG_PALETTE[255] = RGB15(31, 31, 31);
	BG_PALETTE[0] = RGB15(15, 0, 0);
	BG_PALETTE_SUB[0] = RGB15(15, 0, 0);
    consoleInitDefault((u16 *) BG_MAP_RAM(0), (u16 *) BG_TILE_RAM(1), 16);
	puts("\n\n          Tepeyollotl\n\n\n"
		"  FATAL ERROR !\n"
		"  File System init failed.\n"
		"  Try to DLDI patch.\n\n"
		"  More info on : \n"
		"  chishm.drunkencoders.com/DLDI");
	exit(EXIT_FAILURE); // does it works with devkitPro/libnds ?
  }

#ifdef _DEBUG
	flog = fopen("log_tepeyo.txt", "a");
	if ( flog == 0 ) {
		videoSetMode(MODE_0_2D);
		BG_PALETTE[0] = RGB15(31, 0, 0); // Red screen of death
		exit(EXIT_FAILURE);
	}
#endif

  // Clear sprites
  SpriteEntry * p_oam = (SpriteEntry *) 0x07000000,
			  * p_oam_sub = (SpriteEntry *) 0x07000400;
  for ( int i = 128 ; i > 0 ; i--, p_oam++, p_oam_sub++ ) {
	  p_oam->attribute[1] = 256;
	  p_oam_sub->attribute[1] = 256;
  }

  // Read some input to remove garbage
  scanKeys(); scanKeys(); scanKeys(); 
  touchReadXY(); touchReadXY(); touchReadXY();

  // Init screen to black
  setBrightness(0, 0);
  setBrightness(1, 0);
  // Set target brightness to normal
  targetBrightness[0] = 16;
  targetBrightness[1] = 16;

  // Timers used in the streaming sound system
  TIMER0_DATA = TIMER_FREQ(SoundCaCaSampleRate);
  TIMER0_CR = TIMER_ENABLE;
  TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
	
#else
 // PC init
#endif
  init_genrand(time(0));

  log("\n-----------------------\n"
	  "Beginning of session\n"
	  "Version : " __DATE__ " , " __TIME__);
}

void
Engine::log(const char* format, ...)
{
#ifdef _DEBUG
  //FILE * flog = fopen("log_tepeyo.txt", "a");

 // if (flog == NULL)
   // exit(EXIT_FAILURE);

  va_list args;
  va_start (args, format);
  vfiprintf (flog, format, args);
  fputs("\n", flog);
  viprintf(format, args);
  putchar('\n');
  va_end (args);
  
  logpending = true;
  //fclose(flog);  //done in Engine::update() now
#endif
}

void
Engine::logError(const char* format, ...)
{
#ifdef _DEBUG
  //FILE * flog = fopen("log_tepeyo.txt", "a");

 // if (flog == NULL)
   // exit(EXIT_FAILURE);

  va_list args;
  va_start (args, format);
  vfiprintf (flog, format, args);
  fputs("\n", flog);
  viprintf(format, args);
  putchar('\n');
  va_end (args);
  
  logpending = true;
  //fclose(flog);  //done in Engine::update() now

  fclose(flog);
  fopen("log_tepeyo.txt", "a");
#endif
}

void
Engine::waitForDisplay()
{
#ifdef _NDSBUILD_
  swiWaitForVBlank();
#else
  //doTropPleinDeTrucs();
#endif
}

Sprite*
Engine::getAvailableSprite()
{
	bool found = false;
	Sprite* pSprite = NULL;
	int i = 0;

	//log("Query for available Sprite");
	while (!found && i < MAX_SPRITES)
	{
		if (m_sprite_availability[i])
		{
			pSprite = m_sprites[i];
			m_sprite_availability[i] = false;
			found = true;
		}
		i++;
	}
	log("getAvailableSprite()->%p", pSprite);
	return pSprite;
}

Sprite16c*
Engine::getAvailableSprite16c()
{
	bool found = false;
	Sprite16c* pSprite16c = NULL;
	int i = 0;

	//log("Query for available Sprite");
	while (!found && i < MAX_SPRITES)
	{
		if (m_sprite16c_availability[i])
		{
			pSprite16c = m_sprites16c[i];
			m_sprite16c_availability[i] = false;
			found = true;
		}
		i++;
	}
	log("getAvailableSprite16c()->%p", pSprite16c);
	pSprite16c->show();
	pSprite16c->setPosition(256, 192, 256, 192);
	return pSprite16c;
}

void
Engine::releaseSprite(Sprite * pSprite)
{
	log("releaseSprite(%p)", pSprite);
	bool found = false;
	int i = 0;

	while (!found && i < MAX_SPRITES)
	{
		if (m_sprites[i] == pSprite)
		{
			m_sprite_availability[i] = true;
			m_sprites[i]->setPosition(256, 200, 256, 200);
			found = true;
		}
		i++;
	}
}

void
Engine::releaseSprite16c(Sprite16c * pSprite16c)
{
	log("releaseSprite16c(%p)", pSprite16c);
	bool found = false;
	int i = 0;

	while (!found && i < MAX_SPRITES)
	{
		if (m_sprites16c[i] == pSprite16c)
		{
			m_sprite16c_availability[i] = true;
			found = true;
		}
		i++;
	}
	m_sprites16c[i]->hide();
}

int
Engine::spriteComparator(const void * a, const void * b)
{
	Sprite ** A = (Sprite **) a;
	Sprite ** B = (Sprite **) b;
	int yA, yB;

	yA = (*A)->getPriority();
	yB = (*B)->getPriority();

	if ( yA == yB ) {
		yA = (*A)->getY();
		if ( yA >= 192 )
			yA -= 192;

		yB = (*B)->getY();
		if ( yB >= 192 )
			yB -= 192;

		return yB - yA;
	}

	return yA - yB;
}

void
Engine::update()
{
	int count = 0;

	getStreamedSound()->update();
	SoundPool::update();

#ifdef _NDSBUILD_
	scanKeys();
#endif

	//Engine::log("Engine::update()");
	static SpriteEntry shadowOAM_top[MAX_SPRITES];
	static SpriteEntry shadowOAM_bottom[MAX_SPRITES];
	static Sprite * tmpSprPtr[MAX_SPRITES];

	//update BGs
	for (int i = 0; i < MAX_BG; i++) {
		m_backs[0][i]->update();
		m_backs[1][i]->update();
	}

	// Update sprites
	for ( int i = 0 ; i < MAX_SPRITES ; i++ ) {
		if ( ! m_sprite_availability[i] ) { // sprite in use, add it !
			m_sprites[i]->update();
			count++;
			tmpSprPtr[i] = m_sprites[i];
		}
	}

	// Z sorting
	// (displays lower-Y sprites in back of higher's and thus mimic depth)
	qsort(tmpSprPtr, count, sizeof(Sprite *), Engine::spriteComparator);
	int i;
	for ( i = 0 ; i < count ; i++ )
		shadowOAM_top[i] = tmpSprPtr[i]->getTopSpriteEntry();
	for ( ; i < MAX_SPRITES ; i++ )
		shadowOAM_top[i].posX = 256;

	qsort(tmpSprPtr, count , sizeof(Sprite *), Engine::spriteComparator);
	for ( i = 0 ; i < count ; i++ )
		shadowOAM_bottom[i] = tmpSprPtr[i]->getBottomSpriteEntry();
	for ( ; i < MAX_SPRITES ; i++ )
		shadowOAM_bottom[i].posX = 256;

	// Copy sprites to OAM
#ifdef _NDSBUILD_
	DC_FlushRange(shadowOAM_top, MAX_SPRITES * sizeof(SpriteEntry));
	dmaCopy(shadowOAM_top, OAM, MAX_SPRITES * sizeof(SpriteEntry));
	DC_FlushRange(shadowOAM_bottom, MAX_SPRITES * sizeof(SpriteEntry));
	dmaCopy(shadowOAM_bottom, OAM_SUB, MAX_SPRITES * sizeof(SpriteEntry));
#else
	memcpy(OAM, shadowOAM_top, MAX_SPRITES * sizeof(SpriteEntry));
	memcpy(OAM_SUB, shadowOAM_bottom, MAX_SPRITES * sizeof(SpriteEntry));
#endif

	// 16c sprites
	for ( int i = 0 ; i < MAX_SPRITES ; i++ ) {
		shadowOAM_top[i] = m_sprites16c[i]->getTopSpriteEntry();
		shadowOAM_bottom[i] = m_sprites16c[i]->getBottomSpriteEntry();
	}

	// Z sorting
	// (displays lower-Y sprites in back of higher's and thus mimic depth)
	qsort(shadowOAM_top, MAX_SPRITES, sizeof(SpriteEntry),
		Engine::spriteComparator);
	qsort(shadowOAM_bottom, MAX_SPRITES, sizeof(SpriteEntry),
		Engine::spriteComparator);

	// Copy sprites to OAM
#ifdef _NDSBUILD_
	DC_FlushRange(shadowOAM_top, MAX_SPRITES * sizeof(SpriteEntry));
	dmaCopy(shadowOAM_top, OAM+MAX_SPRITES*sizeof(SpriteEntry)/sizeof(u16),
		MAX_SPRITES * sizeof(SpriteEntry));
	DC_FlushRange(shadowOAM_bottom, MAX_SPRITES * sizeof(SpriteEntry));
	dmaCopy(shadowOAM_bottom, OAM_SUB+MAX_SPRITES*sizeof(SpriteEntry)/sizeof(u16),
		MAX_SPRITES * sizeof(SpriteEntry));
#else
	memcpy(OAM, shadowOAM_top, MAX_SPRITES * sizeof(SpriteEntry));
	memcpy(OAM_SUB, shadowOAM_bottom, MAX_SPRITES * sizeof(SpriteEntry));
#endif

#ifdef _DEBUG
	logtime++;
	if ( logpending && logtime >= LOGTIME ) {
		fclose(flog);
		flog = fopen("log_tepeyo.txt", "a");
		if ( flog == 0 ) {
			videoSetMode(MODE_0_2D);
			BG_PALETTE[0] = RGB15(31, 0, 0); // Red screen of death
			exit(EXIT_FAILURE);
		}
		logtime = 0;
		logpending = false;
	}
#endif
	
	// Fade screens
	int current;
	current = getBrightness(0);
	if ( current < targetBrightness[0] )
		setBrightness(0, current+1);
	else if ( current > targetBrightness[0] )
		setBrightness(0, current-1);
	current = getBrightness(1);
	if ( current < targetBrightness[1] )
		setBrightness(1, current+1);
	else if ( current > targetBrightness[1] )
		setBrightness(1, current-1);
	//log("Brightness : %d %d", getBrightness(0), getBrightness(1));
}

touchPosition
Engine::getTouchPosition()
{
	return touchReadXY();
}

bool
Engine::isScreenTouched() const
{
#ifdef _NDSBUILD_
	return keysHeld() & KEY_TOUCH;
#else
	return false;
#endif
}

bool
Engine::isScreenClicked() const
{
#ifdef _NDSBUILD_
	return keysDown() & KEY_TOUCH;
#else
	return false;
#endif
}

bool
Engine::isScreenReleased() const
{
#ifdef _NDSBUILD_
	return keysUp() & KEY_TOUCH;
#else
	return false;
#endif
}

bool
Engine::isBlowing() const
{
#ifdef _NDSBUILD_
	return keysHeld() & KEY_X;
#else
	return false;
#endif
}

Background*
Engine::getBackground(int priority, int screen)
{
	return m_backs[screen][priority];
}

#define BRIGHT (1<<14)
#define DARK (2<<14)

void Engine::setBrightness(int screen, int power)
{
    vuint16 * scr_brightness = ( screen == 0 ) ? & BRIGHTNESS : & SUB_BRIGHTNESS ;

    if ( power < 16 ) {
		if ( power < 0 )
			power = 0;
		*scr_brightness = DARK | (16-power);
    } else if ( power > 16 ) {
		if ( power > 32 )
		    power = 32;
		*scr_brightness = BRIGHT | (power-16);
    } else {
		*scr_brightness = 0;
    }
}

int Engine::getBrightness(int screen)
{
    u16 scr_brightness = ( screen == 0 ) ? BRIGHTNESS : SUB_BRIGHTNESS ;

    if ( ! (scr_brightness & (BRIGHT|DARK)) )
		return 16;

    if ( scr_brightness & BRIGHT )
		return 16 + (scr_brightness & 31);

    return 16 - (scr_brightness & 31);
}

void Engine::fade(int screen, int brightness)
{
	targetBrightness[screen] = brightness;
}

void
Engine::resetBackgrounds(int screen)
{
	bgVramManagers[screen]->reset();
}


