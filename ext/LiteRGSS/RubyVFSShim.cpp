#ifdef LITECGSS_USE_PHYSFS

#include "RubyVFSShim.h"
#include "LiteRGSS.h"
#include "RubyValue.h"

#include <LiteCGSS/Common/VFS.h>

#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <string>

// =============================================================================
// Design notes
// -----------------------------------------------------------------------------
// Every override below has the same shape:
//
//   1. Parse path arg.
//   2. resolveVirtualPath() applies any active virtual cwd.
//   3. Branch ONCE on cgss::vfs (exists / isDirectory / Backend) — no rescue.
//   4. Either VFS-handle the call or rb_call_super to the original method.
//
// The shim is registered as a Ruby module *prepended* to File / Dir / IO and
// Kernel — that's how rb_call_super reaches the original implementation
// without any Old_* method captures.
//
// Virtual cwd state is thread_local because Ruby threads share the GVL and
// can each have their own scoped Dir.chdir(block) state. cgss::vfs itself
// stays a pure path-in/path-out API (no implicit cwd), so the C++ engine
// doesn't depend on Ruby semantics.
// =============================================================================

namespace {

	// ------------------------------------------------------------------
	// Activation state. The prepended modules are installed in the MRO
	// exactly once (Ruby has no rb_unprepend); each override checks
	// g_shim_active at the top and short-circuits to super when inactive.
	// ------------------------------------------------------------------

	bool g_modules_prepended = false;
	bool g_shim_active = false;

	#define SHIM_PASSTHROUGH_IF_INACTIVE() \
		do { if (!g_shim_active) return rb_call_super(argc, argv); } while (0)

	// ------------------------------------------------------------------
	// Helpers — small, single-purpose, used by every override.
	// ------------------------------------------------------------------

	thread_local std::string g_virtual_pwd;  // empty = inactive

	std::string toStdString(VALUE v) {
		Check_Type(v, T_STRING);
		return std::string{ RSTRING_PTR(v), static_cast<std::size_t>(RSTRING_LEN(v)) };
	}

	// Apply the virtual chdir state if the verbatim path doesn't exist on
	// the real filesystem. Mirrors what the legacy patch did with its
	// path_in_assets() helper, but in a single uniform call site.
	std::string resolveVirtualPath(VALUE rb_path) {
		std::string path = toStdString(rb_path);
		if (g_virtual_pwd.empty() || path.empty() || path[0] == '/') {
			return path;
		}
		std::error_code ec;
		if (std::filesystem::exists(path, ec)) {
			return path;
		}
		return g_virtual_pwd + "/" + path;
	}

	enum class OpenIntent { Read, Write };

	OpenIntent parseMode(VALUE rb_mode) {
		if (NIL_P(rb_mode)) return OpenIntent::Read;
		if (!RB_TYPE_P(rb_mode, T_STRING)) return OpenIntent::Read;
		const std::string m = toStdString(rb_mode);
		for (char c : m) {
			if (c == 'w' || c == 'a' || c == '+') return OpenIntent::Write;
		}
		return OpenIntent::Read;
	}

	VALUE readVfsAsRubyString(const std::string& path) {
		const auto buf = cgss::vfs::loadFully(path);
		return rb_str_new(buf.data(), static_cast<long>(buf.size()));
	}

	VALUE forceUtf8(VALUE str) {
		static ID id_force_encoding = rb_intern("force_encoding");
		rb_funcall(str, id_force_encoding, 1, rb_str_new_cstr("UTF-8"));
		return str;
	}

	// ------------------------------------------------------------------
	// File overrides — uniform shape.
	// ------------------------------------------------------------------

	VALUE rb_File_exist_q(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path; rb_scan_args(argc, argv, "1", &path);
		if (cgss::vfs::exists(resolveVirtualPath(path))) return Qtrue;
		return rb_call_super(argc, argv);
	}

	VALUE rb_File_directory_q(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path; rb_scan_args(argc, argv, "1", &path);
		if (cgss::vfs::isDirectory(resolveVirtualPath(path))) return Qtrue;
		return rb_call_super(argc, argv);
	}

	VALUE rb_File_file_q(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path; rb_scan_args(argc, argv, "1", &path);
		const auto p = resolveVirtualPath(path);
		if (cgss::vfs::exists(p) && !cgss::vfs::isDirectory(p)) return Qtrue;
		return rb_call_super(argc, argv);
	}

	VALUE rb_File_mtime(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path; rb_scan_args(argc, argv, "1", &path);
		const auto p = resolveVirtualPath(path);
		if (cgss::vfs::exists(p)) return rb_time_new(cgss::vfs::mtime(p), 0);
		return rb_call_super(argc, argv);
	}

