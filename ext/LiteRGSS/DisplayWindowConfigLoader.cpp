#include <SFML/Window/VideoMode.hpp>
#include "LiteRGSS.h"
#include "RubyValue.h"
#include "NormalizeNumbers.h"
#include "DisplayWindowConfigLoader.h"

cgss::DisplayWindowVideoSettings DisplayWindowConfigLoader::loadVideoFromConfigs() const {
	const ID screenWidthId = rb_intern("ScreenWidth");
	const ID screenHeightId = rb_intern("ScreenHeight");
	const ID screenScaleId = rb_intern("ScreenScale");
	const ID screenBitsPerPixelId = rb_intern("ScreenBitsPerPixel");

	const VALUE screenWidth = rb_const_defined(rb_mConfig, screenWidthId) ? rb_const_get(rb_mConfig, screenWidthId) : Qnil;
	const VALUE screenHeight = rb_const_defined(rb_mConfig, screenHeightId) ? rb_const_get(rb_mConfig, screenHeightId) : Qnil;
	const VALUE screenScale = rb_const_defined(rb_mConfig, screenScaleId) ? rb_const_get(rb_mConfig, screenScaleId) : Qnil;
	const VALUE screenBitsPerPixel = rb_const_defined(rb_mConfig, screenBitsPerPixelId) ? rb_const_get(rb_mConfig, screenBitsPerPixelId) : Qnil;

	return loadVideoFromData(
			screenWidth == Qnil ? -1 : rb_num2long(screenWidth),
			screenHeight == Qnil ? -1 : rb_num2long(screenHeight),
			screenScale == Qnil ? -1.0 : NUM2DBL(screenScale),
			screenBitsPerPixel == Qnil ? -1 : rb_num2long(screenBitsPerPixel));
}

cgss::DisplayWindowVideoSettings DisplayWindowConfigLoader::loadVideoFromData(long width, long height, double scale, long bitsPerPixel) const {
	sf::VideoMode vmode(640, 480, 32);
	
	int bitsPerPixelDefault = 32;
	long maxWidth = 0xFFFFFF;
	long maxHeight = 0xFFFFFF;
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	
	/* If there's a fullscreen mode */
	if (modes.size() > 0) {
		maxWidth = modes[0].width;
		maxHeight = modes[0].height;
		bitsPerPixelDefault = modes[0].bitsPerPixel;
		vmode.bitsPerPixel = modes[0].bitsPerPixel;
	}
	
	/* Adjust Width */
	if (width != -1) {
		vmode.width = normalize_long(width, 160, maxWidth);
	}

	/* Adjust Height */
	if (height != -1) {
		vmode.height = normalize_long(height, 144, maxHeight);
	}

	/* Adjust Scale */
	double normalized_scale = scale < 0.0 ? 1.0 : normalize_double(scale, 0.1, 10);
	
	/* Adjust Bits per pixel */
	vmode.bitsPerPixel = (bitsPerPixel != -1) ? bitsPerPixelDefault : normalize_long(bitsPerPixel, 16, bitsPerPixelDefault);	

	return { vmode.bitsPerPixel, vmode.width, vmode.height, normalized_scale };
}

cgss::DisplayWindowContextSettings DisplayWindowConfigLoader::loadContext() const {
	return sf::ContextSettings(0, 0, 0, 4, 5);
}

bool DisplayWindowConfigLoader::loadSmoothScreenFromConfigs() const {
	ID fsc = rb_intern("SmoothScreen");
	return rb_const_defined(rb_mConfig, fsc) && RTEST(rb_const_get(rb_mConfig, fsc));
}

std::string DisplayWindowConfigLoader::loadTitleFromConfigs() const {
	ID title = rb_intern("Title");
	if(rb_const_defined(rb_mConfig, title)) {
		VALUE str_title = rb_const_get(rb_mConfig, title);
		rb_check_type(str_title, T_STRING);
		std::string str(RSTRING_PTR(str_title));
		return sf::String::fromUtf8(str.begin(), str.end());
	}
	return "LiteRGSS2";
}

unsigned int DisplayWindowConfigLoader::loadFrameRateFromConfigs() const {
	ID framerate = rb_intern("FrameRate");
	if(rb_const_defined(rb_mConfig, framerate)) {
		return normalize_long(rb_num2long(rb_const_get(rb_mConfig, framerate)), 1, 240);
	}
	return 60;
}

bool DisplayWindowConfigLoader::loadVSYNCFromConfigs() const {
	ID vsync = rb_intern("Vsync");
	if(rb_const_defined(rb_mConfig, vsync)) {
		return RTEST(rb_const_get(rb_mConfig, vsync));
	}
	return true;
}

bool DisplayWindowConfigLoader::loadFullScreenFromConfigs() const {
	ID fsc = rb_intern("FullScreen");
	if(rb_const_defined(rb_mConfig, fsc)) {
		return RTEST(rb_const_get(rb_mConfig, fsc));
	}
	return false;
}

cgss::DisplayWindowSettings DisplayWindowConfigLoader::load() const {
	auto vmode = loadVideoFromConfigs();
	auto context = loadContext();
	auto smoothScreen = loadSmoothScreenFromConfigs();
	auto title = loadTitleFromConfigs();
	auto frameRate = loadFrameRateFromConfigs();
	auto vSync = loadVSYNCFromConfigs();
	auto fullscreen = loadFullScreenFromConfigs();
	return {
		false,
		std::move(vmode),
		std::move(context),
		smoothScreen,
		sf::String{std::move(title)},
		frameRate,
		vSync,
		fullscreen,
		false
	};
}
