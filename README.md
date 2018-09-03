omen Game Engine
================
- A hobby project for learning the OpenGL rendering pipeline
- Lauri Kortevaara (2015-2018)
- lauri.kortevaara@gmail.com
- http://github.com/lauri.kortevaara

- Clone all submodules under thirdparty
- $> git submodule init .
- $> git submodule update --recursive
============================================================
```
[submodule "thirdparty/glfw"]
	path = thirdparty/glfw
	url = https://github.com/glfw/glfw.git
	ignore = dirty
[submodule "thirdparty/glm"]
	path = thirdparty/glm
	url = https://github.com/g-truc/glm.git
	ignore = dirty
[submodule "thirdparty/stb"]
	path = thirdparty/stb
	url = https://github.com/nothings/stb.git
	ignore = dirty
[submodule "thirdparty/glew"]
	path = thirdparty/glew
	url = https://github.com/nigels-com/glew.git
	ignore = dirty
[submodule "thirdparty/boxer"]
	path = thirdparty/boxer
	url = https://github.com/aaronmjacobs/Boxer.git
	ignore = dirty
[submodule "thirdparty/googletest"]
	path = thirdparty/googletest
	url = https://github.com/google/googletest.git
	ignore = dirty
[submodule "thirdparty/freetype"]
	path = thirdparty/freetype
	url = git://git.sv.nongnu.org/freetype/freetype2.git
	ignore = dirty
[submodule "thirdparty/tinydir"]
	path = thirdparty/tinydir
	url = https://github.com/cxong/tinydir.git
	ignore = dirty
[submodule "thirdparty/bullet"]
	path = thirdparty/bullet
	url = https://github.com/bulletphysics/bullet3.git
	ignore = dirty
[submodule "thirdparty/libpng"]
	path = thirdparty/libpng
	url = git://git.code.sf.net/p/libpng/code
	ignore = dirty
[submodule "thirdparty/zlib"]
	path = thirdparty/zlib
	url = https://github.com/madler/zlib.git
	ignore = dirty
[submodule "thirdparty/lodepng"]
	path = thirdparty/lodepng
	url = https://github.com/lvandeve/lodepng.git
	ignore = dirty
[submodule "thirdparty/openvr"]
	path = thirdparty/openvr
	url = https://github.com/ValveSoftware/openvr
	ignore = dirty
[submodule "thirdparty/steam-audio"]
	path = thirdparty/steam-audio
	url = https://github.com/ValveSoftware/steam-audio
	ignore = dirty
[submodule "thirdparty/openal-soft"]
	path = thirdparty/openal-soft
	url = https://github.com/kcat/openal-soft
	ignore = dirty
```
============================================================

- Glew:
	- Download latest glew (currently 2.0.0)
	- Extract the glew zip folder under thirdparty/glew, as github repository doesn't contain source or headers at all
- ZLib:
 	- cmakelists.txt contains a zconf.h rename section which will mess up the build, so modify it not to rename the zconf.h
- LibPNG:
	- If you aren't using "configure", you may need to copy pnglibconf.h.prebuilt from the scripts directory to pnglibconf.h in your working directory. See Section III of the INSTALL file in your libpng source directory.

- FBX SDK (REQUIRED):
	- Download and install Autodesk FBX SDK from https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2019-0


Create the development and build projects with CMake
============================================================
- The cmakelists.txt is located in the root folder
- Windows & MacOS:
	- Open CMake IDE
	or
	- Open Command Line or appropriate terminal application and goto Omen repository root folder
	```
	$> mkdir build
	$> cd build
	```
	Mac & XCode
	```
	$> cmake -G Xcode ..
 	```

 	Windows & Visual Studio 15 2017
 	```
 	$> cmake -G "Visual Studio 15 2017" ..
  ```
