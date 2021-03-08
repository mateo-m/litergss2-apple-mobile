#include <SFML/Window.hpp>
#include "LiteRGSS.h"
#include "rbAdapter.h"

VALUE rb_SfJoystick_connected(VALUE self, VALUE id) {
	return sf::Joystick::isConnected(NUM2UINT(id)) ? Qtrue : Qfalse;
}

VALUE rb_SfJoystick_buttonCount(VALUE self, VALUE id) {
	auto count = sf::Joystick::getButtonCount(NUM2UINT(id));
	return UINT2NUM(count);
}

VALUE rb_SfJoystick_axis(VALUE self, VALUE id, VALUE axis) {
	auto vaxis = NUM2LONG(axis);

	return sf::Joystick::hasAxis(NUM2UINT(id), static_cast<sf::Joystick::Axis>(vaxis)) ? Qtrue : Qfalse;
}

VALUE rb_SfJoystick_press(VALUE self, VALUE id, VALUE button) {
	return sf::Joystick::isButtonPressed(NUM2UINT(id), NUM2UINT(button)) ? Qtrue : Qfalse;
}

VALUE rb_SfJoystick_axis_position(VALUE self, VALUE id, VALUE axis) {
	auto vaxis = NUM2LONG(axis);
	auto position = sf::Joystick::getAxisPosition(NUM2UINT(id), static_cast<sf::Joystick::Axis>(vaxis));

	return DBL2NUM(static_cast<double>(position));
}

VALUE rb_SfJoystick_update(VALUE self) {
	sf::Joystick::update();

	return self;
}

VALUE rb_SfJoyStick_identification(VALUE self, VALUE id) {
	auto identity = sf::Joystick::getIdentification(NUM2UINT(id));
	auto ary = rb_ary_new_capa(3);
	VALUE str = rb_utf8_str_new_cstr((char*)identity.name.toUtf8().c_str());
	rb_ary_push(ary, str);
	rb_ary_push(ary, UINT2NUM(identity.vendorId));
	rb_ary_push(ary, UINT2NUM(identity.productId));

	return ary;
}

void Init_SfJoystick() {
	VALUE rb_mSf = rb_define_module("Sf");
	VALUE rb_mSfJoystick = rb_define_module_under(rb_mSf, "Joystick");

	rb_define_const(rb_mSfJoystick, "COUNT", LONG2NUM(sf::Joystick::Count));
	rb_define_const(rb_mSfJoystick, "BUTTON_COUNT", LONG2NUM(sf::Joystick::ButtonCount));
	rb_define_const(rb_mSfJoystick, "AXIS_COUNT", LONG2NUM(sf::Joystick::AxisCount));
	rb_define_const(rb_mSfJoystick, "X", LONG2NUM(sf::Joystick::Axis::X));
	rb_define_const(rb_mSfJoystick, "Y", LONG2NUM(sf::Joystick::Axis::Y));
	rb_define_const(rb_mSfJoystick, "Z", LONG2NUM(sf::Joystick::Axis::Z));
	rb_define_const(rb_mSfJoystick, "R", LONG2NUM(sf::Joystick::Axis::R));
	rb_define_const(rb_mSfJoystick, "U", LONG2NUM(sf::Joystick::Axis::U));
	rb_define_const(rb_mSfJoystick, "V", LONG2NUM(sf::Joystick::Axis::V));
	rb_define_const(rb_mSfJoystick, "POV_X", LONG2NUM(sf::Joystick::Axis::PovX));
	rb_define_const(rb_mSfJoystick, "POV_Y", LONG2NUM(sf::Joystick::Axis::PovY));

	rb_define_module_function(rb_mSfJoystick, "connected?", _rbf rb_SfJoystick_connected, 1);
	rb_define_module_function(rb_mSfJoystick, "button_count", _rbf rb_SfJoystick_buttonCount, 1);
	rb_define_module_function(rb_mSfJoystick, "axis_available?", _rbf rb_SfJoystick_axis, 2);
	rb_define_module_function(rb_mSfJoystick, "press?", _rbf rb_SfJoystick_press, 2);
	rb_define_module_function(rb_mSfJoystick, "axis_position", _rbf rb_SfJoystick_axis_position, 2);
	rb_define_module_function(rb_mSfJoystick, "update", _rbf rb_SfJoystick_update, 0);
	rb_define_module_function(rb_mSfJoystick, "identification", _rbf rb_SfJoyStick_identification, 1);
}
