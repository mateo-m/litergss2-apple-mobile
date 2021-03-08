#include <LiteCGSS/Graphics/Serializers/TextureSerializer.h>
#include <LiteCGSS/Common/NormalizeNumbers.h>
#include "LiteRGSS.h"
#include "rbAdapter.h"

#include "Color.h"
#include "Drawable_Disposable.h"
#include "Texture_Bitmap.h"
#include "Rect.h"

VALUE rb_cBitmap = Qnil;

VALUE TextureElement::snapToTexture(const cgss::SnapshotCapturable& toCapture) {
	auto outputTexture = toCapture.takeSnapshot();
	//Allocates memory ruby-side to take a snapshot
	VALUE bmp = rb_obj_alloc(rb_cBitmap);
	auto& texture = rb::Get<TextureElement>(bmp);
	texture.init(cgss::Texture::create(std::move(*outputTexture)));
	return bmp;
}

template<>
void rb::Mark<TextureElement>(TextureElement* texture) {
}

static VALUE rb_Bitmap_Initialize(int argc, VALUE *argv, VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	bitmap.init();

	VALUE string = Qnil;
	VALUE fromMemory = Qnil;
	rb_scan_args(argc, argv, "11", &string, &fromMemory);
	/* Load From filename */
	if (NIL_P(fromMemory)) {
		rb_check_type(string, T_STRING);
		const char* filename = RSTRING_PTR(string);
		auto loader = cgss::TextureFileSerializer{filename};
		if (!bitmap->load(loader)) {
			errno = ENOENT;
			rb_sys_fail(filename);
		}
	} else if (fromMemory == Qtrue) {
		rb_check_type(string, T_STRING);
		unsigned char* rawData = reinterpret_cast<unsigned char*>(RSTRING_PTR(string));
		const auto length = RSTRING_LEN(string);
		auto loader = cgss::TextureMemorySerializer{ { rawData, length } };
		if (!bitmap->load(loader)) {
			rb_raise(rb_eRGSSError, "Failed to load bitmap from memory.");
		}
	} else {
		rb_check_type(string, T_FIXNUM);
		rb_check_type(fromMemory, T_FIXNUM);
		const unsigned int width = static_cast<unsigned int>(rb_num2long(string));
		const unsigned int height = static_cast<unsigned int>(rb_num2long(fromMemory));
		auto loader = cgss::TextureEmptySerializer{width, height};
		if (!bitmap->load(loader)) {
			rb_raise(rb_eRGSSError, "Invalid texture size (%u x %u) !", width, height);
		}
	}
	return self;
}

static VALUE rb_Bitmap_Initialize_Copy(VALUE self, VALUE other) {
	rb_check_frozen(self);

	auto& destination = rb::Get<TextureElement>(self);
	destination.init();
	const auto* source = rb::GetSafeOrNull<TextureElement>(other, rb_cBitmap);
	if (source == nullptr) {
		rb_raise(rb_eTypeError, "Cannot clone %s into Bitmap.", RSTRING_PTR(rb_class_name(CLASS_OF(other))));
		return self;
	}

	if (destination.instance() == nullptr) {
		rb_raise(rb_eTypeError, "Cannot clone into an empty Bitmap.");
		return self;
	}

	*destination.instance() = (*source)->clone();
	return self;
}

static VALUE rb_Bitmap_Dispose(VALUE self) {
	return rb::RawDispose<TextureElement>(self);
}

static VALUE rb_Bitmap_Width(VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	const auto size = bitmap->getSize();
	return rb_int2inum(size.x);
}

static VALUE rb_Bitmap_Height(VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	const auto size = bitmap->getSize();
	return rb_int2inum(size.y);
}

static VALUE rb_Bitmap_Rect(VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	const auto size = bitmap->getSize();
	VALUE argv[4] = {LONG2FIX(0), LONG2FIX(0), rb_int2inum(size.x), rb_int2inum(size.y)};
	return rb_class_new_instance(4, argv, rb_cRect);
}

static VALUE rb_Bitmap_Update(VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	bitmap->update();
	return self;
}

static VALUE rb_Bitmap_blt(VALUE self, VALUE x, VALUE y, VALUE src_bitmap, VALUE rect) {
	auto& bitmap = rb::Get<TextureElement>(self);

	const auto* sourceRectangleElement = rb::GetSafeOrNull<RectangleElement>(rect, rb_cRect);
	if (sourceRectangleElement == nullptr) {
		rb_raise(rb_eRGSSError, "Textures require having a valid Rect as 4th parameter of blt method.");
		return self;
	}

	const auto* sourceTextureElement = rb::GetSafeOrNull<TextureElement>(src_bitmap, rb_cBitmap);
	if (sourceTextureElement == nullptr)  {
		rb_raise(rb_eRGSSError, "Textures require having a valid Texture as 3rd parameter of blt method.");
		return self;
	}

	auto rectangle = cgss::Rectangle{};
	rectangle.setValue((*sourceRectangleElement)->getValue());
	bitmap->blit(
		NUM2ULONG(x),
		NUM2ULONG(y),
		*sourceTextureElement->instance(),
		rectangle
	);
	return self;
}

