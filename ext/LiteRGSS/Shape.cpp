#include <LiteCGSS/Common/NormalizeNumbers.h>
#include "LiteRGSS.h"
#include "rbAdapter.h"

#include "Shape.h"
#include "Texture_Bitmap.h"
#include "Shader.h"
#include "Drawable_Disposable.h"
#include "RenderStates_BlendMode.h"
#include "Color.h"
#include "Rect.h"
#include "Viewport.h"

VALUE rb_cShape = Qnil;
ID rb_iShapeCircle = Qnil;
ID rb_iShapeConvex = Qnil;
ID rb_iShapeRectangle = Qnil;

template<>
void rb::Mark<ShapeElement>(ShapeElement* shape) {
	if (shape == nullptr) {
		return;
	}
	rb_gc_mark(shape->rViewport);
	rb_gc_mark(shape->rBitmap);
	rb_gc_mark(shape->rX);
	rb_gc_mark(shape->rY);
	rb_gc_mark(shape->rOX);
	rb_gc_mark(shape->rOY);
	rb_gc_mark(shape->rAngle);
	rb_gc_mark(shape->rZoomX);
	rb_gc_mark(shape->rZoomY);
	rb_gc_mark(shape->rRect);
	rb_gc_mark(shape->rRenderStates);
	rb_gc_mark(shape->rShapeType);
	rb_gc_mark(shape->rColor);
	rb_gc_mark(shape->rOutlineColor);
	rb_gc_mark(shape->rOutlineThickness);
}

VALUE rb_Shape_Initialize(int argc, VALUE* argv, VALUE self) {
	VALUE viewport, type, rad_numPoint, numPoint;

	auto& shape = rb::Get<ShapeElement>(self);
	rb_scan_args(argc, argv, "22", &viewport, &type, &rad_numPoint, &numPoint);

	auto& viewportEl = rb::GetSafe<ViewportElement>(viewport, rb_cViewport);
	shape.rViewport = viewport;

	// Shape initialization
	ID itype = SYM2ID(type);

	cgss::ShapeType innerShapeType;
	std::unique_ptr<sf::Shape> innerShape;

	/* Circle Shape */
	if (itype == rb_iShapeCircle) {
		float radius = 1.0f;
		unsigned long numpt;
		if (NIL_P(numPoint)) {
			numpt = NUM2ULONG(rad_numPoint);
		} else {
			if (!NIL_P(rad_numPoint)) {
				radius = abs(NUM2DBL(rad_numPoint));
			}
			numpt = NUM2ULONG(numPoint);
		}
		innerShape = std::make_unique<sf::CircleShape>(radius, numpt);
		innerShapeType = cgss::ShapeType::Circle;
		shape.rShapeType = type;
	} else if (itype == rb_iShapeConvex) {
		/* Convex Shape */
		unsigned long numpt = 4;
		if (!NIL_P(rad_numPoint)) {
			numpt = NUM2ULONG(rad_numPoint);
		}
		innerShape = std::make_unique<sf::ConvexShape>(numpt);
		innerShapeType = cgss::ShapeType::Convex;
		shape.rShapeType = type;
	} else {
		/* Rectangle Shape */
		const auto hasDimensions = !NIL_P(rad_numPoint) && !NIL_P(numPoint);
		if (hasDimensions) {
			innerShape = std::make_unique<sf::RectangleShape>(sf::Vector2f(NUM2DBL(rad_numPoint), NUM2DBL(numPoint)));
		} else {
			innerShape = std::make_unique<sf::RectangleShape>();
		}
		innerShapeType = cgss::ShapeType::Rectangle;
		shape.rShapeType = ID2SYM(rb_iShapeRectangle);
	}

	auto data = cgss::ShapeData{ innerShapeType, std::move(innerShape) };
	viewportEl.initAndAdd(shape, std::move(data));

	return self;
}

VALUE rb_Shape_Dispose(VALUE self) {
	return rb::Dispose<ShapeElement>(self);
}

VALUE rb_Shape_Disposed(VALUE self) {
	return RDATA(self)->data == nullptr ? Qtrue : Qfalse;
}

VALUE rb_Shape_getBitmap(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rBitmap;
}

VALUE rb_Shape_setBitmap(VALUE self, VALUE bitmap) {
	auto& shape = rb::Get<ShapeElement>(self);

	if (bitmap == Qnil) {
		shape->setVisible(false);
		shape.rBitmap = bitmap;
		return self;
	}
	auto& bmp = rb::GetSafe<TextureElement>(bitmap, rb_cBitmap);

	shape->setTexture(bmp.instance(), true);
	shape->setVisible(true);
	shape.rBitmap = bitmap;
	return self;
}

