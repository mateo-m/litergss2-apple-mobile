#ifndef Viewport_H
#define Viewport_H

#include <LiteCGSS/Views/Viewport.h>
#include "CgssWrapper.h"
#include "RubyValue.h"
#include "ToneBindable.h"
#include "ColorBindable.h"

extern VALUE rb_cViewport;
void Init_Viewport();

struct ViewportElement : 
    public CgssInstance<cgss::Viewport>,
    public ToneBindable,
    public ColorBindable {
        
    VALUE rRect = Qnil;
    VALUE rTone = Qnil;
    VALUE rColor = Qnil;
	VALUE rAngle = LONG2FIX(0);
    VALUE rZoom = LONG2FIX(1);
    VALUE rRenderState = Qnil;

    template <class Drawable, class ... Args>
    void initAndAdd(Drawable& drawable, Args&& ... args) {
        drawable.init(Drawable::create(*instance(), std::forward<Args>(args)...));
    }

    void updateFromValue(const sf::Glsl::Vec4* toneValue) override;
    void updateFromValue(const sf::Color* colorValue) override;
};

#endif