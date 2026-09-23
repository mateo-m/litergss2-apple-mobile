#include "LiteRGSS.h"
#include "RubyValue.h"
#include "DisplayWindowConfigLoader.h"

// mkxp-ios: a host app draws the picture in part of its window, and it
// needs the game's own resolution to keep the proportions. The game
// gives that resolution to DisplayWindow.new and to set_settings, and
// both calls come through here. The host app defines the function. A
// build without one links, because the symbol is weak.
extern "C" __attribute__((weak)) void psdk_game_resolution(long width, long height);

cgss::DisplayWindowVideoSettings DisplayWindowConfigLoader::loadVideoFromData(long width, long height, double scale, long bitsPerPixel) const {
	/* Adjust min width (this hardcoded value is not handled in LiteCGSS) */
	if (width != -1 && width < 160) {
		width = 160;
	}

	/* Adjust min height (this hardcoded value is not handled in LiteCGSS) */
	if (height != -1 && height < 144) {
		height = 144;
	}

	if (psdk_game_resolution && width > 0 && height > 0) {
		psdk_game_resolution(width, height);
	}

	return { bitsPerPixel, width, height, scale };
}
