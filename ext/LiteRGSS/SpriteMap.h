#ifndef SpriteMap_H
#define SpriteMap_H

#include <LiteCGSS/Graphics/SpriteMap.h>
#include "CgssWrapper.h"
#include "RubyValue.h"
#include "rbAdapter.h"

extern VALUE rb_cSpriteMap;
void Init_SpriteMap();

struct SpriteMapElement :
	public CgssInstance<cgss::SpriteMap> {

	VALUE rViewport = Qnil;
	VALUE rX = LONG2FIX(0);
	VALUE rY = LONG2FIX(0);
	VALUE rOX = LONG2FIX(0);
	VALUE rOY = LONG2FIX(0);
	VALUE rScale = LONG2FIX(1);
};

template<>
void rb::Mark<SpriteMapElement>(void* ptr);

#endif
