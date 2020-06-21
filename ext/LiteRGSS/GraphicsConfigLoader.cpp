#include "LiteRGSS.h"
#include "RubyValue.h"
#include "NormalizeNumbers.h"
#include "GraphicsConfigLoader.h"

cgss::DisplayWindowVideoSettings GraphicsConfigLoader::loadVideoFromConfigs() const {
	sf::VideoMode vmode(640, 480, 32);

	ID screenWidth = rb_intern("ScreenWidth");
	ID screenHeight = rb_intern("ScreenHeight");
	ID screenScale = rb_intern("ScreenScale");
	ID screenBitsPerPixel = rb_intern("ScreenBitsPerPixel");

	int bitsPerPixel = 32;
	long maxWidth = 0xFFFFFF;
	long maxHeight = 0xFFFFFF;
	double scale = 1.0;
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	
	/* If there's a fullscreen mode */
	if (modes.size() > 0) {
		maxWidth = modes[0].width;
		maxHeight = modes[0].height;
		bitsPerPixel = modes[0].bitsPerPixel;
		vmode.bitsPerPixel = modes[0].bitsPerPixel;
	}
	
	/* Adjust Width */
	if (rb_const_defined(rb_mConfig, screenWidth)) {
		vmode.width = normalize_long(rb_num2long(rb_const_get(rb_mConfig, screenWidth)), 160, maxWidth);
	}

	/* Adjust Height */
	if (rb_const_defined(rb_mConfig, screenHeight)) {
		vmode.height = normalize_long(rb_num2long(rb_const_get(rb_mConfig, screenHeight)), 144, maxHeight);
	}

	/* Adjust Scale */
	if (rb_const_defined(rb_mConfig, screenScale)) {
		scale = normalize_double(NUM2DBL(rb_const_get(rb_mConfig, screenScale)), 0.1, 10);
	}

	/* Adjust Bits per pixel */
	if (rb_const_defined(rb_mConfig, screenBitsPerPixel)) {
		vmode.bitsPerPixel = normalize_long(rb_num2long(rb_const_get(rb_mConfig, screenBitsPerPixel)), 16, bitsPerPixel);
	}

	return { vmode.bitsPerPixel, vmode.width, vmode.height, scale };
}

bool GraphicsConfigLoader::loadSmoothScreenFromConfigs() const {
	ID fsc = rb_intern("SmoothScreen");
	return rb_const_defined(rb_mConfig, fsc) && RTEST(rb_const_get(rb_mConfig, fsc));
}

std::string GraphicsConfigLoader::loadTitleFromConfigs() const {
	ID title = rb_intern("Title");
	if(rb_const_defined(rb_mConfig, title)) {
		VALUE str_title = rb_const_get(rb_mConfig, title);
		rb_check_type(str_title, T_STRING);
		std::string str(RSTRING_PTR(str_title));
		return sf::String::fromUtf8(str.begin(), str.end());
	}
	return "LiteRGSS2";
}

unsigned int GraphicsConfigLoader::loadFrameRateFromConfigs() const {
	ID framerate = rb_intern("FrameRate");
	if(rb_const_defined(rb_mConfig, framerate)) {
		return normalize_long(rb_num2long(rb_const_get(rb_mConfig, framerate)), 1, 240);
	}
	return 60;
}

bool GraphicsConfigLoader::loadVSYNCFromConfigs() const {
	ID vsync = rb_intern("Vsync");
	if(rb_const_defined(rb_mConfig, vsync)) {
		return RTEST(rb_const_get(rb_mConfig, vsync));
	}
	return true;
}

bool GraphicsConfigLoader::loadFullScreenFromConfigs() const {
	ID fsc = rb_intern("FullScreen");
	if(rb_const_defined(rb_mConfig, fsc)) {
		return RTEST(rb_const_get(rb_mConfig, fsc));
	}
	return false;
}

cgss::DisplayWindowSettings GraphicsConfigLoader::load() const {
	auto vmode = loadVideoFromConfigs();
	auto smoothScreen = loadSmoothScreenFromConfigs();
	auto title = loadTitleFromConfigs();
	auto frameRate = loadFrameRateFromConfigs();
	auto vSync = loadVSYNCFromConfigs();
	auto fullscreen = loadFullScreenFromConfigs();
	return {
		false,
		std::move(vmode),
		smoothScreen,
		sf::String{std::move(title)},
		frameRate,
		vSync,
		fullscreen,
		false
	};
}
