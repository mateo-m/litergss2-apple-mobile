#include "LiteRGSS.h"
#include "FramedView_Window.h"
#include "Drawable_Disposable.h"
#include "Texture_Bitmap.h"
#include "rbAdapter.h"
#include "NormalizeNumbers.h"
#include "GraphicsSingleton.h"
#include "Rect.h"
#include "Viewport.h"

VALUE rb_cWindow = Qnil;

bool FramedViewElement::onViewportChange(cgss::ViewportChangeEvent& event) {
	auto& viewportRect = rb::Get<RectangleElement>(rRect);
	auto& viewportBox = event.viewportBox;
	viewportRect->setValue(viewportBox);
	return true;
}

template<>
void rb::Mark<FramedViewElement>(FramedViewElement* framedView) {
	if (framedView == nullptr) {
		return;
	}
	rb_gc_mark(framedView->rViewport);
	rb_gc_mark(framedView->rBitmap);
	rb_gc_mark(framedView->rX);
	rb_gc_mark(framedView->rY);
	rb_gc_mark(framedView->rOX);
	rb_gc_mark(framedView->rOY);
	rb_gc_mark(framedView->rRect);
	rb_gc_mark(framedView->rWidth);
	rb_gc_mark(framedView->rHeight);
	rb_gc_mark(framedView->rCursorRect);
	rb_gc_mark(framedView->rBackOpacity);
	rb_gc_mark(framedView->rContentOpacity);
	rb_gc_mark(framedView->rOpacity);
	rb_gc_mark(framedView->rWindowBuilder);
	rb_gc_mark(framedView->rPauseSkin);
	rb_gc_mark(framedView->rCursorSkin);
	rb_gc_mark(framedView->rPause);
	rb_gc_mark(framedView->rPauseX);
	rb_gc_mark(framedView->rPauseY);
	rb_gc_mark(framedView->rActive);
	rb_gc_mark(framedView->rStretch);
}

VALUE rb_Window_Initialize(int argc, VALUE* argv, VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	VALUE viewport = Qnil;
	
	rb_scan_args(argc, argv, "01", &viewport);

	const auto viewportIsSpecified = argc == 1 && rb_obj_is_kind_of(argv[0], rb_cViewport) == Qtrue;
	if (viewportIsSpecified) {
		auto& viewport = rb::Get<ViewportElement>(argv[0]);		
		if (viewport.instance() == nullptr) {
			rb_raise(rb_eRGSSError, "Invalid viewport provided to instanciate a Sprite.");
			return Qnil;
		}
		framedView.init(GraphicsSingleton::Get().addViewOn<cgss::FramedView>(*viewport.instance(), viewport->weak()));
		framedView.rViewport = argv[0];
	} else {
		framedView.init(GraphicsSingleton::Get().addView<cgss::FramedView>());
		framedView.rViewport = Qnil;
	}

	/* Rect definition */
	VALUE args[4] = { LONG2FIX(0), LONG2FIX(0), LONG2FIX(0), LONG2FIX(0) };
	framedView.rRect = rb_class_new_instance(4, args, rb_cRect);
	rb_obj_freeze(framedView.rRect);

	return self;
}

VALUE rb_Window_getViewport(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rViewport;
}

VALUE rb_Window_Dispose(VALUE self) {
	return rb::Dispose<FramedViewElement>(self);
}

VALUE rb_Window_Disposed(VALUE self) {
	return RDATA(self)->data == nullptr ? Qtrue : Qfalse;
}

VALUE rb_Window_setWindowSkin(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	if (val != Qnil) {
		auto& bmp = rb::GetSafe<TextureElement>(val, rb_cBitmap);
		framedView->setSkin(&bmp->getTexture());
		framedView.rBitmap = val;		
	} else {
		framedView->setSkin(nullptr);
		framedView.rBitmap = Qnil;
	}
	
	return self;
}

VALUE rb_Window_getWindowSkin(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rBitmap;
}

VALUE rb_Window_setWidth(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	auto width = NUM2ULONG(val);
	framedView->resize(width, framedView->getHeight());
	framedView.rWidth = val;
	return self;
}

VALUE rb_Window_getWidth(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rWidth;
}

VALUE rb_Window_setHeight(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	auto height = NUM2ULONG(val);
	framedView->resize(framedView->getWidth(), height);
	framedView.rHeight = val;
	return self;
}

VALUE rb_Window_getHeight(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rHeight;
}

