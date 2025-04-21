echo off
echo ---
echo ==========================================================
echo Script to compile MAX, Handle build files, Deploy to Steam
echo ==========================================================
echo ---

echo Ask if PULLED LATEST from GITHUB Repo?
choice /m "Have you pulled the latest from the GitHub repo?"
if errorlevel 2 (
    echo Please pull the latest changes from GitHub before continuing.
    goto :eof
)

@echo off
setlocal EnableDelayedExpansion
echo For each LUA script in GITHUB Shared Area, copy from BUILD Area in case a change has been made there..
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\ai" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\ai"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\animals" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\animals"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\effects" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\effects"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\horror" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\horror"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\huds" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\huds"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\images" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\images"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\markers" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\markers"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\objects" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\objects"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\people" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\people"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\puzzle" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\puzzle"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\rpg" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\rpg"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\user" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\user"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\scriptbank\weather" "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\weather"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\titlesbank" "D:\DEV\GAMEGURUMAXREPO\Scripts\titlesbank"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\titlesbank\default" "D:\DEV\GAMEGURUMAXREPO\Scripts\titlesbank\default"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\imagebank\crosshairs" "D:\DEV\GAMEGURUMAXREPO\Scripts\imagebank\crosshairs"
call :copyOverThisFolder "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\Files\imagebank\navbar" "D:\DEV\GAMEGURUMAXREPO\Scripts\imagebank\navbar"
echo Completed of all scripts from BUILD Area (check GITHUB Desktop for issues).
::pause

echo UseENCRYPTIONANDSECURITYCODES (secure compile begin)
echo Compile MAX ReleaseForSteam (and Sign)
echo UseNONE (secure compile ends)
echo .

@echo off
setlocal enabledelayedexpansion
echo Make a copy of EXE/PDB pair for crash report investigations
set "sourceFile=D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.exe"
for /f %%t in ('powershell -nologo -command "(Get-Item '%sourceFile%').LastWriteTime.ToString('dd-MM-yyyy-HH-mm')"') do (
    set "timestamp=%%t"
)
echo Using timestamp: %timestamp%
set "targetDir=D:\DEV\BUILDS\GameGuru Wicked MAX PDBs"
copy "%sourceFile%" "%targetDir%\GameGuruMAX_%timestamp%.exe"
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.pdb" "%targetDir%\GameGuruMAX_%timestamp%.pdb"
echo .

echo Update VERSION.INI to reflect the exact EXE/PDB pair with its correct version/date number
set "output=GameGuru MAX Build %timestamp%"
echo !output! > "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\version.ini"
echo .

echo Copy key files from GITHUB Shared Area to DEPLOY Area (content_max), including:
echo - 'Regular Scripts'
echo - 'Titlebank Scripts'
echo - 'Guides'
echo - 'Max Collection Misc'
xcopy "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\scriptbank\" /s /e /y
xcopy "D:\DEV\GAMEGURUMAXREPO\Scripts\titlesbank\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\titlesbank\" /s /e /y
xcopy "D:\DEV\GAMEGURUMAXREPO\Guides\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Guides\" /s /e /y
xcopy "D:\DEV\GAMEGURUMAXREPO\Max Collection Misc\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\entitybank\Max Collection\Misc\" /s /e /y
echo .

echo Copy key files from BUILD Area to DEPLOY Area (content_max)
xcopy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\shaders\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\shaders\" /s /e /y
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\version.ini" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\version.ini"
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\changelog.txt" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\changelog.txt"
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\WorkshopTrustedItems.ini" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\WorkshopTrustedItems.ini"
echo .

echo Copy key files from BUILD Area to DEPLOY Area (content_max_steam)
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.exe" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max_steam\Max\GameGuruMAX.exe"
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.pdb" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max_steam\Max\GameGuruMAX.pdb"
echo .

echo Run Steam deploy script to send DEPLOY Area files to Steam
C:
cd "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder"
call "UPDATE JUST MAX.bat"
echo .

echo Control back to Command Line..
echo .
cmd.exe


:: Subroutine to take two folders A and B, and copy over all files that exist in B from A
:copyOverThisFolder
setlocal EnableDelayedExpansion
set "folderA=%~1"
set "folderB=%~2"
echo Now checking %folderA%
for %%F in ("%folderA%\*") do (
	if exist %folderB%\%%~nxF (
		xcopy "%%~fF" "%folderB%\%%~nxF" /y		
	)
)
endlocal
goto :eof
