#include <LiteCGSS/Common/NormalizeNumbers.h>
#include "LiteRGSS.h"
#include "rbAdapter.h"

#include "DisplayWindowConfigLoader.h"
#include "DisplayWindow.h"
#include "Texture_Bitmap.h"
#include "RenderStates_BlendMode.h"
#include "Image.h"

VALUE rb_cDisplayWindow = Qnil;
VALUE rb_eStoppedWindowError = Qnil;
VALUE rb_eClosedWindow = Qnil;

static constexpr int DefaultBitsPerPixel = 32;
static constexpr int DefaultFramerate = 60;

template<>
void rb::Mark<DisplayWindowElement>(void* ptr) {
	auto* window = static_cast<DisplayWindowElement*>(ptr);
	if (window == nullptr) {
		return;
	}
	rb_gc_mark(window->rKeyboard);
	rb_gc_mark(window->rMouse);
	rb_gc_mark(window->rShader);
	rb_gc_mark(window->rOnClosed);
	rb_gc_mark(window->rOnResized);
	rb_gc_mark(window->rOnLostFocus);
	rb_gc_mark(window->rOnGainedFocus);
	rb_gc_mark(window->rOnTextEntered);
	rb_gc_mark(window->rOnKeyPressed);
	rb_gc_mark(window->rOnKeyReleased);
	rb_gc_mark(window->rOnMouseWheelScrolled);
	rb_gc_mark(window->rOnMouseButtonPressed);
	rb_gc_mark(window->rOnMouseButtonRelease);
	rb_gc_mark(window->rOnMouseMoved);
	rb_gc_mark(window->rOnMouseEntered);
	rb_gc_mark(window->rOnMouseLeft);
	rb_gc_mark(window->rOnJoystickButtonPressed);
	rb_gc_mark(window->rOnJoystickButtonReleased);
	rb_gc_mark(window->rOnJoystickMoved);
	rb_gc_mark(window->rOnJoystickConnected);
	rb_gc_mark(window->rOnJoystickDisconnected);
	rb_gc_mark(window->rOnTouchBegan);
	rb_gc_mark(window->rOnTouchMoved);
	rb_gc_mark(window->rOnTouchEnded);
	rb_gc_mark(window->rOnSensorChanged);
}

// mkxp-ios: a host app owns the smooth switch, because Ruby never sets
// this field. A texture reads the flag when the game creates it, so a new
// value only reaches the whole game on the next start. The host app
// defines the function. A build without one links, because the symbol is
// weak, and keeps the sharp picture.
extern "C" __attribute__((weak)) int psdk_smooth_scaling_enabled(void);

static cgss::DisplayWindowSettings BuildSettings(int argc, VALUE* argv, VALUE self) {
	VALUE title, width, height, scale, bitsPerPixel, framerate, vsync, fullscreen, visibleMouse;
	rb_scan_args(argc, argv, "45", &title, &width, &height, &scale, &bitsPerPixel, &framerate, &vsync, &fullscreen, &visibleMouse);

	rb_check_type(title, T_STRING);

	if (NIL_P(bitsPerPixel)) {
		bitsPerPixel = rb_int2inum(DefaultBitsPerPixel);
	}

	if (NIL_P(framerate)) {
		framerate = rb_int2inum(DefaultFramerate);
	}

	auto configLoader = DisplayWindowConfigLoader {};
	auto videoSettings = configLoader.loadVideoFromData(rb_num2long(width), rb_num2long(height), NUM2DBL(scale), rb_num2long(bitsPerPixel));

	/* We take default context settings because setting one explicitely can cause issues */
	auto contextSettings = sf::ContextSettings();

	const std::string titleStr { RSTRING_PTR(title) };
	return cgss::DisplayWindowSettings {
		false,
		std::move(videoSettings),
		std::move(contextSettings),
		psdk_smooth_scaling_enabled ? psdk_smooth_scaling_enabled() != 0 : false,
		sf::String::fromUtf8(titleStr.begin(), titleStr.end()),
		static_cast<unsigned int>(rb_num2long(framerate)),
		RTEST(vsync),
		RTEST(fullscreen),
		RTEST(visibleMouse)
	};
}

static VALUE rb_DisplayWindow_setSettings(VALUE self, VALUE settings) {
	rb_check_type(settings, T_ARRAY);
	const int argc = RARRAY_LEN(settings);
	if (argc != 9) {
		rb_raise(rb_eTypeError, "Settings are represented by an array of 9 values");
		return self;
	}

	VALUE argv[9];
	for (int i = 0; i < argc; i++) {
		argv[i] = rb_ary_entry(settings, i);
	}

	auto config = BuildSettings(argc, argv, self);

	auto& window = rb::Get<DisplayWindowElement>(self);
	window->reload(std::move(config));

	return self;
}