VALUE rb_Window_setSize(VALUE self, VALUE x, VALUE y) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto width = NUM2ULONG(x);
	const auto height = NUM2ULONG(y);
	framedView->resize(width, height);
	framedView.rWidth = x;
	framedView.rHeight = y;
	return self;
}

VALUE rb_Window_setWindowBuilder(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	Check_Type(val, T_ARRAY);
	
	// Lenght check
	if (rb_array_len(val) < 6) {
		rb_raise(rb_eArgError, "framedView Builder should be 6 Integer long.");
	}

	if (rb_array_len(val) == 8) {
		std::array<long, 8> windowBuilder;
		for (long i = 0; i < 8; i++) {
			windowBuilder[i] = NUM2ULONG(rb_ary_entry(val, i));
		}
		framedView->setBuilder(std::move(windowBuilder));
	} else {
		std::array<long, 6> windowBuilder;
		for (long i = 0; i < 6; i++) {
			windowBuilder[i] = NUM2ULONG(rb_ary_entry(val, i));
		}
		framedView->setBuilder(std::move(windowBuilder));
	}

	// Freeze framedView builder
	rb_obj_freeze(val);

	framedView.rWindowBuilder = val;
	return self;
}

VALUE rb_Window_getWindowBuilder(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rWindowBuilder;
}

VALUE rb_Window_setX(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	auto x = NUM2LONG(val);
	framedView->move(x, framedView->getY());
	framedView.rX = val;
	return self;
}

VALUE rb_Window_getX(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rX;
}

VALUE rb_Window_setY(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	auto y = NUM2LONG(val);
	framedView->move(framedView->getX(), y);
	framedView.rY = val;
	return self;
}

VALUE rb_Window_getY(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rY;
}

VALUE rb_Window_setPosition(VALUE self, VALUE x, VALUE y) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->move(NUM2LONG(x), NUM2LONG(y));
	framedView.rX = x;
	framedView.rY = y;
	return self;
}

VALUE rb_Window_setZ(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->setZ(NUM2LONG(val));
	return self;
}

VALUE rb_Window_getZ(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return LONG2FIX(framedView->getZ().z);;
}

VALUE rb_Window_setOX(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->moveOrigin(NUM2LONG(val), framedView->getOy());
	framedView.rOX = val;
	return self;
}

VALUE rb_Window_getOX(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rOX;
}

VALUE rb_Window_setOY(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->moveOrigin(framedView->getOx(), NUM2LONG(val));
	framedView.rOY = val;
	return self;
}

VALUE rb_Window_getOY(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rOY;
}

VALUE rb_Window_setOrigin(VALUE self, VALUE x, VALUE y) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->moveOrigin(NUM2LONG(x), NUM2LONG(y));
	framedView.rOX = x;
	framedView.rOY = y;
	return self;
}

VALUE rb_Window_setStretch(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto stretch = RTEST(val);
	framedView.rStretch = stretch ? Qtrue : Qfalse;
	framedView->stretch(stretch);
	return self;
}

VALUE rb_Window_getStretch(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rStretch;
}

VALUE rb_Window_getCursorRect(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return rb_Rect_LazyInitDrawable(framedView.rCursorRect, *framedView.instance(), framedView->getCursorRectangle());
}

VALUE rb_Window_setCursorRect(VALUE self, VALUE val) {	
	auto& framedView = rb::Get<FramedViewElement>(self);
	rb_Window_getCursorRect(self);

	const auto& rectSource = rb::GetSafe<RectangleElement>(val, rb_cRect);
	if (rectSource.instance() == nullptr) { return Qnil; }
	framedView->setCursorRectangle(rectSource->getValue());

	return self;
}

VALUE rb_Window_getCursorSkin(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rCursorSkin;
}

VALUE rb_Window_setCursorSkin(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	if (NIL_P(val)) {
		framedView.rCursorSkin = Qnil;
	} else {
		auto& bmp = rb::GetSafe<TextureElement>(val, rb_cBitmap);
		framedView->setCursorSkin(bmp->getTexture());
		framedView.rCursorSkin = val;
	}
	return self;
}

VALUE rb_Window_getPauseSkin(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rPauseSkin;
}

VALUE rb_Window_setPauseSkin(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	if (NIL_P(val)) {
		framedView.rPauseSkin = Qnil;
	} else {
		auto& bmp = rb::GetSafe<TextureElement>(val, rb_cBitmap);
		framedView->setPauseSkin(bmp->getTexture());
		framedView.rPauseSkin = val;
	}
	return self;
}

VALUE rb_Window_getPause(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rPause;
}

VALUE rb_Window_setPause(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto pause = RTEST(val);
	framedView->pause(pause);
	framedView.rPause = pause ? Qtrue : Qfalse;
	return self;
}

