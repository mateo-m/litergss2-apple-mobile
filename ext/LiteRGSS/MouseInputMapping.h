#ifndef MouseInputMapping_H
#define MouseInputMapping_H

#include <unordered_set>
#include <SFML/Window/Mouse.hpp>

enum class MouseVirtualKeys {
	Left = 0,
	Right,
	Middle,
	X1,
	X2,
	KeyCount
};

template <class>
class InputMapping;

struct MouseInputMapping {

	using VirtualKeys = MouseVirtualKeys;
	static const constexpr auto PhysicalKeyCount = sf::Mouse::ButtonCount;
	static const constexpr auto PhysicalNegativeOffset = 0;

	static constexpr const char* VirtualKeyNames[] = {
		"LEFT", "RIGHT", "MIDDLE", "X1", "X2"
	};

	static const std::unordered_set<std::string> VirtualKeyNamesAliases;

	static void fill(InputMapping<MouseInputMapping>&);
};

#endif