static VALUE rb_DisplayWindow_getSettings(VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	const auto& settings = window->getSettings();

	auto ary = rb_ary_new_capa(9);
	rb_ary_push(ary, rb_utf8_str_new_cstr((char*) settings.title.toUtf8().c_str()));
	rb_ary_push(ary, UINT2NUM(settings.video.width));
	rb_ary_push(ary, UINT2NUM(settings.video.height));
	rb_ary_push(ary, DBL2NUM(settings.video.scale));
	rb_ary_push(ary, UINT2NUM(settings.video.bitsPerPixel));
	rb_ary_push(ary, UINT2NUM(settings.frameRate));
	rb_ary_push(ary, settings.vSync ? Qtrue : Qfalse);
	rb_ary_push(ary, settings.fullscreen ? Qtrue : Qfalse);
	rb_ary_push(ary, settings.visibleMouse ? Qtrue : Qfalse);
	return ary;
}

static VALUE rb_DisplayWindow_initialize(int argc, VALUE* argv, VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	window.init();
	auto config = BuildSettings(argc, argv, self);
	window->reload(std::move(config), true);
	return self;
}

static VALUE rb_DisplayWindow_dispose(VALUE self) {
	return rb::RawDispose<DisplayWindowElement>(self);
}

static VALUE rb_DisplayWindow_snap_to_bitmap(VALUE self) {
	const auto& window = rb::Get<DisplayWindowElement>(self);
	return TextureElement::snapToTexture(*window.instance());
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

static VALUE rb_DisplayWindow_getX(VALUE self) {
	const auto& window = rb::Get<DisplayWindowElement>(self);
	return rb_int2inum(window->getX());
}

static VALUE rb_DisplayWindow_getY(VALUE self) {
	const auto& window = rb::Get<DisplayWindowElement>(self);
	return rb_int2inum(window->getY());
}

static VALUE rb_DisplayWindow_setX(VALUE self, VALUE x) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	window->move(NUM2INT(x), window->getY());
	return self;
}

static VALUE rb_DisplayWindow_setY(VALUE self, VALUE y) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	window->move(window->getX(), NUM2INT(y));
	return self;
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
	window->setBrightness(cgss::normalize_long(rb_num2long(brightness), 0, 255));
	return self;
}

static VALUE rb_DisplayWindow_setPolling(VALUE self, VALUE polling) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	window->changeEventPolicy(polling == Qtrue);
	return self;
}

static VALUE rb_DisplayWindow_getShader(VALUE self) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	return window.rShader;
}

static VALUE rb_DisplayWindow_setShader(VALUE self, VALUE shader) {
	auto& window = rb::Get<DisplayWindowElement>(self);
	if (rb_obj_is_kind_of(shader, rb_cBlendMode) == Qtrue) {
		auto* renderStates = rb::GetSafeOrNull<RenderStatesElement>(shader, rb_cBlendMode);
		if (renderStates) {
			window.rShader = shader;
			window->bindRenderStates(renderStates);
		}
	} else if (shader == Qnil) {
		window->bindRenderStates(nullptr);
	}
	return self;
}

// mkxp-ios: see DisplayWindowConfigLoader.cpp. This path skips the
// config loader, so it reports the new resolution itself.
extern "C" __attribute__((weak)) void psdk_game_resolution(long width, long height);

static VALUE rb_DisplayWindow_resize_screen(VALUE self, VALUE width, VALUE height) {
	const int iwidth = NUM2INT(width);
	const int iheight = NUM2INT(height);
	auto& window = rb::Get<DisplayWindowElement>(self);
	window->resizeScreen(iwidth, iheight);
	if (psdk_game_resolution && iwidth > 0 && iheight > 0) {
		psdk_game_resolution(iwidth, iheight);
	}
	return self;
}

