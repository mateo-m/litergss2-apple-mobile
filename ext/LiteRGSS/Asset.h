#ifdef LITECGSS_USE_PHYSFS

#ifndef Asset_H
#define Asset_H

// Because <windows.h>...
#undef stat

#include <LiteCGSS/Common/Assets.h>
#include "RubyValue.h"
#include "CgssWrapper.h"
#include "rbAdapter.h"

using AssetsArchiveElement = CgssInstance<cgss::AssetsArchive>;
using AssetFileElement = CgssInstance<cgss::AssetFile>;

extern VALUE rb_cAssetsArchive;
extern VALUE rb_cAssetFile;

extern VALUE rb_mAssetWriter;
void Init_Asset();

template<>
void rb::Mark<AssetsArchiveElement>(void* ptr);

template<>
void rb::Mark<AssetFileElement>(void* ptr);

#endif
#endif