#include "LiteRGSS.h"
#include "GraphicsSingleton.h"
#include "NormalizeNumbers.h"
#include "rbAdapter.h"
#include "Tone.h"
#include "RenderStates_BlendMode.h"
#include "Color.h"

#include "Texture_Bitmap.h"
#include "Drawable_Disposable.h"
#include "Viewport.h"
#include "Rect.h"

VALUE rb_cViewport = Qnil;

void ViewportElement::updateFromValue(const sf::Glsl::Vec4* toneValue) {
	(*this)->setTone(toneValue);
}

void ViewportElement::updateFromValue(const sf::Color* colorValue) {
	(*this)->setColor(colorValue);
}

template<>
void rb::Mark<ViewportElement>(ViewportElement* viewport) {
	if (viewport == nullptr) {
		return;
	}
	rb_gc_mark(viewport->rRect);
	rb_gc_mark(viewport->rTone);
	rb_gc_mark(viewport->rColor);
	rb_gc_mark(viewport->rAngle);
	rb_gc_mark(viewport->rZoom);
	rb_gc_mark(viewport->rRenderState);
}

static VALUE rb_Viewport_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "Viewports can not be cloned or duplicated.");
	return self;
}

static VALUE rb_Viewport_Initialize(int argc, VALUE* argv, VALUE self) {
	/* Viewport setting */
	auto& viewport = rb::Get<ViewportElement>(self);
	viewport.init(GraphicsSingleton::Get().addView<cgss::Viewport>());

	/* Creating rect */
	VALUE rc = rb_class_new_instance(argc, argv, rb_cRect);
	auto& rect = rb::Get<RectangleElement>(rc);

	/* Linking Rect */
	viewport->bindRectangle(rect.instance());
	viewport.rRect = rc;
	return self;
}


static VALUE rb_Viewport_Dispose(VALUE self) {
	return rb::Dispose<ViewportElement>(self);
}

static VALUE rb_Viewport_Disposed(VALUE self) {
	return RDATA(self)->data == nullptr ? Qtrue : Qfalse;
}

static VALUE rb_Viewport_getOX(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return rb_int2inum(viewport->getOx());
}

static VALUE rb_Viewport_setOX(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	viewport->moveOrigin(rb_num2long(val), viewport->getOy());
	return val;
}

static VALUE rb_Viewport_getOY(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return rb_int2inum(viewport->getOy());
}

static VALUE rb_Viewport_setOY(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	viewport->moveOrigin(viewport->getOx(), rb_num2long(val));
	return val;
}

static VALUE rb_Viewport_getRect(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return viewport.rRect;
}

static VALUE rb_Viewport_setRect(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);

	if (!rb::CheckType<RectangleElement>(self, rb_cRect).empty()) {
		return Qnil;
	}

	auto* rect = rb::GetPtr<RectangleElement>(val);	
	if (rect == nullptr || *rect == nullptr) { 
		viewport->bindRectangle(nullptr);
		viewport.rRect = Qnil;
		return Qnil;
	}
	/* Setting rect parameter */
	(*rect)->setValue(viewport->getViewportBox());
	viewport->bindRectangle(rect->instance());
	viewport.rRect = val;
	return val;

}

static VALUE rb_Viewport_getTone(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	VALUE tn = viewport.rTone;
	if (!NIL_P(tn)) {
		return tn;
	}

	/* New tone + new color */
	VALUE argv[4] = {LONG2FIX(0), LONG2FIX(0), LONG2FIX(0), LONG2FIX(0)};
	
	viewport.rColor = rb_class_new_instance(4, argv, rb_cColor);
	auto& color = rb::GetSafe<ColorElement>(viewport.rColor, rb_cColor);
	color.bind(&viewport);

	tn = rb_class_new_instance(4, argv, rb_cTone);
	auto& tone = rb::GetSafe<ToneElement>(tn, rb_cTone);
	tone.bind(&viewport);
	viewport.rTone = tn;
	return tn;
}

static VALUE rb_Viewport_setTone(VALUE self, VALUE val) {
	VALUE tn = rb_Viewport_getTone(self);
	auto& tonesrc = rb::GetSafe<ToneElement>(val, rb_cTone);
	auto& tonedst = rb::Get<ToneElement>(tn);
	tonedst.setValue(tonesrc.getValue());
	return val;
}

static VALUE rb_Viewport_getColor(VALUE self) {
	rb_Viewport_getTone(self);
	auto& viewport = rb::Get<ViewportElement>(self);
	return viewport.rColor;
}

static VALUE rb_Viewport_setColor(VALUE self, VALUE val) {
	rb_Viewport_getTone(self);
	auto& viewport = rb::Get<ViewportElement>(self);
	auto& color = rb::GetSafe<ColorElement>(val, rb_cColor);
	color.bind(&viewport);
	viewport.rColor = val;
	return self;
}

static VALUE rb_Viewport_getVisible(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return viewport->isVisible() ? Qtrue : Qfalse;
}

static VALUE rb_Viewport_setVisible(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	viewport->setVisible(RTEST(val));
	return self;
}

static VALUE rb_Viewport_Update(VALUE self) {
	// Deprecated
	return self;
}

static VALUE rb_Viewport_getZ(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return LONG2FIX(viewport->getZ().z);
}

static VALUE rb_Viewport_setZ(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	const auto z = rb_num2long(val);
	viewport->setZ(z);
	return self;
}