static VALUE rb_DisplayWindow_set_icon(VALUE self, VALUE icon) {
	const auto* iconImage = rb::GetSafeOrNull<ImageElement>(icon, rb_cImage);
	if (iconImage != nullptr) {
		auto& window = rb::Get<DisplayWindowElement>(self);
		window->setIcon((*iconImage)->raw());
	}
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

static VALUE rb_DisplayWindow_list_res(VALUE self) {
	VALUE array = rb_ary_new();
	auto modes = sf::VideoMode::getFullscreenModes();
	for (const auto& mode : modes) {
		if (mode.bitsPerPixel == 32) {
			rb_ary_push(array, rb_ary_new3(2, rb_int2inum(mode.width), rb_int2inum(mode.height)));
		}
	}
	return array;
}

static VALUE rb_DisplayWindow_desktop_width(VALUE self) {
	return rb_int2inum(cgss::DisplayWindow::DesktopWidth());
}

static VALUE rb_DisplayWindow_desktop_height(VALUE self) {
	return rb_int2inum(cgss::DisplayWindow::DesktopHeight());
}

static VALUE rb_DisplayWindow_set_onClosed(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnClosed = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onResized(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnResized = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onLostFocus(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnLostFocus = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onGainedFocus(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnGainedFocus = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onTextEntered(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnTextEntered = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onKeyPressed(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnKeyPressed = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onKeyReleased(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnKeyReleased = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onMouseWheelScrolled(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnMouseWheelScrolled = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onMouseButtonPressed(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnMouseButtonPressed = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onMouseButtonRelease(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnMouseButtonRelease = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onMouseMoved(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnMouseMoved = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onMouseEntered(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnMouseEntered = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onMouseLeft(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnMouseLeft = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onJoystickButtonPressed(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnJoystickButtonPressed = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onJoystickButtonReleased(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnJoystickButtonReleased = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onJoystickMoved(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnJoystickMoved = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onJoystickConnected(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnJoystickConnected = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onJoystickDisconnected(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnJoystickDisconnected = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onTouchBegan(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnTouchBegan = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onTouchMoved(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnTouchMoved = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onTouchEnded(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnTouchEnded = proc;

	return proc;
}

static VALUE rb_DisplayWindow_set_onSensorChanged(VALUE self, VALUE proc) {
	if (!(NIL_P(proc) || rb_respond_to(proc, rb_intern("call")))) {
		rb_raise(rb_eTypeError, "`on_stuff` events must respond to #call!");
	}

	auto& window = rb::Get<DisplayWindowElement>(self);
	window.rOnSensorChanged = proc;

	return proc;
}

static VALUE rb_DisplayWindow_max_texture_size(VALUE self) {
	return LONG2FIX(sf::Texture::getMaximumSize());
}

void Init_DisplayWindow() {
	rb_cDisplayWindow = rb_define_class_under(rb_mLiteRGSS, "DisplayWindow", rb_cObject);

	/* Defining the Stopped Graphics Error */
	rb_eStoppedWindowError = rb_define_class_under(rb_cDisplayWindow, "StoppedError", rb_eStandardError);
	rb_eClosedWindow = rb_define_class_under(rb_cDisplayWindow, "ClosedWindowError", rb_eStandardError);

	rb_define_alloc_func(rb_cDisplayWindow, rb::Alloc<DisplayWindowElement>);

	rb_define_method(rb_cDisplayWindow, "initialize", _rbf rb_DisplayWindow_initialize, -1);
	rb_define_method(rb_cDisplayWindow, "dispose", _rbf rb_DisplayWindow_dispose, 0);
	rb_define_method(rb_cDisplayWindow, "update", _rbf rb_DisplayWindow_update, 0);
	rb_define_method(rb_cDisplayWindow, "sort_z", _rbf rb_DisplayWindow_sort_z, 0);
	rb_define_method(rb_cDisplayWindow, "snap_to_bitmap", _rbf rb_DisplayWindow_snap_to_bitmap, 0);
	rb_define_method(rb_cDisplayWindow, "width", _rbf rb_DisplayWindow_width, 0);
	rb_define_method(rb_cDisplayWindow, "height", _rbf rb_DisplayWindow_height, 0);
	rb_define_method(rb_cDisplayWindow, "update_no_input", _rbf rb_DisplayWindow_update_no_input_count, 0);
	rb_define_method(rb_cDisplayWindow, "update_only_input", _rbf rb_DisplayWindow_update_only_input, 0);
	rb_define_method(rb_cDisplayWindow, "brightness", _rbf rb_DisplayWindow_getBrightness, 0);
	rb_define_method(rb_cDisplayWindow, "brightness=", _rbf rb_DisplayWindow_setBrightness, 1);
	rb_define_method(rb_cDisplayWindow, "polling=", _rbf rb_DisplayWindow_setPolling, 1);
	rb_define_method(rb_cDisplayWindow, "shader", _rbf rb_DisplayWindow_getShader, 0);
	rb_define_method(rb_cDisplayWindow, "shader=", _rbf rb_DisplayWindow_setShader, 1);
	rb_define_method(rb_cDisplayWindow, "icon=", _rbf rb_DisplayWindow_set_icon, 1);
	rb_define_method(rb_cDisplayWindow, "resize_screen", _rbf rb_DisplayWindow_resize_screen, 2);
	rb_define_method(rb_cDisplayWindow, "openGL_version", _rbf rb_DisplayWindow_get_ogl_version, 0);
	rb_define_method(rb_cDisplayWindow, "settings", _rbf rb_DisplayWindow_getSettings, 0);
	rb_define_method(rb_cDisplayWindow, "settings=", _rbf rb_DisplayWindow_setSettings, 1);
	rb_define_method(rb_cDisplayWindow, "x", _rbf rb_DisplayWindow_getX, 0);
	rb_define_method(rb_cDisplayWindow, "y", _rbf rb_DisplayWindow_getY, 0);
	rb_define_method(rb_cDisplayWindow, "x=", _rbf rb_DisplayWindow_setX, 1);
	rb_define_method(rb_cDisplayWindow, "y=", _rbf rb_DisplayWindow_setY, 1);

	/* Events */
	rb_define_method(rb_cDisplayWindow, "on_closed=", _rbf rb_DisplayWindow_set_onClosed, 1);
	rb_define_method(rb_cDisplayWindow, "on_resized=", _rbf rb_DisplayWindow_set_onResized, 1);
	rb_define_method(rb_cDisplayWindow, "on_lost_focus=", _rbf rb_DisplayWindow_set_onLostFocus, 1);
	rb_define_method(rb_cDisplayWindow, "on_gained_focus=", _rbf rb_DisplayWindow_set_onGainedFocus, 1);
	rb_define_method(rb_cDisplayWindow, "on_text_entered=", _rbf rb_DisplayWindow_set_onTextEntered, 1);
	rb_define_method(rb_cDisplayWindow, "on_key_pressed=", _rbf rb_DisplayWindow_set_onKeyPressed, 1);
	rb_define_method(rb_cDisplayWindow, "on_key_released=", _rbf rb_DisplayWindow_set_onKeyReleased, 1);
	rb_define_method(rb_cDisplayWindow, "on_mouse_wheel_scrolled=", _rbf rb_DisplayWindow_set_onMouseWheelScrolled, 1);
	rb_define_method(rb_cDisplayWindow, "on_mouse_button_pressed=", _rbf rb_DisplayWindow_set_onMouseButtonPressed, 1);
	rb_define_method(rb_cDisplayWindow, "on_mouse_button_released=", _rbf rb_DisplayWindow_set_onMouseButtonRelease, 1);
	rb_define_method(rb_cDisplayWindow, "on_mouse_moved=", _rbf rb_DisplayWindow_set_onMouseMoved, 1);
	rb_define_method(rb_cDisplayWindow, "on_mouse_entered=", _rbf rb_DisplayWindow_set_onMouseEntered, 1);
	rb_define_method(rb_cDisplayWindow, "on_mouse_left=", _rbf rb_DisplayWindow_set_onMouseLeft, 1);
	rb_define_method(rb_cDisplayWindow, "on_joystick_button_pressed=", _rbf rb_DisplayWindow_set_onJoystickButtonPressed, 1);
	rb_define_method(rb_cDisplayWindow, "on_joystick_button_released=", _rbf rb_DisplayWindow_set_onJoystickButtonReleased, 1);
	rb_define_method(rb_cDisplayWindow, "on_joystick_moved=", _rbf rb_DisplayWindow_set_onJoystickMoved, 1);
	rb_define_method(rb_cDisplayWindow, "on_joystick_connected=", _rbf rb_DisplayWindow_set_onJoystickConnected, 1);
	rb_define_method(rb_cDisplayWindow, "on_joystick_disconnected=", _rbf rb_DisplayWindow_set_onJoystickDisconnected, 1);
	rb_define_method(rb_cDisplayWindow, "on_touch_began=", _rbf rb_DisplayWindow_set_onTouchBegan, 1);
	rb_define_method(rb_cDisplayWindow, "on_touch_moved=", _rbf rb_DisplayWindow_set_onTouchMoved, 1);
	rb_define_method(rb_cDisplayWindow, "on_touch_ended=", _rbf rb_DisplayWindow_set_onTouchEnded, 1);
	rb_define_method(rb_cDisplayWindow, "on_sensor_changed=", _rbf rb_DisplayWindow_set_onSensorChanged, 1);

	/* Utility */
	rb_define_singleton_method(rb_cDisplayWindow, "list_resolutions", _rbf rb_DisplayWindow_list_res, 0);
	rb_define_singleton_method(rb_cDisplayWindow, "desktop_width", _rbf rb_DisplayWindow_desktop_width, 0);
	rb_define_singleton_method(rb_cDisplayWindow, "desktop_height", _rbf rb_DisplayWindow_desktop_height, 0);
	rb_define_singleton_method(rb_cDisplayWindow, "max_texture_size", _rbf rb_DisplayWindow_max_texture_size, 0);
}
