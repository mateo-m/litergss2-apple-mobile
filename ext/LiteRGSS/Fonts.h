#ifndef L_FONTS_HEADER
#define L_FONTS_HEADER

#include <SFML/Graphics/Font.hpp>
#include "RubyValue.h"

VALUE rb_Fonts_get_outline_color(VALUE self, VALUE id);
VALUE rb_Fonts_get_shadow_color(VALUE self, VALUE id);
VALUE rb_Fonts_get_fill_color(VALUE self, VALUE id);
VALUE rb_Fonts_get_default_size(VALUE self, VALUE id);
sf::Font& rb_Fonts_get_font(unsigned long id);

extern VALUE rb_mFonts;

void Init_Fonts();

#endif