VALUE rb_Window_getPauseX(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rPauseX;
}

VALUE rb_Window_setPauseX(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	if (NIL_P(val)) {
		framedView.rPauseX = Qnil;
	} else {
		framedView->setPausePosition(NUM2LONG(val), framedView->getPauseY());
		framedView.rPauseX = val;
	}
	return self;
}

VALUE rb_Window_getPauseY(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rPauseY;
}

VALUE rb_Window_setPauseY(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	if (NIL_P(val)) {
		framedView.rPauseY = Qnil;
	} else {
		framedView->setPausePosition(framedView->getPauseX(), NUM2LONG(val));
		framedView.rPauseY = val;
	}
	return self;
}

VALUE rb_Window_getActive(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rActive;
}

VALUE rb_Window_setActive(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto active = RTEST(val);
	framedView.rActive = active ? Qtrue : Qfalse;
	framedView->active(active);
	return self;
}

VALUE rb_Window_update(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->update();
	return self;
}

VALUE rb_Window_sort_z(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->sortZ();
	return self;
}

VALUE rb_Window_getOpacity(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rOpacity;
}

VALUE rb_Window_setOpacity(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto opacity = normalize_long(NUM2LONG(val), 0, 255);
	framedView->setOpacity(opacity);
	framedView.rOpacity = LONG2NUM(opacity);
	return self;
}

VALUE rb_Window_getBackOpacity(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rBackOpacity;
}

VALUE rb_Window_setBackOpacity(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto backOpacity = normalize_long(NUM2LONG(val), 0, 255);
	framedView->setBackOpacity(backOpacity);
	framedView.rBackOpacity = LONG2NUM(backOpacity);
	return self;
}

VALUE rb_Window_getContentsOpacity(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rContentOpacity;
}

VALUE rb_Window_setContentsOpacity(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	const auto contentOpacity = normalize_long(NUM2LONG(val), 0, 255);
	framedView->setContentsOpacity(contentOpacity);
	framedView.rContentOpacity = LONG2NUM(contentOpacity);
	return self;
}

VALUE rb_Window_getRect(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView.rRect;
}

VALUE rb_Window_getVisible(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView->isVisible() ? Qtrue : Qfalse;
}

VALUE rb_Window_setVisible(VALUE self, VALUE val) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->setVisible(RTEST(val));
	return self;
}

VALUE rb_Window_getIndex(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return ULONG2NUM(framedView->getZ().index);
}

VALUE rb_Window_lock(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->lock();
	return self;
}

VALUE rb_Window_unlock(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	framedView->unlock();
	return self;
}

VALUE rb_Window_locked(VALUE self) {
	auto& framedView = rb::Get<FramedViewElement>(self);
	return framedView->isLocked() ? Qtrue : Qfalse;
}

VALUE rb_Window_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "Windows cannot be cloned or duplicated.");
	return self;
}

