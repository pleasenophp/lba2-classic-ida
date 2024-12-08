# Prepare to Debug

1. VS 2019 build tools with C++ dev enabled should be installed, if you use VS 2022 or later

1. Edit DEFINES.H to provide path to local game data files (where the HQR files are):
  For example:

  #define	PATH_RESSOURCE		"c:\\Games\\tlba2-classic\\Common\\"
  #define	PATH_JINGLE	        "C:\\Games\\tlba2-classic\\Common\\Music\\"

1. Edit DEFINES.H to provide path to the save files folders in the debug mode. Replace c:\\Projects\\lba2-classic-community with your local path to this source code.

#define	PATH_SAVE		    "c:\\Projects\\lba2-classic-community\\GameRun\\save\\"
#define	PATH_PCX_SAVE		"c:\\Projects\\lba2-classic-community\\GameRun\\save\\shoot\\"
#define	PATH_SAVE_BUGS		"c:\\Projects\\lba2-classic-community\\GameRun\\save\\bugs\\"


1. Create GameRun directory in the root of this project and create the following empty directories structure inside:
- GameRun
  - save
    - bugs
    - shoot

  This will be derictory for the save files, logs and configuration file for the development. 

1. copy SOURCES\LBA2.CFG GameRun\LBA2.CFG

1. In Visual studio LBA2 project properties go to Debugger settings and add environment variable in Environment section:
  ADELINE=<PathToThisProject>\GameRun\LBA2.CFG
  
  For example: 
    ADELINE=C:\Projects\lba2-classic-community\GameRun\LBA2.CFG

1. Build and run using Win32 configuration

1. If when running the LBA2.exe it is complaining about SDL dll not found, copy all the files from Win32\Debug to Debug\

# Current issues

- The music doesn't work
- Full screen / higher res doesn't work
- The videos don't work
- The volume and other options seem to be ignored when changing them in the options menu, even though they saved and loaded to/from config

## Minor issues
- Voice audio has clicks after every dialog in the end



# Little Big Adventure 2 - Engine source code - Community

Little Big Adventure 2 (aka Twinsen's Odyssey) is the sequel of Little Big Adventure (aka Relentless : Twinsen's Adventure) in 1997.

We are releasing this code with preservation in mind, as this piece of work was exceptional for the time and we believe it can be a valuable source of education.

The engine uses Assembly code and was originally compiled with non-open source libraries which have been excluded from the project. 

### Licence
This source code is licensed under the [GNU General Public License](https://github.com/2point21/lba2-classic-community/blob/main/LICENSE).

Please note this license only applies to **Little Big Adventure 2** engine source code. **Little Big Adventure 2** game assets (art, models, textures, audio, etc.) are not open-source and therefore aren't redistributable.

## How can I contribute ?

Read our [Contribution Guidelines](https://github.com/2point21/lba2-classic-community/blob/main/CONTRIBUTING.md).

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