	// File.read / binread / readlines: only the simple "read whole file" form
	// is VFS-backed. length/offset variants fall through to super.
	VALUE rb_File_read(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		if (argc != 1) return rb_call_super(argc, argv);
		const auto p = resolveVirtualPath(argv[0]);
		if (!cgss::vfs::exists(p)) return rb_call_super(argc, argv);
		return forceUtf8(readVfsAsRubyString(p));
	}

	VALUE rb_File_binread(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		if (argc != 1) return rb_call_super(argc, argv);
		const auto p = resolveVirtualPath(argv[0]);
		if (!cgss::vfs::exists(p)) return rb_call_super(argc, argv);
		return readVfsAsRubyString(p);
	}

	VALUE rb_File_readlines(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		if (argc != 1) return rb_call_super(argc, argv);
		const auto p = resolveVirtualPath(argv[0]);
		if (!cgss::vfs::exists(p)) return rb_call_super(argc, argv);
		VALUE bytes = forceUtf8(readVfsAsRubyString(p));
		return rb_funcall(bytes, rb_intern("split"), 1, rb_str_new_cstr("\n"));
	}

	struct OpenCtx { VALUE io; };
	VALUE openYieldBody(VALUE arg) {
		return rb_yield(reinterpret_cast<OpenCtx*>(arg)->io);
	}
	VALUE openCloseEnsure(VALUE arg) {
		rb_funcall(reinterpret_cast<OpenCtx*>(arg)->io, rb_intern("close"), 0);
		return Qnil;
	}

	// Read intent → StringIO over VFS bytes (or super if not in VFS).
	// Write intent → straight to super (real fopen, supports rename/fsync).
	VALUE rb_File_open(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		if (argc < 1) return rb_call_super(argc, argv);
		const VALUE mode_v = (argc >= 2) ? argv[1] : Qnil;
		if (parseMode(mode_v) == OpenIntent::Write) return rb_call_super(argc, argv);
		const auto p = resolveVirtualPath(argv[0]);
		if (!cgss::vfs::exists(p)) return rb_call_super(argc, argv);

		VALUE bytes = readVfsAsRubyString(p);
		VALUE rb_StringIO = rb_const_get(rb_cObject, rb_intern("StringIO"));
		VALUE io = rb_funcall(rb_StringIO, rb_intern("new"), 1, bytes);
		if (!rb_block_given_p()) return io;
		OpenCtx ctx{ io };
		return rb_ensure(openYieldBody, reinterpret_cast<VALUE>(&ctx),
		                 openCloseEnsure, reinterpret_cast<VALUE>(&ctx));
	}

	// File.copy_stream(src, dst): if src is in VFS, read it and write dst
	// natively. Else super (which handles real-FS to real-FS copies).
	VALUE rb_File_copy_stream(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE src, dst;
		rb_scan_args(argc, argv, "2*", &src, &dst);
		const auto sp = resolveVirtualPath(src);
		if (!cgss::vfs::exists(sp)) return rb_call_super(argc, argv);
		const auto buf = cgss::vfs::loadFully(sp);
		const auto dst_path = toStdString(dst);
		std::error_code ec;
		const auto parent = std::filesystem::path(dst_path).parent_path();
		if (!parent.empty()) std::filesystem::create_directories(parent, ec);
		std::FILE* f = std::fopen(dst_path.c_str(), "wb");
		if (!f) rb_raise(rb_eIOError, "Cannot open '%s' for writing", dst_path.c_str());
		const auto written = std::fwrite(buf.data(), 1, buf.size(), f);
		std::fclose(f);
		return LL2NUM(static_cast<long long>(written));
	}

	// ------------------------------------------------------------------
	// Dir overrides
	// ------------------------------------------------------------------

	VALUE rb_Dir_chdir(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path;
		rb_scan_args(argc, argv, "01", &path);
		if (NIL_P(path)) return rb_call_super(argc, argv);
		const std::string p = toStdString(path);
		std::error_code ec;
		if (std::filesystem::is_directory(p, ec)) return rb_call_super(argc, argv);

		// Virtual chdir — scoped if a block was given, otherwise persistent.
		const std::string old = g_virtual_pwd;
		g_virtual_pwd = p;
		if (!rb_block_given_p()) return Qnil;

		struct ChdirCtx { std::string previous; };
		ChdirCtx ctx{ old };
		return rb_ensure(
			[](VALUE) -> VALUE { return rb_yield(Qnil); }, Qnil,
			[](VALUE arg) -> VALUE {
				g_virtual_pwd = reinterpret_cast<ChdirCtx*>(arg)->previous;
				return Qnil;
			},
			reinterpret_cast<VALUE>(&ctx)
		);
	}

