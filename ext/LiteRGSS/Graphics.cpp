#include "LiteRGSS.h"
#include "NormalizeNumbers.h"
#include "rbAdapter.h"
#include "Image.h"
#include "Graphics.h"
#include "GraphicsSingleton.h"
#include "RenderStates_BlendMode.h"

VALUE rb_mGraphics = Qnil;
VALUE rb_eStoppedGraphics = Qnil;
VALUE rb_eClosedWindow = Qnil;
static ID rb_iGraphicsShader = Qnil;

VALUE rb_Graphics_start(VALUE self) {
	GraphicsSingleton::Get().init();
	return self;
}

VALUE rb_Graphics_stop(VALUE self) {
	GraphicsSingleton::Get().stop();
	return self;
}

VALUE rb_Graphics_snap_to_bitmap(VALUE self) {
	return GraphicsSingleton::Get().takeSnapshot();
}

VALUE rb_Graphics_freeze(VALUE self) {
	GraphicsSingleton::Get().freeze(self);
	return self;
}

VALUE rb_Graphics_transition(int argc, VALUE* argv, VALUE self) {
	GraphicsSingleton::Get().transition(self, argc, argv);
	return self;
}

VALUE rb_Graphics_list_res(VALUE self) {
	VALUE array = rb_ary_new();
	auto modes = sf::VideoMode::getFullscreenModes();
	for (const auto& mode : modes) {
		if (mode.bitsPerPixel == 32) {
			rb_ary_push(array, rb_ary_new3(2, rb_int2inum(mode.width), rb_int2inum(mode.height)));
		}
	}
	return array;
}

VALUE rb_Graphics_update(VALUE self) {
	GraphicsSingleton::Get().update(self);
	return self;
}

VALUE rb_Graphics_update_no_input_count(VALUE self) {
	GraphicsSingleton::Get().update(self, false);
	return self;
}

VALUE rb_Graphics_update_only_input(VALUE self) {
	GraphicsSingleton::Get().updateOnlyInput(self);
	return self;
}

VALUE rb_Graphics_get_frame_count(VALUE self) {
	return RB_UINT2NUM(GraphicsSingleton::Get().frameCount());
}

VALUE rb_Graphics_set_frame_count(VALUE self, VALUE val) {
	auto frame_count = rb_num2ulong(val);
	GraphicsSingleton::Get().setFrameCount(frame_count);
	return val;
}

VALUE rb_Graphics_width(VALUE self) {
	return rb_int2inum(GraphicsSingleton::Get().screenWidth());
}

VALUE rb_Graphics_height(VALUE self) {
	return rb_int2inum(GraphicsSingleton::Get().screenHeight());
}

VALUE rb_Graphics_ReloadStack(VALUE self) {
	// Deprecated
	return self;
}

VALUE rb_Graphics_getBrightness(VALUE self) {
	return LONG2FIX(GraphicsSingleton::Get().brightness());
}

VALUE rb_Graphics_setBrightness(VALUE self, VALUE brightness) {
	GraphicsSingleton::Get().setBrightness(normalize_long(rb_num2long(brightness), 0, 255));
	return self;
}

VALUE rb_Graphics_getShader(VALUE self) {
	return rb_ivar_get(self, rb_iGraphicsShader);
}

VALUE rb_Graphics_setShader(VALUE self, VALUE shader) {
	sf::RenderStates* render_state;
	if (rb_obj_is_kind_of(shader, rb_cBlendMode) == Qtrue) {
		rb_ivar_set(self, rb_iGraphicsShader, shader);
		Data_Get_Struct(shader, sf::RenderStates, render_state);
		GraphicsSingleton::Get().setShader(render_state);
	} else if (shader == Qnil) {
		GraphicsSingleton::Get().setShader(nullptr);
	}
	return self;
}

