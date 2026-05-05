#include "LiteRGSS.h"
#include "rbAdapter.h"

#include "ShaderSprite.h"
#include "RenderStates_BlendMode.h"
#include "Shader.h"

VALUE rb_cShaderSprite = Qnil;

template<>
void rb::Mark<ShaderSpriteElement>(void* ptr) {
	auto* sprite = static_cast<ShaderSpriteElement*>(ptr);
	if (sprite == nullptr) {
		return;
	}
	rb_gc_mark(sprite->rViewport);
	rb_gc_mark(sprite->rBitmap);
	rb_gc_mark(sprite->rX);
	rb_gc_mark(sprite->rY);
	rb_gc_mark(sprite->rOX);
	rb_gc_mark(sprite->rOY);
	rb_gc_mark(sprite->rAngle);
	rb_gc_mark(sprite->rZoomX);
	rb_gc_mark(sprite->rZoomY);
	rb_gc_mark(sprite->rRect);
	rb_gc_mark(sprite->rMirror);
	rb_gc_mark(sprite->rRenderStates);
}

template <>
rb_data_type_t& rb::GetDataType<ShaderSpriteElement>() {
	static rb_data_type_t type = {
		typeid(ShaderSpriteElement).name(),
		{ Mark<ShaderSpriteElement>, Free<ShaderSpriteElement>, nullptr, nullptr },
		&GetDataType<SpriteElement>(),
		nullptr,
		RUBY_TYPED_FREE_IMMEDIATELY
	};
	return type;
}

VALUE rb_ShaderSprite_getShader(VALUE self) {
	auto& sprite = rb::Get<ShaderSpriteElement>(self);
	return sprite.rRenderStates;
}

VALUE rb_ShaderSprite_setShader(VALUE self, VALUE shader) {
	auto& sprite = rb::Get<ShaderSpriteElement>(self);
	// BlendMode is the base class of every shader
	if (rb_obj_is_kind_of(shader, rb_cBlendMode) == Qtrue)  {
		auto* renderStates = rb::GetSafeOrNull<RenderStatesElement>(shader, rb_cBlendMode);
		sprite.rRenderStates = shader;
		sprite->bindRenderStates(renderStates);
		return self;
	}
	sprite.rRenderStates = Qnil;
	sprite->bindRenderStates(nullptr);
	return self;
}

void Init_ShaderSprite() {
	rb_cShaderSprite = rb_define_class_under(rb_mLiteRGSS, "ShaderedSprite", rb_cSprite);
	rb_define_alloc_func(rb_cShaderSprite, rb::Alloc<ShaderSpriteElement>);

	rb_define_method(rb_cShaderSprite, "shader", _rbf rb_ShaderSprite_getShader, 0);
	rb_define_method(rb_cShaderSprite, "shader=", _rbf rb_ShaderSprite_setShader, 1);

	rb_define_method(rb_cShaderSprite, "blendmode", _rbf rb_ShaderSprite_getShader, 0);
	rb_define_method(rb_cShaderSprite, "blendmode=", _rbf rb_ShaderSprite_setShader, 1);
}
