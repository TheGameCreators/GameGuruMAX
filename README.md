# The GameGuru MAX Repository

This is not open source and remains the copyright of The Game Creators Ltd, see Wiki for more information on license matters. This repository contains the current source code to GameGuru MAX and is still under development as an Early Access project.

![GameGuruMAX](https://fstore.thegamecreators.com/VRQuest/TGCMisc/GitHubMAXREPO.jpg)

GameGuru MAX is a 3D game maker for simple first person games and is available to buy from Steam: https://store.steampowered.com/app/1247290?utm_source=github&utm_campaign=general&utm_medium=web

You will need the media files associated with the above product to use this repository.

This source code requires Visual Studio 2022 Community Edition. Ensure you add Visual C++ MFC for x86 and x64 to a clean install of VS2022.

Steps to compiling and running GameGuru MAX:

Compiling WICKED ENGINE:

1. Create a DEV folder somewhere close to a root drive with lots of storage
2. Use GitHub and go to this repository, copy the URL to the clipboard: https://github.com/TheGameCreators/WickedRepo
3. Use GitHub Desktop to CLONE this repository and name it WICKEDREPO located in the DEV folder (i.e. D:\DEV\WICKEDREPO)
4. Open "WickedEngine.sln" and select "Set as Startup Project" on the 'WickedEngine_Windows' project
5. Set the Solution Configuration from "Debug" to "ReleaseForGG"
6. Compile to test, it should produce a file "DEV\GAMEGURUMAXREPO\GameGuru Core\Guru-WickedMAX\x64\Release\WickedEngine_Windows.lib".

Compiling GAMEGURU MAX:

1. Buy GameGuru MAX from Steam and install. Download VS2022 and install.
2. In Windows 10, type ENV in the search box in the bottom left and hit ENTER
3. Click ENVIRONMENT VARIABLES and click NEW in the system variables section
4. Copy contents of the MAX root (default "D:\SteamLibrary\steamapps\common\GameGuru MAX" to new BUILD folder of your choice
5. Enter "GG_MAX_BUILD_PATH" for the variable name and the path for the variable value, enter the BUILD folder of your choice
6. The BUILD folder might look like: "C:\DEV\BUILDS\GameGuru Wicked MAX Build Area\"
7. Now to go the CODE tab above, then click the CODE button and copy the Repository URL into your clipboard
8. Install GitHub Desktop and log in with a previously registered GitHub account
9. Use GitHub Desktop to CLONE a new repository, and provide the URL you previously copied
10. Ensure folder you specify is called GAMEGURUMAXREPO, located in a folder called DEV (i.e. D:|DEV\GAMEGURUMAXREPO)
11. Ensure both GAMEGURUMAXREPO and WICKEDREPO folders are next to each other in the DEV folder and they are populated with files
12. Download the "GameGuruMAX-MissingLIBs.zip" zip and extract to copy in missing LIB files to make compiling quicker
13. For the above, the "GameGuru Core" folder should overwrite the one found at: "\DEV\GAMEGURUMAXREPO\GameGuru Core"
14. Now open the project "GameGuruWickedMAX.sln" using VS2022
15. Set the Solution Configuration from "Debug" to "Release"
16. Right click the "Wicked-MAX" on the left and select "Set as Startup Project"
17. Right click "Wicked-MAX" again and select REBUILD
18. When the compiling has finished, press the "Local Windows Debugger" button at the top to run GameGuru MAX via Visual Studio

To report issues, we have consolidated issues from GameGuru Classic and GameGuru MAX into a single issues board, you can find it here: https://github.com/TheGameCreators/GameGuruRepo/issues?q=is%3Aopen+is%3Aissue+label%3AMax
