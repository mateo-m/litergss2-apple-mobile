#ifndef InputModule_H
#define InputModule_H
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include "RubyValue.h"
#include "KeyboardInputMapping.h"
#include "MouseInputMapping.h"
#include "InputState.h"
#include "Input.h"

extern VALUE rb_mInputModule;
extern VALUE rb_mMouseModule;

void Init_Input();

#endif