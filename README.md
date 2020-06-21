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

LiteRGSS2 doesn't gives a default Audio module, I (Nuri Yuri) consider you should be free to choose whatever you want to use to process Audio in your game. Personally I like Fmod so I made a Fmod Audio module using [RubyFmod](https://github.com/NuriYuri/Ruby-Fmod). If you want you can use an other gem or make your own Audio module using OpenAL (or SFML-Audio).

LiteRGSS2 doesn't display text inside bitmaps. It uses a different system : the [Text](https://psdk.pokemonworkshop.fr/litergss/LiteRGSS/Text.html) object (a sprite like object but for texts) and the [Fonts](https://psdk.pokemonworkshop.fr/litergss/LiteRGSS/Fonts.html) module. See the documentation to learn more about those two things.

LiteRGSS2 doesn't allow tone and color (or wave) inside Sprites. If you want sprites with that sort of processing, use the [ShaderedSprites](https://psdk.pokemonworkshop.fr/litergss/LiteRGSS/ShaderedSprite.html). By the way, LiteRGSS2 allow Shader processing on tone (faster than in sprites) and in the Graphics module (more global), with this ability you can for example implement HqnX, ScaleX or SABR filter if you want Higher screen resolution without remaking all your ressources :)

## Differences between LiteRGSS and LiteRGSS2
- Native C++ Z-sorting (provided by LiteCGSS) exposed
	- either insertion z-sorting (object is moved when z is changed)
	- or manual z-sorting (with `sort_z` on Graphics, Viewport and Window)
- No more `__elementtable` field
- `Graphics.icon=` support for changing the main game window icon

## How to install the LiteRGSS2 + LiteCGSS 32 Bits development environment on Windows, step by step

### Personal (Scorbutics) advice

I highly recommand the use of "scoop" on Windows for the install of development related tools.
scoop is a command line installer for a lot of tools listed in several repositories called "buckets".
By using scoop, you can easily get the following required tools : CMake and git.
Plus it keeps them up-to-date and automatically add them to the path.
(I promise I do not have any personal affiliation with scoop, it is just that I use it and I think it is a must-have on Windows :D)

Therefore, as there are plenty of ways / a lot of tutorial on CMake and git installations and for sake of brevity, I will only mention the installation with scoop.
(And because the install of them is a two-liner and I am lazy)

### Get the required toolchain (CMake + git + Ruby x86 Dev Kit 2.6.6 + Rake extension)

__CMake__

1 - "scoop install cmake", or another method to install CMake and get it in your path.

__Git__

2 - "scoop install git", or another method to install git and get it in your path.

__Ruby__

3 - THIS TIME DO NOT INSTALL WITH SCOOP if you are on Windows x64. It will install the Ruby x64 environment.
	Let us do it manually by going to https://rubyinstaller.org/downloads/ and selecting Ruby + dev kit x86 2.6.6.
	Then follow the installation step.

4 - Install the ruby dev kit using "ridk install" (chose 3 and when everything is done press enter)
	As you can notice, it will download and install MSYS2.

5 - Enable the ruby dev kit using ridk enable

6 - Test your compilation toolchain access from PATH by typing "mingw32-make", if it is located, you won.

__Rake__

7 - Once you have Ruby, it is pretty straight forward : "gem install rake-compiler"
	(Again do not forget to verify that you can access it from the path, it is not mandatory but it will help you later)
	
### Compilation of dependencies (SFML 2.5.1)

__SFML 2.5.1__

1 - Download SFML directly from sources, you will need to compile it with your Mingw32 x86 toolchain https://www.sfml-dev.org/download/sfml/2.5.1/index-fr.php

2 - Extract it on the folder you want to. I recommand to install it the closest to your drive letter for simplicity.
	We will take "D:\Dev\SFML-2.5.1" as an example here.

3 - Open a shell in this folder, and configure your compilation parameters with the following command line :
	`cmake -G "MinGW Makefiles" -DSFML_BUILD_EXAMPLES=False .`
	(you can leave SFML_BUILD_EXAMPLES to True, but it will take a bit longer)

4 - Type "mingw32-make", wait and contemplate your victory.

5 - Now SFML libs have been generated. Ensure that you have the DLL version of the libs, if not, you missed up a point in the process.
	If you encounter problems with the CMake configuration of SFML, an official page is dedicated to it : https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php

6 - Add the variable "SFML_DIR" to your environment. It should contain the path to the root of SFML folder, meaning for us here it is "D:\Dev\SFML-2.5.1"
	(CMake generated files are located here, and it will help your other projects to correctly find SFML)

__Get the projects from Gitlab__

7 - Now go to another place in your folder hierarchy and extract this actual repository of LiteRGSS2 with "git clone https://gitlab.com/pokemonsdk/litergss2.git"

