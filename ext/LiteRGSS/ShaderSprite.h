#ifndef ShaderSprite_H
#define ShaderSprite_H

#include "RubyValue.h"
#include "Sprite.h"

extern VALUE rb_cShaderSprite;
void Init_ShaderSprite();

struct ShaderSpriteElement :
	public SpriteElement {

	VALUE rRenderStates;
};

#endif