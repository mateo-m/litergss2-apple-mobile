#include "LiteRGSS.h"
#include "rbAdapter.h"
#include "InputMappingTable.h"
#include "InputModule.h"
#include "NormalizeNumbers.h"

VALUE rb_mInputModule = Qnil;
VALUE rb_mMouseModule = Qnil;

VALUE rb_InputModule_Press(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping->isPressed(keyName) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_Trigger(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping->isTriggered(keyName) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_Repeat(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping->repeat(keyName) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_Released(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.keyMapping->isReleased(keyName) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_dir4(VALUE self)
{
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up)))
		return LONG2FIX(8);
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down)))
		return LONG2FIX(2);
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
		return LONG2FIX(4);
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
		return LONG2FIX(6);
	return LONG2FIX(0);
}

VALUE rb_InputModule_dir8(VALUE self)
{
	// Up Left / Up Right / Up
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Up)))
	{
		if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
			return LONG2FIX(7);
		else if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
			return LONG2FIX(9);
		return LONG2FIX(8);
	}
	// Down Left / Down Right / Down
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Down)))
	{
		if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
			return LONG2FIX(1);
		else if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
			return LONG2FIX(3);
		return LONG2FIX(2);
	}
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Left)))
		return LONG2FIX(4);
	if(MainInput.keyMapping->isPressed(static_cast<VirtualKeyIndex>(KeyboardVirtualKeys::Right)))
		return LONG2FIX(6);
	return LONG2FIX(0);
}

VALUE rb_InputModule_setMainJoypad(VALUE self, VALUE id)
{
	rb_check_type(id, T_FIXNUM);
	MainInput.mainJoy = rb_num2long(id);
	return id;
}

VALUE rb_InputModule_setMainXAxis(VALUE self, VALUE axis)
{
	rb_check_type(axis, T_FIXNUM);
	MainInput.mainAxisX = rb_num2long(axis);
	return axis;
}

VALUE rb_InputModule_setMainYAxis(VALUE self, VALUE axis)
{
	rb_check_type(axis, T_FIXNUM);
	MainInput.mainAxisY = rb_num2long(axis);
	return axis;
}

VALUE rb_InputModule_getMainJoypad(VALUE self)
{
	return LONG2FIX(MainInput.mainJoy);
}

VALUE rb_InputModule_getMainXAxis(VALUE self)
{
	return LONG2FIX(MainInput.mainAxisX);
}

VALUE rb_InputModule_getMainYAxis(VALUE self)
{
	return LONG2FIX(MainInput.mainAxisY);
}

VALUE rb_InputModule_getText(VALUE self)
{
	if (MainInput.enteredText.size() > 0)
		return rb_utf8_str_new_cstr(MainInput.enteredText.c_str());
	return Qnil;
}

VALUE rb_InputModule_getInvertAxisX(VALUE self)
{
	return MainInput.invertAxisX ? Qtrue : Qfalse;
}

VALUE rb_InputModule_setInvertAxisX(VALUE self, VALUE val)
{
	MainInput.invertAxisX = RTEST(val);
	return self;
}

VALUE rb_InputModule_getInvertAxisY(VALUE self)
{
	return MainInput.invertAxisY ? Qtrue : Qfalse;
}

VALUE rb_InputModule_setInvertAxisY(VALUE self, VALUE val)
{
	MainInput.invertAxisY = RTEST(val);
	return self;
}

