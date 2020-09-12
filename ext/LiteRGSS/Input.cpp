#include "LiteRGSS.h"
#include "rbAdapter.h"
#include "InputMappingTable.h"
#include "Input.h"
#include "NormalizeNumbers.h"

VALUE rb_cInputKeyboard = Qnil;
VALUE rb_cInputMouse = Qnil;

InputMapping<KeyboardInputMapping> MainKeyboardMapping = {};
InputMapping<MouseInputMapping> MainMouseMapping = {};

static Input BuildInput() {
	auto result = Input{};
	result.keyMapping = std::make_unique<InputState<KeyboardInputMapping>>(MainKeyboardMapping);
	result.mouseMapping = std::make_unique<InputState<MouseInputMapping>>(MainMouseMapping);
	return result;
}

Input MainInput = BuildInput();

VALUE rb_Input_Initialize(int argc, VALUE *argv, VALUE self) {
	auto& keyboard = rb::Get<InputKeyboardElement>(self);
	keyboard.edit([](InputKeyboard& keyboard) {
		keyboard.keyMapping = std::make_unique<InputState<KeyboardInputMapping>>(MainKeyboardMapping);
	});
	return self;
}

VALUE rb_Mouse_Initialize(int argc, VALUE *argv, VALUE self) {
	auto& mouse = rb::Get<InputMouseElement>(self);
	mouse.edit([](InputMouse& mouse) {
		mouse.mouseMapping = std::make_unique<InputState<MouseInputMapping>>(MainMouseMapping);
	});
	return self;
}

void L_Input_Mouse_Pos_Update(InputMouse& input, int x, int y) {
	if (x < 0) {
		x = -256;
	}
	input.mousePosX = static_cast<double>(x);
	input.mousePosY = static_cast<double>(y);
}

void L_Input_Mouse_Wheel_Update(InputMouse& input, long delta) {
	input.mouseWheelDelta += delta;
}

void L_Input_Update_Joy(InputKeyboard& input, unsigned int joy_id, unsigned int key, bool state) {
	if (!sf::Joystick::isConnected(joy_id)) {
		return;
	}

	const PhysicalKeyIndex physicalJoyKey = -(32 * joy_id) - key - 1;
	input.keyMapping->update(physicalJoyKey, state);
}

#define JOY_MIN_DEADZONE -25.0f
#define JOY_MAX_DEADZONE 25.0f

void L_Input_Update_JoyXPos(InputKeyboard& input, float position) {
	if (input.invertAxisX) {
		position = -position;
	}

	if (position < JOY_MIN_DEADZONE) {
		if (!input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left), true);
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right), false);
		}
	} else if (position > JOY_MAX_DEADZONE) {
		if (!input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left), false);
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right), true);
		}
	} else {
		if (input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right), false);
		}
		if (input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left), false);
		}
	}
}

void L_Input_Update_JoyYPos(InputKeyboard& input, float position) {
	if (input.invertAxisY) {
		position = -position;
	}

	if (position < JOY_MIN_DEADZONE) {
		if (!input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up), true);
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down), false);
		}
	} else if (position > JOY_MAX_DEADZONE) {
		if (!input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up), false);
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down), true);
		}
	} else {
		if (input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up), false);
		}
		if (input.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down))) {
			input.keyMapping->forceUpdate(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down), false);
		}
	}
}

void L_Input_Update_JoyPos(InputKeyboard& input, unsigned int joy_id, long axis, float position) {
	if (joy_id != input.mainJoy) {
		return;
	}

	if (static_cast<unsigned int>(axis) == input.mainAxisX) {
		L_Input_Update_JoyXPos(input, position);
	} else if (static_cast<unsigned int>(axis) == input.mainAxisY) {
		L_Input_Update_JoyYPos(input, position);
	}
}

void L_Input_Reset_JoyPos(InputKeyboard& input, unsigned int joy_id) {
	L_Input_Update_JoyPos(input, joy_id, input.mainAxisX, 0.0f);
	L_Input_Update_JoyPos(input, joy_id, input.mainAxisY, 0.0f);
}

