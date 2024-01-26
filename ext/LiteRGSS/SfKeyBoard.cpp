#include <SFML/Window.hpp>
#include "LiteRGSS.h"
#include "rbAdapter.h"

VALUE rb_SfKeyboard_press(VALUE self, VALUE key) {
	auto vkey = RB_NUM2LONG(key);

	return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(vkey)) ? Qtrue : Qfalse;
}

VALUE rb_SfKeyboard_localize(VALUE self, VALUE scan) {
	auto vscan = RB_NUM2LONG(scan);
	auto vkey = sf::Keyboard::localize(static_cast<sf::Keyboard::Scan::Scancode>(vscan));

	return LONG2NUM(vkey);
}

VALUE rb_SfKeyboard_delocalize(VALUE self, VALUE key) {
	auto vkey = RB_NUM2LONG(key);
	auto vscan = sf::Keyboard::delocalize(static_cast<sf::Keyboard::Key>(vkey));

	return LONG2NUM(vscan);
}

void DefineRubySFMLKeyboardConstants(VALUE rb_mSfKeyboard) {
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
		rb_define_const(rb_mSfKeyboard, key, LONG2NUM(keyIndex++));
	}
}

void DefineRubySFMLScanCodeConstants(VALUE rb_mSfScancode) {
	static constexpr const char* SFML_KEY_NAMES[] = {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P",
		"Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Num1", "Num2", "Num3", "Num4",
		"Num5", "Num6", "Num7", "Num8", "Num9", "Num0", "Enter", "Escape", "Backspace",
		"Tab", "Space", "Hyphen", "Equal", "LBracket", "RBracket", "Backslash", "Semicolon",
		"Apostrophe", "Grave", "Comma", "Period", "Slash", "F1", "F2", "F3", "F4", "F5", "F6",
		"F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19",
		"F20", "F21", "F22", "F23", "F24", "CapsLock", "PrintScreen", "ScrollLock", "Pause",
		"Insert", "Home", "PageUp", "Delete", "End", "PageDown", "Right", "Left", "Down", "Up",
		"NumLock", "NumpadDivide", "NumpadMultiply", "NumpadMinus", "NumpadPlus", "NumpadEqual",
		"NumpadEnter", "NumpadDecimal", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5",
		"Numpad6", "Numpad7", "Numpad8", "Numpad9", "Numpad0", "NonUsBackslash", "Application",
		"Execute", "ModeChange", "Help", "Menu", "Select", "Redo", "Undo", "Cut", "Copy", "Paste",
		"VolumeMute", "VolumeUp", "VolumeDown", "MediaPlayPause", "MediaStop", "MediaNextTrack",
		"MediaPreviousTrack", "LControl", "LShift", "LAlt", "LSystem", "RControl", "RShift", "RAlt",
		"RSystem", "Back", "Forward", "Refresh", "Stop", "Search", "Favorites", "HomePage",
		"LaunchApplication1", "LaunchApplication2", "LaunchMail", "LaunchMediaSelect" };

	static_assert((sizeof(SFML_KEY_NAMES) / sizeof(SFML_KEY_NAMES[0])) == sf::Keyboard::Scan::Scancode::ScancodeCount);

	std::size_t keyIndex = 0;
	for (const auto* key : SFML_KEY_NAMES) {
		rb_define_const(rb_mSfScancode, key, LONG2NUM(keyIndex++));
	}
}

void Init_SfKeyboard() {
	VALUE rb_mSf = rb_define_module("Sf");
	VALUE rb_mSfKeyboard = rb_define_module_under(rb_mSf, "Keyboard");
	VALUE rb_mSfScancode = rb_define_module_under(rb_mSfKeyboard, "Scancode");

	DefineRubySFMLKeyboardConstants(rb_mSfKeyboard);
	DefineRubySFMLScanCodeConstants(rb_mSfScancode);
	rb_define_module_function(rb_mSfKeyboard, "press?", _rbf rb_SfKeyboard_press, 1);
	rb_define_module_function(rb_mSfKeyboard, "localize", _rbf rb_SfKeyboard_localize, 1);
	rb_define_module_function(rb_mSfKeyboard, "delocalize", _rbf rb_SfKeyboard_delocalize, 1);
}
