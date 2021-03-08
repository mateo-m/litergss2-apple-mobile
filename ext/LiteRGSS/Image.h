#ifndef Image_H
#define Image_H

#include <LiteCGSS/Image/Image.h>
#include "RubyValue.h"
#include "CgssWrapper.h"

extern VALUE rb_cImage;
using ImageElement = CgssInstance<cgss::Image>;
void Init_Image();

#endif