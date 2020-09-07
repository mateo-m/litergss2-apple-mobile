#include <iostream>
#include "LiteRGSS.h"
#include "rbAdapter.h"
#include "InputMappingTable.h"
#include "Input.h"
#include "NormalizeNumbers.h"

VALUE rb_mInput = Qnil;
VALUE rb_mMouse = Qnil;

Input MainInput = {};

void L_Input_Mouse_Pos_Update(int x, int y)
{
	if (x < 0)
		x = -256;
	MainInput.mousePosX = static_cast<double>(x);
	MainInput.mousePosY = static_cast<double>(y);
}

void L_Input_Mouse_Wheel_Update(long delta)
{
	MainInput.mouseWheelDelta += delta;
}

void L_Input_Update_Joy(unsigned int joy_id, unsigned int key, bool state)
{
	if (!sf::Joystick::isConnected(joy_id))
		return;

	const KeyboardInputMapping::PhysicalKeyIndex physicalJoyKey = -(32 * joy_id) - key - 1;
	const KeyboardInputMapping::PhysicalKeyIndex positivePhysicalJoyKey = sf::Keyboard::KeyCount - physicalJoyKey - 1;
	MainInput.keyMapping.update(positivePhysicalJoyKey, state);
}

#define JOY_MIN_DEADZONE -25.0f
#define JOY_MAX_DEADZONE 25.0f

void L_Input_Update_JoyXPos(float position)
{
	if (MainInput.invertAxisX)
		position = -position;

	if (position < JOY_MIN_DEADZONE)
	{
		if (!MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
		{
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left), true);
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right), false);
		}
	}
	else if (position > JOY_MAX_DEADZONE)
	{
		if (!MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
		{
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left), false);
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right), true);
		}
	}
	else
	{
		if (MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right), false);
		if (MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left), false);
	}
}

void L_Input_Update_JoyYPos(float position)
{
	if (MainInput.invertAxisY)
		position = -position;

	if (position < JOY_MIN_DEADZONE)
	{
		if (!MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up)))
		{
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up), true);
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down), false);
		}
	}
	else if (position > JOY_MAX_DEADZONE)
	{
		if (!MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down)))
		{
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up), false);
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down), true);
		}
	}
	else
	{
		if (MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up)))
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up), false);
		if (MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down)))
			MainInput.keyMapping.forceUpdate(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down), false);
	}
}

void L_Input_Update_JoyPos(unsigned int joy_id, long axis, float position)
{
	if(joy_id != MainInput.mainJoy)
		return;

	if (axis == MainInput.mainAxisX)
		L_Input_Update_JoyXPos(position);
	else if (axis == MainInput.mainAxisY)
		L_Input_Update_JoyYPos(position);
}

void L_Input_Reset_JoyPos(unsigned int joy_id)
{
	L_Input_Update_JoyPos(joy_id, MainInput.mainAxisX, 0.0f);
	L_Input_Update_JoyPos(joy_id, MainInput.mainAxisY, 0.0f);
}
//////////////////////////////////////////////////////////////////////////////


VALUE rb_Input_Press(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping.isPressed(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_Trigger(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping.isTriggered(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_Repeat(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping.repeat(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Input_Released(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping.isReleased(keyName) ? Qtrue : Qfalse;
}


VALUE rb_Input_dir4(VALUE self)
{
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up)))
		return LONG2FIX(8);
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down)))
		return LONG2FIX(2);
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
		return LONG2FIX(4);
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
		return LONG2FIX(6);
	return LONG2FIX(0);
}