8 - And then do it (of course in another place) for the Gitlab repository of LiteCGSS with "git clone https://gitlab.com/NuriYuri/litecgss.git"

__LiteCGSS__

9 - Then let us compile LiteCGSS first, by going to its root folder and typing :
		`cmake -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=True .`
	Ensure that you see a line mentioning something like 
		"Found SFML 2.5.1 in D:/Dev/SFML-2.5.1"
10 - Build LiteCGSS with mingw32-make. All lights are green.

### The LiteRGSS2

__Junctions and setup__

11 - First, you have to make the LiteRGSS2 to "know" the LiteCGSS. For this, the best way is to make symlinks (aka junctions on Windows) to the include and lib folders.
- Go to your LiteRGSS2 root folder and type the following, replacing PATH_TO_YOUR_LITECGSS_ROOT by your real LiteCGSS folder :
     `cmd.exe /c mklink /J LiteCGSS $PATH_TO_YOUR_LITECGSS_ROOT/src/src`
- It will create a symbolic folder named LiteCGSS and pointing to you LiteCGSS src folder.
- Then type the equivalent to link your generated LiteCGSS DLL :
     `cmd.exe /c mklink /J LiteCGSS_bin {PATH_TO_YOUR_LITECGSS_ROOT}/bin`


12 - We could do the same for SFML, but as it is a fixed version dependency, the easiest way is to copy paste generated libraries, from %SFML_DIR%\lib :
- libsfml-audio.a
- libsfml-graphics.a
- libsfml-main.a
- libsfml-network.a
- libsfml-system.a
- libsfml-window.a

To your ruby toolchain MSYS2 lib folder, located at something like C:\Ruby$RUBY_VERSION\msys32\mingw32\i686-w64-mingw32\lib, example for Ruby 2.7.X : C:\Ruby27\msys32\mingw32\i686-w64-mingw32\lib

Then you can do the same for the SFML includes, by creating a SFML folder in C:\Ruby$RUBY_VERSION\msys32\mingw32\i686-w64-mingw32\include and copy pasting the content of %SFML_DIR%\include into it.

__Compilation__

13 - Ensure you have the Ruby dev kit toolchain by typing "ridk enable"

14 - Go back to LiteRGSS root folder.
	 This time we will use the ruby rake compiler toolchain, by simply typing
		`rake clean; rake compile`

15 - Copy / Paste all the DLL generated to the mingw32 binary folder in C:\Ruby$RUBY_VERSION\msys32\mingw32\bin :
- libLiteCGSS_engine.dll
- sfml-graphics-2.dll
- sfml-window-2.dll
- sfml-audio-2.dll
- sfml-network-2.dll
- sfml-system-2.dll

### Use LiteRGSS2 in PSDK

1. Download and install PSDK for Windows. I will not detail this step as it is done in other places.
2. Copy paste your own generated "LiteRGSS.so" in the PSDK lib folder.
3. Try to start the game by typing "ruby Game.rb". If you have an error about "RubyFmod.so" that cannot load a module... then you will have to continue this tutorial.
4. We have to find and compile FMod for Windows and use it to compile our own version of RubyFmod.so, first, go to https://www.fmod.com/download
5. You need an account to download "FMOD Studio API". Once the account created, log in and download the 1.X version (not the 2.X one !). Be careful, you do not need FMod Studio Tools !
6. On Windows you have an easy to use installer. Follow the steps, it will install all headers + dll required to use Fmod as a dependency in our project.
7. Again and as we did with SFML, just copy paste the include, bin and lib folders to C:\Ruby$RUBY_VERSION\msys32\mingw32\i686-w64-mingw32.
By default, the FMOD installation folder of lowlevel api is "C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\lowlevel"
The required dll are `fmod.dll` and `fmodL.dll`
8. Now get the RubyFmod project in the folder of your choice from the command "git clone https://github.com/NuriYuri/Ruby-Fmod.git"
9. Go into this folder, and with ridk tools enabled, (type "ridk enable" if needed), type : `rake clean; rake compile`
10. Take the generated RubyFmod.so and copy / paste it to the "lib" folder of PSDK.
11. Again, try to start the game by typing "ruby Game.rb".
12. World is yours. Enjoy the start of the Game !

## Specials Thanks / Maintainers
* [NuriYuri](https://gitlab.com/NuriYuri) original writer of LiteRGSS, PSDK project creator
* [Scorbutics](https://gitlab.com/Scorbutics) author of the adaptation of LiteRGSS to LiteRGSS2 + LiteCGSS

## Credits of dependencies
* The LiteCGSS project for the underlying engine https://gitlab.com/NuriYuri/litecgss mapped to SFML
* SFML for their library https://www.sfml-dev.org
* NetSurf Browser for their gif library :) https://netsurf-browser.org
* Lode Vandevenne for LodePNG