VALUE rb_Shape_getRect(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return rb_Rect_LazyInitDrawable(shape.rRect, *shape.instance(), shape->getRectangle());
}

VALUE rb_Shape_setValue(VALUE self, VALUE rect) {
	VALUE rc = rb_Shape_getRect(self);

	auto* rect1 = rb::GetSafeOrNull<RectangleElement>(rect, rb_cRect);
	auto& rect2 = rb::Get<RectangleElement>(rc);

	auto rectangle = rect1 == nullptr || *rect1 == nullptr ? sf::IntRect{} : rect1->instance()->getValue();
	rect2->setValue(rectangle);
	return self;
}

VALUE rb_Shape_getX(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rX;
}

VALUE rb_Shape_setX(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->move(NUM2DBL(val), NUM2DBL(shape.rY));
	shape.rX = val;
	return self;
}

VALUE rb_Shape_getY(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rY;
}

VALUE rb_Shape_setY(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->move(NUM2DBL(shape.rX), NUM2DBL(val));
	shape.rY = val;
	return self;
}

VALUE rb_Shape_setPosition(VALUE self, VALUE x, VALUE y) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->move(NUM2DBL(x), NUM2DBL(y));
	shape.rX = x;
	shape.rY = y;
	return self;
}

VALUE rb_Shape_getZ(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return LONG2FIX(shape->getZ().z);
}

VALUE rb_Shape_setZ(VALUE self, VALUE val)
{
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setZ(NUM2LONG(val));
	return self;
}

VALUE rb_Shape_getOX(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rOX;
}

VALUE rb_Shape_setOX(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->moveOrigin(NUM2DBL(val), NUM2DBL(shape.rOY));
	shape.rOX = val;
	return self;
}

VALUE rb_Shape_getOY(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rOY;
}

VALUE rb_Shape_setOY(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->moveOrigin(NUM2DBL(shape.rOX), NUM2DBL(val));
	shape.rOY = val;
	return self;
}

VALUE rb_Shape_setOrigin(VALUE self, VALUE ox, VALUE oy) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->moveOrigin(NUM2DBL(ox), NUM2DBL(oy));
	shape.rOX = ox;
	shape.rOY = oy;
	return self;
}

VALUE rb_Shape_getAngle(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rAngle;
}

VALUE rb_Shape_setAngle(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	float angle = NUM2DBL(val);
	if (angle >= 360 || angle < 0) {
		angle = fmod(angle, 360);
		shape.rAngle = angle == (long)angle ? LONG2NUM((long)angle) : DBL2NUM(angle);
	} else {
		shape.rAngle = val;
	}
	shape->setAngle(angle);
	return self;
}

VALUE rb_Shape_getZoomX(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rZoomX;
}

VALUE rb_Shape_setZoomX(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->scale(NUM2DBL(val), NUM2DBL(shape.rZoomY));
	shape.rZoomX = val;
	return self;
}

VALUE rb_Shape_getZoomY(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rZoomY;
}

VALUE rb_Shape_setZoomY(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->scale(NUM2DBL(shape.rZoomX), NUM2DBL(val));
	shape.rZoomY = val;
	return self;
}

VALUE rb_Shape_setZoom(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	float zoom = NUM2DBL(val);
	shape->scale(zoom, zoom);
	shape.rZoomX = val;
	shape.rZoomY = val;
	return self;
}

VALUE rb_Shape_getIndex(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return LONG2NUM(shape->getZ().index);
}

VALUE rb_Shape_getViewport(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rViewport;
}

VALUE rb_Shape_getVisible(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape->isVisible() ? Qtrue : Qfalse;
}

VALUE rb_Shape_setVisible(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setVisible(RTEST(val));
	return self;
}

VALUE rb_Shape_getPointCount(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return LONG2NUM(shape->getPointCount());
}

VALUE rb_Shape_setPointCount(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setPointCount(NUM2LONG(val));
	return self;
}

VALUE rb_Shape_getRadius(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return LONG2NUM(shape->getRadius());
}

VALUE rb_Shape_setRadius(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setRadius(NUM2DBL(val));
	return self;
}

VALUE rb_Shape_getType(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rShapeType;
}

VALUE rb_Shape_getPoint(VALUE self, VALUE index) {
	auto& shape = rb::Get<ShapeElement>(self);
	long lindex = NUM2LONG(index);
	if (lindex < 0 || static_cast<std::size_t>(lindex) >= shape->getPointCount()) {
		return Qnil;
	}
	sf::Vector2f point = shape->getPoint(lindex);
	VALUE arr = rb_ary_new();
	rb_ary_push(arr, DBL2NUM(point.x));
	rb_ary_push(arr, DBL2NUM(point.y));
	return arr;
}

