#ifndef DisplayWindow_H
#define DisplayWindow_H

#include "DisplayWindowInput.h"
#include "CgssWrapper.h"
#include "RubyValue.h"

extern VALUE rb_cDisplayWindow;
extern VALUE rb_eStoppedWindowError;
extern VALUE rb_eClosedWindow;

void Init_DisplayWindow();

struct DisplayWindowElement : 
	public CgssInstance<DisplayWindowInput> {
	
	VALUE rShader = Qnil;
	VALUE rKeyboard = Qnil;
	VALUE rMouse = Qnil;
	/**
	 * Event addition
	 **/
	VALUE rOnClosed = Qnil;
	VALUE rOnResized = Qnil;
	VALUE rOnLostFocus = Qnil;
	VALUE rOnGainedFocus = Qnil;
	VALUE rOnTextEntered = Qnil;
	VALUE rOnKeyPressed = Qnil;
	VALUE rOnKeyReleased = Qnil;
	VALUE rOnMouseWheelScrolled = Qnil;
	VALUE rOnMouseButtonPressed = Qnil;
	VALUE rOnMouseButtonRelease = Qnil;
	VALUE rOnMouseMoved = Qnil;
	VALUE rOnMouseEntered = Qnil;
	VALUE rOnMouseLeft = Qnil;
	VALUE rOnJoystickButtonPressed = Qnil;
	VALUE rOnJoystickButtonReleased = Qnil;
	VALUE rOnJoystickMoved = Qnil;
	VALUE rOnJoystickConnected = Qnil;
	VALUE rOnJoystickDisconnected = Qnil;
	VALUE rOnTouchBegan = Qnil;
	VALUE rOnTouchMoved = Qnil;
	VALUE rOnTouchEnded = Qnil;
	VALUE rOnSensorChanged = Qnil;

	template <class Drawable, class ... Args>
	void initAndAdd(Drawable& drawable, Args&& ... args) {
		drawable.init(Drawable::create(*instance(), std::forward<Args>(args)...));
	}
};

#endif
