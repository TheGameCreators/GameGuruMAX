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

echo Assumes GITHUB repo latest on local machine, including:
echo - 'Guides'
echo - 'Max Collection Misc'
echo - 'Scripts'
echo xcopy "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\scriptbank\" /s /e /y
echo xcopy "D:\DEV\GAMEGURUMAXREPO\Scripts\titlesbank\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\titlesbank\" /s /e /y
echo xcopy "D:\DEV\GAMEGURUMAXREPO\Guides\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Guides\" /s /e /y
echo xcopy "D:\DEV\GAMEGURUMAXREPO\Max Collection Misc\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\entitybank\Max Collection\Misc\" /s /e /y
echo .

echo Copy key files from BUILD Area to GITHUB Share Folders (scripts, assets)
echo TODO
echo .

echo Check for conflicts against GITHUB Repo?
echo TODO
echo .

echo UseENCRYPTIONANDSECURITYCODES (secure compile begin)
echo TODO
echo .

echo Compile MAX ReleaseForSteam (and Sign)
echo TODO
echo .

echo UseNONE (secure compile ends)
echo TODO
echo .

echo Copy key files from BUILD Area to DEPLOY Area
echo xcopy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.exe" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max_steam\Max\GameGuruMAX.exe" /y
echo xcopy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.pdb" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max_steam\Max\GameGuruMAX.pdb" /y
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


echo Run Steam deploy script to send DEPLOY Area files to Steam
echo TODO
echo .

echo Control back to Command Line..
echo .
cmd.exe
