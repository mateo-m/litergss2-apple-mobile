#ifndef CGraphicsConfig_H
#define CGraphicsConfig_H

#include <tuple>
#include "LiteCGSS/Configuration/DisplayWindowSettings.h"

class GraphicsConfigLoader {
public:
	cgss::DisplayWindowSettings load() const;

private:
	cgss::DisplayWindowVideoSettings loadVideoFromConfigs() const;
	bool loadSmoothScreenFromConfigs() const;
	std::string loadTitleFromConfigs() const;
	unsigned int loadFrameRateFromConfigs() const;
	bool loadVSYNCFromConfigs() const;
	bool loadFullScreenFromConfigs() const;
};

#endif