#ifndef Drawable_Disposable_H
#define Drawable_Disposable_H

#include "RubyValue.h"

VALUE rb_DrawableDisposable_disposed(VALUE self);

void Init_DrawableDisposable();

extern VALUE rb_cDisposable;
extern VALUE rb_cDrawable;

#endif