#ifndef Asset_H
#define Asset_H

#include <LiteCGSS/Common/Assets.h>
#include "RubyValue.h"
#include "CgssWrapper.h"

using AssetsArchiveElement = CgssInstance<cgss::AssetsArchive>;
using AssetFileElement = CgssInstance<cgss::AssetFile>;

extern VALUE rb_cAssetsArchive;
extern VALUE rb_cAssetFile;

extern VALUE rb_mAssetWriter;
void Init_Asset();

#endif