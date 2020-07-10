#include "LiteRGSS.h"
#include "RenderStates_BlendMode.h"
#include "rbAdapter.h"

VALUE rb_cBlendMode = Qnil;

VALUE rb_BlendMode_setColorSrcFactor(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	renderStates.data().setBlendColorSrcFactor(static_cast<sf::BlendMode::Factor>(NUM2LONG(val)));
	return self;
}

VALUE rb_BlendMode_getColorSrcFactor(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	return LONG2NUM(renderStates.data().getBlendColorSrcFactor());
}

VALUE rb_BlendMode_setColorDestFactor(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	renderStates.data().setBlendColorDstFactor(static_cast<sf::BlendMode::Factor>(NUM2LONG(val)));
	return self;
}

VALUE rb_BlendMode_getColorDestFactor(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	return LONG2NUM(renderStates.data().getBlendColorDstFactor());
}

VALUE rb_BlendMode_setAlphaSrcFactor(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	renderStates.data().setBlendAlphaSrcFactor(static_cast<sf::BlendMode::Factor>(NUM2LONG(val)));
	return self;
}

VALUE rb_BlendMode_getAlphaSrcFactor(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	return LONG2NUM(renderStates.data().getBlendAlphaSrcFactor());
}

VALUE rb_BlendMode_setAlphaDestFactor(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	renderStates.data().setBlendAlphaDstFactor(static_cast<sf::BlendMode::Factor>(NUM2LONG(val)));
	return self;
}

VALUE rb_BlendMode_getAlphaDestFactor(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	return LONG2NUM(renderStates.data().getBlendAlphaDstFactor());
}

VALUE rb_BlendMode_setColorEquation(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	renderStates.data().setBlendColorEquation(static_cast<sf::BlendMode::Equation>(NUM2LONG(val)));
	return self;
}

VALUE rb_BlendMode_getColorEquation(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	return LONG2NUM(renderStates.data().getBlendColorEquation());
}

VALUE rb_BlendMode_setAlphaEquation(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	renderStates.data().setBlendAlphaEquation(static_cast<sf::BlendMode::Equation>(NUM2LONG(val)));
	return self;
}

VALUE rb_BlendMode_getAlphaEquation(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	return LONG2NUM(renderStates.data().getBlendAlphaEquation());
}

VALUE rb_BlendMode_setBlendType(VALUE self, VALUE val) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);
	long mode = NUM2LONG(val);
	switch (mode) {
	// Add
	case 1:
		renderStates.data().setBlendColorEquation(sf::BlendMode::Equation::Add);
		renderStates.data().setBlendAlphaEquation(sf::BlendMode::Equation::Add);
		renderStates.data().setBlendColorSrcFactor(sf::BlendMode::Factor::SrcAlpha);
		renderStates.data().setBlendColorDstFactor(sf::BlendMode::Factor::One);
		renderStates.data().setBlendAlphaSrcFactor(sf::BlendMode::Factor::One);
		renderStates.data().setBlendAlphaDstFactor(sf::BlendMode::Factor::One);
		break;
	// Sub
	case 2:
		renderStates.data().setBlendColorEquation(sf::BlendMode::Equation::ReverseSubtract);
		renderStates.data().setBlendAlphaEquation(sf::BlendMode::Equation::ReverseSubtract);
		renderStates.data().setBlendColorSrcFactor(sf::BlendMode::Factor::SrcAlpha);
		renderStates.data().setBlendColorDstFactor(sf::BlendMode::Factor::One);
		renderStates.data().setBlendAlphaSrcFactor(sf::BlendMode::Factor::Zero);
		renderStates.data().setBlendAlphaDstFactor(sf::BlendMode::Factor::One);
		break;
	// Normal
	default:
		renderStates.data().setBlendColorEquation(sf::BlendMode::Equation::Add);
		renderStates.data().setBlendAlphaEquation(sf::BlendMode::Equation::Add);
		renderStates.data().setBlendColorSrcFactor(sf::BlendMode::Factor::SrcAlpha);
		renderStates.data().setBlendColorDstFactor(sf::BlendMode::Factor::OneMinusSrcAlpha);
		renderStates.data().setBlendAlphaSrcFactor(sf::BlendMode::Factor::One);
		renderStates.data().setBlendAlphaDstFactor(sf::BlendMode::Factor::OneMinusSrcAlpha);
		break;
	}
	return self;
}

