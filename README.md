# Little Big Adventure 2 - Ida edition

This is a work on top of the **Little Big Adventure 2 - Engine source code - Community** and **yaz0r** fork, that makes the LBA2 community project buildable and fully playable on Windows.

## List of fixes on top of the **yaz0r** fork 
(https://github.com/yaz0r/lba2-classic-community/)

- **Added Video Playback Support**  
  Incorporated xesf's fix of video playback functionality; Implemented custom SmackerStream to address video sound issues.

- **Enabled Full Screen mode**  
  Added support for rendering to high-resolution textures

- **Restored and fixed Audio Playback in Dialogs**  
  Restored French and Spanish voices in the dialogs, that didn't work due to wrong language names encoding. Also addressed audio crackles and delays affecting dialog playback, that was caused by wrong sample count calculation.

- **Fixed Memory Leaks in Audio and Video Systems**  
  Resolved memory leak issues in the video buffer and audio playback systems, enhancing stability and performance.

- **Made Music Playback work**  
  Incorporated xesf's fix to make music work, fixed further issues

- **Improved Build Process for Debug and Release Modes**  
  Automated configuring, fixed paths and folders

- **Updated Platform Toolset to v143 (Visual Studio 2022)**  
  Migrated to the latest toolset

- **Reorganized Project Structure**  
  Separated new code into distinct Ida project

# Build and Run

## Prerequisites

- Windows 10 or later
- Visual Studio 2022, with the following components enabled:
  - Desktop development with C++ (including MSVC v143)
  - Windows application development
- Powershell
- Git

## Fetching the project

1. Git clone the lba2-classic-ida repository

1. Open the git command line in the root of your repository and run

```git submodule update --init --recursive```

This will clone the SoLoud and SDL2. Note that the modules are configured via https, so you might need to setup github access token.

## Build and run for Debug

1. Open the PowerShell command line in the root of your repository and run:

```.\configure.ps1```

1. In the dialog prompt select *Common* folder from the installation of your LBA2 game. You should use either **GoG** or **Steam** purchased version of the LBA2 classic game.

The configure command will save paths to your game assets to read in this project. It will not modify or write anything into your installed LBA2 game folders.

1. Open the **LBA2.sln** in Visual Studio 2022 and build the solution in **Debug/Win32** configuration.

1. If the build completed successfully, you can now run the game in Visual Studio. The debug session game logs, config, and save game are put in **GameRun** folder in the root of your solution.

1. By default game runs in window mode in Debug configuration. If you want to run fullscreen in Debug mode, change the *LIB386\cfg-defines.h* file and set *CFG_FULLSCREEN* to 1

## Build and run for Release

If you made some fun / mods and are ready to ship the release build, follow those steps: 

1. Open the PowerShell command line in the root of your repository and run:

```.\configure.ps1 -BuildType Release```

1. Open the **LBA2.sln** in Visual Studio 2022 and build the solution in **Release/Win32** configuration.

1. If the build completed successfully, the portable deployable files will be in the **build** folder in this solution. Those files include
- LBA2.exe
- LBA2.cfg
- SDL2.dll
- save folder
- bugs folder

All those files and folders need to be put to the root of the existing LBA2 GoG or Steam installations (to the folder where the **Common** folder is, but **NOT inside the Common folder**)

Make backup of existing files before deploying if necessary.

1. Edit LBA2.cfg file as necessary to change the default settings (for example, language, etc)


# Little Big Adventure 2 - Engine source code - Community

Little Big Adventure 2 (aka Twinsen's Odyssey) is the sequel of Little Big Adventure (aka Relentless : Twinsen's Adventure) in 1997.

We are releasing this code with preservation in mind, as this piece of work was exceptional for the time and we believe it can be a valuable source of education.

The engine uses Assembly code and was originally compiled with non-open source libraries which have been excluded from the project. 

### Licence
This source code is licensed under the [GNU General Public License](https://github.com/2point21/lba2-classic-community/blob/main/LICENSE).

Please note this license only applies to **Little Big Adventure 2** engine source code. **Little Big Adventure 2** game assets (art, models, textures, audio, etc.) are not open-source and therefore aren't redistributable.

## How can I contribute ?

Read [Contribution Guidelines](https://github.com/pleasenophp/lba2-classic-ida/blob/main/CONTRIBUTING.md).

## Links:
**Official Website:** https://www.2point21.com/games/little-big-adventure-2-twinsen-odyssey

**Discord:** https://discord.gg/wQse6WB6

**Docs:** https://lba-classic-doc.readthedocs.io/

## Buy the game:
 [[GoG]](https://www.gog.com/game/little_big_adventure_2)  [[Steam]](https://store.steampowered.com/app/398000/Little_Big_Adventure_2/)

## Original Dev Team
Direction: Frédérick Raynal

Programmers: Sébastien Viannay / Laurent Salmeron / Cédric Bermond / Frantz Cournil / Marc Bureau du Colombier

3D Artists & Animations: Paul-Henri Michaud / Arnaud Lhomme

Artists: Yaeël Barroz, Sabine Morlat, Didier Quentin

Story & Design: Frédérick Raynal / Didier Chanfray / Yaël Barroz / Laurent Salmeron / Marc Albinet

Dialogs: Marc Albinet

Story coding: Frantz Cournil / Lionel Chaze / Pascal Dubois

Video Sequences: Frédéric Taquet / Benoît Boucher / Ludovic Rubin / Merlin Pardot

Music & Sound FX: Philippe Vachey

Testing: Bruno Marion / Thomas Ferraz / Alexis Madinier / Christopher Horwood / Bertrand Fillardet

Quality Control: Emmanuel Oualid

## Copyright
The intellectual property is currently owned by [2.21]. Copyright [2.21]

Originaly developed by Adeline Software International in 1994
