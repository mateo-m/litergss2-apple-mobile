#ifndef Sprite_H
#define Sprite_H

#include <LiteCGSS/Graphics/Sprite.h>
#include "CgssWrapper.h"
#include "RubyValue.h"

extern VALUE rb_cSprite;
void Init_Sprite();

struct SpriteElement : 
    public CgssInstance<cgss::Sprite> {

	VALUE rBitmap = Qnil;
	VALUE rViewport = Qnil;
	VALUE rX = LONG2FIX(0);
	VALUE rY = LONG2FIX(0);
	VALUE rOX = LONG2FIX(0);
	VALUE rOY = LONG2FIX(0);
	VALUE rAngle = LONG2FIX(0);
	VALUE rZoomX = LONG2FIX(1);
	VALUE rZoomY = LONG2FIX(1);
	VALUE rRect = Qnil;
	VALUE rMirror = Qfalse;
};

#endif