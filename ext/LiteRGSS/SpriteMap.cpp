#include "LiteRGSS.h"
#include "SpriteMap.h"

#include "rbAdapter.h"
#include "Texture_Bitmap.h"
#include "Sprite.h"
#include "Drawable_Disposable.h"
#include "Rect.h"
#include "Viewport.h"

VALUE rb_cSpriteMap = Qnil;

template<>
void rb::Mark<SpriteMapElement>(void* ptr) {
	auto* sprite = static_cast<SpriteMapElement*>(ptr);
	if (sprite == nullptr) {
		return;
	}
	rb_gc_mark(sprite->rViewport);
	rb_gc_mark(sprite->rX);
	rb_gc_mark(sprite->rY);
	rb_gc_mark(sprite->rOX);
	rb_gc_mark(sprite->rOY);
	rb_gc_mark(sprite->rScale);
}

VALUE rb_SpriteMap_Initialize(int argc, VALUE* argv, VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	VALUE viewport, tile_width, tile_count;
	rb_scan_args(argc, argv, "30", &viewport, &tile_width, &tile_count);

	auto* viewportElement = rb::GetSafeOrNull<ViewportElement>(viewport, rb_cViewport);
	if (viewportElement != nullptr) {
		viewportElement->initAndAdd(spriteMap);
		spriteMap.rViewport = viewport;
	}

	spriteMap->defineMap(NUM2ULONG(tile_width), NUM2ULONG(tile_count));
	return self;
}

VALUE rb_SpriteMap_Dispose(VALUE self) {
	return rb::Dispose<SpriteMapElement>(self);
}

VALUE rb_SpriteMap_Disposed(VALUE self) {
	return RTYPEDDATA_DATA(self) == nullptr ? Qtrue : Qfalse;
}

VALUE rb_SpriteMap_Viewport(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return spriteMap.rViewport;
}

VALUE rb_SpriteMap_X(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return spriteMap.rX;
}

VALUE rb_SpriteMap_SetX(VALUE self, VALUE val) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->move(NUM2DBL(val), NUM2DBL(spriteMap.rY));
	spriteMap.rX = val;
	return self;
}

VALUE rb_SpriteMap_Y(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return spriteMap.rY;
}

VALUE rb_SpriteMap_SetY(VALUE self, VALUE val) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->move(NUM2DBL(spriteMap.rX), NUM2DBL(val));
	spriteMap.rY = val;
	return self;
}

VALUE rb_SpriteMap_SetPosition(VALUE self, VALUE x, VALUE y) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->move(NUM2DBL(x), NUM2DBL(y));
	spriteMap.rX = x;
	spriteMap.rY = y;
	return self;
}

VALUE rb_SpriteMap_Z(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return LONG2FIX(spriteMap->getZ().z);
}

VALUE rb_SpriteMap_SetZ(VALUE self, VALUE val) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->setZ(NUM2LONG(val));
	return self;
}

VALUE rb_SpriteMap_OX(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return spriteMap.rOX;
}

VALUE rb_SpriteMap_SetOX(VALUE self, VALUE val) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->moveOrigin(NUM2DBL(val), NUM2DBL(spriteMap.rOY));
	spriteMap.rOX = val;
	return self;
}

VALUE rb_SpriteMap_OY(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return spriteMap.rOY;
}

VALUE rb_SpriteMap_SetOY(VALUE self, VALUE val) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->moveOrigin(NUM2DBL(spriteMap.rOX), NUM2DBL(val));
	spriteMap.rOY = val;
	return self;
}

VALUE rb_SpriteMap_SetOrigin(VALUE self, VALUE x, VALUE y) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->moveOrigin(NUM2DBL(x), NUM2DBL(y));
	spriteMap.rOX = x;
	spriteMap.rOY = y;
	return self;
}

VALUE rb_SpriteMap_Reset(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->reset();
	return self;
}

VALUE rb_SpriteMap_Set(int argc, VALUE* argv, VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	VALUE index, bitmap, rect;
	rb_scan_args(argc, argv, "30", &index, &bitmap, &rect);

	auto* textureElement = rb::GetSafeOrNull<TextureElement>(bitmap, rb_cBitmap);
	const auto* rectangleElement = rb::GetSafeOrNull<RectangleElement>(rect, rb_cRect);
	if (textureElement != nullptr && rectangleElement != nullptr) {
		spriteMap->setTile(NUM2LONG(index), (*rectangleElement)->getValue(), (*textureElement)->raw());
	}
	return self;
}