VALUE rb_Input_Press(VALUE self, VALUE keyId) {
	const char* keyName = rb_id2name(SYM2ID(keyId));
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return input->keyMapping->isPressed(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_Trigger(VALUE self, VALUE keyId) {
	const char* keyName = rb_id2name(SYM2ID(keyId));
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return input->keyMapping->isTriggered(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_Repeat(VALUE self, VALUE keyId) {
	const char* keyName = rb_id2name(SYM2ID(keyId));
	auto& input = rb::Get<InputKeyboardElement>(self);
	return input->keyMapping->repeat(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_Released(VALUE self, VALUE keyId) {
	const char* keyName = rb_id2name(SYM2ID(keyId));
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return input->keyMapping->isReleased(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_dir4(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);

	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up))) {
		return LONG2FIX(8);
	}

	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down))) {
		return LONG2FIX(2);
	}

	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left))) {
		return LONG2FIX(4);
	}

	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right))) {
		return LONG2FIX(6);
	}

	return LONG2FIX(0);
}

VALUE rb_Input_dir8(VALUE self)
{
	const auto& input = rb::Get<InputKeyboardElement>(self);
	// Up Left / Up Right / Up
	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up))) {
		if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left))) {
			return LONG2FIX(7);
		} else if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right))) {
			return LONG2FIX(9);
		}
		return LONG2FIX(8);
	}

	// Down Left / Down Right / Down
	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down))) {
		if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left))) {
			return LONG2FIX(1);
		} else if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right))) {
			return LONG2FIX(3);
		}
		return LONG2FIX(2);
	}

	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left))) {
		return LONG2FIX(4);
	}

	if (input->keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right))) {
		return LONG2FIX(6);
	}
	return LONG2FIX(0);
}

VALUE rb_Input_setMainJoypad(VALUE self, VALUE id) {
	auto& input = rb::Get<InputKeyboardElement>(self);
	rb_check_type(id, T_FIXNUM);
	input.edit([&id](InputKeyboard& keyboard) {
		keyboard.mainJoy = rb_num2long(id);
	});
	return id;
}

VALUE rb_Input_setMainXAxis(VALUE self, VALUE axis) {
	auto& input = rb::Get<InputKeyboardElement>(self);
	rb_check_type(axis, T_FIXNUM);
	input.edit([&axis](InputKeyboard& keyboard) {
		keyboard.mainAxisX = rb_num2long(axis);
	});
	return axis;
}

VALUE rb_Input_setMainYAxis(VALUE self, VALUE axis) {
	auto& input = rb::Get<InputKeyboardElement>(self);
	rb_check_type(axis, T_FIXNUM);
	input.edit([&axis](InputKeyboard& keyboard) {
		keyboard.mainAxisY = rb_num2long(axis);
	});
	return axis;
}

VALUE rb_Input_getMainJoypad(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return LONG2FIX(input->mainJoy);
}

VALUE rb_Input_getMainXAxis(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return LONG2FIX(input->mainAxisX);
}

VALUE rb_Input_getMainYAxis(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return LONG2FIX(input->mainAxisY);
}

VALUE rb_Input_getText(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);
	if (input->enteredText.size() > 0) {
		return rb_utf8_str_new_cstr(input->enteredText.c_str());
	}
	return Qnil;
}

VALUE rb_Input_getInvertAxisX(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return input->invertAxisX ? Qtrue : Qfalse;
}

VALUE rb_Input_setInvertAxisX(VALUE self, VALUE val) {
	auto& input = rb::Get<InputKeyboardElement>(self);
	input.edit([&val](InputKeyboard& keyboard) {
		keyboard.invertAxisX = RTEST(val);
	});
	return self;
}

VALUE rb_Input_getInvertAxisY(VALUE self) {
	const auto& input = rb::Get<InputKeyboardElement>(self);
	return input->invertAxisY ? Qtrue : Qfalse;
}

VALUE rb_Input_setInvertAxisY(VALUE self, VALUE val) {
	auto& input = rb::Get<InputKeyboardElement>(self);
	input.edit([&val](InputKeyboard& keyboard) {
		keyboard.invertAxisY = RTEST(val);
	});
	return self;
}

