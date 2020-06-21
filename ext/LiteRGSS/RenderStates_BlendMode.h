#ifndef BlendMode_H
#define BlendMode_H

#include "RubyValue.h"
#include <LiteCGSS/Graphics/RenderStates.h>

void Init_BlendMode();
extern VALUE rb_cBlendMode;


using RenderStatesElement = cgss::RenderStates;
#endif
