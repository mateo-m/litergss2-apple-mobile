#include <LiteCGSS/Image/Serializers/ImageSerializer.h>
#include "LiteRGSS.h"
#include "rbAdapter.h"
#include "Image.h"
#include "Texture_Bitmap.h"
#include "Drawable_Disposable.h"
#include "Color.h"
#include "Rect.h"

VALUE rb_cImage = Qnil;

template<>
void rb::Mark<ImageElement>(ImageElement* image) {
}

VALUE rb_Image_Initialize(int argc, VALUE *argv, VALUE self) {
	VALUE string;
	VALUE fromMemory;
	auto& image = rb::Get<ImageElement>(self);
	image.init();
	rb_scan_args(argc, argv, "11", &string, &fromMemory);

	/* Load From filename */
	if (NIL_P(fromMemory)) {
		rb_check_type(string, T_STRING);
		const char* filename = RSTRING_PTR(string);
		const auto loader = cgss::ImageFileSerializer{filename};
		if (!image->load(loader)) {
			rb_raise(rb_eRGSSError, "Failed to load image from file %s.", filename);
		}
	} else if (fromMemory == Qtrue) {
		/* Load From Memory */
		rb_check_type(string, T_STRING);
		unsigned char* rawData = reinterpret_cast<unsigned char*>(RSTRING_PTR(string));
		const auto length = RSTRING_LEN(string);
		const auto loader = cgss::ImageMemorySerializer{ { rawData, length }, false };
		if (!image->load(loader)) {
			rb_raise(rb_eRGSSError, "Failed to load image from memory.");
		}
	} else {
		rb_check_type(string, T_FIXNUM);
		rb_check_type(fromMemory, T_FIXNUM);
		const unsigned int width = static_cast<unsigned int>(rb_num2long(string));
		const unsigned int height = static_cast<unsigned int>(rb_num2long(fromMemory));
		const auto loader = cgss::ImageEmptySerializer{width, height};
		if (!image->load(loader)) {
			rb_raise(rb_eRGSSError, "Invalid image size (%u x %u) !", width, height);
		}
	}
	return self;
}

VALUE rb_Image_Initialize_Copy(VALUE self, VALUE other) {
	rb_check_frozen(self);
	auto& imageElement = rb::Get<ImageElement>(self);
	const auto* otherImage = rb::GetSafeOrNull<ImageElement>(other, rb_cImage);
	if (otherImage == nullptr) {
		rb_raise(rb_eTypeError, "Cannot clone %s into Image, or invalid Image.", RSTRING_PTR(rb_class_name(CLASS_OF(other))));
		return self;
	}

	/* Uses copy operator */
	*imageElement.instance() = *otherImage->instance();
	return self;
}

VALUE rb_Image_Dispose(VALUE self) {
	return rb::RawDispose<ImageElement>(self);
}

VALUE rb_Image_Width(VALUE self) {
	const auto& image = rb::Get<ImageElement>(self);
	return rb_int2inum(image->width());
}

VALUE rb_Image_Height(VALUE self) {
	const auto& image = rb::Get<ImageElement>(self);
	return rb_int2inum(image->height());
}

VALUE rb_Image_Rect(VALUE self) {
	const auto& image = rb::Get<ImageElement>(self);
	const auto box = image->box();
	VALUE argv[4] = {LONG2FIX(box.left), LONG2FIX(box.top), rb_int2inum(box.width), rb_int2inum(box.height)};
	return rb_class_new_instance(4, argv, rb_cRect);
}

VALUE rb_Image_Copy_to_Bitmap(VALUE self, VALUE texture) {
	const auto& image = rb::Get<ImageElement>(self);
	auto* textureElement = rb::GetSafeOrNull<TextureElement>(texture, rb_cBitmap);
	if (textureElement != nullptr) {
		(*textureElement)->update(*image.instance());
	}
	return self;
}

VALUE rb_Image_blt_fast(VALUE self, VALUE x, VALUE y, VALUE src_image, VALUE rect) {
	auto& image = rb::Get<ImageElement>(self);
	const auto* sourceImage = rb::GetSafeOrNull<ImageElement>(src_image, rb_cImage);
	const auto* sourceRect = rb::GetSafeOrNull<RectangleElement>(rect, rb_cRect);
	if (sourceImage != nullptr && sourceRect != nullptr) {
		image->blit((*sourceImage)->raw(), NUM2ULONG(x), NUM2ULONG(y), *sourceRect->instance());
	}
	return self;
}

