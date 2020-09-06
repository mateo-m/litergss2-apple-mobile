#ifndef Input_H
#define Input_H
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include "RubyValue.h"
#include "KeyboardInputMapping.h"
#include "MouseInputMapping.h"
#include "InputState.h"

extern VALUE rb_mInput;
extern VALUE rb_mMouse;

void Init_Input();

void L_Input_Update_Joy(unsigned int joy_id, unsigned int key, bool state);
void L_Input_Update_JoyPos(unsigned int joy_id, long axis, float position);
void L_Input_Reset_JoyPos(unsigned int joy_id);
void L_Input_Mouse_Pos_Update(int x, int y);
void L_Input_Mouse_Wheel_Update(long delta);

struct Input {
	long mouseWheelDelta = 0;
	double mousePosX = 0;
	double mousePosY = 0;
	std::string enteredText = {};
	bool invertAxisY = false;
	bool invertAxisX = false;
	unsigned int mainJoy = 0;
	unsigned int mainAxisX = sf::Joystick::Axis::X;
	unsigned int mainAxisY = sf::Joystick::Axis::Y;

	InputState<KeyboardInputMapping> keyMapping {};
	InputState<MouseInputMapping> mouseMapping {};
};

extern Input MainInput;
#endif