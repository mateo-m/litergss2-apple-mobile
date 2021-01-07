#ifndef DisplayWindowInput_H
#define DisplayWindowInput_H

#include <LiteCGSS/Views/DisplayWindow.h>
#include <LiteCGSS/Views/ViewAuthorizations.h>
#include <LiteCGSS/Common/Bindable.h>
#include "GraphicsUpdateMessage.h"
#include "RubyValue.h"
#include "Input.h"

class DisplayWindowInput : 
	public cgss::DisplayWindow {
public:
	DisplayWindowInput() = default;
	virtual ~DisplayWindowInput() = default;

	void update(VALUE self, bool input = true);
	void updateOnlyInput(VALUE self);

	void transition(VALUE self, int argc, VALUE* argv);

private:
	std::unique_ptr<GraphicsUpdateMessage> realDraw();
	void manageErrorMessage(VALUE self, const GraphicsUpdateMessage& message);
	void updateProcessEvent(GraphicsUpdateMessage& message, DisplayWindowElement& window);

	bool m_insideGraphicsUpdate = false;
};

namespace cgss {
	template <>
	struct ViewAuthorizations<DisplayWindowInput> : public ViewAuthorizations<cgss::DisplayWindow> {};
}

#endif