VALUE rb_Shape_setPoint(VALUE self, VALUE index, VALUE x, VALUE y) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setPoint(NUM2LONG(index), NUM2DBL(x), NUM2DBL(y));
	return self;
}

VALUE rb_Shape_getColor(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	if (shape.rColor == Qnil) {
		sf::Color col = shape->getFillColor();
		VALUE args[4] = {
			LONG2NUM(col.r),
			LONG2NUM(col.g),
			LONG2NUM(col.b),
			LONG2NUM(col.a)
		};
		shape.rColor = rb_class_new_instance(4, args, rb_cColor);
	}
	return shape.rColor;
}

VALUE rb_Shape_setColor(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	auto& color = rb::GetSafe<ColorElement>(val, rb_cColor);
	shape->setFillColor(color.getValue());
	shape.rColor = val;
	return self;
}

VALUE rb_Shape_getOutlineColor(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	if (shape.rOutlineColor == Qnil) {
		sf::Color col = shape->getFillColor();
		VALUE args[4] = {
			LONG2NUM(col.r),
			LONG2NUM(col.g),
			LONG2NUM(col.b),
			LONG2NUM(col.a)
		};
		shape.rOutlineColor = rb_class_new_instance(4, args, rb_cColor);
	}
	return shape.rOutlineColor;
}

VALUE rb_Shape_setOutlineColor(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	auto& color = rb::GetSafe<ColorElement>(val, rb_cColor);
	shape->setOutlineColor(color.getValue());
	shape.rOutlineColor = val;
	return self;
}

VALUE rb_Shape_getOutlineThickness(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rOutlineThickness;
}

VALUE rb_Shape_setOutlineThickness(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setOutlineThickness(NUM2DBL(val));
	shape.rOutlineThickness = val;
	return self;
}

VALUE rb_Shape_getWidth(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return LONG2NUM(shape->getWidth());
}

VALUE rb_Shape_setWidth(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setWidth(NUM2DBL(val));
	return self;
}

VALUE rb_Shape_getHeight(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return LONG2NUM(shape->getHeight());
}

VALUE rb_Shape_setHeight(VALUE self, VALUE val) {
	auto& shape = rb::Get<ShapeElement>(self);
	shape->setHeight(NUM2DBL(val));
	return self;
}

VALUE rb_Shape_getShader(VALUE self) {
	auto& shape = rb::Get<ShapeElement>(self);
	return shape.rRenderStates;
}

VALUE rb_Shape_setShader(VALUE self, VALUE shader) {
	auto& shape = rb::Get<ShapeElement>(self);
	// TODO : avoid copy.
	// Bind instead to existing element, as it's done for Rect.
	if (rb_obj_is_kind_of(shader, rb_cBlendMode) == Qtrue) {
		auto* renderState = rb::GetPtr<sf::RenderStates>(shader);
		if (renderState != nullptr) {
			shape.rRenderStates = shader;
			shape->setRenderState(*renderState);
			return self;
		}
	}
	shape.rRenderStates = Qnil;
	shape->setRenderState(sf::RenderStates{});
	return self;
}

VALUE rb_Shape_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "Shapes cannot be cloned or duplicated.");
	return self;
}

