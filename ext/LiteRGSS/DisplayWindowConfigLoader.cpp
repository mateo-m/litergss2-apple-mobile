#include "LiteRGSS.h"
#include "RubyValue.h"
#include "DisplayWindowConfigLoader.h"

#include <atomic>

// apple-mobile: a host app draws the picture in part of its window, and it
// needs the game's own resolution to keep the proportions. The game
// gives that resolution to DisplayWindow.new, set_settings and
// resize_screen, and each call reports it here.
namespace {
	std::atomic<void (*)(long, long, void*)> resolutionCallback { nullptr };
	std::atomic<void*> resolutionUserdata { nullptr };
}

extern "C" void litergss_set_resolution_callback(void (*callback)(long width, long height, void* userdata), void* userdata) {
	resolutionUserdata.store(userdata, std::memory_order_release);
	resolutionCallback.store(callback, std::memory_order_release);
}

void ReportGameResolution(long width, long height) {
	auto callback = resolutionCallback.load(std::memory_order_acquire);
	if (callback && width > 0 && height > 0) {
		callback(width, height, resolutionUserdata.load(std::memory_order_acquire));
	}
}

cgss::DisplayWindowVideoSettings DisplayWindowConfigLoader::loadVideoFromData(long width, long height, double scale, long bitsPerPixel) const {
	/* Adjust min width (this hardcoded value is not handled in LiteCGSS) */
	if (width != -1 && width < 160) {
		width = 160;
	}

	/* Adjust min height (this hardcoded value is not handled in LiteCGSS) */
	if (height != -1 && height < 144) {
		height = 144;
	}

	ReportGameResolution(width, height);

	return { bitsPerPixel, width, height, scale };
}
