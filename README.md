# LiteRGSS2
## What is LiteRGSS2 ?

`LiteRGSS2` is a little Ruby Game library that reproduces some features of the RGSS. It has been written with `SFML` and only manage the Graphic part.

This is the second version of the original LiteRGSS, originally written by Nuri Yuri during his free time, that's why there is a lot of shared code between the two.
The major difference between LiteRGSS and LiteRGSS2 is the backend :
- First one (LiteRGSS) used to have directly [SFML](https://www.sfml-dev.org/index-fr.php"SFML") as the graphics library
- This one (LiteRGSS2) is based on an additional layer named [LiteCGSS](https://gitlab.com/NuriYuri/litecgss) which itself is the engine based on SFML

Therefore `LiteRGSS2` is a lot less coupled with SFML and can simply be an enhanced Ruby binding, instead of also having to contain a whole engine.

## Why using LiteRGSS2 instead of RGSS1 RGSS2 or RGSS3 ?

LiteRGSS2 is a fully OpenSource project where everyone is free to contribute. The RGSS from enterbrain is highly closed and using a very old version of Ruby.

If you use LiteRGSS you can freely include any Ruby STD library (like socket) or any Ruby Gem.

Moreover, performances are key here and we try to get LiteRGSS2 running on very low configurations for maximum portability, which was not part of the goal of the original RGSS.

## Scope of LiteRGSS2 and limitations

LiteRGSS2 doesn't gives a default Audio module, I (Nuri Yuri) consider you should be free to choose whatever you want to use to process Audio in your game. Personally I like Fmod so I made a Fmod Audio module using [RubyFmod](https://github.com/NuriYuri/Ruby-Fmod). But if you want you can also use this [SFML Audio binding](https://gitlab.com/NuriYuri/sfmlaudio).

LiteRGSS2 doesn't display text inside bitmaps. It uses a different system : the [Text](https://psdk.pokemonworkshop.fr/litergss/LiteRGSS/Text.html) object (a sprite like object but for texts) and the [Fonts](https://psdk.pokemonworkshop.fr/litergss/LiteRGSS/Fonts.html) module. See the documentation to learn more about those two things.

LiteRGSS2 doesn't allow tone and color (or wave) inside Sprites. If you want sprites with that sort of processing, use the [ShaderedSprites](https://psdk.pokemonworkshop.fr/litergss/LiteRGSS/ShaderedSprite.html). By the way, LiteRGSS2 allow Shader processing on tone (faster than in sprites) and in the Graphics module (more global), with this ability you can for example implement HqnX, ScaleX or SABR filter if you want Higher screen resolution without remaking all your ressources :)

## Differences between LiteRGSS and LiteRGSS2
- Native C++ Z-sorting (provided by LiteCGSS) exposed
	- either insertion z-sorting (object is moved when z is changed)
	- or once-per-frame z-sorting (with `sort_z`), only if required
- No more `__elementtable` field
- No more Graphics module : multiple window support
- `DisplayWindow.icon=` for changing the main game window icon
- `DisplayWindow.settings` containing the full settings of a LiteCGSS Window and `DisplayWindow.settings=` to override them

## How to install the LiteRGSS2 + LiteCGSS 32 Bits development environment on Windows, step by step

### Personal (Scorbutics) advice

I highly recommand the use of "scoop" on Windows for the install of development related tools.
scoop is a command line installer for a lot of tools listed in several repositories called "buckets".
By using scoop, you can easily get the following required tools : CMake and git.
Plus it keeps them up-to-date and automatically add them to the path.
(I promise I do not have any personal affiliation with scoop, it is just that I use it and I think it is a must-have on Windows :D)

Therefore, as there are plenty of ways / a lot of tutorial on CMake and git installations and for sake of brevity, I will only mention the installation with scoop.
(And because the install of them is a two-liner and I am lazy)

### Get the required toolchain (CMake + git + Ruby x86 Dev Kit 3.0.0 + Rake extension + SFML binaries)

__CMake__

1 - "scoop install cmake", or another method to install CMake and get it in your path.

__Git__

2 - "scoop install git", or another method to install git and get it in your path.

__Ruby__

3 - "scoop install -a 32 ruby@3.0.0", or you can do it manually for a specific ruby version by going to https://rubyinstaller.org/downloads/ and selecting Ruby + dev kit x86 3.0.0.
	Here is the direct link : https://github.com/oneclick/rubyinstaller2/releases/download/RubyInstaller-3.0.0-1/rubyinstaller-devkit-3.0.0-1-x86.exe
	Then follow the installation step.

4 - Install the ruby dev kit using "ridk install" (chose 3 and when everything is done press enter)
	As you can notice, it will download and install MSYS2.

5 - Enable the ruby dev kit using ridk enable

6 - Test your compilation toolchain access from PATH by typing "mingw32-make", if it is located, you won.

__Rake__

7 - Once you have Ruby, it is pretty straight forward : "gem install rake-compiler"
	(Again do not forget to verify that you can access it from the path, it is not mandatory but it will help you later)

__SFML__

8 - Two choices here:
- You compile SFML from the sources by using the compiler provided by the Ruby dev kit (with `ridk enable`).
After that, you can provide a "SFML_DIR" variable in your environment variables to locate the SFML root dir and make it detected by your system
- You use precompiled SFML binaries.
	- On Windows: start your msys2 executable located at C:\Ruby$RUBY_VERSION\msys32\, then type `pacman -S mingw-w64-i686-sfml` inside the bash env.
	- On Linux: `sudo apt install libsfml-dev` should do the trick
	- On MacOS: there should be some SFML dev package that you can install with `homebrew`

### Get projects from Gitlab

9 - Extract this actual repository of LiteRGSS2 with `git clone https://gitlab.com/pokemonsdk/litergss2.git` into the directory of your choice

10 - LiteRGSS2 automatically embeds LiteCGSS (and therefore skalog) as git submodule. To keep everything functional, you have to init all submodules recursively by typing
		`git submodule update --recursive --init`

### The LiteRGSS2

__Compilation__

11 - Ensure you have the Ruby dev kit toolchain by typing `ridk enable`

12 - Go back to LiteRGSS root folder.
	 First you have to configure (only the first time you build) the whole project.
	 For that, type
		`rake configure`

13 - This time we will use the ruby rake compiler toolchain, by simply typing
		`rake clean; rake compile`

14 - Copy / Paste all the DLL generated (in external/litecgss/bin) to the mingw32 binary folder in C:\Ruby$RUBY_VERSION\msys32\mingw32\bin :
- libLiteCGSS_engine.dll
- sfml-graphics-2.dll
- sfml-window-2.dll
- sfml-audio-2.dll
- sfml-network-2.dll
- sfml-system-2.dll

### Use LiteRGSS2 in PSDK

1. Download and install PSDK for Windows. I will not detail this step as it is done in other places.
2. Copy paste your own generated "LiteRGSS.so" in the PSDK lib folder.
3. Start the game by typing "ruby Game.rb".

## Specials Thanks / Maintainers
* [NuriYuri](https://gitlab.com/NuriYuri) original writer of LiteRGSS, PSDK project creator
* [Scorbutics](https://gitlab.com/Scorbutics) author of the adaptation of LiteRGSS to LiteRGSS2 + LiteCGSS

## Credits of dependencies
* The LiteCGSS project for the underlying engine https://gitlab.com/NuriYuri/litecgss mapped to SFML
* SFML for their library https://www.sfml-dev.org
* NetSurf Browser for their gif library :) https://netsurf-browser.org
* Lode Vandevenne for LodePNG