VALUE rb_Graphics_resize_screen(VALUE self, VALUE width, VALUE height) {
	ID swidth = rb_intern("ScreenWidth");
	ID sheight = rb_intern("ScreenHeight");
	/* Adjust screen resolution */
	if (rb_const_defined(rb_mConfig, swidth)) {
		rb_const_remove(rb_mConfig, swidth);
	}
	if (rb_const_defined(rb_mConfig, sheight)) {
		rb_const_remove(rb_mConfig, sheight);
	}
	
	const int iwidth = NUM2INT(width);
	const int iheight = NUM2INT(height);

	rb_const_set(rb_mConfig, swidth, INT2NUM(iwidth));
	rb_const_set(rb_mConfig, sheight, INT2NUM(iheight));
	GraphicsSingleton::Get().resizeScreen(iwidth, iheight);
	return self;
}

VALUE rb_Graphics_set_icon(VALUE self, VALUE icon) {
	const auto& iconImage = rb::GetSafe<ImageElement>(icon, rb_cImage);
	GraphicsSingleton::Get().setIcon(iconImage);
	return self;
}

VALUE rb_Graphics_sort_z(VALUE self) {
	GraphicsSingleton::Get().sortZ();
	return self;
}

void Init_Graphics() {
	rb_mGraphics = rb_define_module_under(rb_mLiteRGSS, "Graphics");
	/* Defining the Stopped Graphics Error */
	rb_eStoppedGraphics = rb_define_class_under(rb_mGraphics, "StoppedError", rb_eStandardError);
	rb_eClosedWindow = rb_define_class_under(rb_mGraphics, "ClosedWindowError", rb_eStandardError);
	/* Defining the Graphics functions */
	rb_define_module_function(rb_mGraphics, "start", _rbf rb_Graphics_start, 0);
	rb_define_module_function(rb_mGraphics, "stop", _rbf rb_Graphics_stop, 0);
	rb_define_module_function(rb_mGraphics, "update", _rbf rb_Graphics_update, 0);
	rb_define_module_function(rb_mGraphics, "sort_z", _rbf rb_Graphics_sort_z, 0);
	rb_define_module_function(rb_mGraphics, "snap_to_bitmap", _rbf rb_Graphics_snap_to_bitmap, 0);
	rb_define_module_function(rb_mGraphics, "freeze", _rbf rb_Graphics_freeze, 0);
	rb_define_module_function(rb_mGraphics, "transition", _rbf rb_Graphics_transition, -1);
	rb_define_module_function(rb_mGraphics, "list_resolutions", _rbf rb_Graphics_list_res, 0);
	rb_define_module_function(rb_mGraphics, "frame_count", _rbf rb_Graphics_get_frame_count, 0);
	rb_define_module_function(rb_mGraphics, "frame_count=", _rbf rb_Graphics_set_frame_count, 1);
	rb_define_module_function(rb_mGraphics, "width", _rbf rb_Graphics_width, 0);
	rb_define_module_function(rb_mGraphics, "height", _rbf rb_Graphics_height, 0);
	rb_define_module_function(rb_mGraphics, "reload_stack", _rbf rb_Graphics_ReloadStack, 0);
	rb_define_module_function(rb_mGraphics, "update_no_input", _rbf rb_Graphics_update_no_input_count, 0);
	rb_define_module_function(rb_mGraphics, "update_only_input", _rbf rb_Graphics_update_only_input, 0);
	rb_define_module_function(rb_mGraphics, "brightness", _rbf rb_Graphics_getBrightness, 0);
	rb_define_module_function(rb_mGraphics, "brightness=", _rbf rb_Graphics_setBrightness, 1);
	rb_define_module_function(rb_mGraphics, "shader", _rbf rb_Graphics_getShader, 0);
	rb_define_module_function(rb_mGraphics, "shader=", _rbf rb_Graphics_setShader, 1);
	rb_define_module_function(rb_mGraphics, "icon=", _rbf rb_Graphics_set_icon, 1);
	rb_define_module_function(rb_mGraphics, "resize_screen", _rbf rb_Graphics_resize_screen, 2);
	
	rb_iGraphicsShader = rb_intern("@__GraphicsShader");
	/* Store the max texture size */
	rb_define_const(rb_mGraphics, "MAX_TEXTURE_SIZE", LONG2FIX(sf::Texture::getMaximumSize()));
}

