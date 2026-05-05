#ifndef Color_H
#define Color_H

#include <SFML/Graphics/Color.hpp>
#include <LiteCGSS/Common/BondElement.h>
#include "RubyValue.h"
#include "rbAdapter.h"

void Init_Color();
extern VALUE rb_cColor;

struct ColorElement :
	public cgss::BondElement<sf::Color> {

	void setR(uint8_t r) {
		edit([&r](sf::Color& color) {
			color.r = r;
		});
	}

	void setG(uint8_t g) {
		edit([&g](sf::Color& color) {
			color.g = g;
		});
	}

	void setB(uint8_t b) {
		edit([&b](sf::Color& color) {
			color.b = b;
		});
	}

	void setA(uint8_t a) {
		edit([&a](sf::Color& color) {
			color.a = a;
		});
	}

};

template<>
void rb::Mark<ColorElement>(void* ptr);

#endif