static VALUE rb_Viewport_getAngle(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return viewport.rAngle;
}

static VALUE rb_Viewport_setAngle(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	const auto angle = NUM2LONG(val) % 360;
	viewport.rAngle = LONG2NUM(angle);
	viewport->setAngle(angle);
	return self;
}

static VALUE rb_Viewport_getZoom(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return viewport.rZoom;
}

static VALUE rb_Viewport_setZoom(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	const auto zoom = 1.0 / normalize_double(NUM2DBL(val), 0.001, 1000.0);
	viewport.rZoom = DBL2NUM(zoom);
	viewport->setZoom(NUM2DBL(val));
	return self;
}

static VALUE rb_Viewport_getRenderState(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return viewport.rRenderState;
}

static VALUE rb_Viewport_setRenderState(VALUE self, VALUE val) {
	auto& viewport = rb::Get<ViewportElement>(self);
	rb_Viewport_getColor(self);
	if (rb_obj_is_kind_of(val, rb_cBlendMode) == Qtrue) {
		auto* renderStates = rb::GetPtr<RenderStatesElement>(val);
		if (renderStates) {
			viewport->bindRenderStates(renderStates);
			viewport.rRenderState = val;
			return self;
		}
	}
	viewport->bindRenderStates(nullptr);
	viewport.rRenderState = Qfalse; // False to prevent intempestive delete
	return self;
}

static VALUE rb_Viewport_ReloadStack(VALUE self) {
	// Deprecated
	return self;
}

static VALUE rb_Viewport_Index(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return rb_uint2inum(viewport->getZ().index);
}

static VALUE rb_Viewport_snapToBitmap(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	return TextureElement::snapToTexture(*viewport.instance());
}

static VALUE rb_Viewport_sort_z(VALUE self) {
	auto& viewport = rb::Get<ViewportElement>(self);
	viewport->sortZ();
	return self;
}

void Init_Viewport() {
	rb_cViewport = rb_define_class_under(rb_mLiteRGSS, "Viewport", rb_cDrawable);

	rb_define_alloc_func(rb_cViewport, rb::Alloc<ViewportElement>);

	rb_define_method(rb_cViewport, "initialize", _rbf rb_Viewport_Initialize, -1);
	rb_define_method(rb_cViewport, "ox", _rbf rb_Viewport_getOX, 0);
	rb_define_method(rb_cViewport, "ox=", _rbf rb_Viewport_setOX, 1);
	rb_define_method(rb_cViewport, "oy", _rbf rb_Viewport_getOY, 0);
	rb_define_method(rb_cViewport, "oy=", _rbf rb_Viewport_setOY, 1);
	rb_define_method(rb_cViewport, "rect", _rbf rb_Viewport_getRect, 0);
	rb_define_method(rb_cViewport, "rect=", _rbf rb_Viewport_setRect, 1);
	rb_define_method(rb_cViewport, "sort_z", _rbf rb_Viewport_sort_z, 0);
	rb_define_method(rb_cViewport, "dispose", _rbf rb_Viewport_Dispose, 0);
	rb_define_method(rb_cViewport, "disposed?", _rbf rb_Viewport_Disposed, 0);
	rb_define_method(rb_cViewport, "tone", _rbf rb_Viewport_getTone, 0);
	rb_define_method(rb_cViewport, "tone=", _rbf rb_Viewport_setTone, 1);
	rb_define_method(rb_cViewport, "color", _rbf rb_Viewport_getColor, 0);
	rb_define_method(rb_cViewport, "color=", _rbf rb_Viewport_setColor, 1);
	rb_define_method(rb_cViewport, "update", _rbf rb_Viewport_Update, 0);
	rb_define_method(rb_cViewport, "visible", _rbf rb_Viewport_getVisible, 0);
	rb_define_method(rb_cViewport, "visible=", _rbf rb_Viewport_setVisible, 1);
	rb_define_method(rb_cViewport, "z", _rbf rb_Viewport_getZ, 0);
	rb_define_method(rb_cViewport, "z=", _rbf rb_Viewport_setZ, 1);
	rb_define_method(rb_cViewport, "zoom", _rbf rb_Viewport_getZoom, 0);
	rb_define_method(rb_cViewport, "zoom=", _rbf rb_Viewport_setZoom, 1);
	rb_define_method(rb_cViewport, "angle", _rbf rb_Viewport_getAngle, 0);
	rb_define_method(rb_cViewport, "angle=", _rbf rb_Viewport_setAngle, 1);
	rb_define_method(rb_cViewport, "shader", _rbf rb_Viewport_getRenderState, 0);
	rb_define_method(rb_cViewport, "shader=", _rbf rb_Viewport_setRenderState, 1);
	rb_define_method(rb_cViewport, "blendmode", _rbf rb_Viewport_getRenderState, 0);
	rb_define_method(rb_cViewport, "blendmode=", _rbf rb_Viewport_setRenderState, 1);
	rb_define_method(rb_cViewport, "reload_stack", _rbf rb_Viewport_ReloadStack, 0);
	rb_define_method(rb_cViewport, "__index__", _rbf rb_Viewport_Index, 0);
	rb_define_method(rb_cViewport, "snap_to_bitmap", _rbf rb_Viewport_snapToBitmap, 0);

	rb_define_method(rb_cViewport, "clone", _rbf rb_Viewport_Copy, 0);
	rb_define_method(rb_cViewport, "dup", _rbf rb_Viewport_Copy, 0);
}
