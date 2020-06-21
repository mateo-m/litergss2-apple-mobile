#ifndef Input_H
#define Input_H
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include "RubyValue.h"
extern VALUE rb_mInput;
extern VALUE rb_mMouse;

void Init_Input();

void L_Input_Reset_Clocks();
void L_Input_Update_Key(sf::Keyboard::Key code, bool state);
void L_Input_Update_Joy(unsigned int joy_id, unsigned int key, bool state);
void L_Input_Update_JoyPos(unsigned int joy_id, long axis, float position);
void L_Input_Reset_JoyPos(unsigned int joy_id);
void L_Input_Setusec_threshold(long usec);
void L_Input_Mouse_Pos_Update(int x, int y);
void L_Input_Mouse_Wheel_Update(long delta);
void L_Input_Mouse_Button_Update(long button, bool state);

extern std::string L_EnteredText;
#endif