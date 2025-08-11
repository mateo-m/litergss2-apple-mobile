require 'mkmf'
ext_name = 'LiteRGSS'

litecgss_root_dir = File.expand_path(File.dirname(__FILE__) + "/../../external/litecgss")

# add include path to the internal folder
# $(srcdir) is a root folder, where "extconf.rb" is stored
$INCFLAGS << " -I/usr/include/LiteCGSS/ -I'$(srcdir)/../../' -I'" + litecgss_root_dir + "/src/src' "
$LDFLAGS << " -L'" + litecgss_root_dir + "/bin' -L'" + litecgss_root_dir + "/lib' " + "-L/usr/i686-w64-mingw32/lib/ "

sfml_dir_env = ENV["SFML_DIR"]
if sfml_dir_env.nil?
    sfml_dir_env = litecgss_root_dir + "/external/sfml"
    puts "SFML not found from SFML_DIR, adding LD flags for : #{sfml_dir_env}"
else
    puts "SFML found from SFML_DIR (#{sfml_dir_env})"
end

if !sfml_dir_env.nil? && !sfml_dir_env.empty?
  $INCFLAGS << " -I'" + (sfml_dir_env + "/include'")
  $LDFLAGS << " -L'" + (sfml_dir_env + "/lib'")
end

find_library('sfml-graphics', nil, sfml_dir_env + "/include", sfml_dir_env + "/lib")

if enable_config('sfml-debug')
  have_library('sfml-graphics-d')
  have_library('sfml-window-d')
  have_library('sfml-system-d')
else
  have_library('sfml-graphics-d') or have_library('sfml-graphics')
  have_library('sfml-window-d') or have_library('sfml-window')
  have_library('sfml-system-d') or have_library('sfml-system')
end

dir_config('skalog', litecgss_root_dir + '/external/skalog/src/src', litecgss_root_dir + '/lib')
have_library('skalog') or fail "Unable to find skalog library. Build the LiteCGSS to build it."

if enable_config('physfs')
  if !(find_library('physfs', nil, litecgss_root_dir + "/_deps/physfs-src/src", litecgss_root_dir + '/lib') || have_library('physfs'))
    fail "Unable to find PhysFS library. Build the LiteCGSS to build it."
  end
  $CXXFLAGS += " -DLITECGSS_USE_PHYSFS "
else
  puts "PhysFS is disabled"
end

dir_config('LiteCGSS_engine', litecgss_root_dir + '/src/src', litecgss_root_dir + '/lib')
have_library('LiteCGSS_engine') or fail "Unable to find LiteCGSS library. Build it inside 'external/litecgss' first."

def is_clang_compiler()
    CONFIG['CXX'] =~ /clang/
end

puts "C++ Compiler is #{CONFIG['CXX']}"

$CXXFLAGS += " -frelaxed-template-template-args -fdeclspec " if is_clang_compiler()
$CXXFLAGS += " -std=c++17 -Wall "

# override normal build configuration to build debug friendly library
# if installed via 'gem install oops-null -- --enable-debug'
if enable_config('debug')
    puts '[INFO] enabling debug library build configuration.'
    if RUBY_VERSION < '1.9'
        $CFLAGS = CONFIG['CFLAGS'].gsub(/\s\-O\d?\s/, ' -O0 ')
        $CFLAGS.gsub!(/\s?\-g\w*\s/, ' -ggdb3 ')
        CONFIG['LDSHARED'] = CONFIG['LDSHARED'].gsub(/\s\-s(\s|\z)/, ' ')
    else
        CONFIG['debugflags'] << ' -ggdb3 -O0'
        CONFIG['optflags'] = '-O0 -fno-omit-frame-pointer'
    end
end

create_makefile(ext_name)
