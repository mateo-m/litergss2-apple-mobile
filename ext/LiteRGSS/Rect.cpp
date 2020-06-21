#include "LiteRGSS.h"
#include "rbAdapter.h"
#include "Rect.h"

VALUE rb_cRect = Qnil;

template<>
void rb::Mark<RectangleElement>(RectangleElement* rectangle) {
}

/*
 * Initialisation :
 * Rect.new(width, height)
 * Rect.new(x, width, height)
 * Rect.new(x, y, width, height)
 */
VALUE rb_Rect_initialize(int argc, VALUE* argv, VALUE self) {
	VALUE x, y, width, height;
	rb_scan_args(argc, argv, "22", &x, &y, &width, &height);
	auto& rect = rb::Get<RectangleElement>(self);
	rect.init();

	/* Parameter normalization */
	if (NIL_P(width)) {
		width = x;
		height = y;
		x = LONG2FIX(0);
		y = LONG2FIX(0);
	} else if (NIL_P(height)) {
		height = width;
		width = y;
		y = LONG2FIX(0);
	}

	/* Rect definition */
	sf::IntRect srect = rect->getValue();
	srect.left = rb_num2long(x);
	srect.top = rb_num2long(y);
	srect.width = rb_num2long(width);
	srect.height = rb_num2long(height);
	rect->setValue(std::move(srect));
	/* Pointed element nullification */
	rect->bind(nullptr);
	return self;
}

VALUE rb_Rect_initialize_copy(VALUE self, VALUE other) {
	auto& rect = rb::Get<RectangleElement>(self);
	rect.init();
	const auto* rect2 = rb::GetSafeOrNull<RectangleElement>(other, rb_cRect);
	if (rect2 == nullptr || *rect2 == nullptr) { return Qnil; }

	rect->setValue((*rect2)->getValue());
	rect->bind(nullptr);
	return self;
}

/*
 * rect.set(x)
 * rect.set(x,y)
 * rect.set(x,y,width)
 * rect.set(x,y,width,height)
 * rect.set(x, nil, width) etc...
 */
VALUE rb_Rect_set(int argc, VALUE* argv, VALUE self) {
	VALUE x, y, width, height;
	rb_scan_args(argc, argv, "13", &x, &y, &width, &height);
	auto& rect = rb::Get<RectangleElement>(self);
	auto srect = rect->getValue();

	if (!NIL_P(x)) {
		srect.left = rb_num2long(x);
	}

	if (!NIL_P(y)) {
		srect.top = rb_num2long(y);
	}

	if (!NIL_P(width)) {
		srect.width = rb_num2long(width);
	}

	if (!NIL_P(height)) {
		srect.height = rb_num2long(height);
	}

	rect->setValue(std::move(srect));
	return self;
}

VALUE rb_Rect_getX(VALUE self) {
	auto& rect = rb::Get<RectangleElement>(self);
	return rb_int2inum(rect->getValue().left);
}

VALUE rb_Rect_setX(VALUE self, VALUE val) {
	auto& rect = rb::Get<RectangleElement>(self);
	rect->setX(rb_num2long(val));
	return val;
}

VALUE rb_Rect_getY(VALUE self) {
	auto& rect = rb::Get<RectangleElement>(self);
	return rb_int2inum(rect->getValue().top);
}

VALUE rb_Rect_setY(VALUE self, VALUE val) {
	auto& rect = rb::Get<RectangleElement>(self);
	rect->setY(rb_num2long(val));
	return val;
}

VALUE rb_Rect_getWidth(VALUE self) {
	auto& rect = rb::Get<RectangleElement>(self);
	return rb_int2inum(rect->getValue().width);
}

VALUE rb_Rect_setWidth(VALUE self, VALUE val) {
	auto& rect = rb::Get<RectangleElement>(self);
	rect->setWidth(rb_num2long(val));
	return val;
}

VALUE rb_Rect_getHeight(VALUE self) {
	auto& rect = rb::Get<RectangleElement>(self);
	return rb_int2inum(rect->getValue().height);
}

VALUE rb_Rect_setHeight(VALUE self, VALUE val) {
	auto& rect = rb::Get<RectangleElement>(self);
	rect->setHeight(rb_num2long(val));
	return val;
}

VALUE rb_Rect_load(VALUE self, VALUE str) {
	rb_check_type(str, T_STRING);
	VALUE arr[4];
	if (RSTRING_LEN(str) < static_cast<long>(sizeof(int) * 4)) {
		arr[1] = arr[0] = LONG2FIX(1);
		return rb_class_new_instance(2, arr, self);
	}
	int* rc = reinterpret_cast<int*>(RSTRING_PTR(str));
	arr[0] = LONG2FIX(rc[0]);
	arr[1] = LONG2FIX(rc[1]);
	arr[2] = LONG2FIX(rc[2]);
	arr[3] = LONG2FIX(rc[3]);
	return rb_class_new_instance(4, arr, self);
}

