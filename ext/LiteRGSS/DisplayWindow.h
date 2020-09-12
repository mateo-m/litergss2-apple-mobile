#ifndef DisplayWindow_H
#define DisplayWindow_H

#include "DisplayWindowInput.h"
#include "CgssWrapper.h"
#include "RubyValue.h"

extern VALUE rb_cDisplayWindow;
void Init_DisplayWindow();

struct DisplayWindowElement : 
	public CgssInstance<DisplayWindowInput> {
	
	VALUE rShader = Qnil;
	VALUE rKeyboard = Qnil;
	VALUE rMouse = Qnil;

	template <class Drawable, class ... Args>
	void initAndAdd(Drawable& drawable, Args&& ... args) {
		drawable.init(Drawable::create(*instance(), std::forward<Args>(args)...));
	}
};

#endif