VALUE rb_Image_blt(VALUE self, VALUE x, VALUE y, VALUE src_image, VALUE rect) {
	auto& image = rb::Get<ImageElement>(self);
	const auto& sourceImage = rb::GetSafeOrNull<ImageElement>(src_image, rb_cImage);
	const auto& sourceRect = rb::GetSafeOrNull<RectangleElement>(rect, rb_cRect);
	if (sourceImage != nullptr && sourceRect != nullptr) {
		image->blit((*sourceImage)->raw(), NUM2ULONG(x), NUM2ULONG(y), *sourceRect->instance(), true);
	}
	return self;
}

VALUE rb_Image_clear_rect(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
	auto& image = rb::Get<ImageElement>(self);
	rb_check_type(x, T_FIXNUM);
	rb_check_type(y, T_FIXNUM);
	rb_check_type(width, T_FIXNUM);
	rb_check_type(height, T_FIXNUM);
	image->fillRect(sf::Color(0, 0, 0, 0), NUM2LONG(x), NUM2LONG(y), NUM2LONG(width), NUM2LONG(height));
	return self;
}

VALUE rb_Image_fill_rect(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height, VALUE color) {
	auto& image = rb::Get<ImageElement>(self);
	rb_check_type(x, T_FIXNUM);
	rb_check_type(y, T_FIXNUM);
	rb_check_type(width, T_FIXNUM);
	rb_check_type(height, T_FIXNUM);
	long x1 = NUM2LONG(x);
	if (x1 < 0) { x1 = 0; }
	long y1 = NUM2LONG(y);
	if (y1 < 0) { y1 = 0; }
	const auto* rcolor = rb::GetSafeOrNull<ColorElement>(color, rb_cColor);
	const auto realColor = rcolor == nullptr ? sf::Color{} : rcolor->getValue();
	image->fillRect(realColor, x1, y1, NUM2LONG(width), NUM2LONG(height));
	return self;
}

VALUE rb_Image_toPNG(VALUE self) {
	auto& image = rb::Get<ImageElement>(self);
	auto saver = cgss::ImageMemorySerializer { {nullptr, 0u}, true };
	image->write(saver);
	VALUE out;
	saver.finalizeMemory([&out](const cgss::MemorySerializerData& rawData) {
		out = rb_str_new(reinterpret_cast<const char*>(rawData.first), rawData.second);
	});
	return out;
}

VALUE rb_Image_toPNG_file(VALUE self, VALUE filename) {
	rb_check_type(filename, T_STRING);
	std::string filenameValue = RSTRING_PTR(filename);
	auto& image = rb::Get<ImageElement>(self);
	auto saver = cgss::ImageFileSerializer { std::move(filenameValue) };
	return image->write(saver) == 0u ? Qtrue : Qfalse;
}

VALUE rb_Image_get_pixel(VALUE self, VALUE x, VALUE y) {
	const auto& image = rb::Get<ImageElement>(self);
	const auto color = image->getPixel(NUM2ULONG(x), NUM2ULONG(y));
	if (color.has_value()) {
		VALUE argv[4] = { LONG2FIX(color->r), LONG2FIX(color->g), LONG2FIX(color->b), LONG2FIX(color->a) };
		return rb_class_new_instance(4, argv, rb_cColor);
	}
	return Qnil;
}

VALUE rb_Image_get_pixel_alpha(VALUE self, VALUE x, VALUE y) {
	const auto& image = rb::Get<ImageElement>(self);
	const auto color = image->getPixel(NUM2ULONG(x), NUM2ULONG(y));
	return LONG2FIX(color.has_value() ? color->a : 0);
}

VALUE rb_Image_set_pixel(VALUE self, VALUE x, VALUE y, VALUE color) {
	auto& image = rb::Get<ImageElement>(self);
	const auto* colorElement = rb::GetSafeOrNull<ColorElement>(color, rb_cColor);
	if (colorElement != nullptr) {
		image->setPixel(NUM2ULONG(x), NUM2ULONG(y), colorElement->getValue());
	}
	return self;
}

