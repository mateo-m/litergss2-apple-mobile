#include <LiteCGSS/Graphics/Loaders/TextureLoader.h>
#include "LiteRGSS.h"
#include "NormalizeNumbers.h"
#include "log.h"
#include "rbAdapter.h"

#include "Color.h"
#include "Drawable_Disposable.h"
#include "Texture_Bitmap.h"
#include "GraphicsSingleton.h"
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
	bitmap->setSmooth(GraphicsSingleton::Get().smoothScreen());

	VALUE string = Qnil;
	VALUE fromMemory = Qnil;
	rb_scan_args(argc, argv, "11", &string, &fromMemory);
	/* Load From filename */
	if(NIL_P(fromMemory)) {
		rb_check_type(string, T_STRING);
		//LOG("[Bitmap] Init from filename");
		const char* filename = RSTRING_PTR(string);
		auto loader = cgss::TextureFileLoader{filename};
		if (!bitmap->load(loader)) {
			errno = ENOENT;
			rb_sys_fail(filename);
		}
	} else if(fromMemory == Qtrue) {
		rb_check_type(string, T_STRING);
		//LOG("[Bitmap] Init from memory");
		const char* rawData = RSTRING_PTR(string);
		const auto length = RSTRING_LEN(string);
		auto loader = cgss::TextureMemoryLoader{std::vector<unsigned char>{ rawData, rawData + length }};
		if (!bitmap->load(loader)) {
			rb_raise(rb_eRGSSError, "Failed to load bitmap from memory.");
		}
	} else {
		rb_check_type(string, T_FIXNUM);
		rb_check_type(fromMemory, T_FIXNUM);
		//LOG("[Bitmap] Init empty with dimensions");
		const unsigned int width = static_cast<unsigned int>(rb_num2long(string));
		const unsigned int height = static_cast<unsigned int>(rb_num2long(fromMemory));
		auto loader = cgss::TextureEmptyLoader{width, height};
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
	const auto& source = rb::GetSafeOr<TextureElement>(other, rb_cBitmap, [&](const std::string& message) {
		rb_raise(rb_eTypeError, "Cannot clone %s into Bitmap.", RSTRING_PTR(rb_class_name(CLASS_OF(other))));
	});

	if (destination.instance() == nullptr) {
		rb_raise(rb_eTypeError, "Cannot clone into an empty Bitmap.");
		return self;
	}

	*destination.instance() = source->clone();
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
	auto& s_rect = rb::GetSafe<RectangleElement>(rect, rb_cRect);

	auto& s_bitmap = rb::Get<TextureElement>(src_bitmap);
	if(s_bitmap.instance() == nullptr)  {
		rb_raise(rb_eRGSSError, "Invalid Bitmap"); 
		return self;
	}

	auto rectangle = cgss::Rectangle{};
	rectangle.setValue(s_rect->getValue());
	bitmap->blit(
		NUM2ULONG(x),
		NUM2ULONG(y),
		*s_bitmap.instance(),
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
	auto& rcolor = rb::GetSafe<ColorElement>(color, rb_cColor).getValue();

	long xValue = NUM2LONG(x);
	xValue = xValue < 0 ? 0 : xValue;

	long yValue = NUM2LONG(y);
	yValue = yValue < 0 ? 0 : yValue;

	bitmap->fillRect(xValue, yValue, static_cast<unsigned int>(NUM2LONG(width)), static_cast<unsigned int>(NUM2LONG(height)), rcolor);
	return self;
}

static VALUE rb_Bitmap_toPNG(VALUE self) {
	auto& bitmap = rb::Get<TextureElement>(self);
	auto saver = cgss::TextureMemoryLoader { {} };
	bitmap->write(saver);
	auto out = saver.stealMemory();
	return rb_str_new(reinterpret_cast<const char*>(&out[0]), out.size());
}

static VALUE rb_Bitmap_toPNG_file(VALUE self, VALUE filename) {
	rb_check_type(filename, T_STRING);
	std::string filenameValue = RSTRING_PTR(filename);
	auto& bitmap = rb::Get<TextureElement>(self);
	auto saver = cgss::TextureFileLoader { std::move(filenameValue) };
	bitmap->write(saver);
	return saver.writtenStatus() ? Qtrue : Qfalse;
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