VALUE rb_InputModule_JoyConnected(VALUE self, VALUE id)
{
	return sf::Joystick::isConnected(NUM2UINT(id)) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_JoyGetButtonCount(VALUE self, VALUE id)
{
	return UINT2NUM(sf::Joystick::getButtonCount(NUM2UINT(id)));
}

VALUE rb_InputModule_JoyHasAxis(VALUE self, VALUE id, VALUE axis)
{
	return sf::Joystick::hasAxis(NUM2UINT(id), static_cast<sf::Joystick::Axis>(NUM2LONG(axis))) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_JoyIsButtonPressed(VALUE self, VALUE id, VALUE button)
{
	return sf::Joystick::isButtonPressed(NUM2UINT(id), NUM2UINT(button)) ? Qtrue : Qfalse;
}

VALUE rb_InputModule_JoyGetAxisPosition(VALUE self, VALUE id, VALUE axis)
{
	return LONG2NUM(static_cast<long>(sf::Joystick::getAxisPosition(NUM2UINT(id), static_cast<sf::Joystick::Axis>(NUM2LONG(axis)))));
}

VALUE rb_InputModule_JoyGetName(VALUE self, VALUE id)
{
	sf::String name = sf::Joystick::getIdentification(NUM2LONG(id)).name;
	return rb_utf8_str_new_cstr(reinterpret_cast<const char*>(name.toUtf8().c_str()));
}

VALUE rb_MouseModule_Press(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.mouseMapping->isPressed(keyName) ? Qtrue : Qfalse;
}

VALUE rb_MouseModule_Trigger(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.mouseMapping->isTriggered(keyName) ? Qtrue : Qfalse;
}

VALUE rb_MouseModule_Released(VALUE self, VALUE keyId)
{
	const char* keyName = rb_id2name(SYM2ID(keyId));
	return MainInput.mouseMapping->isReleased(keyName) ? Qtrue : Qfalse;
}

VALUE rb_MouseModule_x(VALUE self) {
	return LONG2NUM(static_cast<long>(MainInput.mousePosX / GraphicsSingleton::Get().scale()));
}

VALUE rb_MouseModule_y(VALUE self) {
	return LONG2NUM(static_cast<long>(MainInput.mousePosY / GraphicsSingleton::Get().scale()));
}

VALUE rb_MouseModule_Wheel(VALUE self) {
	return LONG2NUM(MainInput.mouseWheelDelta);
}

VALUE rb_MouseModule_Wheel_set(VALUE self, VALUE val) {
	MainInput.mouseWheelDelta = NUM2LONG(val);
	return val;
}

VALUE rb_KeyBoard_Press(VALUE self, VALUE val) {
	return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(rb_num2long(val))) ? Qtrue : Qfalse;
}

void DefineRubySFMLKeyboardConstants() {
	assert(!NIL_P(rb_mInputModule));
	VALUE rb_mKeyboard = rb_define_module_under(rb_mInputModule, "Keyboard");
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
		rb_define_const(rb_mKeyboard, key, LONG2NUM(keyIndex++));
	}
}

void DefineRubySFMLJoypadConstants() {
	assert(!NIL_P(rb_mInputModule));
	rb_define_const(rb_mInputModule, "JoyAxisX", LONG2NUM(sf::Joystick::Axis::X));
	rb_define_const(rb_mInputModule, "JoyAxisY", LONG2NUM(sf::Joystick::Axis::Y));
	rb_define_const(rb_mInputModule, "JoyAxisZ", LONG2NUM(sf::Joystick::Axis::Z));
	rb_define_const(rb_mInputModule, "JoyAxisR", LONG2NUM(sf::Joystick::Axis::R));
	rb_define_const(rb_mInputModule, "JoyAxisU", LONG2NUM(sf::Joystick::Axis::U));
	rb_define_const(rb_mInputModule, "JoyAxisV", LONG2NUM(sf::Joystick::Axis::V));
	rb_define_const(rb_mInputModule, "JoyAxisPovX", LONG2NUM(sf::Joystick::Axis::PovX));
	rb_define_const(rb_mInputModule, "JoyAxisPovY", LONG2NUM(sf::Joystick::Axis::PovY));
}

void DefineRubySFMLMouseBinding() {
	VALUE rb_mMouseKey = rb_hash_new();
	rb_define_const(rb_mMouseModule, "Keys", rb_mMouseKey);
	rb_gc_register_address(&rb_mMouseKey); // Protect the Hash from being GC'd
	RHASH_SET_IFNONE(rb_mMouseKey, LONG2NUM(sf::Mouse::Button::Left));

	for (const auto* vkeyName : MouseInputMapping::VirtualKeyNames) {
		const auto& physicalKeyCodes = MainInput.mouseMapping->mapping().reverseLookup(vkeyName);
		assert(physicalKeyCodes.size() == 1);
		VALUE tmp = LONG2NUM(physicalKeyCodes[0]);
		rb_hash_aset(rb_mMouseKey, rb_id2sym(rb_intern(vkeyName)), tmp);

		/* Add lower case aliases */
		std::string lowerVKeyName = vkeyName;
		std::transform(lowerVKeyName.begin(), lowerVKeyName.end(), lowerVKeyName.begin(), [](unsigned char c){ return std::tolower(c); });
		if (MouseInputMapping::VirtualKeyNamesAliases().count(lowerVKeyName) != 0) {
			rb_hash_aset(rb_mMouseKey, rb_id2sym(rb_intern(lowerVKeyName.c_str())), tmp);
		}
	}
}