VALUE rb_Mouse_Press(VALUE self, VALUE keyId) {
	const auto& input = rb::Get<InputMouseElement>(self);
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return input->mouseMapping->isPressed(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Mouse_Trigger(VALUE self, VALUE keyId) {
	const auto& input = rb::Get<InputMouseElement>(self);
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return input->mouseMapping->isTriggered(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Mouse_Released(VALUE self, VALUE keyId) {
	const auto& input = rb::Get<InputMouseElement>(self);
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return input->mouseMapping->isReleased(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Mouse_x(VALUE self) {
	const auto& input = rb::Get<InputMouseElement>(self);
	return LONG2NUM(static_cast<long>(input->mousePosX / GraphicsSingleton::Get().scale()));
}

VALUE rb_Mouse_y(VALUE self) {
	const auto& input = rb::Get<InputMouseElement>(self);
	return LONG2NUM(static_cast<long>(input->mousePosY / GraphicsSingleton::Get().scale()));
}

VALUE rb_Mouse_Wheel(VALUE self) {
	const auto& input = rb::Get<InputMouseElement>(self);
	return LONG2NUM(input->mouseWheelDelta);
}

VALUE rb_Mouse_Wheel_set(VALUE self, VALUE val) {
	auto& input = rb::Get<InputMouseElement>(self);
	input.edit([&val](InputMouse& mouse) {
		mouse.mouseWheelDelta = NUM2LONG(val);
	});
	return val;
}

void Init_InputKeyboard() {
	rb_cInputKeyboard = rb_define_class_under(rb_mLiteRGSS, "InputKeyboard", rb_cObject);
	rb_define_alloc_func(rb_cInputKeyboard, rb::Alloc<InputKeyboardElement>);

	rb_define_method(rb_cInputKeyboard, "initialize", _rbf rb_Input_Initialize, -1);
	rb_define_method(rb_cInputKeyboard, "press?", _rbf rb_Input_Press, 1);
	rb_define_method(rb_cInputKeyboard, "trigger?", _rbf rb_Input_Trigger, 1);
	rb_define_method(rb_cInputKeyboard, "repeat?", _rbf rb_Input_Repeat, 1);
	rb_define_method(rb_cInputKeyboard, "released?", _rbf rb_Input_Released, 1);
	rb_define_method(rb_cInputKeyboard, "dir4", _rbf rb_Input_dir4, 0);
	rb_define_method(rb_cInputKeyboard, "dir8", _rbf rb_Input_dir8, 0);
	rb_define_method(rb_cInputKeyboard, "main_joy", _rbf rb_Input_getMainJoypad, 0);
	rb_define_method(rb_cInputKeyboard, "main_joy=", _rbf rb_Input_setMainJoypad, 1);
	rb_define_method(rb_cInputKeyboard, "x_axis", _rbf rb_Input_getMainXAxis, 0);
	rb_define_method(rb_cInputKeyboard, "x_axis=", _rbf rb_Input_setMainXAxis, 1);
	rb_define_method(rb_cInputKeyboard, "y_axis", _rbf rb_Input_getMainYAxis, 0);
	rb_define_method(rb_cInputKeyboard, "y_axis=", _rbf rb_Input_setMainYAxis, 1);
	rb_define_method(rb_cInputKeyboard, "x_axis_inverted", _rbf rb_Input_getInvertAxisX, 0);
	rb_define_method(rb_cInputKeyboard, "x_axis_inverted=", _rbf rb_Input_setInvertAxisX, 1);
	rb_define_method(rb_cInputKeyboard, "y_axis_inverted", _rbf rb_Input_getInvertAxisY, 0);
	rb_define_method(rb_cInputKeyboard, "y_axis_inverted=", _rbf rb_Input_setInvertAxisY, 1);
	rb_define_method(rb_cInputKeyboard, "get_text", _rbf rb_Input_getText, 0);
}

void Init_InputMouse() {
	rb_cInputMouse = rb_define_class_under(rb_mLiteRGSS, "InputMouse", rb_cObject);
	rb_define_alloc_func(rb_cInputMouse, rb::Alloc<InputMouseElement>);

	rb_define_method(rb_cInputMouse, "initialize", _rbf rb_Mouse_Initialize, -1);
	rb_define_method(rb_cInputMouse, "press?", _rbf rb_Mouse_Press, 1);
	rb_define_method(rb_cInputMouse, "trigger?", _rbf rb_Mouse_Trigger, 1);
	rb_define_method(rb_cInputMouse, "released?", _rbf rb_Mouse_Released, 1);
	rb_define_method(rb_cInputMouse, "x", _rbf rb_Mouse_x, 0);
	rb_define_method(rb_cInputMouse, "y", _rbf rb_Mouse_y, 0);
	rb_define_method(rb_cInputMouse, "wheel", _rbf rb_Mouse_Wheel, 0);
	rb_define_method(rb_cInputMouse, "wheel=", _rbf rb_Mouse_Wheel_set, 1);
}
