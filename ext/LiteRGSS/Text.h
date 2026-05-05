#ifndef Text_H
#define Text_H

#include <LiteCGSS/Graphics/Text.h>
#include "CgssWrapper.h"
#include "RubyValue.h"
#include "rbAdapter.h"

extern VALUE rb_cText;
void Init_Text();

struct TextElement :
	public CgssInstance<cgss::Text> {

	VALUE rViewport = Qnil;
	VALUE rwidth = LONG2FIX(0);
	VALUE rheight = LONG2FIX(0);
	VALUE rtext = Qnil;
	VALUE rAlign = Qfalse;
	VALUE rX = LONG2FIX(0);
	VALUE rY = LONG2FIX(0);
};

template<>
void rb::Mark<TextElement>(void* ptr);

#endif