void DefineRubySFMLKeyboardBinding() {
	VALUE rb_mInputKey = rb_hash_new();
	rb_define_const(rb_mInputModule, "Keys", rb_mInputKey);
	rb_gc_register_address(&rb_mInputKey); // Protect the Hash from being GC'd
	RHASH_SET_IFNONE(rb_mInputKey, rb_class_new_instance(0, nullptr, rb_cInputMappingTable));

	for (const auto* vkeyName : KeyboardInputMapping::VirtualKeyNames) {
		VALUE tmp = rb_class_new_instance(0, nullptr, rb_cInputMappingTable);
		auto& tableElement = rb::Get<InputMappingTableElement>(tmp);

		tableElement.vIndex = MainInput.keyMapping->mapping().lookup(vkeyName);
		tableElement.data = &MainInput.keyMapping->mapping();

		rb_hash_aset(rb_mInputKey, rb_id2sym(rb_intern(vkeyName)), tmp);

		/* Add lower case aliases */
		std::string lowerVKeyName = vkeyName;
		std::transform(lowerVKeyName.begin(), lowerVKeyName.end(), lowerVKeyName.begin(), [](unsigned char c){ return std::tolower(c); });
		if (KeyboardInputMapping::VirtualKeyNamesAliases().count(lowerVKeyName) != 0) {
			rb_hash_aset(rb_mInputKey, rb_id2sym(rb_intern(lowerVKeyName.c_str())), tmp);
		}
	}
}

void Init_Input()
{
	rb_mInputModule = rb_define_module_under(rb_mLiteRGSS, "Input");
	rb_define_module_function(rb_mInputModule, "press?", _rbf rb_InputModule_Press, 1);
	rb_define_module_function(rb_mInputModule, "trigger?", _rbf rb_InputModule_Trigger, 1);
	rb_define_module_function(rb_mInputModule, "repeat?", _rbf rb_InputModule_Repeat, 1);
	rb_define_module_function(rb_mInputModule, "released?", _rbf rb_InputModule_Released, 1);
	rb_define_module_function(rb_mInputModule, "dir4", _rbf rb_InputModule_dir4, 0);
	rb_define_module_function(rb_mInputModule, "dir8", _rbf rb_InputModule_dir8, 0);
	rb_define_module_function(rb_mInputModule, "main_joy", _rbf rb_InputModule_getMainJoypad, 0);
	rb_define_module_function(rb_mInputModule, "main_joy=", _rbf rb_InputModule_setMainJoypad, 1);
	rb_define_module_function(rb_mInputModule, "x_axis", _rbf rb_InputModule_getMainXAxis, 0);
	rb_define_module_function(rb_mInputModule, "x_axis=", _rbf rb_InputModule_setMainXAxis, 1);
	rb_define_module_function(rb_mInputModule, "y_axis", _rbf rb_InputModule_getMainYAxis, 0);
	rb_define_module_function(rb_mInputModule, "y_axis=", _rbf rb_InputModule_setMainYAxis, 1);
	rb_define_module_function(rb_mInputModule, "x_axis_inverted", _rbf rb_InputModule_getInvertAxisX, 0);
	rb_define_module_function(rb_mInputModule, "x_axis_inverted=", _rbf rb_InputModule_setInvertAxisX, 1);
	rb_define_module_function(rb_mInputModule, "y_axis_inverted", _rbf rb_InputModule_getInvertAxisY, 0);
	rb_define_module_function(rb_mInputModule, "y_axis_inverted=", _rbf rb_InputModule_setInvertAxisY, 1);
	rb_define_module_function(rb_mInputModule, "get_text", _rbf rb_InputModule_getText, 0);
	rb_define_module_function(rb_mInputModule, "joy_connected?", _rbf rb_InputModule_JoyConnected, 1);
	rb_define_module_function(rb_mInputModule, "joy_button_count", _rbf rb_InputModule_JoyGetButtonCount, 1);
	rb_define_module_function(rb_mInputModule, "joy_has_axis?", _rbf rb_InputModule_JoyHasAxis, 2);
	rb_define_module_function(rb_mInputModule, "joy_button_press?", _rbf rb_InputModule_JoyIsButtonPressed, 2);
	rb_define_module_function(rb_mInputModule, "joy_axis_position", _rbf rb_InputModule_JoyGetAxisPosition, 2);
	rb_define_module_function(rb_mInputModule, "joy_name", _rbf rb_InputModule_JoyGetName, 1);

	rb_mMouseModule = rb_define_module_under(rb_mLiteRGSS, "Mouse");
	rb_define_module_function(rb_mMouseModule, "press?", _rbf rb_MouseModule_Press, 1);
	rb_define_module_function(rb_mMouseModule, "trigger?", _rbf rb_MouseModule_Trigger, 1);
	rb_define_module_function(rb_mMouseModule, "released?", _rbf rb_MouseModule_Released, 1);
	rb_define_module_function(rb_mMouseModule, "x", _rbf rb_MouseModule_x, 0);
	rb_define_module_function(rb_mMouseModule, "y", _rbf rb_MouseModule_y, 0);
	rb_define_module_function(rb_mMouseModule, "wheel", _rbf rb_MouseModule_Wheel, 0);
	rb_define_module_function(rb_mMouseModule, "wheel=", _rbf rb_MouseModule_Wheel_set, 1);

	DefineRubySFMLMouseBinding();
	DefineRubySFMLKeyboardBinding();

	DefineRubySFMLJoypadConstants();
	DefineRubySFMLKeyboardConstants();
}
