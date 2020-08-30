#ifndef DisplayWindow_H
#define DisplayWindow_H

#include "DisplayWindowInput.h"
#include "CgssWrapper.h"
#include "RubyValue.h"

extern VALUE rb_cDisplayWindow;
void Init_DisplayWindow();

struct DisplayWindowElement : 
    public CgssInstance<DisplayWindowInput> {
    
    VALUE rShader = Qnil;
};

#endif