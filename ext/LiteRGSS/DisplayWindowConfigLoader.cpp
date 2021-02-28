#include "LiteRGSS.h"
#include "RubyValue.h"
#include "DisplayWindowConfigLoader.h"

cgss::DisplayWindowVideoSettings DisplayWindowConfigLoader::loadVideoFromData(long width, long height, double scale, long bitsPerPixel) const {
	/* Adjust min width (this hardcoded value is not handled in LiteCGSS) */
	if (width != -1 && width < 160) {
		width = 160;
	}

	/* Adjust min height (this hardcoded value is not handled in LiteCGSS) */
	if (height != -1 && height < 144) {
		height = 144;
	}

	return { bitsPerPixel, width, height, scale };
}
