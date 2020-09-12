#ifndef DisplayWindowInput_H
#define DisplayWindowInput_H

#include <LiteCGSS/Views/DisplayWindow.h>
#include <LiteCGSS/Views/ViewAuthorizations.h>
#include <LiteCGSS/Common/Bindable.h>
#include "GraphicsUpdateMessage.h"
#include "RubyValue.h"
#include "Input.h"

class DisplayWindowInput : 
	public cgss::DisplayWindow,
	public cgss::Bindable<InputKeyboard>,
	public cgss::Bindable<InputMouse> {
public:
	DisplayWindowInput() = default;
	virtual ~DisplayWindowInput() = default;

	unsigned long frameCount() const { return m_frameCount; }
	void setFrameCount(unsigned long frameCount) { m_frameCount = frameCount; }

	void update(VALUE self, bool input = true);
	void updateOnlyInput(VALUE self);

	void transition(VALUE self, int argc, VALUE* argv);

private:
	void updateFromValue(const InputKeyboard* value) override {
		m_keyboard = cgss::Bindable<InputKeyboard>::directAccess();
	}
	void updateFromValue(const InputMouse* value) override {
		m_mouse = cgss::Bindable<InputMouse>::directAccess();
	}
	std::unique_ptr<GraphicsUpdateMessage> realDraw();
	void manageErrorMessage(VALUE self, const GraphicsUpdateMessage& message);
	void updateProcessEvent(GraphicsUpdateMessage& message);

	InputKeyboard* m_keyboard = nullptr;
	InputMouse* m_mouse = nullptr;
	bool m_insideGraphicsUpdate = false;
	unsigned long m_frameCount = 0;
};

namespace cgss {
	template <>
	struct ViewAuthorizations<DisplayWindowInput> : public ViewAuthorizations<cgss::DisplayWindow> {};
}

#endif