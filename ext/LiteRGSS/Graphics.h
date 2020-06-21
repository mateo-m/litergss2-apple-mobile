#ifndef Graphics_H
#define Graphics_H

#include "RubyValue.h"

void Init_Graphics();

extern VALUE rb_mGraphics;
extern VALUE rb_eStoppedGraphics;
extern VALUE rb_eClosedWindow;

#endif