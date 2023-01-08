# The GameGuru MAX Repository

This is not open source and remains the copyright of The Game Creators Ltd, see Wiki for more information on license matters. This repository contains the current source code to GameGuru MAX and is still under development as an Early Access project.

![GameGuruMAX](https://fstore.thegamecreators.com/VRQuest/TGCMisc/GitHubMAXREPO.jpg)

GameGuru MAX is a 3D game maker for simple first person games and is available to buy from Steam: https://store.steampowered.com/app/1247290?utm_source=github&utm_campaign=general&utm_medium=web

You will need the media files associated with the above product to use this repository.

This source code requires Visual Studio 2022 Community Edition. Select the "Desktop Development with C++" theme. Ensure you add Visual C++ MFC for x86 and x64 to a clean install of VS2022. Also untick the Windows 11 SDKs and tick the most recent Windows 10 SDK. You can also untiuck the Boost and Google Tests. The install should be about 10GB.

Steps to compiling and running GameGuru MAX:

Compiling WICKED ENGINE:

1. Create a DEV folder somewhere close to a root drive with lots of storage
2. Use GitHub and go click the green CODE button to copy the repo URL: https://github.com/TheGameCreators/WickedRepo
3. Use GitHub Desktop to CLONE this repository and name it WICKEDREPO located in the DEV folder (i.e. D:\DEV\WICKEDREPO)
4. You will need to register a GitHub account and log in when installed GitHub Desktop
5. Select CLONE a repository from the internet and select UTL, enter the URL you copied earlier
6. Change the LOCAL PATH to your DEV folder and name this repo folder WICKEDREPO (i.e. D:\DEV\WICKEDREPO)
7. Open "D:\DEV\WICKEDREPO\WickedEngine.sln" (use VS2022) and right click 'WickedEngine_Windows' project and select "Set as Startup Project" 
8. Set the Solution Configuration from "Debug" to "ReleaseForGG". Now right click 'WickedEngine_Windows' project and select Properties
9. Ensure your project properties "Output Directory" in General looks like: "$(SolutionDir)..\GAMEGURUMAXREPO\GameGuru Core\Guru-WickedMAX\x64\Release\"
10. Right click 'WickedEngine_Windows' project and select REBUILD, it should produce a file "DEV\GAMEGURUMAXREPO\GameGuru Core\Guru-WickedMAX\x64\Release\WickedEngine_Windows.lib".
11. Close Visual Studio

Cloning ASSIMP:

1. The "GameGuru MAX Missing LIBs" zip (see below) contains the required ASSIMP.LIB file, but you will need ASSIMP repo side by side with the other repos
2. Use GitHub to get the CODE URL so you can clone the following URL: https://github.com/TheGameCreators/assimp
3. Ensure the name you give for the local folder inside DEV is ASSIMP (i.e. D:\DEV\ASSIMP). Use capitals.

Compiling GAMEGURU MAX:

1. Buy GameGuru MAX from Steam and install. Steam destioation 'MAX root' might be: "D:\SteamLibrary\steamapps\common\GameGuru MAX"
2. Copy contents of the 'MAX root' to new 'BUILD\GameGuru Wicked MAX Build Area' folder created inside DEV folder (i.e. D:\DEV\BUILD\GameGuru Wicked MAX Build Area)
3. In Windows 10, type ENV in the search box in the bottom left and hit ENTER
4. Click ENVIRONMENT VARIABLES and click NEW in the system variables section
5. Enter "GG_MAX_BUILD_PATH" for the variable name and the path for the variable value, enter the BUILD folder above (i.e. D:\DEV\BUILD\GameGuru Wicked MAX Build Area)
6. The BUILD folder might look like: "C:\DEV\BUILDS\GameGuru Wicked MAX Build Area\" (see below for understanding GG_MAX_BUILD_PATH). Click OK and OK.
7. Now to go the CODE tab above, then click the CODE button and copy the Repository URL into your clipboard
8. Install GitHub Desktop and log in with a previously registered GitHub account
9. Use GitHub Desktop to CLONE a new repository, and provide the URL you previously copied
10. Ensure folder you specify is called GAMEGURUMAXREPO, located in a folder called DEV (i.e. D:|DEV\GAMEGURUMAXREPO)
11. Ensure both GAMEGURUMAXREPO and WICKEDREPO folders are next to each other in the DEV folder and they are populated with files
12. Download the "GameGuruMAX-MissingLIBs.zip" zip and extract to copy in missing LIB files to make compiling quicker
13. When cloning complete, check to make sure you also have this file existing: "GAMEGURUMAXREPO\GameGuru Core\GameGuru\Include\assimp\config.h"
14. For the above, the "GameGuru Core" folder should overwrite the one found at: "\DEV\GAMEGURUMAXREPO\GameGuru Core"
15. Now open the project "GameGuruWickedMAX.sln" using VS2022
16. Set the Solution Configuration from "Debug" to "Release"
17. Right click the "Wicked-MAX" on the left and select "Set as Startup Project"
18. Right click "Wicked-MAX" again and select REBUILD
19. When the compiling has finished, press the "Local Windows Debugger" button at the top to run GameGuru MAX via Visual Studio


Understahding GG_MAX_BUILD_PATH:

This is what is called an environmental variable that can be read by the whole system, including VS projects. We use this to store the path to out working copy of GameGuru MAX for working on the software. We do not use the official Steam folder location as we want to keep that clean. To this end, you need to create a DEV folder to place all your many dev files, and then in there create a folder called BUILD. Inside that you can create a folder called "GameGuru Wicked MAX Build Area" and it in in here that all our MAX root files will be copied from the Steam version (which you need to do manually). The GG_MAX_BUILD_PATH parh you specify should only point to the build folder, the VS project will do the rest.

Required LIBs:

Even though you can compile these LIBs yourself, we have provided them pre-compiled to make things easier. Here is a handy link to them, download the zip, extract and copy to your project area: https://github.com/TheGameCreators/GameGuruMAX/releases/tag/VS2022

To report issues, we have consolidated issues from GameGuru Classic and GameGuru MAX into a single issues board, you can find it here: https://github.com/TheGameCreators/GameGuruRepo/issues?q=is%3Aopen+is%3Aissue+label%3AMax