VALUE rb_Image_stretch_blt_fast(VALUE self, VALUE dest_rect, VALUE src_image, VALUE src_rect) {
	auto& image = rb::Get<ImageElement>(self);
	const auto* destinationRectElement = rb::GetSafeOrNull<RectangleElement>(dest_rect, rb_cRect);
	const auto* sourceRectElement = rb::GetSafeOrNull<RectangleElement>(src_rect, rb_cRect);
	const auto* imageSourceElement = rb::GetSafeOrNull<ImageElement>(src_image, rb_cImage);
	if (destinationRectElement != nullptr && sourceRectElement != nullptr && imageSourceElement != nullptr) {
		image->stretchBlit((*imageSourceElement)->raw(), *destinationRectElement->instance(), *sourceRectElement->instance());
	}
	return self;
}

VALUE rb_Image_stretch_blt(VALUE self, VALUE dest_rect, VALUE src_image, VALUE src_rect) {
	auto& image = rb::Get<ImageElement>(self);
	const auto* destinationRectElement = rb::GetSafeOrNull<RectangleElement>(dest_rect, rb_cRect);
	const auto* sourceRectElement = rb::GetSafeOrNull<RectangleElement>(src_rect, rb_cRect);
	const auto* imageSourceElement = rb::GetSafeOrNull<ImageElement>(src_image, rb_cImage);
	if (destinationRectElement != nullptr && sourceRectElement != nullptr && imageSourceElement != nullptr) {
		image->stretchBlit((*imageSourceElement)->raw(), *destinationRectElement->instance(), *sourceRectElement->instance(), true);
	}
	return self;
}

VALUE rb_Image_create_mask(VALUE self, VALUE color, VALUE alpha) {
	auto& image = rb::Get<ImageElement>(self);
	const auto* colorElement = rb::GetSafeOrNull<ColorElement>(color, rb_cColor);
	if (colorElement != nullptr) {
		image->createMaskFromColor(colorElement->getValue(), NUM2ULONG(alpha));
	}
	return self;
}

void Init_Image() {
	rb_cImage = rb_define_class_under(rb_mLiteRGSS, "Image", rb_cDisposable);
	rb_define_alloc_func(rb_cImage, rb::Alloc<ImageElement>);
	rb_define_method(rb_cImage, "initialize", _rbf rb_Image_Initialize, -1);
	rb_define_method(rb_cImage, "initialize_copy", _rbf rb_Image_Initialize_Copy, 1);
	rb_define_method(rb_cImage, "dispose", _rbf rb_Image_Dispose, 0);
	rb_define_method(rb_cImage, "width", _rbf rb_Image_Width, 0);
	rb_define_method(rb_cImage, "height", _rbf rb_Image_Height, 0);
	rb_define_method(rb_cImage, "rect", _rbf rb_Image_Rect, 0);
	rb_define_method(rb_cImage, "copy_to_bitmap", _rbf rb_Image_Copy_to_Bitmap, 1);
	rb_define_method(rb_cImage, "blt", _rbf rb_Image_blt, 4);
	rb_define_method(rb_cImage, "blt!", _rbf rb_Image_blt_fast, 4);
	rb_define_method(rb_cImage, "stretch_blt", _rbf rb_Image_stretch_blt, 3);
	rb_define_method(rb_cImage, "stretch_blt!", _rbf rb_Image_stretch_blt_fast, 3);
	rb_define_method(rb_cImage, "get_pixel", _rbf rb_Image_get_pixel, 2);
	rb_define_method(rb_cImage, "get_pixel_alpha", _rbf rb_Image_get_pixel_alpha, 2);
	rb_define_method(rb_cImage, "set_pixel", _rbf rb_Image_set_pixel, 3);
	rb_define_method(rb_cImage, "clear_rect", _rbf rb_Image_clear_rect, 4);
	rb_define_method(rb_cImage, "fill_rect", _rbf rb_Image_fill_rect, 5);
	rb_define_method(rb_cImage, "create_mask", _rbf rb_Image_create_mask, 2);
	rb_define_method(rb_cImage, "to_png", _rbf rb_Image_toPNG, 0);
	rb_define_method(rb_cImage, "to_png_file", _rbf rb_Image_toPNG_file, 1);
}
