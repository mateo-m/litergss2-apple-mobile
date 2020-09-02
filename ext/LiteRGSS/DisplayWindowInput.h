#ifndef DisplayWindowInput_H
#define DisplayWindowInput_H

#include <LiteCGSS/Views/DisplayWindow.h>
#include <LiteCGSS/Views/ViewAuthorizations.h>
#include "GraphicsUpdateMessage.h"
#include "RubyValue.h"

class DisplayWindowInput : 
    public cgss::DisplayWindow {
public:
    DisplayWindowInput() = default;
    virtual ~DisplayWindowInput() = default;

	unsigned long frameCount() const { return m_frameCount; }
	void setFrameCount(unsigned long frameCount) { m_frameCount = frameCount; }
    
    void update(VALUE self, bool input = true);
	void updateOnlyInput(VALUE self);

	void transition(VALUE self, int argc, VALUE* argv);

private:
    std::unique_ptr<GraphicsUpdateMessage> realDraw();
	void manageErrorMessage(VALUE self, const GraphicsUpdateMessage& message);
	void updateProcessEvent(GraphicsUpdateMessage& message);

    bool m_insideGraphicsUpdate = false;
	unsigned long m_frameCount = 0;
};

namespace cgss {
	template <>
	struct ViewAuthorizations<DisplayWindowInput> : public ViewAuthorizations<cgss::DisplayWindow> {};
}

#endif