static VALUE rb_Bitmap_clear_rect(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
	rb_check_type(x, T_FIXNUM);
	rb_check_type(y, T_FIXNUM);
	rb_check_type(width, T_FIXNUM);
	rb_check_type(height, T_FIXNUM);

	auto& bitmap = rb::Get<TextureElement>(self);

	long xValue = NUM2LONG(x);
	xValue = xValue < 0 ? 0 : xValue;

	long yValue = NUM2LONG(y);
	yValue = yValue < 0 ? 0 : yValue;

	bitmap->clearRect(xValue, yValue, static_cast<unsigned int>(NUM2LONG(width)), static_cast<unsigned int>(NUM2LONG(height)));
	return self;
}

static VALUE rb_Bitmap_fill_rect(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height, VALUE color) {
	rb_check_type(x, T_FIXNUM);
	rb_check_type(y, T_FIXNUM);
	rb_check_type(width, T_FIXNUM);
	rb_check_type(height, T_FIXNUM);

	auto& bitmap = rb::Get<TextureElement>(self);
	const auto* rcolor = rb::GetSafeOrNull<ColorElement>(color, rb_cColor);
	if (rcolor == nullptr) {
		rb_raise(rb_eRGSSError, "Textures require having a valid Color as 5th parameter of blt method.");
		return self;
	}

	long xValue = NUM2LONG(x);
	xValue = xValue < 0 ? 0 : xValue;

	long yValue = NUM2LONG(y);
	yValue = yValue < 0 ? 0 : yValue;

	bitmap->fillRect(xValue, yValue, static_cast<unsigned int>(NUM2LONG(width)), static_cast<unsigned int>(NUM2LONG(height)), rcolor->getValue());
	return self;
}

static VALUE rb_Bitmap_toPNG(VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	auto saver = cgss::TextureMemorySerializer { {nullptr, 0u} };
	bitmap->write(saver);
	VALUE out;
	saver.finalizeMemory([&out](const cgss::MemorySerializerData& rawData) {
		out = rb_str_new(reinterpret_cast<const char*>(rawData.first), rawData.second);
	});
	return out;
}

static VALUE rb_Bitmap_toPNG_file(VALUE self, VALUE filename) {
	rb_check_type(filename, T_STRING);
	std::string filenameValue = RSTRING_PTR(filename);
	auto& bitmap = rb::Get<TextureElement>(self);
	auto saver = cgss::TextureFileSerializer { std::move(filenameValue) };
	return bitmap->write(saver) == 0u ? Qtrue : Qfalse;
}

void Init_Bitmap() {
	rb_cBitmap = rb_define_class_under(rb_mLiteRGSS, "Bitmap", rb_cDisposable);
	rb_define_alloc_func(rb_cBitmap, rb::Alloc<TextureElement>);
	rb_define_method(rb_cBitmap, "initialize", _rbf rb_Bitmap_Initialize, -1);
	rb_define_method(rb_cBitmap, "initialize_copy", _rbf rb_Bitmap_Initialize_Copy, 1);
	rb_define_method(rb_cBitmap, "dispose", _rbf rb_Bitmap_Dispose, 0);
	rb_define_method(rb_cBitmap, "width", _rbf rb_Bitmap_Width, 0);
	rb_define_method(rb_cBitmap, "height", _rbf rb_Bitmap_Height, 0);
	rb_define_method(rb_cBitmap, "rect", _rbf rb_Bitmap_Rect, 0);
	rb_define_method(rb_cBitmap, "update", _rbf rb_Bitmap_Update, 0);
	rb_define_method(rb_cBitmap, "blt", _rbf rb_Bitmap_blt, 4);
	rb_define_method(rb_cBitmap, "clear_rect", _rbf rb_Bitmap_clear_rect, 4);
	rb_define_method(rb_cBitmap, "fill_rect", _rbf rb_Bitmap_fill_rect, 5);
	rb_define_method(rb_cBitmap, "to_png", _rbf rb_Bitmap_toPNG, 0);
	rb_define_method(rb_cBitmap, "to_png_file", _rbf rb_Bitmap_toPNG_file, 1);
}
