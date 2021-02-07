#ifndef KeyboardInputMapping_H
#define KeyboardInputMapping_H

#include <unordered_set>
#include <SFML/Window/Keyboard.hpp>

struct XBox360Keys {
	static constexpr const auto A = -1;
	static constexpr const auto B = -2;
	static constexpr const auto X = -3;
	static constexpr const auto Y = -4;
	static constexpr const auto L = -5;
	static constexpr const auto R = -6;
	static constexpr const auto Back = -7;
	static constexpr const auto Start = -8;
	static constexpr const auto KeyCount = 8;
};

enum class KeyboardVirtualKeys {
	A = 0, B, X, Y, L, R, L2, R2, L3, R3, 
	Start, Select, Home, Up, Down, Left, Right,
	KeyCount
};

template <class>
class InputMapping;

struct KeyboardInputMapping {
	using VirtualKeys = KeyboardVirtualKeys;

	static const constexpr auto PhysicalKeyCount = sf::Keyboard::KeyCount;
	static const constexpr auto PhysicalNegativeOffset = XBox360Keys::KeyCount;

	static constexpr const char* VirtualKeyNames[] = { 
		"A", "B", "X", "Y", "L", "R", "L2", "R2", "L3", "R3",
		"START", "SELECT", "HOME", "UP", "DOWN", "LEFT", "RIGHT"
	};

	static const std::unordered_set<std::string>& VirtualKeyNamesAliases() {
		static std::unordered_set<std::string> aliases { "start", "select", "home", "up", "down", "left", "right" };
		return aliases;
	}

	static void fill(InputMapping<KeyboardInputMapping>&);
};

#endif