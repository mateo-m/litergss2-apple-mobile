#ifndef KeyboardInputMapping_H
#define KeyboardInputMapping_H

#include <unordered_set>
#include <SFML/Window/Keyboard.hpp>
#include "BaseInputMapping.h"

struct XBox360Keys {
	static constexpr const auto A = sf::Keyboard::KeyCount;
	static constexpr const auto B = sf::Keyboard::KeyCount + 1;
	static constexpr const auto X = sf::Keyboard::KeyCount + 2;
	static constexpr const auto Y = sf::Keyboard::KeyCount + 3;
	static constexpr const auto L = sf::Keyboard::KeyCount + 4;
	static constexpr const auto R = sf::Keyboard::KeyCount + 5;
	static constexpr const auto Back = sf::Keyboard::KeyCount + 6;
	static constexpr const auto Start = sf::Keyboard::KeyCount + 7;
	static constexpr const auto KeyCount = 8;
};

enum class KeyboardVirtualKeys {
	A = 0, B, X, Y, L, R, L2, R2, L3, R3, 
	Start, Select, Home, Up, Down, Left, Right,
	KeyCount
};

struct KeyboardInputMapping : 
	public BaseInputMapping<sf::Keyboard::KeyCount + XBox360Keys::KeyCount, static_cast<std::size_t>(KeyboardVirtualKeys::KeyCount)> {
	using VirtualKeys = KeyboardVirtualKeys;

	static constexpr MappingEntry VirtualKeyNames[] = { 
		"A", "B", "X", "Y", "L", "R", "L2", "R2", "L3", "R3",
		"START", "SELECT", "HOME", "UP", "DOWN", "LEFT", "RIGHT"
	};

	static const std::unordered_set<std::string> VirtualKeyNamesAliases;

	KeyboardInputMapping();
	virtual ~KeyboardInputMapping() = default;
};

#endif