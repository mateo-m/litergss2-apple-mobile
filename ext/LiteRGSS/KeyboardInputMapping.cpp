#include "InputMapping.h"
#include "KeyboardInputMapping.h"

const std::unordered_set<std::string> KeyboardInputMapping::VirtualKeyNamesAliases = { 
	"start", "select", "home", "up", "down", "left", "right"
};

void KeyboardInputMapping::fill(InputMapping<KeyboardInputMapping>& mapping) {
	/* This is a default mapping */
	/* It is also possible to modify it from Ruby if an interface is provided */
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::A), { sf::Keyboard::C, sf::Keyboard::Enter, sf::Keyboard::Space, XBox360Keys::A });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::B), { sf::Keyboard::X, sf::Keyboard::Escape, sf::Keyboard::LShift, sf::Keyboard::RShift, sf::Keyboard::BackSpace, XBox360Keys::B });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::X), { sf::Keyboard::V, sf::Keyboard::LAlt, XBox360Keys::X });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Y), { sf::Keyboard::W, sf::Keyboard::RAlt, XBox360Keys::Y });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::L), { sf::Keyboard::A, XBox360Keys::L });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::R), { sf::Keyboard::E, XBox360Keys::R });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::L2), { sf::Keyboard::Num1 });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::R2), { sf::Keyboard::Num3 });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Start), { sf::Keyboard::B, XBox360Keys::Start });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Select), { sf::Keyboard::N, XBox360Keys::Back });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Home), { sf::Keyboard::LControl, sf::Keyboard::RControl });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Up), { sf::Keyboard::Up, sf::Keyboard::Z, sf::Keyboard::Numpad8 });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Down), { sf::Keyboard::Down, sf::Keyboard::S, sf::Keyboard::Numpad2 });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Left), { sf::Keyboard::Left, sf::Keyboard::Q, sf::Keyboard::Numpad4 });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Right), { sf::Keyboard::Right, sf::Keyboard::D, sf::Keyboard::Numpad6 });
}
