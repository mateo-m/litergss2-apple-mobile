#include "LiteRGSS.h"
#include "rbAdapter.h"
#include "NormalizeNumbers.h"

#include "GraphicsSingleton.h"
#include "GraphicsConfigLoader.h"
#include "DisplayWindow.h"
#include "Texture_Bitmap.h"
#include "RenderStates_BlendMode.h"
#include "Image.h"

VALUE rb_cDisplayWindow = Qnil;

static constexpr int DefaultBitsPerPixel = 32;
static constexpr int DefaultFramerate = 60;

template<>
void rb::Mark<DisplayWindowElement>(DisplayWindowElement* rectangle) {
}

static VALUE rb_DisplayWindow_initialize(int argc, VALUE* argv, VALUE self) {
	VALUE title, width, height, scale, bitsPerPixel, framerate;
	rb_scan_args(argc, argv, "42", &title, &width, &height, &scale, &bitsPerPixel, &framerate);
    
    rb_check_type(title, T_STRING);
    std::string titleStr (RSTRING_PTR(title));

    if (NIL_P(bitsPerPixel)) {
        bitsPerPixel = rb_int2inum(DefaultBitsPerPixel);
    }

    if (NIL_P(framerate)) {
        framerate = rb_int2inum(DefaultFramerate);
    }

    auto configLoader = GraphicsConfigLoader {};
    auto videoSettings = configLoader.loadVideoFromData(rb_num2long(width), rb_num2long(height), NUM2DBL(scale), rb_num2long(bitsPerPixel));
    auto contextSettings = configLoader.loadContext();
    
	auto config = cgss::DisplayWindowSettings {
		false,
		std::move(videoSettings),
		std::move(contextSettings),
		false,
		sf::String::fromUtf8(titleStr.begin(), titleStr.end()),
		static_cast<unsigned int>(rb_num2long(framerate))
	};

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.init();
    window->reload(std::move(config));

	return self;
}

static VALUE rb_DisplayWindow_dispose(VALUE self) {
    return rb::RawDispose<DisplayWindowElement>(self);
}

static VALUE rb_DisplayWindow_snap_to_bitmap(VALUE self) {
    const auto& window = rb::Get<DisplayWindowElement>(self);
	return TextureElement::snapToTexture(*window.instance());
}

static VALUE rb_DisplayWindow_freeze(VALUE self) {
    auto& window = rb::Get<DisplayWindowElement>(self);
	window->freeze();
	return self;
}

static VALUE rb_DisplayWindow_transition(int argc, VALUE* argv, VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
    window->transition(self, argc, argv);
	return self;
}

static VALUE rb_DisplayWindow_update(VALUE self) {
    auto& window = rb::Get<DisplayWindowElement>(self);
    window->update(self);
	return self;
}

static VALUE rb_DisplayWindow_update_no_input_count(VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
    window->update(self, false);
	return self;
}

static VALUE rb_DisplayWindow_update_only_input(VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
    window->updateOnlyInput(self);
	return self;
}

static VALUE rb_DisplayWindow_get_frame_count(VALUE self) {
    const auto& window = rb::Get<DisplayWindowElement>(self);
	return RB_UINT2NUM(window->frameCount());
}

static VALUE rb_DisplayWindow_set_frame_count(VALUE self, VALUE val) {
	auto& window = rb::Get<DisplayWindowElement>(self);
    auto framecount = rb_num2ulong(val);
	window->setFrameCount(framecount);
	return val;
}

static VALUE rb_DisplayWindow_width(VALUE self) {
    const auto& window = rb::Get<DisplayWindowElement>(self);
	return rb_int2inum(window->screenWidth());
}

static VALUE rb_DisplayWindow_height(VALUE self) {
    const auto& window = rb::Get<DisplayWindowElement>(self);
	return rb_int2inum(window->screenHeight());
}

static VALUE rb_DisplayWindow_getBrightness(VALUE self) {
    const auto& window = rb::Get<DisplayWindowElement>(self);
	return LONG2FIX(window->brightness());
}

static VALUE rb_DisplayWindow_setBrightness(VALUE self, VALUE brightness) {
    auto& window = rb::Get<DisplayWindowElement>(self);
	window->setBrightness(normalize_long(rb_num2long(brightness), 0, 255));
	return self;
}

static VALUE rb_DisplayWindow_getShader(VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
    return window.rShader;
}

