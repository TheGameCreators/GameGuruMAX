echo Once a ReleaseForSteam compile has been completed, this script
echo will copy the required minimal files to the Steam Area ready for
echo uploaded without the need to do so manually (saves 30 seconds)

echo MAX Main Contents Folder
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\changelog.txt" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\changelog.txt"
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\version.ini" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\version.ini"
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\shaders\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\shaders\"

echo MAX Executable For Steam Release
copy "D:\DEV\BUILDS\GameGuru Wicked MAX Build Area\Max\GameGuruMAX.exe" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max_steam\Max\GameGuruMAX.exe"

echo Finally, run the UPLOAD SCRIPT (saves 10 seconds)
C:
cd "Dropbox\MAXMASTER"
cd "Steam Upload"
cd "Steamworks SDK"
cd "tools"
cd "ContentBuilder"
call "UPDATE JUST MAX.bat"
