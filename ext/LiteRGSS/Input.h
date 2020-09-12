#ifndef Input_H
#define Input_H
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include <LiteCGSS/Common/BondElement.h>
#include "KeyboardInputMapping.h"
#include "MouseInputMapping.h"
#include "InputState.h"
#include "RubyValue.h"

extern VALUE rb_cInputKeyboard;
extern VALUE rb_cInputMouse;

struct InputMouse {
	long mouseWheelDelta = 0;
	double mousePosX = 0;
	double mousePosY = 0;
	std::unique_ptr<InputState<MouseInputMapping>> mouseMapping;
};

struct InputKeyboard {
	bool invertAxisY = false;
	bool invertAxisX = false;
	std::string enteredText = {};
	unsigned int mainJoy = 0;
	unsigned int mainAxisX = sf::Joystick::Axis::X;
	unsigned int mainAxisY = sf::Joystick::Axis::Y;
	std::unique_ptr<InputState<KeyboardInputMapping>> keyMapping;
};

struct Input : 
	public InputKeyboard,
	public InputMouse {
};

extern InputMapping<KeyboardInputMapping> MainKeyboardMapping;
extern InputMapping<MouseInputMapping> MainMouseMapping;
extern Input MainInput;

using InputKeyboardElement = cgss::BondElement<InputKeyboard>;
using InputMouseElement = cgss::BondElement<InputMouse>;

void L_Input_Update_Joy(InputKeyboard& input, unsigned int joy_id, unsigned int key, bool state);
void L_Input_Update_JoyPos(InputKeyboard& input, unsigned int joy_id, long axis, float position);
void L_Input_Reset_JoyPos(InputKeyboard& input, unsigned int joy_id);

void L_Input_Mouse_Pos_Update(InputMouse& input, int x, int y);
void L_Input_Mouse_Wheel_Update(InputMouse& input, long delta);

void Init_InputKeyboard();
void Init_InputMouse();
#endif