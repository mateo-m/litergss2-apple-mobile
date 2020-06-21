#include "LiteRGSS.h"

#include "Drawable_Disposable.h"
#include "Graphics.h"
#include "Texture_Bitmap.h"
#include "Image.h"
#include "Sprite.h"
#include "Rect.h"
#include "Viewport.h"
#include "Color.h"
#include "Tone.h"
#include "Fonts.h"
#include "Text.h"
#include "Input.h"
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

VALUE rb_mLiteRGSS = Qnil;
VALUE rb_mConfig = Qnil;
VALUE rb_eRGSSError = Qnil;

extern "C" {
  void Init_LiteRGSS() {
	rb_mLiteRGSS = rb_define_module("LiteRGSS");
	rb_mConfig = rb_define_module_under(rb_mLiteRGSS, "Config");
	rb_eRGSSError = rb_define_class_under(rb_mLiteRGSS, "Error", rb_eStandardError);

	Init_DrawableDisposable();
	Init_Graphics();
	Init_Bitmap();
	Init_Image();
	Init_Sprite();
	Init_Rect();
	Init_Viewport();
	Init_Color();
	Init_Tone();
	Init_Fonts();
	Init_Text();
	Init_Input();
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
  }
}