	// Dir.[] / Dir.glob — merge VFS results with real-FS results so callers
	// that mix archive and on-disk paths see both.
	VALUE rb_Dir_glob(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		if (argc < 1) return rb_call_super(argc, argv);
		const auto pattern = resolveVirtualPath(argv[0]);
		VALUE out = rb_ary_new();
		for (const auto& m : cgss::vfs::glob(pattern)) {
			rb_ary_push(out, rb_str_new(m.data(), static_cast<long>(m.size())));
		}
		const VALUE native = rb_call_super(argc, argv);
		if (RB_TYPE_P(native, T_ARRAY)) rb_ary_concat(out, native);
		return rb_funcall(out, rb_intern("uniq"), 0);
	}

	VALUE rb_Dir_entries(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path;
		rb_scan_args(argc, argv, "1*", &path);
		const auto p = resolveVirtualPath(path);
		if (!cgss::vfs::isDirectory(p)) return rb_call_super(argc, argv);
		const auto entries = cgss::vfs::enumerate(p);
		VALUE out = rb_ary_new_capa(static_cast<long>(entries.size()) + 2);
		rb_ary_push(out, rb_str_new_cstr("."));
		rb_ary_push(out, rb_str_new_cstr(".."));
		for (const auto& e : entries) {
			rb_ary_push(out, rb_str_new(e.data(), static_cast<long>(e.size())));
		}
		return out;
	}

	VALUE rb_Dir_exist_q(int argc, VALUE* argv, VALUE /*self*/) {
		SHIM_PASSTHROUGH_IF_INACTIVE();
		VALUE path; rb_scan_args(argc, argv, "1", &path);
		if (cgss::vfs::isDirectory(resolveVirtualPath(path))) return Qtrue;
		return rb_call_super(argc, argv);
	}

	// ------------------------------------------------------------------
	// IO override
	// ------------------------------------------------------------------

	VALUE rb_IO_copy_stream(int argc, VALUE* argv, VALUE self) {
		return rb_File_copy_stream(argc, argv, self);
	}

	// ------------------------------------------------------------------
	// Kernel#require / require_relative
	// ------------------------------------------------------------------

	VALUE callRequireSuper(VALUE name) {
		return rb_call_super(1, &name);
	}

	VALUE rb_Kernel_require(VALUE /*self*/, VALUE name) {
		if (!g_shim_active) return callRequireSuper(name);

		// Try the original loader first — it covers gems, .so, stdlib.
		int state = 0;
		const VALUE result = rb_protect(callRequireSuper, name, &state);
		if (!state) return result;
		const VALUE err = rb_errinfo();
		rb_set_errinfo(Qnil);
		if (!rb_obj_is_kind_of(err, rb_eLoadError)) rb_exc_raise(err);

		// Try VFS as the fallback. Append .rb if the caller didn't.
		std::string n = toStdString(name);
		if (n.size() < 3 || n.substr(n.size() - 3) != ".rb") n += ".rb";
		if (!cgss::vfs::exists(n)) rb_exc_raise(err);

		// Honor $LOADED_FEATURES so re-require is idempotent.
		const VALUE loaded = rb_gv_get("$LOADED_FEATURES");
		const VALUE n_val = rb_str_new(n.data(), static_cast<long>(n.size()));
		if (RTEST(rb_funcall(loaded, rb_intern("include?"), 1, n_val))) return Qfalse;

		const auto buf = cgss::vfs::loadFully(n);
		const VALUE code = rb_str_new(buf.data(), static_cast<long>(buf.size()));
		rb_ary_push(loaded, n_val);  // mark loaded BEFORE eval to prevent recursive reload
		const VALUE binding = rb_const_get(rb_cObject, rb_intern("TOPLEVEL_BINDING"));
		rb_funcall(binding, rb_intern("eval"), 2, code, n_val);
		return Qtrue;
	}

	VALUE rb_Kernel_require_relative(VALUE self, VALUE name) {
		if (!g_shim_active) return rb_call_super(1, &name);

		// Resolve relative to the caller's __FILE__ — same as Ruby's stdlib.
		const VALUE caller_loc = rb_funcall(rb_mKernel, rb_intern("caller_locations"), 2,
		                                    INT2FIX(1), INT2FIX(1));
		std::string base;
		if (RB_TYPE_P(caller_loc, T_ARRAY) && RARRAY_LEN(caller_loc) > 0) {
			const VALUE loc = RARRAY_AREF(caller_loc, 0);
			const VALUE path = rb_funcall(loc, rb_intern("absolute_path"), 0);
			if (!NIL_P(path) && RB_TYPE_P(path, T_STRING)) {
				base = std::filesystem::path(toStdString(path)).parent_path().string();
			}
		}
		const std::string rel = toStdString(name);
		const std::string full = base.empty() ? rel : (base + "/" + rel);
		const VALUE expanded = rb_str_new(full.data(), static_cast<long>(full.size()));
		return rb_Kernel_require(self, expanded);
	}

}  // namespace

