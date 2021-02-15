#include <SFML/Window.hpp>
#include "LiteRGSS.h"
#include "rbAdapter.h"

VALUE rb_SfSensor_isAvailable(VALUE self, VALUE val) {
  auto sensorType = NUM2LONG(val);
  if (sensorType >= sf::Sensor::Type::Count || sensorType < 0) {
    return Qfalse;
  }

  return sf::Sensor::isAvailable(static_cast<sf::Sensor::Type>(sensorType)) ? Qtrue : Qfalse;
}

VALUE rb_SfSensor_setEnabled(VALUE self, VALUE sType, VALUE enabled) {
  auto sensorType = NUM2LONG(sType);
  if (sensorType >= sf::Sensor::Type::Count || sensorType < 0) {
    return Qfalse;
  }

  sf::Sensor::setEnabled(static_cast<sf::Sensor::Type>(sensorType), RTEST(enabled));
  return self;
}

VALUE rb_SfSensor_getValue(VALUE self, VALUE sType) {
  auto sensorType = NUM2LONG(sType);
  if (sensorType >= sf::Sensor::Type::Count || sensorType < 0) {
    return Qfalse;
  }

  auto vector = sf::Sensor::getValue(static_cast<sf::Sensor::Type>(sensorType));
  auto ary = rb_ary_new_capa(3);
  rb_ary_push(ary, DBL2NUM(static_cast<double>(vector.x)));
  rb_ary_push(ary, DBL2NUM(static_cast<double>(vector.y)));
  rb_ary_push(ary, DBL2NUM(static_cast<double>(vector.z)));
  return ary;
}

void Init_SfSensor() {
  VALUE rb_mSf = rb_define_module("Sf");
  VALUE rb_mSfSensor = rb_define_module_under(rb_mSf, "Sensor");

  rb_define_const(rb_mSfSensor, "ACCELEROMETER", LONG2NUM(sf::Sensor::Type::Accelerometer));
  rb_define_const(rb_mSfSensor, "GYROSCOPE", LONG2NUM(sf::Sensor::Type::Gyroscope));
  rb_define_const(rb_mSfSensor, "MAGNETOMETER", LONG2NUM(sf::Sensor::Type::Magnetometer));
  rb_define_const(rb_mSfSensor, "GRAVITY", LONG2NUM(sf::Sensor::Type::Gravity));
  rb_define_const(rb_mSfSensor, "USER_ACCELERATION", LONG2NUM(sf::Sensor::Type::UserAcceleration));
  rb_define_const(rb_mSfSensor, "ORIENTATION", LONG2NUM(sf::Sensor::Type::Orientation));

  rb_define_module_function(rb_mSfSensor, "available?", _rbf rb_SfSensor_isAvailable, 1);
  rb_define_module_function(rb_mSfSensor, "set_enabled", _rbf rb_SfSensor_setEnabled, 2);
  rb_define_module_function(rb_mSfSensor, "value", _rbf rb_SfSensor_getValue, 1);
}
