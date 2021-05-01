#include <cstddef>
#include "Yuki.h"
#include "Yuki_Gif.h"

#include "Texture_Bitmap.h"

VALUE rb_cYukiGifReader = Qnil;
VALUE rb_cYukiGifError = Qnil;

VALUE rb_Yuki_GifReader_Initialize(int argc, VALUE *argv, VALUE self) {
	auto& gif = rb::Get<YukiGifElement>(self);
	gif.init();

	VALUE str, from_memory;
	rb_scan_args(argc, argv, "11", &str, &from_memory);
	rb_check_type(str, T_STRING);
	if (RTEST(from_memory)) {
		rb_str_freeze(str);
		rb_iv_set(self, "@__gif_data", str);
		if (!gif->load(RSTRING_PTR(str), RSTRING_LEN(str))) {
			rb_raise(rb_cYukiGifError, "Failed to load GIF from Memory");
		}
	} else {
		VALUE file = rb_file_open(RSTRING_PTR(str), "rb");
		VALUE data = rb_funcall(file, rb_intern("size"), 0);
		data = rb_funcall(file, rb_intern("read"), 1, data);
		rb_io_close(file);
		rb_str_freeze(data);
		rb_iv_set(self, "@__gif_data", data);
		if (!gif->load(RSTRING_PTR(data), RSTRING_LEN(data))) {
			rb_raise(rb_cYukiGifError, "Failed to load GIF from File (%s)", RSTRING_PTR(str));
		}
	}
	return self;
}

VALUE rb_Yuki_GifReader_Draw(VALUE self, VALUE texture) {
	auto& gif = rb::Get<YukiGifElement>(self);
	auto* textureElement = rb::GetSafeOrNull<TextureElement>(texture, rb_cBitmap);
	if (textureElement != nullptr) {
		gif->drawOn((*textureElement)->raw());
	}
	return self;
}

VALUE rb_Yuki_GifReader_Update(VALUE self, VALUE texture) {
	auto* textureElement = rb::GetSafeOrNull<TextureElement>(texture, rb_cBitmap);
	if (textureElement != nullptr) {
		auto& gif = rb::Get<YukiGifElement>(self);
		if (!gif->update((*textureElement)->raw())) {
			rb_raise(rb_cYukiGifError, "Failed to decode GIF frame");
		}
	}
	return self;
}

VALUE rb_Yuki_GifReader_Width(VALUE self) {
	const auto& gif = rb::Get<YukiGifElement>(self);
	return UINT2NUM(gif->width());
}

VALUE rb_Yuki_GifReader_Height(VALUE self) {
	const auto& gif = rb::Get<YukiGifElement>(self);
	return UINT2NUM(gif->height());
}

VALUE rb_Yuki_GifReader_Frame(VALUE self) {
	const auto& gif = rb::Get<YukiGifElement>(self);
	return ULONG2NUM(gif->frame());
}

VALUE rb_Yuki_GifReader_Frame_set(VALUE self, VALUE frame) {
	auto& gif = rb::Get<YukiGifElement>(self);
	gif->setFrame(NUM2ULONG(frame));
	return self;
}

VALUE rb_Yuki_GifReader_FrameCount(VALUE self) {
	const auto& gif = rb::Get<YukiGifElement>(self);
	return UINT2NUM(gif->frameCount());
}

VALUE rb_Yuki_GifReader_SetDeltaCounter(VALUE self, VALUE delta) {
	cgss::YukiGif::FrameDelta() = NUM2DBL(delta);
	return delta;
}

VALUE rb_Yuki_GifReader_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "Gif cannot be cloned or duplicated.");
	return self;
}

void Init_YukiGifReader() {
	rb_cYukiGifReader = rb_define_class_under(rb_mYuki, "GifReader", rb_cObject);
	rb_define_alloc_func(rb_cYukiGifReader, rb::Alloc<YukiGifElement>);
	rb_cYukiGifError = rb_define_class_under(rb_cYukiGifReader, "Error", rb_eStandardError);
	rb_define_method(rb_cYukiGifReader, "initialize", _rbf rb_Yuki_GifReader_Initialize, -1);
	rb_define_method(rb_cYukiGifReader, "update", _rbf rb_Yuki_GifReader_Update, 1);
	rb_define_method(rb_cYukiGifReader, "draw", _rbf rb_Yuki_GifReader_Draw, 1);
	rb_define_method(rb_cYukiGifReader, "width", _rbf rb_Yuki_GifReader_Width, 0);
	rb_define_method(rb_cYukiGifReader, "height", _rbf rb_Yuki_GifReader_Height, 0);
	rb_define_method(rb_cYukiGifReader, "frame", _rbf rb_Yuki_GifReader_Frame, 0);
	rb_define_method(rb_cYukiGifReader, "frame=", _rbf rb_Yuki_GifReader_Frame_set, 1);
	rb_define_method(rb_cYukiGifReader, "frame_count", _rbf rb_Yuki_GifReader_FrameCount, 0);

	rb_define_method(rb_cYukiGifReader, "clone", _rbf rb_Yuki_GifReader_Copy, 0);
	rb_define_method(rb_cYukiGifReader, "dup", _rbf rb_Yuki_GifReader_Copy, 0);

	rb_define_singleton_method(rb_cYukiGifReader, "delta_counter=", _rbf rb_Yuki_GifReader_SetDeltaCounter, 1);
}
