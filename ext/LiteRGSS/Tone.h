#ifndef Tone_H
#define Tone_H

#include <SFML/Graphics/Glsl.hpp>
#include <LiteCGSS/Common/BondElement.h>
#include "RubyValue.h"

extern VALUE rb_cTone;
void Init_Tone();

using ToneElement = cgss::BondElement<sf::Glsl::Vec4>;

#endif