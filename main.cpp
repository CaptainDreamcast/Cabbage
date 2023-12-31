#include <prism/framerateselectscreen.h>
#include <prism/physics.h>
#include <prism/file.h>
#include <prism/drawing.h>
#include <prism/log.h>
#include <prism/wrapper.h>
#include <prism/system.h>
#include <prism/stagehandler.h>
#include <prism/logoscreen.h>
#include <prism/mugentexthandler.h>
#include <prism/debug.h>
#include <prism/soundeffect.h>

#include "bookscreen.h"
#include "gamescreen.h"

#ifdef DREAMCAST
KOS_INIT_FLAGS(INIT_DEFAULT);

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

#endif

// #define DEVELOP

void exitGame() {
	shutdownPrismWrapper();

#ifdef DEVELOP
	if (isOnDreamcast()) {
		abortSystem();
	}
	else {
		returnToMenu();
	}
#else
	returnToMenu();
#endif
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	#ifdef DEVELOP
	setDevelopMode();
	#endif

	setGameName("THE DUDE WITH A WOLF, A GOAT AND A CABBAGE ~ A MODERN INTERPRETATION ~");
	setScreenSize(320, 240);
	
	initPrismWrapperWithConfigFile("data/config.cfg");
	setFont("$/rd/fonts/segoe.hdr", "$/rd/fonts/segoe.pkg");
	setMugenAnimationHandlerPixelCenter(Vector2D(0.0, 0.0));

	addMugenFont(-1, "font/f4x6.fnt");
	addMugenFont(1, "font/jg_german.fnt");
	addMugenFont(2, "font/f6x8f_german.fnt");

	logg("Check framerate");
	FramerateSelectReturnType framerateReturnType = selectFramerate();
	if (framerateReturnType == FRAMERATE_SCREEN_RETURN_ABORT) {
		exitGame();
	}

	if (isOnDreamcast())
	{
		setSoundEffectVolume(1.0);
	}
	else
	{
		setVolume(0.05);
		setSoundEffectVolume(0.2);
	}

	if(isInDevelopMode()) {
		ScreenSize sz = getScreenSize();
		//setDisplayedScreenSize(sz.x, sz.y);
		disableWrapperErrorRecovery();	
		setMinimumLogType(LOG_TYPE_NORMAL);
	}
	else {
		setMinimumLogType(LOG_TYPE_NONE);
	}
	setWrapperAbortEnabled(false);

	setBookName("intro");
	setScreenAfterWrapperLogoScreen(getLogoScreenFromWrapper());
	startScreenHandling(getBookScreen());

	exitGame();
	
	return 0;
}


