#ifndef Yuki_Gif_H
#define Yuki_Gif_H

#include <LiteCGSS/Image/YukiGif.h>
#include "RubyValue.h"
#include "CgssWrapper.h"
#include "rbAdapter.h"

extern VALUE rb_cYukiGifReader;
void Init_YukiGifReader();

using YukiGifElement = CgssInstance<cgss::YukiGif>;

template<>
void rb::Mark<YukiGifElement>(void* ptr);

#endif
