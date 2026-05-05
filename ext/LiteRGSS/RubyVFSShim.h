#ifdef LITECGSS_USE_PHYSFS

#ifndef RUBY_VFS_SHIM_H
#define RUBY_VFS_SHIM_H

// Because <windows.h>...
#undef stat

#include "RubyValue.h"

// Defines LiteRGSS::VFS.install_shim! / .uninstall_shim! / .shim_installed?
// — these toggle the C-implemented overrides for File / Dir / IO and
// Kernel#require that re-route filesystem calls through cgss::vfs.
//
// Lifecycle:
//   - Requiring 'LiteRGSS' alone has zero effect on File/Dir behaviour.
//   - The first LiteRGSS::VFS.mount() auto-activates the shim.
//   - The last LiteRGSS::VFS.unmount() auto-deactivates it.
//   - install_shim! / uninstall_shim! are explicit overrides callable any
//     time (useful for tests, or for tools that want the shim before any
//     mount, or that want to force-disable it while mounts remain).
//
// Implementation note: Ruby's prepended-module chain is append-only — there
// is no rb_unprepend_module. Activation prepends once on first install;
// subsequent toggles flip a boolean that every override checks at the top
// (one branch + rb_call_super when inactive, which is exactly the stock
// Ruby path). Cost when inactive is invisible in real workloads.
void Init_RubyVFSShimBindings();

// C++ surface for the mount/unmount lifecycle hooks in RubyVFS.cpp.
// Returns true on state transition, false if already in the requested state.
bool RubyVFSShim_Activate();
bool RubyVFSShim_Deactivate();

#endif
#endif
