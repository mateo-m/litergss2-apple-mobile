#include <SFML/Window.hpp>
#include "LiteRGSS.h"
#include "rbAdapter.h"

VALUE rb_SfMouse_press(VALUE self, VALUE button) {
	auto vbutton = NUM2LONG(button);

	return sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(vbutton)) ? Qtrue : Qfalse;
}

VALUE rb_SfMouse_getPosition(VALUE self) {
	auto position = sf::Mouse::getPosition();

	auto ary = rb_ary_new_capa(2);
	rb_ary_push(ary, INT2NUM(position.x));
	rb_ary_push(ary, INT2NUM(position.y));

	return ary;
}

VALUE rb_SfMouse_setPosition(VALUE self, VALUE x, VALUE y) {
	sf::Vector2i pos = sf::Vector2i(NUM2INT(x), NUM2INT(y));
	sf::Mouse::setPosition(pos);

	return self;
}

void Init_SfMouse() {
	VALUE rb_mSf = rb_define_module("Sf");
	VALUE rb_mSfMouse = rb_define_module_under(rb_mSf, "Mouse");

	rb_define_const(rb_mSfMouse, "Left", LONG2NUM(sf::Mouse::Button::Left));
	rb_define_const(rb_mSfMouse, "LEFT", LONG2NUM(sf::Mouse::Button::Left));
	rb_define_const(rb_mSfMouse, "Right", LONG2NUM(sf::Mouse::Button::Right));
	rb_define_const(rb_mSfMouse, "RIGHT", LONG2NUM(sf::Mouse::Button::Right));
	rb_define_const(rb_mSfMouse, "Middle", LONG2NUM(sf::Mouse::Button::Middle));
	rb_define_const(rb_mSfMouse, "XButton1", LONG2NUM(sf::Mouse::Button::XButton1));
	rb_define_const(rb_mSfMouse, "XButton2", LONG2NUM(sf::Mouse::Button::XButton2));
	rb_define_const(rb_mSfMouse, "VerticalWheel", LONG2NUM(sf::Mouse::Wheel::VerticalWheel));
	rb_define_const(rb_mSfMouse, "HorizontalWheel", LONG2NUM(sf::Mouse::Wheel::HorizontalWheel));

	rb_define_module_function(rb_mSfMouse, "press?", _rbf rb_SfMouse_press, 1);
	rb_define_module_function(rb_mSfMouse, "position", _rbf rb_SfMouse_getPosition, 0);
	rb_define_module_function(rb_mSfMouse, "set_position", _rbf rb_SfMouse_setPosition, 2);
}
