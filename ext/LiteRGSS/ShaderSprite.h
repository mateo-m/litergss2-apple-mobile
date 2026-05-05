#ifndef ShaderSprite_H
#define ShaderSprite_H

#include "RubyValue.h"
#include "Sprite.h"
#include "rbAdapter.h"

extern VALUE rb_cShaderSprite;
void Init_ShaderSprite();

struct ShaderSpriteElement :
	public SpriteElement {

	VALUE rRenderStates;
};

template<>
void rb::Mark<ShaderSpriteElement>(void* ptr);

template <>
rb_data_type_t& rb::GetDataType<ShaderSpriteElement>();

#endif