#include "Asset.h"
#include "LiteRGSS.h"
#include "rbAdapter.h"

VALUE rb_cAssetsArchive = Qnil;
VALUE rb_cAssetFile = Qnil;

VALUE rb_mAssetWriter = Qnil;

template<>
void rb::Mark<AssetsArchiveElement>(AssetsArchiveElement* asset) {
}

VALUE rb_AssetsArchive_Dispose(VALUE self) {
	return rb::RawDispose<AssetsArchiveElement>(self);
}

VALUE rb_AssetsArchive_Initialize(int argc, VALUE *argv, VALUE self) {
	VALUE path;
	auto& assetArchive = rb::Get<AssetsArchiveElement>(self);

	rb_scan_args(argc, argv, "1", &path);
	rb_check_type(path, T_STRING);

	try {
		assetArchive.init(RSTRING_PTR(path));
	} catch (const std::runtime_error& e) {
		rb_raise(rb_eRGSSError, "%s", e.what());
	}

	return self;
}

//

template<>
void rb::Mark<AssetFileElement>(AssetFileElement* asset) {
}

VALUE rb_AssetFile_Dispose(VALUE self) {
	return rb::RawDispose<AssetFileElement>(self);
}

VALUE rb_AssetFile_Close(VALUE self) {
	auto& assetFile = rb::Get<AssetFileElement>(self);
	assetFile->close();
	return self;
}

VALUE rb_AssetFile_Exist(VALUE self, VALUE filepath) {
	rb_check_type(filepath, T_STRING);
	return cgss::AssetFile::exists(RSTRING_PTR(filepath)) ? Qtrue : Qfalse;
}

VALUE rb_AssetFile_IsDirectory(VALUE self, VALUE filepath) {
	rb_check_type(filepath, T_STRING);
	auto stat = cgss::AssetFile::stat(RSTRING_PTR(filepath));
	if (!stat.has_value()) {
		return Qfalse;
	}

	return stat->filetype == cgss::AssetFile::FileType::Directory ? Qtrue : Qfalse;
}

VALUE rb_AssetFile_IsSymbolicLink(VALUE self, VALUE filepath) {
	rb_check_type(filepath, T_STRING);
	auto stat = cgss::AssetFile::stat(RSTRING_PTR(filepath));
	if (!stat.has_value()) {
		return Qfalse;
	}

	return stat->filetype == cgss::AssetFile::FileType::Symlink ? Qtrue : Qfalse;
}

VALUE rb_AssetFile_Initialize(int argc, VALUE *argv, VALUE self) {
	VALUE path;
	VALUE mode;
	auto& assetFile = rb::Get<AssetFileElement>(self);

	rb_scan_args(argc, argv, "2", &path, &mode);
	rb_check_type(path, T_STRING);
	rb_check_type(mode, T_STRING);

	try {
		assetFile.init(RSTRING_PTR(path), RSTRING_PTR(mode));
	} catch (const std::runtime_error& e) {
		rb_raise(rb_eRGSSError, "Error when initializing asset %s with mode %s : %s", RSTRING_PTR(path), RSTRING_PTR(mode), e.what());
	}

	return self;
}

VALUE rb_AssetFile_Read(VALUE self, VALUE filepath) {
	auto& assetFile = rb::Get<AssetFileElement>(self);
	try {
		const auto& content = assetFile->fullLoad();
		return rb_str_new(content.data(), sizeof(char) * content.size());
	} catch (const std::runtime_error& e) {
		rb_raise(rb_eRGSSError, "Error when reading asset : %s", e.what());
	}
	return Qnil;
}

VALUE rb_AssetFile_Append(VALUE self, VALUE content) {
	rb_check_type(content, T_STRING);
	auto& assetFile = rb::Get<AssetFileElement>(self);
	try {
		*assetFile.instance() << RSTRING_PTR(content);
	} catch (const std::runtime_error& e) {
		rb_raise(rb_eRGSSError, "Error when appending to asset : %s", e.what());
	}
	return self;
}

VALUE rb_AssetWriter_SetWriteDir(VALUE self, VALUE directory_path) {
	rb_check_type(directory_path, T_STRING);
	if (cgss::AssetWriter::SetDirectory(RSTRING_PTR(directory_path)) == 0) {
		rb_raise(rb_eRGSSError, "Error during setting the assets write directory '%s'", RSTRING_PTR(directory_path));
	}
	return self;
}

VALUE rb_AssetFile_Enumerate(VALUE self, VALUE directory_path) {
	rb_check_type(directory_path, T_STRING);
	auto list = cgss::AssetFile::enumerate(RSTRING_PTR(directory_path));
	VALUE result = list.empty() ? rb_ary_new() : rb_ary_new_capa(list.size());
	for (const auto& file: list) {
		rb_ary_push(result, rb_str_new_cstr(file.c_str()));
	}
	return result;
}

void Init_Asset() {
	rb_cAssetsArchive = rb_define_class_under(rb_mLiteRGSS, "AssetsArchive", rb_cObject);
	rb_cAssetFile = rb_define_class_under(rb_mLiteRGSS, "AssetFile", rb_cObject);

	rb_define_alloc_func(rb_cAssetsArchive, rb::Alloc<AssetsArchiveElement>);
	rb_define_method(rb_cAssetsArchive, "initialize", _rbf rb_AssetsArchive_Initialize, -1);
	rb_define_method(rb_cAssetsArchive, "dispose", _rbf rb_AssetsArchive_Dispose, 0);

	rb_define_alloc_func(rb_cAssetFile, rb::Alloc<AssetFileElement>);
	rb_define_method(rb_cAssetFile, "initialize", _rbf rb_AssetFile_Initialize, -1);
	rb_define_method(rb_cAssetFile, "read", _rbf rb_AssetFile_Read, 0);
	rb_define_method(rb_cAssetFile, "dispose", _rbf rb_AssetFile_Dispose, 0);
	rb_define_method(rb_cAssetFile, "close", _rbf rb_AssetFile_Close, 0);
	rb_define_method(rb_cAssetFile, "<<", _rbf rb_AssetFile_Append, 1);
	rb_define_singleton_method(rb_cAssetFile, "exist?", _rbf rb_AssetFile_Exist, 1);
	rb_define_singleton_method(rb_cAssetFile, "is_directory?", _rbf rb_AssetFile_IsDirectory, 1);
	rb_define_singleton_method(rb_cAssetFile, "is_symlink?", _rbf rb_AssetFile_IsSymbolicLink, 1);
	rb_define_singleton_method(rb_cAssetFile, "enumerate", _rbf rb_AssetFile_Enumerate, 1);

	rb_mAssetWriter = rb_define_module_under(rb_mLiteRGSS, "AssetWriter");

	rb_define_module_function(rb_mAssetWriter, "write_dir=", _rbf rb_AssetWriter_SetWriteDir, 1);
}