VALUE rb_BlendMode_getBlendType(VALUE self) {
	auto& renderStates = rb::Get<RenderStatesElement>(self);

	if (renderStates.data().getBlendColorEquation() == sf::BlendMode::Equation::ReverseSubtract) {
		return LONG2FIX(2);
	}

	if (renderStates.data().getBlendAlphaDstFactor() == sf::BlendMode::Factor::One) {
		return LONG2FIX(1);
	}

	return LONG2FIX(0);
}

VALUE rb_BlendMode_Copy(VALUE self) {
	rb_raise(rb_eRGSSError, "BlendMode cannot be cloned or duplicated.");
	return self;
}

void Init_BlendMode() {
	rb_cBlendMode = rb_define_class_under(rb_mLiteRGSS, "BlendMode", rb_cObject); 
	rb_define_alloc_func(rb_cBlendMode, rb::Alloc<RenderStatesElement>);

	rb_define_method(rb_cBlendMode, "color_src_factor", _rbf rb_BlendMode_getColorSrcFactor, 0);
	rb_define_method(rb_cBlendMode, "color_src_factor=", _rbf rb_BlendMode_setColorSrcFactor, 1);
	rb_define_method(rb_cBlendMode, "color_dest_factor", _rbf rb_BlendMode_getColorDestFactor, 0);
	rb_define_method(rb_cBlendMode, "color_dest_factor=", _rbf rb_BlendMode_setColorDestFactor, 1);
	rb_define_method(rb_cBlendMode, "alpha_src_factor", _rbf rb_BlendMode_getAlphaSrcFactor, 0);
	rb_define_method(rb_cBlendMode, "alpha_src_factor=", _rbf rb_BlendMode_setAlphaSrcFactor, 1);
	rb_define_method(rb_cBlendMode, "alpha_dest_factor", _rbf rb_BlendMode_getAlphaDestFactor, 0);
	rb_define_method(rb_cBlendMode, "alpha_dest_factor=", _rbf rb_BlendMode_setAlphaDestFactor, 1);
	rb_define_method(rb_cBlendMode, "color_equation", _rbf rb_BlendMode_getColorEquation, 0);
	rb_define_method(rb_cBlendMode, "color_equation=", _rbf rb_BlendMode_setColorEquation, 1);
	rb_define_method(rb_cBlendMode, "alpha_equation", _rbf rb_BlendMode_getAlphaEquation, 0);
	rb_define_method(rb_cBlendMode, "alpha_equation=", _rbf rb_BlendMode_setAlphaEquation, 1);
	rb_define_method(rb_cBlendMode, "blend_type", _rbf rb_BlendMode_getBlendType, 0);
	rb_define_method(rb_cBlendMode, "blend_type=", _rbf rb_BlendMode_setBlendType, 1);

	rb_define_method(rb_cBlendMode, "clone", _rbf rb_BlendMode_Copy, 0);
	rb_define_method(rb_cBlendMode, "dup", _rbf rb_BlendMode_Copy, 0);

	// Equation
	rb_define_const(rb_cBlendMode, "Add", LONG2FIX(sf::BlendMode::Equation::Add));
	rb_define_const(rb_cBlendMode, "Subtract", LONG2FIX(sf::BlendMode::Equation::Subtract));
	rb_define_const(rb_cBlendMode, "ReverseSubtract", LONG2FIX(sf::BlendMode::Equation::ReverseSubtract));

	// Factor
	rb_define_const(rb_cBlendMode, "Zero", LONG2FIX(sf::BlendMode::Factor::Zero));
	rb_define_const(rb_cBlendMode, "One", LONG2FIX(sf::BlendMode::Factor::One));
	rb_define_const(rb_cBlendMode, "SrcColor", LONG2FIX(sf::BlendMode::Factor::SrcColor));
	rb_define_const(rb_cBlendMode, "OneMinusSrcColor", LONG2FIX(sf::BlendMode::Factor::OneMinusSrcColor));
	rb_define_const(rb_cBlendMode, "DstColor", LONG2FIX(sf::BlendMode::Factor::DstColor));
	rb_define_const(rb_cBlendMode, "OneMinusDstColor", LONG2FIX(sf::BlendMode::Factor::OneMinusDstColor));
	rb_define_const(rb_cBlendMode, "SrcAlpha", LONG2FIX(sf::BlendMode::Factor::SrcAlpha));
	rb_define_const(rb_cBlendMode, "OneMinusSrcAlpha", LONG2FIX(sf::BlendMode::Factor::OneMinusSrcAlpha));
	rb_define_const(rb_cBlendMode, "DstAlpha", LONG2FIX(sf::BlendMode::Factor::DstAlpha));
	rb_define_const(rb_cBlendMode, "OneMinusDstAlpha", LONG2FIX(sf::BlendMode::Factor::OneMinusDstAlpha));

}
