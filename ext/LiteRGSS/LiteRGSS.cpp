#include "LiteRGSS.h"

#include "Drawable_Disposable.h"
#include "Texture_Bitmap.h"
#include "Image.h"
#include "Sprite.h"
#include "Rect.h"
#include "Viewport.h"
#include "Color.h"
#include "Tone.h"
#include "Fonts.h"
#include "Text.h"
#include "Table.h"
#include "Table32.h"
#include "Yuki.h"
#include "RenderStates_BlendMode.h"
#include "Shader.h"
#include "ShaderSprite.h"
#include "Yuki_Gif.h"
#include "Shape.h"
#include "FramedView_Window.h"
#include "SpriteMap.h"
#include "DisplayWindow.h"
#include "SfSensor.h"
#include "SfKeyBoard.h"
#include "SfMouse.h"
#include "SfJoystick.h"

// mkxp-ios: a host app lets the player run the game faster. The host app
// defines this function. A build without one links, because the symbol is
// weak, and keeps the normal speed.
extern "C" __attribute__((weak)) int psdk_fast_forward_multiplier(void);

static VALUE rb_LiteRGSS_fast_forward_multiplier(VALUE self) {
	const int multiplier = psdk_fast_forward_multiplier ? psdk_fast_forward_multiplier() : 1;
	return INT2FIX(multiplier < 1 ? 1 : multiplier);
}

VALUE rb_mLiteRGSS = Qnil;
VALUE rb_mConfig = Qnil;
VALUE rb_eRGSSError = Qnil;

extern "C" {
	void Init_LiteRGSS() {
		rb_mLiteRGSS = rb_define_module("LiteRGSS");
		rb_mConfig = rb_define_module_under(rb_mLiteRGSS, "Config");
		rb_eRGSSError = rb_define_class_under(rb_mLiteRGSS, "Error", rb_eStandardError);
		rb_define_module_function(rb_mLiteRGSS, "fast_forward_multiplier",
			_rbf rb_LiteRGSS_fast_forward_multiplier, 0);
		Init_DrawableDisposable();
		Init_Bitmap();
		Init_Image();
		Init_Sprite();
		Init_Rect();
		Init_Viewport();
		Init_Color();
		Init_Tone();
		Init_Fonts();
		Init_Text();
		Init_Table();
		Init_Table32();
		Init_Yuki();
		Init_BlendMode();
		Init_Shader();
		Init_ShaderSprite();
		Init_YukiGifReader();
		Init_Shape();
		Init_Window();
		Init_SpriteMap();
		Init_DisplayWindow();
		Init_SfSensor();
		Init_SfKeyboard();
		Init_SfMouse();
		Init_SfJoystick();
	}
}
