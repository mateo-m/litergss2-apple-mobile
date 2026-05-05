#ifdef LITECGSS_USE_PHYSFS

#ifndef RUBY_VFS_H
#define RUBY_VFS_H

// Because <windows.h>...
#undef stat

#include "RubyValue.h"

extern VALUE rb_mLiteRGSSVFS;

void Init_RubyVFS();

#endif
#endif
