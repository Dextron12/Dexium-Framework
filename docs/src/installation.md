\page install Release Notes

Currently, Dexium in in alpha development and there are no official releases of the library.

If you wish to tyo trial Dexium before the offical release, you can follow the build steps below

---

\section compile Building From Source

You will need the following tools/libraries to compile Dexium:
- Libraries:
	- GLFW
	- GLAD
	- STB_Image

- Tools
	- Cmake
	- Visual Studio
	- Optionally VCPKG

The provided CMakeLists.txt file includes code to search for a VCPKG root and use the libraries from there if one is configured

If you have all the required tools already installed and the libraries installed through VCPKG, you can use the install.bat