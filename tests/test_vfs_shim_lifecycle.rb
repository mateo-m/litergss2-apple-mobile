# Lifecycle tests for the auto-install / auto-uninstall behaviour of the
# C-extension shim — verifies that:
#
#   - requiring 'LiteRGSS' alone does NOT activate the shim (zero impact)
#   - the first LiteRGSS::VFS.mount auto-activates it
#   - the last LiteRGSS::VFS.unmount auto-deactivates it
#   - manual install_shim! / uninstall_shim! still work as overrides
#
# Each test self-resets via uninstall_shim! / unmount before asserting,
# so test ordering is irrelevant.
#
# Run with: ruby tests/test_vfs_shim_lifecycle.rb

$LOAD_PATH.unshift(File.expand_path("../tmp/x86_64-linux/stage/lib", __dir__))
$LOAD_PATH.unshift(File.expand_path("../lib", __dir__))

require "minitest/autorun"
require "tmpdir"
require "fileutils"
require "LiteRGSS"

class TestVFSShimLifecycle < Minitest::Test
  def setup
    @archive = Dir.mktmpdir("vfs_lc_")
    File.binwrite(File.join(@archive, "marker.txt"), "in_archive")
    # Reset to a known-inactive state regardless of prior test order.
    LiteRGSS::VFS.uninstall_shim!
  end

  def teardown
    begin LiteRGSS::VFS.unmount(@archive) rescue nil end
    LiteRGSS::VFS.uninstall_shim!
    FileUtils.remove_entry(@archive) if File.directory?(@archive)
  end

  def test_shim_is_inactive_with_no_mounts_and_no_explicit_install
    refute LiteRGSS::VFS.shim_installed?
    # When inactive, File.exist? must NOT see archive contents — even with
    # PhysFS still holding the data internally.
    LiteRGSS::VFS.mount(@archive)
    LiteRGSS::VFS.uninstall_shim!  # force off after auto-activate
    refute File.exist?("marker.txt"),
           "Inactive shim must short-circuit to stock File.exist?"
    assert LiteRGSS::VFS.exist?("marker.txt"),
           "VFS module itself stays functional regardless of shim state"
  end

  def test_first_mount_auto_activates_the_shim
    refute LiteRGSS::VFS.shim_installed?
    LiteRGSS::VFS.mount(@archive)
    assert LiteRGSS::VFS.shim_installed?,
           "Mount should auto-activate the shim"
    assert File.exist?("marker.txt"),
           "After auto-activate, File.exist? must see archive contents"
  end

  def test_last_unmount_auto_deactivates_the_shim
    LiteRGSS::VFS.mount(@archive)
    assert LiteRGSS::VFS.shim_installed?
    LiteRGSS::VFS.unmount(@archive)
    refute LiteRGSS::VFS.shim_installed?,
           "Last unmount should auto-deactivate the shim"
  end

  def test_intermediate_unmount_keeps_shim_active
    second = Dir.mktmpdir("vfs_lc_b_")
    File.binwrite(File.join(second, "other.txt"), "x")
    begin
      LiteRGSS::VFS.mount(@archive)
      LiteRGSS::VFS.mount(second)
      assert LiteRGSS::VFS.shim_installed?

      LiteRGSS::VFS.unmount(@archive)  # one mount remains
      assert LiteRGSS::VFS.shim_installed?,
             "Shim must stay active while at least one archive is mounted"

      LiteRGSS::VFS.unmount(second)  # last unmount
      refute LiteRGSS::VFS.shim_installed?
    ensure
      FileUtils.remove_entry(second)
    end
  end

  def test_explicit_install_shim_returns_true_first_then_false
    assert_equal true,  LiteRGSS::VFS.install_shim!, "first activates"
    assert_equal false, LiteRGSS::VFS.install_shim!, "second is a no-op"
  end

  def test_explicit_uninstall_shim_force_disables_even_with_mounts_active
    LiteRGSS::VFS.mount(@archive)
    assert LiteRGSS::VFS.shim_installed?
    assert_equal true, LiteRGSS::VFS.uninstall_shim!,
                 "Force-disable must work even with an active mount"
    refute LiteRGSS::VFS.shim_installed?
    refute File.exist?("marker.txt"),
           "Stock File.exist? semantics restored despite mount being live"
  end
end