VALUE rb_Input_dir8(VALUE self)
{
	// Up Left / Up Right / Up
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Up)))
	{
		if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
			return LONG2FIX(7);
		else if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
			return LONG2FIX(9);
		return LONG2FIX(8);
	}
	// Down Left / Down Right / Down
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Down)))
	{
		if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
			return LONG2FIX(1);
		else if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
			return LONG2FIX(3);
		return LONG2FIX(2);
	}
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
		return LONG2FIX(4);
	if(MainInput.keyMapping.isPressed(static_cast<KeyboardInputMapping::VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
		return LONG2FIX(6);
	return LONG2FIX(0);
}

VALUE rb_Input_setMainJoypad(VALUE self, VALUE id)
{
	rb_check_type(id, T_FIXNUM);
	MainInput.mainJoy = rb_num2long(id);
	return id;
}

VALUE rb_Input_setMainXAxis(VALUE self, VALUE axis)
{
	rb_check_type(axis, T_FIXNUM);
	MainInput.mainAxisX = rb_num2long(axis);
	return axis;
}

VALUE rb_Input_setMainYAxis(VALUE self, VALUE axis)
{
	rb_check_type(axis, T_FIXNUM);
	MainInput.mainAxisY = rb_num2long(axis);
	return axis;
}

VALUE rb_Input_getMainJoypad(VALUE self)
{
	return LONG2FIX(MainInput.mainJoy);
}

VALUE rb_Input_getMainXAxis(VALUE self)
{
	return LONG2FIX(MainInput.mainAxisX);
}

VALUE rb_Input_getMainYAxis(VALUE self)
{
	return LONG2FIX(MainInput.mainAxisY);
}

VALUE rb_Input_getText(VALUE self)
{
	if (MainInput.enteredText.size() > 0)
		return rb_utf8_str_new_cstr(MainInput.enteredText.c_str());
	return Qnil;
}

VALUE rb_Input_getInvertAxisX(VALUE self)
{
	return MainInput.invertAxisX ? Qtrue : Qfalse;
}

VALUE rb_Input_setInvertAxisX(VALUE self, VALUE val)
{
	MainInput.invertAxisX = RTEST(val);
	return self;
}

VALUE rb_Input_getInvertAxisY(VALUE self)
{
	return MainInput.invertAxisY ? Qtrue : Qfalse;
}

VALUE rb_Input_setInvertAxisY(VALUE self, VALUE val)
{
	MainInput.invertAxisY = RTEST(val);
	return self;
}

VALUE rb_Input_JoyConnected(VALUE self, VALUE id)
{
	return sf::Joystick::isConnected(NUM2UINT(id)) ? Qtrue : Qfalse;
}

VALUE rb_Input_JoyGetButtonCount(VALUE self, VALUE id)
{
	return UINT2NUM(sf::Joystick::getButtonCount(NUM2UINT(id)));
}

VALUE rb_Input_JoyHasAxis(VALUE self, VALUE id, VALUE axis)
{
	return sf::Joystick::hasAxis(NUM2UINT(id), static_cast<sf::Joystick::Axis>(NUM2LONG(axis))) ? Qtrue : Qfalse;
}

VALUE rb_Input_JoyIsButtonPressed(VALUE self, VALUE id, VALUE button)
{
	return sf::Joystick::isButtonPressed(NUM2UINT(id), NUM2UINT(button)) ? Qtrue : Qfalse;
}

VALUE rb_Input_JoyGetAxisPosition(VALUE self, VALUE id, VALUE axis)
{
	return LONG2NUM(static_cast<long>(sf::Joystick::getAxisPosition(NUM2UINT(id), static_cast<sf::Joystick::Axis>(NUM2LONG(axis)))));
}

VALUE rb_Input_JoyGetName(VALUE self, VALUE id)
{
	sf::String name = sf::Joystick::getIdentification(NUM2LONG(id)).name;
	return rb_utf8_str_new_cstr(reinterpret_cast<const char*>(name.toUtf8().c_str()));
}

VALUE rb_Mouse_Press(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	std::cout << "Mouse released Key name " << keyName << std::endl;
	return MainInput.mouseMapping.isPressed(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Mouse_Trigger(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.mouseMapping.isTriggered(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Mouse_Released(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.mouseMapping.isReleased(keyName) ? Qtrue : Qfalse;
}

VALUE rb_Mouse_x(VALUE self) {
	return LONG2NUM(static_cast<long>(MainInput.mousePosX / GraphicsSingleton::Get().scale()));
}

VALUE rb_Mouse_y(VALUE self) {
	return LONG2NUM(static_cast<long>(MainInput.mousePosY / GraphicsSingleton::Get().scale()));
}

VALUE rb_Mouse_Wheel(VALUE self) {
	return LONG2NUM(MainInput.mouseWheelDelta);
}

VALUE rb_Mouse_Wheel_set(VALUE self, VALUE val) {
	MainInput.mouseWheelDelta = NUM2LONG(val);
	return val;
}

VALUE rb_KeyBoard_Press(VALUE self, VALUE val) {
	return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(rb_num2long(val))) ? Qtrue : Qfalse;
}

void DefineRubySFMLKeyboardConstants() {
	assert(!NIL_P(rb_mInput));
	VALUE rb_mKeyboard = rb_define_module_under(rb_mInput, "Keyboard");
	/* function definition */
	rb_define_module_function(rb_mKeyboard, "press?", _rbf rb_KeyBoard_Press, 1);

	static constexpr const char* SFML_KEY_NAMES[] = { 
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", 
	  "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Num0", "Num1", "Num2", "Num3",
		"Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Escape", "LControl", "LShift",
		"LAlt", "LSystem", "RControl", "RShift", "RAlt", "RSystem", "Menu", "LBracket",
		"RBracket", "Semicolon", "Comma", "Period", "Quote", "Slash", "Backslash", "Tilde",
		"Equal", "Hyphen", "Space", "Enter", "Backspace", "Tab", "PageUp", "PageDown",
		"End", "Home", "Insert", "Delete", "Add", "Subtract", "Multiply", "Divide", "Left",
		"Right", "Up", "Down", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4",
		"Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9", "F1", "F2", "F3", "F4", "F5",
		"F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "Pause" };
	
	static_assert((sizeof(SFML_KEY_NAMES) / sizeof(SFML_KEY_NAMES[0])) == sf::Keyboard::KeyCount);

	std::size_t keyIndex = 0;
	for (const auto* key : SFML_KEY_NAMES) {
		std::cout << key << std::endl;
		rb_define_const(rb_mKeyboard, key, LONG2NUM(keyIndex++));
	}
}

void DefineRubySFMLJoypadConstants() {
	assert(!NIL_P(rb_mInput));
	rb_define_const(rb_mInput, "JoyAxisX", LONG2NUM(sf::Joystick::Axis::X));
	rb_define_const(rb_mInput, "JoyAxisY", LONG2NUM(sf::Joystick::Axis::Y));
	rb_define_const(rb_mInput, "JoyAxisZ", LONG2NUM(sf::Joystick::Axis::Z));
	rb_define_const(rb_mInput, "JoyAxisR", LONG2NUM(sf::Joystick::Axis::R));
	rb_define_const(rb_mInput, "JoyAxisU", LONG2NUM(sf::Joystick::Axis::U));
	rb_define_const(rb_mInput, "JoyAxisV", LONG2NUM(sf::Joystick::Axis::V));
	rb_define_const(rb_mInput, "JoyAxisPovX", LONG2NUM(sf::Joystick::Axis::PovX));
	rb_define_const(rb_mInput, "JoyAxisPovY", LONG2NUM(sf::Joystick::Axis::PovY));
}

void DefineRubySFMLMouseBinding() {
	VALUE rb_mMouseKey = rb_hash_new();
	rb_define_const(rb_mMouse, "Keys", rb_mMouseKey);
	rb_gc_register_address(&rb_mMouseKey); // Protect the Hash from being GC'd
	RHASH_SET_IFNONE(rb_mMouseKey, LONG2NUM(sf::Mouse::Button::Left));

	for (const auto* vkeyName : MouseInputMapping::VirtualKeyNames) {
		const auto& physicalKeyCodes = MainInput.mouseMapping.mapping().reverseLookup(vkeyName);
		assert(physicalKeyCodes.size() == 1);
		VALUE tmp = LONG2NUM(physicalKeyCodes[0]);
		rb_hash_aset(rb_mMouseKey, rb_id2sym(rb_intern(vkeyName)), tmp);

		/* Add lower case aliases */
		std::string lowerVKeyName = vkeyName;
		std::transform(lowerVKeyName.begin(), lowerVKeyName.end(), lowerVKeyName.begin(), [](unsigned char c){ return std::tolower(c); });
		if (MouseInputMapping::VirtualKeyNamesAliases.count(lowerVKeyName) != 0) {
			rb_hash_aset(rb_mMouseKey, rb_id2sym(rb_intern(lowerVKeyName.c_str())), tmp);
		}
	}
}

void DefineRubySFMLKeyboardBinding() {
	VALUE rb_mInputKey = rb_hash_new();
	rb_define_const(rb_mInput, "Keys", rb_mInputKey);
	rb_gc_register_address(&rb_mInputKey); // Protect the Hash from being GC'd
	RHASH_SET_IFNONE(rb_mInputKey, rb_ary_new());

	for (const auto* vkeyName : KeyboardInputMapping::VirtualKeyNames) {
		VALUE tmp = rb_class_new_instance(0, nullptr, rb_cInputMappingTable);
		auto& tableElement = rb::Get<InputMappingTableElement>(tmp);

		auto& physicalKeyCodes = MainInput.keyMapping.mapping().reverseLookup(vkeyName);
		tableElement.data = &physicalKeyCodes;

		rb_hash_aset(rb_mInputKey, rb_id2sym(rb_intern(vkeyName)), tmp);

		/* Add lower case aliases */
		std::string lowerVKeyName = vkeyName;
		std::transform(lowerVKeyName.begin(), lowerVKeyName.end(), lowerVKeyName.begin(), [](unsigned char c){ return std::tolower(c); });
		if (KeyboardInputMapping::VirtualKeyNamesAliases.count(lowerVKeyName) != 0) {
			rb_hash_aset(rb_mInputKey, rb_id2sym(rb_intern(lowerVKeyName.c_str())), tmp);
		}
	}
}

void Init_Input()
{
	rb_mInput = rb_define_module_under(rb_mLiteRGSS, "Input");
	rb_define_module_function(rb_mInput, "press?", _rbf rb_Input_Press, 1);
	rb_define_module_function(rb_mInput, "trigger?", _rbf rb_Input_Trigger, 1);
	rb_define_module_function(rb_mInput, "repeat?", _rbf rb_Input_Repeat, 1);
	rb_define_module_function(rb_mInput, "released?", _rbf rb_Input_Released, 1);
	rb_define_module_function(rb_mInput, "dir4", _rbf rb_Input_dir4, 0);
	rb_define_module_function(rb_mInput, "dir8", _rbf rb_Input_dir8, 0);
	rb_define_module_function(rb_mInput, "main_joy", _rbf rb_Input_getMainJoypad, 0);
	rb_define_module_function(rb_mInput, "main_joy=", _rbf rb_Input_setMainJoypad, 1);
	rb_define_module_function(rb_mInput, "x_axis", _rbf rb_Input_getMainXAxis, 0);
	rb_define_module_function(rb_mInput, "x_axis=", _rbf rb_Input_setMainXAxis, 1);
	rb_define_module_function(rb_mInput, "y_axis", _rbf rb_Input_getMainYAxis, 0);
	rb_define_module_function(rb_mInput, "y_axis=", _rbf rb_Input_setMainYAxis, 1);
	rb_define_module_function(rb_mInput, "x_axis_inverted", _rbf rb_Input_getInvertAxisX, 0);
	rb_define_module_function(rb_mInput, "x_axis_inverted=", _rbf rb_Input_setInvertAxisX, 1);
	rb_define_module_function(rb_mInput, "y_axis_inverted", _rbf rb_Input_getInvertAxisY, 0);
	rb_define_module_function(rb_mInput, "y_axis_inverted=", _rbf rb_Input_setInvertAxisY, 1);
	rb_define_module_function(rb_mInput, "get_text", _rbf rb_Input_getText, 0);
	rb_define_module_function(rb_mInput, "joy_connected?", _rbf rb_Input_JoyConnected, 1);
	rb_define_module_function(rb_mInput, "joy_button_count", _rbf rb_Input_JoyGetButtonCount, 1);
	rb_define_module_function(rb_mInput, "joy_has_axis?", _rbf rb_Input_JoyHasAxis, 2);
	rb_define_module_function(rb_mInput, "joy_button_press?", _rbf rb_Input_JoyIsButtonPressed, 2);
	rb_define_module_function(rb_mInput, "joy_axis_position", _rbf rb_Input_JoyGetAxisPosition, 2);
	rb_define_module_function(rb_mInput, "joy_name", _rbf rb_Input_JoyGetName, 1);

	rb_mMouse = rb_define_module_under(rb_mLiteRGSS, "Mouse");
	rb_define_module_function(rb_mMouse, "press?", _rbf rb_Mouse_Press, 1);
	rb_define_module_function(rb_mMouse, "trigger?", _rbf rb_Mouse_Trigger, 1);
	rb_define_module_function(rb_mMouse, "released?", _rbf rb_Mouse_Released, 1);
	rb_define_module_function(rb_mMouse, "x", _rbf rb_Mouse_x, 0);
	rb_define_module_function(rb_mMouse, "y", _rbf rb_Mouse_y, 0);
	rb_define_module_function(rb_mMouse, "wheel", _rbf rb_Mouse_Wheel, 0);
	rb_define_module_function(rb_mMouse, "wheel=", _rbf rb_Mouse_Wheel_set, 1);

	DefineRubySFMLMouseBinding();
	DefineRubySFMLKeyboardBinding();

	DefineRubySFMLJoypadConstants();
	DefineRubySFMLKeyboardConstants();
	std::cout << "Test" << std::endl;
}
