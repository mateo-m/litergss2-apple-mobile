
#include "InputMapping.h"
#include "MouseInputMapping.h"

const std::unordered_set<std::string> MouseInputMapping::VirtualKeyNamesAliases = { 
	"left", "middle", "right"
};

void MouseInputMapping::fill(InputMapping<MouseInputMapping>& mapping) {
	/* This is a default mapping */
	/* It is also possible to modify it from Ruby if an interface is provided */
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Left), { sf::Mouse::Left });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Middle), { sf::Mouse::Middle });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::Right), { sf::Mouse::Right });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::X1), { sf::Mouse::XButton1 });
	mapping.keymap(static_cast<VirtualKeyIndex>(VirtualKeys::X2), { sf::Mouse::XButton2 });
}
