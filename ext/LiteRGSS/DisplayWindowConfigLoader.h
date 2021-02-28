#ifndef DisplayWindowConfigLoader_H
#define DisplayWindowConfigLoader_H

#include <tuple>
#include "LiteCGSS/Configuration/DisplayWindowSettings.h"

class DisplayWindowConfigLoader {
public:
	cgss::DisplayWindowVideoSettings loadVideoFromData(long width, long height, double scale, long bitsPerPixel) const;
};

#endif