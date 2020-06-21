#ifndef Image_H
#define Image_H

#include <SFML/Graphics/Image.hpp>

#include "RubyValue.h"
extern VALUE rb_cImage;

using ImageElement = sf::Image;

void Init_Image();

#endif