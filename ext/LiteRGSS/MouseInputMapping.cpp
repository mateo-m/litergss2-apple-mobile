
#include "MouseInputMapping.h"

const std::unordered_set<std::string> MouseInputMapping::VirtualKeyNamesAliases = { 
	"left", "middle", "right"
};

MouseInputMapping::MouseInputMapping() {
	/* This is a default mapping */
	/* It is also possible to modify it from Ruby if an interface is provided */
	keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Left), { sf::Mouse::Left });
	keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Middle), { sf::Mouse::Middle });
	keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Right), { sf::Mouse::Right });
	keymap(static_cast<VirtualKeyIndex>(VirtualKeys::X1), { sf::Mouse::XButton1 });
	keymap(static_cast<VirtualKeyIndex>(VirtualKeys::X2), { sf::Mouse::XButton2 });
}