VALUE rb_SpriteMap_SetRect(int argc, VALUE* argv, VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	VALUE index, x, y, width, height;
	rb_scan_args(argc, argv, "23", &index, &x, &y, &width, &height);

	if (NIL_P(y)) {
		const auto* rectangleElement = rb::GetSafeOrNull<RectangleElement>(x, rb_cRect);
		if (rectangleElement != nullptr) {
			spriteMap->setTileRect(NUM2LONG(index), (*rectangleElement)->getValue());
		} else {
			rb_raise(rb_eRGSSError, "SpriteMaps require having a Rect as 2nd parameter in set_rect method, or list of 4 integers representing Rect dimensions.");
		}
	} else {
		sf::IntRect rect = sf::IntRect(NUM2INT(x), NUM2INT(y), NUM2INT(width), NUM2INT(height));
		spriteMap->setTileRect(NUM2LONG(index), rect);
	}
	return self;
}

VALUE rb_SpriteMap_TileScale(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return spriteMap.rScale;
}

VALUE rb_SpriteMap_TileScaleSet(VALUE self, VALUE val) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	spriteMap->setTileScale(NUM2DBL(val));
	spriteMap.rScale = val;
	return self;
}

VALUE rb_SpriteMap_index(VALUE self) {
	auto& spriteMap = rb::Get<SpriteMapElement>(self);
	return LONG2NUM(spriteMap->getZ().index);
}

VALUE rb_SpriteMap_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "SpriteMaps cannot be cloned or duplicated.");
	return self;
}

void Init_SpriteMap() {
	rb_cSpriteMap = rb_define_class_under(rb_mLiteRGSS, "SpriteMap", rb_cDrawable);
	rb_define_alloc_func(rb_cSpriteMap, rb::Alloc<SpriteMapElement>);

	rb_define_method(rb_cSpriteMap, "initialize", _rbf rb_SpriteMap_Initialize, -1);
	rb_define_method(rb_cSpriteMap, "dispose", _rbf rb_SpriteMap_Dispose, 0);
	rb_define_method(rb_cSpriteMap, "disposed?", _rbf rb_SpriteMap_Disposed, 0);
	rb_define_method(rb_cSpriteMap, "viewport", _rbf rb_SpriteMap_Viewport, 0);
	rb_define_method(rb_cSpriteMap, "x", _rbf rb_SpriteMap_X, 0);
	rb_define_method(rb_cSpriteMap, "x=", _rbf rb_SpriteMap_SetX, 1);
	rb_define_method(rb_cSpriteMap, "y", _rbf rb_SpriteMap_Y, 0);
	rb_define_method(rb_cSpriteMap, "y=", _rbf rb_SpriteMap_SetY, 1);
	rb_define_method(rb_cSpriteMap, "set_position", _rbf rb_SpriteMap_SetPosition, 2);
	rb_define_method(rb_cSpriteMap, "z", _rbf rb_SpriteMap_Z, 0);
	rb_define_method(rb_cSpriteMap, "z=", _rbf rb_SpriteMap_SetZ, 1);
	rb_define_method(rb_cSpriteMap, "ox", _rbf rb_SpriteMap_OX, 0);
	rb_define_method(rb_cSpriteMap, "ox=", _rbf rb_SpriteMap_SetOX, 1);
	rb_define_method(rb_cSpriteMap, "oy", _rbf rb_SpriteMap_OY, 0);
	rb_define_method(rb_cSpriteMap, "oy=", _rbf rb_SpriteMap_SetOY, 1);
	rb_define_method(rb_cSpriteMap, "tile_scale", _rbf rb_SpriteMap_TileScale, 0);
	rb_define_method(rb_cSpriteMap, "tile_scale=", _rbf rb_SpriteMap_TileScaleSet, 1);
	rb_define_method(rb_cSpriteMap, "set_origin", _rbf rb_SpriteMap_SetOrigin, 2);
	rb_define_method(rb_cSpriteMap, "reset", _rbf rb_SpriteMap_Reset, 0);
	rb_define_method(rb_cSpriteMap, "set", _rbf rb_SpriteMap_Set, -1);
	rb_define_method(rb_cSpriteMap, "set_rect", _rbf rb_SpriteMap_SetRect, -1);
	rb_define_method(rb_cSpriteMap, "__index__", _rbf rb_SpriteMap_index, 0);

	rb_define_method(rb_cSprite, "clone", _rbf rb_SpriteMap_Copy, 0);
	rb_define_method(rb_cSprite, "dup", _rbf rb_SpriteMap_Copy, 0);
}
