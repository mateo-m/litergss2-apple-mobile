#include "LiteRGSS.h"
#include "Drawable_Disposable.h"

VALUE rb_cDisposable = Qnil;
VALUE rb_cDrawable = Qnil;

VALUE rb_DrawableDisposable_dispose(VALUE self);

void Init_DrawableDisposable() {
	rb_cDisposable = rb_define_class_under(rb_mLiteRGSS, "Disposable", rb_cObject);

	rb_define_method(rb_cDisposable, "dispose", _rbf rb_DrawableDisposable_dispose, 0);
	rb_define_method(rb_cDisposable, "disposed?", _rbf rb_DrawableDisposable_disposed, 0);

	rb_cDrawable = rb_define_class_under(rb_mLiteRGSS, "Drawable", rb_cDisposable);
}

VALUE rb_DrawableDisposable_dispose(VALUE self) {
	return self;
}

VALUE rb_DrawableDisposable_disposed(VALUE self) {
	if (!RB_TYPE_P(self, T_DATA) || !RTYPEDDATA_P(self)) {
        rb_raise(rb_eTypeError, "wrong argument type");
    }	
	return (RTYPEDDATA_DATA(self) == nullptr ? Qtrue : Qfalse);
}