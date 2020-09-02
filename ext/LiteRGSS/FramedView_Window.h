#ifndef Window_H
#define Window_H

#include <LiteCGSS/Views/FramedView.h>
#include "CgssWrapper.h"
#include "RubyValue.h"

extern VALUE rb_cWindow;
void Init_Window();

struct FramedViewElement : 
    public CgssInstance<cgss::FramedView> {

    VALUE rBitmap = Qnil;
    VALUE rX = LONG2FIX(0);
    VALUE rY = LONG2FIX(0);
    VALUE rOX = LONG2FIX(0);
    VALUE rOY = LONG2FIX(0);
    VALUE rRect = Qnil;
    VALUE rWidth = LONG2FIX(1);
    VALUE rHeight = LONG2FIX(1);
    VALUE rCursorRect = Qnil;
    VALUE rBackOpacity = LONG2FIX(255);
    VALUE rContentOpacity = LONG2FIX(255);
    VALUE rOpacity = LONG2FIX(255);
    VALUE rWindowBuilder = Qnil;
    VALUE rPauseSkin = Qnil;
    VALUE rCursorSkin = Qnil;
    VALUE rPause = Qfalse;
    VALUE rPauseX = LONG2FIX(0);
    VALUE rPauseY = LONG2FIX(0);
    VALUE rActive = Qfalse;
    VALUE rStretch = Qfalse;
    VALUE rViewport = Qnil;

    template <class Drawable, class ... Args>
    void initAndAdd(Drawable& drawable, Args&& ... args) {
        drawable.init(Drawable::create(*instance(), std::forward<Args>(args)...));
    }
};

#endif