void Init_Shape() {
	rb_cShape = rb_define_class_under(rb_mLiteRGSS, "Shape", rb_cDrawable);
	rb_define_alloc_func(rb_cShape, rb::Alloc<ShapeElement>);

	rb_iShapeCircle = rb_intern("circle");
	rb_iShapeConvex = rb_intern("convex");
	rb_iShapeRectangle = rb_intern("rectangle");

	rb_define_method(rb_cShape, "initialize", _rbf rb_Shape_Initialize, -1);
	rb_define_method(rb_cShape, "bitmap", _rbf rb_Shape_getBitmap, 0);
	rb_define_method(rb_cShape, "bitmap=", _rbf rb_Shape_setBitmap, 1);
	rb_define_method(rb_cShape, "src_rect", _rbf rb_Shape_getRect, 0);
	rb_define_method(rb_cShape, "src_rect=", _rbf rb_Shape_setValue, 1);
	rb_define_method(rb_cShape, "x", _rbf rb_Shape_getX, 0);
	rb_define_method(rb_cShape, "x=", _rbf rb_Shape_setX, 1);
	rb_define_method(rb_cShape, "y", _rbf rb_Shape_getY, 0);
	rb_define_method(rb_cShape, "y=", _rbf rb_Shape_setY, 1);
	rb_define_method(rb_cShape, "set_position", _rbf rb_Shape_setPosition, 2);
	rb_define_method(rb_cShape, "z", _rbf rb_Shape_getZ, 0);
	rb_define_method(rb_cShape, "z=", _rbf rb_Shape_setZ, 1);
	rb_define_method(rb_cShape, "ox", _rbf rb_Shape_getOX, 0);
	rb_define_method(rb_cShape, "ox=", _rbf rb_Shape_setOX, 1);
	rb_define_method(rb_cShape, "oy", _rbf rb_Shape_getOY, 0);
	rb_define_method(rb_cShape, "oy=", _rbf rb_Shape_setOY, 1);
	rb_define_method(rb_cShape, "set_origin", _rbf rb_Shape_setOrigin, 2);
	rb_define_method(rb_cShape, "angle", _rbf rb_Shape_getAngle, 0);
	rb_define_method(rb_cShape, "angle=", _rbf rb_Shape_setAngle, 1);
	rb_define_method(rb_cShape, "zoom_x", _rbf rb_Shape_getZoomX, 0);
	rb_define_method(rb_cShape, "zoom_x=", _rbf rb_Shape_setZoomX, 1);
	rb_define_method(rb_cShape, "zoom_y", _rbf rb_Shape_getZoomY, 0);
	rb_define_method(rb_cShape, "zoom_y=", _rbf rb_Shape_setZoomY, 1);
	rb_define_method(rb_cShape, "zoom=", _rbf rb_Shape_setZoom, 1);
	rb_define_method(rb_cShape, "viewport", _rbf rb_Shape_getViewport, 0);
	rb_define_method(rb_cShape, "visible", _rbf rb_Shape_getVisible, 0);
	rb_define_method(rb_cShape, "visible=", _rbf rb_Shape_setVisible, 1);
	rb_define_method(rb_cShape, "point_count", _rbf rb_Shape_getPointCount, 0);
	rb_define_method(rb_cShape, "point_count=", _rbf rb_Shape_setPointCount, 1);
	rb_define_method(rb_cShape, "get_point", _rbf rb_Shape_getPoint, 1);
	rb_define_method(rb_cShape, "set_point", _rbf rb_Shape_setPoint, 3);
	rb_define_method(rb_cShape, "color", _rbf rb_Shape_getColor, 0);
	rb_define_method(rb_cShape, "color=", _rbf rb_Shape_setColor, 1);
	rb_define_method(rb_cShape, "outline_color", _rbf rb_Shape_getOutlineColor, 0);
	rb_define_method(rb_cShape, "outline_color=", _rbf rb_Shape_setOutlineColor, 1);
	rb_define_method(rb_cShape, "outline_thickness", _rbf rb_Shape_getOutlineThickness, 0);
	rb_define_method(rb_cShape, "outline_thickness=", _rbf rb_Shape_setOutlineThickness, 1);
	rb_define_method(rb_cShape, "__index__", _rbf rb_Shape_getIndex, 0);
	rb_define_method(rb_cShape, "dispose", _rbf rb_Shape_Dispose, 0);
	rb_define_method(rb_cShape, "disposed?", _rbf rb_Shape_Disposed, 0);
	rb_define_method(rb_cShape, "radius", _rbf rb_Shape_getRadius, 0);
	rb_define_method(rb_cShape, "radius=", _rbf rb_Shape_setRadius, 1);
	rb_define_method(rb_cShape, "type", _rbf rb_Shape_getType, 0);
	rb_define_method(rb_cShape, "width", _rbf rb_Shape_getWidth, 0);
	rb_define_method(rb_cShape, "width=", _rbf rb_Shape_setWidth, 1);
	rb_define_method(rb_cShape, "height", _rbf rb_Shape_getHeight, 0);
	rb_define_method(rb_cShape, "height=", _rbf rb_Shape_setHeight, 1);

	rb_define_method(rb_cShape, "shader", _rbf rb_Shape_getShader, 0);
	rb_define_method(rb_cShape, "shader=", _rbf rb_Shape_setShader, 1);
	rb_define_method(rb_cShape, "blendmode", _rbf rb_Shape_getShader, 0);
	rb_define_method(rb_cShape, "blendmode=", _rbf rb_Shape_setShader, 1);

	rb_define_method(rb_cShape, "clone", _rbf rb_Shape_Copy, 0);
	rb_define_method(rb_cShape, "dup", _rbf rb_Shape_Copy, 0);

	rb_define_const(rb_cShader, "CIRCLE", ID2SYM(rb_iShapeCircle));
	rb_define_const(rb_cShader, "CONVEX", ID2SYM(rb_iShapeConvex));
	rb_define_const(rb_cShader, "RECTANGLE", ID2SYM(rb_iShapeRectangle));
}
