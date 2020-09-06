#ifndef MouseInputMapping_H
#define MouseInputMapping_H

#include <unordered_set>
#include <SFML/Window/Mouse.hpp>
#include "BaseInputMapping.h"

enum class MouseVirtualKeys {
	Left = 0,
	Right,
	Middle,
	X1,
	X2,
	KeyCount
};

struct MouseInputMapping :
	public BaseInputMapping<sf::Mouse::ButtonCount, static_cast<std::size_t>(MouseVirtualKeys::KeyCount)>  {

	using VirtualKeys = MouseVirtualKeys;

	static constexpr MappingEntry VirtualKeyNames[] = {
		"LEFT", "RIGHT", "MIDDLE", "X1", "X2"
	};

	static const std::unordered_set<std::string> VirtualKeyNamesAliases;

	MouseInputMapping();
	~MouseInputMapping() = default;
};

#endif