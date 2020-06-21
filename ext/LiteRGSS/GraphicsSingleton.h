#ifndef CGraphics_H
#define CGraphics_H
#include <memory>
#include "RubyValue.h"
#include "GraphicsConfigLoader.h"
#include "GraphicsUpdateMessage.h"

#include "Views/DisplayWindow.h"

class GraphicsSingleton {
public:
	static GraphicsSingleton& Get() {
		static GraphicsSingleton cg;
		return cg;
	}

	virtual ~GraphicsSingleton() = default;

	unsigned long frameCount() const { return frame_count; }
	void setFrameCount(unsigned long frameCount) { frame_count = frameCount; }
	void setBrightness(unsigned char brightness) { m_gameWindow.setBrightness(brightness); }
	unsigned char brightness() const { return m_gameWindow.brightness(); }
	long screenWidth() const { return m_gameWindow.screenWidth(); }
	long screenHeight() const { return m_gameWindow.screenHeight(); }
	bool smoothScreen() const { return m_gameWindow.smoothScreen(); }
	double scale() const { return m_gameWindow.scale(); }
	auto frameRate() const { return m_gameWindow.frameRate(); }

	void init();
	void stop();
	bool isGameWindowOpen() const;
	void update(VALUE self, bool input = true);
	void sortZ();
	void updateOnlyInput(VALUE self);

	void windowDraw();

	VALUE takeSnapshot();
	void freeze(VALUE self);
	void transition(VALUE self, int argc, VALUE* argv);
	void resizeScreen(int width, int height); 
	void setShader(sf::RenderStates* shader);
	void setIcon(const sf::Image& icon);

	template <class T, class ... Args>
	T add(Args&& ... args) {
		return T::create(m_gameWindow, std::forward<Args>(args)...);
	}

	template <class T, class ... Args>
	T addView(Args&& ... args) {
		return m_gameWindow.template addView<T>(m_gameWindow, std::forward<Args>(args)...);
	}

	template <class T, class Owner, class ... Args>
	T addViewOn(Owner& owner, Args&& ... args) {
		return m_gameWindow.template addView<T>(owner, std::forward<Args>(args)...);
	}

	bool areShadersEnabled() const;

private:
	std::unique_ptr<GraphicsUpdateMessage> draw();
	GraphicsSingleton();

	void manageErrorMessage(VALUE self, const GraphicsUpdateMessage& message);
	sf::RenderTarget& updateDrawPreProc(sf::View& defview);
	void updateDrawPostProc();
	void updateProcessEvent(GraphicsUpdateMessage& message);
   
	bool InsideGraphicsUpdate = false;
	unsigned long frame_count = 0;

	cgss::MainEventDispatcher m_eventDispatcher;
	cgss::DisplayWindow m_gameWindow;

	GraphicsConfigLoader m_configLoader;
};

#endif
