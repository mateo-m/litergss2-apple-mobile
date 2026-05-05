#ifndef Texture_Bitmap_H
#define Texture_Bitmap_H

#include <LiteCGSS/Graphics/Texture.h>
#include <LiteCGSS/Views/SnapshotCapturable.h>
#include "CgssWrapper.h"
#include "RubyValue.h"
#include "rbAdapter.h"

struct TextureElement :
	public CgssInstance<cgss::Texture> {

	static VALUE snapToTexture(const cgss::SnapshotCapturable& toCapture);
};

extern VALUE rb_cBitmap;
void Init_Bitmap();

template<>
void rb::Mark<TextureElement>(void* ptr);

#endif