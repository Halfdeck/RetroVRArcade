RetroVRArcade

- Built using Unreal Engine 4.27
- The app currently runs using OpenXR. This was tested using Windows 10.

Installation for Developers

- Pull the repo from git
- Open the project in Unreal Engine by going to the project directory and opening RetroVRArcade.uproject
- Find mame2010_libretro.DLL from the internet and copy into \RetroVRArcade\Plugins\libretro folder
- Find gauntlet.zip (gauntlet ROM) from the internet and copy into \RetroVRArcade\Content\Roms folder
- Hit Play in Unreal

Explanation of Unreal Files

- Project files are in 01_RetroVRArcade folder
- The GauntletBluePrint blue print runs the MAME ROM
- LibretroGame.cpp in Source folder contains the C++ code for working with MAME




