echo off
echo ---
echo Copy Good Scripts, Guides and Misc To Build Area
echo ================================================
echo ---

echo Copy scripts, guides and Misc assets to Steam files upload area:
xcopy "D:\DEV\GAMEGURUMAXREPO\Scripts\scriptbank\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\scriptbank\" /s /e /y
xcopy "D:\DEV\GAMEGURUMAXREPO\Scripts\titlesbank\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\titlesbank\" /s /e /y
xcopy "D:\DEV\GAMEGURUMAXREPO\Guides\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Guides\" /s /e /y
xcopy "D:\DEV\GAMEGURUMAXREPO\Max Collection Misc\*.*" "C:\Dropbox\MAXMASTER\Steam Upload\Steamworks SDK\tools\ContentBuilder\content_max\Max\Files\entitybank\Max Collection\Misc\" /s /e /y

cmd.exe