void Init_Window() {
	rb_cWindow = rb_define_class_under(rb_mLiteRGSS, "Window", rb_cDrawable);
	rb_define_alloc_func(rb_cWindow, rb::Alloc<FramedViewElement>);

	rb_define_method(rb_cWindow, "initialize", _rbf rb_Window_Initialize, -1);
	rb_define_method(rb_cWindow, "dispose", _rbf rb_Window_Dispose, 0);
	rb_define_method(rb_cWindow, "disposed?", _rbf rb_Window_Disposed, 0);
	rb_define_method(rb_cWindow, "update", _rbf rb_Window_update, 0);
	rb_define_method(rb_cWindow, "sort_z", _rbf rb_Window_sort_z, 0);
	rb_define_method(rb_cWindow, "windowskin=", _rbf rb_Window_setWindowSkin, 1);
	rb_define_method(rb_cWindow, "windowskin", _rbf rb_Window_getWindowSkin, 0);
	rb_define_method(rb_cWindow, "width=", _rbf rb_Window_setWidth, 1);
	rb_define_method(rb_cWindow, "width", _rbf rb_Window_getWidth, 0);
	rb_define_method(rb_cWindow, "height=", _rbf rb_Window_setHeight, 1);
	rb_define_method(rb_cWindow, "height", _rbf rb_Window_getHeight, 0);
	rb_define_method(rb_cWindow, "set_size", _rbf rb_Window_setSize, 2);
	rb_define_method(rb_cWindow, "window_builder=", _rbf rb_Window_setWindowBuilder, 1);
	rb_define_method(rb_cWindow, "window_builder", _rbf rb_Window_getWindowBuilder, 0);
	rb_define_method(rb_cWindow, "x=", _rbf rb_Window_setX, 1);
	rb_define_method(rb_cWindow, "x", _rbf rb_Window_getX, 0);
	rb_define_method(rb_cWindow, "y=", _rbf rb_Window_setY, 1);
	rb_define_method(rb_cWindow, "y", _rbf rb_Window_getY, 0);
	rb_define_method(rb_cWindow, "set_position", _rbf rb_Window_setPosition, 2);
	rb_define_method(rb_cWindow, "z=", _rbf rb_Window_setZ, 1);
	rb_define_method(rb_cWindow, "z", _rbf rb_Window_getZ, 0);
	rb_define_method(rb_cWindow, "ox=", _rbf rb_Window_setOX, 1);
	rb_define_method(rb_cWindow, "ox", _rbf rb_Window_getOX, 0);
	rb_define_method(rb_cWindow, "oy=", _rbf rb_Window_setOY, 1);
	rb_define_method(rb_cWindow, "oy", _rbf rb_Window_getOY, 0);
	rb_define_method(rb_cWindow, "set_origin", _rbf rb_Window_setOrigin, 2);
	rb_define_method(rb_cWindow, "cursor_rect", _rbf rb_Window_getCursorRect, 0);
	rb_define_method(rb_cWindow, "cursor_rect=", _rbf rb_Window_setCursorRect, 1);
	rb_define_method(rb_cWindow, "cursorskin", _rbf rb_Window_getCursorSkin, 0);
	rb_define_method(rb_cWindow, "cursorskin=", _rbf rb_Window_setCursorSkin, 1);
	rb_define_method(rb_cWindow, "pauseskin", _rbf rb_Window_getPauseSkin, 0);
	rb_define_method(rb_cWindow, "pauseskin=", _rbf rb_Window_setPauseSkin, 1);
	rb_define_method(rb_cWindow, "pause", _rbf rb_Window_getPause, 0);
	rb_define_method(rb_cWindow, "pause=", _rbf rb_Window_setPause, 1);
	rb_define_method(rb_cWindow, "pause_x", _rbf rb_Window_getPauseX, 0);
	rb_define_method(rb_cWindow, "pause_x=", _rbf rb_Window_setPauseX, 1);
	rb_define_method(rb_cWindow, "pause_y", _rbf rb_Window_getPauseY, 0);
	rb_define_method(rb_cWindow, "pause_y=", _rbf rb_Window_setPauseY, 1);
	rb_define_method(rb_cWindow, "active", _rbf rb_Window_getActive, 0);
	rb_define_method(rb_cWindow, "active=", _rbf rb_Window_setActive, 1);
	rb_define_method(rb_cWindow, "stretch", _rbf rb_Window_getStretch, 0);
	rb_define_method(rb_cWindow, "stretch=", _rbf rb_Window_setStretch, 1);
	rb_define_method(rb_cWindow, "opacity", _rbf rb_Window_getOpacity, 0);
	rb_define_method(rb_cWindow, "opacity=", _rbf rb_Window_setOpacity, 1);
	rb_define_method(rb_cWindow, "back_opacity", _rbf rb_Window_getBackOpacity, 0);
	rb_define_method(rb_cWindow, "back_opacity=", _rbf rb_Window_setBackOpacity, 1);
	rb_define_method(rb_cWindow, "contents_opacity", _rbf rb_Window_getContentsOpacity, 0);
	rb_define_method(rb_cWindow, "contents_opacity=", _rbf rb_Window_setContentsOpacity, 1);
	rb_define_method(rb_cWindow, "rect", _rbf rb_Window_getRect, 0);
	rb_define_method(rb_cWindow, "viewport", _rbf rb_Window_getViewport, 0);
	rb_define_method(rb_cWindow, "visible", _rbf rb_Window_getVisible, 0);
	rb_define_method(rb_cWindow, "visible=", _rbf rb_Window_setVisible, 1);
	rb_define_method(rb_cWindow, "__index__", _rbf rb_Window_getIndex, 0);
	rb_define_method(rb_cWindow, "lock", _rbf rb_Window_lock, 0);
	rb_define_method(rb_cWindow, "unlock", _rbf rb_Window_unlock, 0);
	rb_define_method(rb_cWindow, "locked?", _rbf rb_Window_locked, 0);

	rb_define_method(rb_cWindow, "clone", _rbf rb_Window_Copy, 0);
	rb_define_method(rb_cWindow, "dup", _rbf rb_Window_Copy, 0);
}
