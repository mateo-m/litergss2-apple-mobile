#ifndef Shape_H
#define Shape_H

#include <LiteCGSS/Graphics/Shape.h>
#include "CgssWrapper.h"
#include "RubyValue.h"
#include "rbAdapter.h"

extern VALUE rb_cShape;
void Init_Shape();

struct ShapeElement :
	public CgssInstance<cgss::Shape> {

	VALUE rViewport = Qnil;
	VALUE rBitmap = Qnil;
	VALUE rX = LONG2FIX(0);
	VALUE rY = LONG2FIX(0);
	VALUE rOX = LONG2FIX(0);
	VALUE rOY = LONG2FIX(0);
	VALUE rAngle = LONG2FIX(0);
	VALUE rZoomX = LONG2FIX(1);
	VALUE rZoomY = LONG2FIX(0);
	VALUE rRect = Qnil;
	VALUE rShapeType = Qnil;
	VALUE rRenderStates = Qnil;
	VALUE rColor = Qnil;
	VALUE rOutlineColor = Qnil;
	VALUE rOutlineThickness = LONG2FIX(0);
};

template<>
void rb::Mark<ShapeElement>(void* ptr);

#endif