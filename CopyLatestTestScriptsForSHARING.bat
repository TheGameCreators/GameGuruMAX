echo off
echo ---
echo Copy Latest Scripts For Sharing
echo ===============================
echo 1. Copy relative paths of any scripts OLDER than latest ones to the 'CopyLatestTestScriptsForSHARING.txt'" text file
echo 2. Run this 'CopyLatestTestScriptsForSHARING.bat' script to copy from latest to 'SharingScripts' folder
echo 3. Zip up the 'SharingScripts' folder and email to Workshop Contributor who has older scripts in Workshop
echo ---

echo Delete old contents from 'SharingScripts'
del /s /q "C:\DEV\GAMEGURUMAXREPO\SharingScripts\scriptbank\*.*"

echo Copy latest scripts to 'SharingScripts'
FOR /F "tokens=* delims=" %%x in (C:\DEV\GAMEGURUMAXREPO\CopyLatestTestScriptsForSHARING.txt) DO copy "C:\DEV\GAMEGURUMAXREPO\%%x" "C:\DEV\GAMEGURUMAXREPO\Sharing%%x"

cmd.exe
