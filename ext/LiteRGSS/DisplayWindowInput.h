#ifndef DisplayWindowInput_H
#define DisplayWindowInput_H

#include <LiteCGSS/Views/DisplayWindow.h>
#include <LiteCGSS/Views/ViewAuthorizations.h>
#include <LiteCGSS/Common/Bindable.h>
#include "DisplayWindowUpdateMessage.h"
#include "RubyValue.h"

class DisplayWindowInput :
	public cgss::DisplayWindow {
public:
	DisplayWindowInput() = default;
	virtual ~DisplayWindowInput() = default;

	void update(VALUE self, bool input = true);
	void updateOnlyInput(VALUE self);

	void transition(VALUE self, int argc, VALUE* argv);

private:
	std::unique_ptr<DisplayWindowUpdateMessage> realDraw();
	void manageErrorMessage(VALUE self, const DisplayWindowUpdateMessage& message);
	void updateProcessEvent(VALUE self, DisplayWindowUpdateMessage& message);

	bool m_insideGraphicsUpdate = false;
};

namespace cgss {
	template <>
	struct ViewAuthorizations<DisplayWindowInput> : public ViewAuthorizations<cgss::DisplayWindow> {};
}

#endif
