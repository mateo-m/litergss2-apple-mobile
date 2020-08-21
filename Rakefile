ext_name = 'LiteRGSS'
require 'rake/extensiontask'

spec = Gem::Specification.new do |s|
  s.name = ext_name
  s.platform = Gem::Platform::RUBY
  s.version = '2.0.0-dev'
  s.summary = "LiteRGSS2 library, make games with Ruby easily"
  s.description = "LiteRGSS2 stands for Lite Ruby Game Scripting System and is an enhanced open-source version of the original RGSS written by Enterbrain."
  s.authors = ["Nuri Yuri", "Scorbutics", "SuperFola", "nthoang-apcs"]
  s.homepage = "https://psdk.pokemonworkshop.fr/litergss"
  s.extensions = FileList["ext/LiteRGSS/extconf.rb"]
  s.files = FileList["ext/LiteRGSS/*.h", "ext/LiteRGSS/*.cpp", "ext/LiteRGSS/*.hpp", "ext/LiteRGSS/*.c"]
  s.metadata    = { "source_code_uri" => "https://gitlab.com/pokemonsdk/litergss2", "documentation_uri" => "https://psdk.pokemonworkshop.fr/litergss" }
  s.required_ruby_version = '>= 2.6.6'
end

Gem::PackageTask.new(spec) do |pkg|
end

desc "Configure the project by checking / setting up a working environment"
task :configure do
  require_relative 'external/litecgss/build/system_env'
  require_relative 'external/litecgss/build/ruby_installer'

  ridk_script = Cgss::RubyInstaller.ridk_guard()
  Cgss::RubyInstaller.ridk_enable(ridk_script)

  litecgss_root_dir = File.expand_path(File.dirname(__FILE__)) + "/external/litecgss"
  Dir.chdir(litecgss_root_dir) {
    system("rake configure")
    system("rake clean")
    system("rake compile")
    if !Cgss::SystemEnv::is_windows()
      # Install LiteCGSS on system
      system("ln -sf #{litecgss_root_dir}/lib/*.so* /usr/lib/")

      have_library("LiteCGSS_engine") or fail "Unable to find LiteCGSS library. Clean everything and try again."
    end
  }
end

namespace :test do
  # partial-loads-ok and undef-value-errors necessary to ignore
  # spurious (and eminently ignorable) warnings from the ruby
  # interpreter
  VALGRIND_BASIC_OPTS = "--num-callers=50 --error-limit=no \
                          --partial-loads-ok=yes --undef-value-errors=no"
  COMMAND = ENV['cmd']
  desc "run #{COMMAND} under valgrind with basic ruby options"
  task :valgrind do
      cmdline = "valgrind #{VALGRIND_BASIC_OPTS} ruby #{COMMAND}"
      puts cmdline
      system cmdline
  end
end

namespace :test do
  desc "run #{COMMAND} under gdb"
  task :gdb do
      system "gdb --args ruby #{COMMAND}"
  end

  desc "start unit-test"
  task :unit do
      system "rspec unit-test/*.rb"
  end
end

Rake::ExtensionTask.new(ext_name, spec) do |ext|
  # cross compilation requires a cross compile toolchain
  ext.cross_compile = true
  ext.cross_platform = ['x86-mingw32']
end