VALUE rb_Rect_save(VALUE self, VALUE limit) {
	auto& rect = rb::Get<RectangleElement>(self);
	const sf::IntRect& srect = rect->getValue();
	int rc[4];
	rc[0] = srect.left;
	rc[1] = srect.top;
	rc[2] = srect.width;
	rc[3] = srect.height;
	return rb_str_new(reinterpret_cast<const char*>(rc), sizeof(int) * 4);
}

static VALUE rb_Rect_eql_rect(RectangleElement& rect, VALUE self) {
	auto& rect2 = rb::Get<RectangleElement>(self);
	return rect == rect2 ? Qtrue : Qfalse;
}

static VALUE rb_Rect_eql_array(RectangleElement& rect, VALUE oth) {
	const sf::IntRect& or1 = rect->getValue();
	if (RARRAY_LEN(oth) != 4) {
		return Qfalse;
	}

	VALUE* arr = RARRAY_PTR(oth);

	if (rb_num2long(arr[0]) != or1.left) {
		return Qfalse;
	}

	if (rb_num2long(arr[1]) != or1.top) {
		return Qfalse;
	}

	if (rb_num2long(arr[2]) != or1.width) {
		return Qfalse;
	}

	if (rb_num2long(arr[3]) != or1.height) {
		return Qfalse;
	}
	return Qtrue;
}

VALUE rb_Rect_eql(VALUE self, VALUE other) {
	auto& rect = rb::Get<RectangleElement>(self);
	if (rb_obj_is_kind_of(other, rb_cRect) == Qtrue) {
		return rb_Rect_eql_rect(rect, other);
	} else if(rb_obj_is_kind_of(other, rb_cArray) == Qtrue) {
		return rb_Rect_eql_array(rect, other);
	}
	return Qfalse;
}

VALUE rb_Rect_empty(VALUE self) {
	auto& rect = rb::Get<RectangleElement>(self);
	rect->setValue({0, 0, 0, 0});
	return self;
}

VALUE rb_Rect_to_s(VALUE self) {
	auto& rect = rb::Get<RectangleElement>(self);
	const sf::IntRect& srect = rect->getValue();
	return rb_sprintf("(%d, %d, %d, %d)", srect.left, srect.top, srect.width, srect.height);
}

void Init_Rect() {
	rb_cRect = rb_define_class_under(rb_mLiteRGSS, "Rect", rb_cObject);

	rb_define_alloc_func(rb_cRect, rb::Alloc<RectangleElement>);

	rb_define_method(rb_cRect, "initialize", _rbf rb_Rect_initialize, -1);
	rb_define_method(rb_cRect, "initialize_copy", _rbf rb_Rect_initialize_copy, 1);
	rb_define_method(rb_cRect, "set", _rbf rb_Rect_set, -1);
	rb_define_method(rb_cRect, "x", _rbf rb_Rect_getX, 0);
	rb_define_method(rb_cRect, "x=", _rbf rb_Rect_setX, 1);
	rb_define_method(rb_cRect, "y", _rbf rb_Rect_getY, 0);
	rb_define_method(rb_cRect, "y=", _rbf rb_Rect_setY, 1);
	rb_define_method(rb_cRect, "width", _rbf rb_Rect_getWidth, 0);
	rb_define_method(rb_cRect, "width=", _rbf rb_Rect_setWidth, 1);
	rb_define_method(rb_cRect, "height", _rbf rb_Rect_getHeight, 0);
	rb_define_method(rb_cRect, "height=", _rbf rb_Rect_setHeight, 1);
	rb_define_method(rb_cRect, "==", _rbf rb_Rect_eql, 1);
	rb_define_method(rb_cRect, "===", _rbf rb_Rect_eql, 1);
	rb_define_method(rb_cRect, "eql?", _rbf rb_Rect_eql, 1);
	rb_define_method(rb_cRect, "to_s", _rbf rb_Rect_to_s, 0);
	rb_define_method(rb_cRect, "inspect", _rbf rb_Rect_to_s, 0);
	rb_define_method(rb_cRect, "empty", _rbf rb_Rect_empty, 0);
	rb_define_method(rb_cRect, "_dump", _rbf rb_Rect_save, 1);
	rb_define_singleton_method(rb_cRect, "_load", _rbf rb_Rect_load, 1);
}