// =============================================================================
// Install / activate — two-stage:
//
//   1. ensureModulesPrepended() runs ONCE on first activation. It prepends
//      our shim modules into File / Dir / IO / Kernel's MRO. This step
//      cannot be undone — Ruby has no rb_unprepend_module.
//
//   2. g_shim_active is a runtime flag flipped by Activate / Deactivate.
//      Every override checks it at the top and short-circuits to super
//      when inactive. This is the "lifecycle" knob driven by mount/unmount.
//
// Cost when inactive: one bool load + one branch + a rb_call_super that
// resolves to the stock Ruby method — i.e. exactly what would happen
// without the shim at all.
// =============================================================================

namespace {
	void ensureModulesPrepended() {
		if (g_modules_prepended) return;

		// Ensure StringIO is available for the File.open block-form path.
		rb_require("stringio");

		// File / Dir / IO are class-method overrides → prepend to the singleton class.
		VALUE m_FileShim = rb_define_module_under(rb_mLiteRGSS, "FileShim");
		rb_define_method(m_FileShim, "exist?",      _rbf rb_File_exist_q,      -1);
		rb_define_method(m_FileShim, "directory?",  _rbf rb_File_directory_q,  -1);
		rb_define_method(m_FileShim, "file?",       _rbf rb_File_file_q,       -1);
		rb_define_method(m_FileShim, "mtime",       _rbf rb_File_mtime,        -1);
		rb_define_method(m_FileShim, "read",        _rbf rb_File_read,         -1);
		rb_define_method(m_FileShim, "binread",     _rbf rb_File_binread,      -1);
		rb_define_method(m_FileShim, "readlines",   _rbf rb_File_readlines,    -1);
		rb_define_method(m_FileShim, "open",        _rbf rb_File_open,         -1);
		rb_define_method(m_FileShim, "copy_stream", _rbf rb_File_copy_stream,  -1);
		rb_prepend_module(rb_singleton_class(rb_cFile), m_FileShim);

		VALUE m_DirShim = rb_define_module_under(rb_mLiteRGSS, "DirShim");
		rb_define_method(m_DirShim, "chdir",   _rbf rb_Dir_chdir,    -1);
		rb_define_method(m_DirShim, "[]",      _rbf rb_Dir_glob,     -1);
		rb_define_method(m_DirShim, "glob",    _rbf rb_Dir_glob,     -1);
		rb_define_method(m_DirShim, "entries", _rbf rb_Dir_entries,  -1);
		rb_define_method(m_DirShim, "exist?",  _rbf rb_Dir_exist_q,  -1);
		rb_prepend_module(rb_singleton_class(rb_cDir), m_DirShim);

		VALUE m_IOShim = rb_define_module_under(rb_mLiteRGSS, "IOShim");
		rb_define_method(m_IOShim, "copy_stream", _rbf rb_IO_copy_stream, -1);
		rb_prepend_module(rb_singleton_class(rb_cIO), m_IOShim);

		// Kernel#require / require_relative are INSTANCE methods on Kernel.
		VALUE m_KernelShim = rb_define_module_under(rb_mLiteRGSS, "KernelShim");
		rb_define_method(m_KernelShim, "require",          _rbf rb_Kernel_require,          1);
		rb_define_method(m_KernelShim, "require_relative", _rbf rb_Kernel_require_relative, 1);
		rb_prepend_module(rb_mKernel, m_KernelShim);

		g_modules_prepended = true;
	}

	VALUE rb_VFS_InstallShim(VALUE /*self*/) {
		return RubyVFSShim_Activate() ? Qtrue : Qfalse;
	}

	VALUE rb_VFS_UninstallShim(VALUE /*self*/) {
		return RubyVFSShim_Deactivate() ? Qtrue : Qfalse;
	}

	VALUE rb_VFS_ShimInstalledQ(VALUE /*self*/) {
		return g_shim_active ? Qtrue : Qfalse;
	}
}

bool RubyVFSShim_Activate() {
	ensureModulesPrepended();
	if (g_shim_active) return false;
	g_shim_active = true;
	return true;
}

bool RubyVFSShim_Deactivate() {
	if (!g_shim_active) return false;
	g_shim_active = false;
	return true;
}

void Init_RubyVFSShimBindings() {
	// rb_mLiteRGSSVFS is defined by RubyVFS.cpp's Init_RubyVFS; this is called
	// after that, so the module already exists.
	extern VALUE rb_mLiteRGSSVFS;
	rb_define_module_function(rb_mLiteRGSSVFS, "install_shim!",   _rbf rb_VFS_InstallShim,     0);
	rb_define_module_function(rb_mLiteRGSSVFS, "uninstall_shim!", _rbf rb_VFS_UninstallShim,   0);
	rb_define_module_function(rb_mLiteRGSSVFS, "shim_installed?", _rbf rb_VFS_ShimInstalledQ,  0);
}

#endif // LITECGSS_USE_PHYSFS