static VALUE rb_DisplayWindow_setShader(VALUE self, VALUE shader) {
    auto& window = rb::Get<DisplayWindowElement>(self);
	if (rb_obj_is_kind_of(shader, rb_cBlendMode) == Qtrue) {
		window.rShader = shader;
		sf::RenderStates* renderStates = nullptr;
        Data_Get_Struct(shader, sf::RenderStates, renderStates);
		window->setShader(renderStates);
	} else if (shader == Qnil) {
		window->setShader(nullptr);
	}
	return self;
}

static VALUE rb_DisplayWindow_resize_screen(VALUE self, VALUE width, VALUE height) {
	const int iwidth = NUM2INT(width);
	const int iheight = NUM2INT(height);
    auto& window = rb::Get<DisplayWindowElement>(self);
	window->resizeScreen(iwidth, iheight);
	return self;
}

static VALUE rb_DisplayWindow_set_icon(VALUE self, VALUE icon) {
	const auto& iconImage = rb::GetSafe<ImageElement>(icon, rb_cImage);
    auto& window = rb::Get<DisplayWindowElement>(self);
	window->setIcon(iconImage);
	return self;
}

static VALUE rb_DisplayWindow_get_ogl_version(VALUE self) {
	const auto& window = rb::Get<DisplayWindowElement>(self);
    VALUE result = rb_ary_new2(2);
	rb_ary_push(result, LONG2NUM(window->getContextSettings().majorVersion));
	rb_ary_push(result, LONG2NUM(window->getContextSettings().minorVersion));
	return result;
}

static VALUE rb_DisplayWindow_sort_z(VALUE self) {
    auto& window = rb::Get<DisplayWindowElement>(self);
	window->sortZ();
	return self;
}

void Init_DisplayWindow() {
	rb_cDisplayWindow = rb_define_class_under(rb_mLiteRGSS, "DisplayWindow", rb_cObject);

	rb_define_alloc_func(rb_cDisplayWindow, rb::Alloc<DisplayWindowElement>);

	rb_define_method(rb_cDisplayWindow, "initialize", _rbf rb_DisplayWindow_initialize, -1);
	rb_define_method(rb_cDisplayWindow, "dispose", _rbf rb_DisplayWindow_dispose, 0);
	rb_define_method(rb_cDisplayWindow, "update", _rbf rb_DisplayWindow_update, 0);
	rb_define_method(rb_cDisplayWindow, "sort_z", _rbf rb_DisplayWindow_sort_z, 0);
	rb_define_method(rb_cDisplayWindow, "snap_to_bitmap", _rbf rb_DisplayWindow_snap_to_bitmap, 0);
	rb_define_method(rb_cDisplayWindow, "freeze", _rbf rb_DisplayWindow_freeze, 0);
	rb_define_method(rb_cDisplayWindow, "transition", _rbf rb_DisplayWindow_transition, -1);
	rb_define_method(rb_cDisplayWindow, "frame_count", _rbf rb_DisplayWindow_get_frame_count, 0);
	rb_define_method(rb_cDisplayWindow, "frame_count=", _rbf rb_DisplayWindow_set_frame_count, 1);
	rb_define_method(rb_cDisplayWindow, "width", _rbf rb_DisplayWindow_width, 0);
	rb_define_method(rb_cDisplayWindow, "height", _rbf rb_DisplayWindow_height, 0);
	rb_define_method(rb_cDisplayWindow, "update_no_input", _rbf rb_DisplayWindow_update_no_input_count, 0);
	rb_define_method(rb_cDisplayWindow, "update_only_input", _rbf rb_DisplayWindow_update_only_input, 0);
	rb_define_method(rb_cDisplayWindow, "brightness", _rbf rb_DisplayWindow_getBrightness, 0);
	rb_define_method(rb_cDisplayWindow, "brightness=", _rbf rb_DisplayWindow_setBrightness, 1);
	rb_define_method(rb_cDisplayWindow, "shader", _rbf rb_DisplayWindow_getShader, 0);
	rb_define_method(rb_cDisplayWindow, "shader=", _rbf rb_DisplayWindow_setShader, 1);
	rb_define_method(rb_cDisplayWindow, "icon=", _rbf rb_DisplayWindow_set_icon, 1);
	rb_define_method(rb_cDisplayWindow, "resize_screen", _rbf rb_DisplayWindow_resize_screen, 2);
	rb_define_method(rb_cDisplayWindow, "openGL_version", _rbf rb_DisplayWindow_get_ogl_version, 0);
}
