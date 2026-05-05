#ifndef Rect_H
#define Rect_H

#include <LiteCGSS/Common/Rectangle.h>
#include <LiteCGSS/Graphics/GraphicsStackItem.h>
#include "RubyValue.h"
#include "CgssWrapper.h"
#include "rbAdapter.h"

using RectangleElement = CgssInstance<cgss::Rectangle>;
extern VALUE rb_cRect;
void Init_Rect();

// Mark hook MUST be declared in the header so every TU that instantiates rb::GetDataType<RectangleElement>
// initialises the static rb_data_type_t with the real mark, not the generic no-op from rbAdapter.h.
// Body lives in Rect.cpp.
template<>
void rb::Mark<RectangleElement>(void* ptr);

template <class Drawable>
VALUE rb_Rect_LazyInitDrawable(VALUE& rectValue, Drawable& drawable, const sf::IntRect& initialValue) {
	// Either it's already initialized, so we just have to return the VALUE
	if (!NIL_P(rectValue)) {
		return rectValue;
	}

	// Or we have to create an intance of RectangleElement and link it to the GraphicsStackItem element
	VALUE argv[] = { LONG2FIX(initialValue.left), LONG2FIX(initialValue.top), LONG2FIX(initialValue.width), LONG2FIX(initialValue.height)  };
	rectValue = rb_class_new_instance(4, argv, rb_cRect);
	auto& rectElement = rb::Get<RectangleElement>(rectValue);
	drawable.bindRectangle(rectElement.instance());
	return rectValue;
}

#endif
