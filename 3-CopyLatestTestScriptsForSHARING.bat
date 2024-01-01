echo off
echo ---
echo Copy Latest Scripts For Sharing
echo ===============================
echo 1. Copy relative paths of any scripts OLDER than latest ones to the '2-CopyLatestTestScriptsForSHARING.txt'" text file
echo 2. This involves checking the DIFF to make sure a newer Vx is not being replaced with an older Vx
echo 3. If this happens, use COPY RELATIVE FILE PATH then DISCARD CHANGES to ensure newer script is not overwritten and reported in TCT file
echo 4. When all diffs checked, run this '3-CopyLatestTestScriptsForSHARING.bat' script to copy from latest to 'SharingScripts' folder
echo 5. Zip up the 'SharingScripts' folder and email to Workshop Contributor who has older scripts in Workshop
echo ---

echo Delete old contents from 'SharingScripts'
del /s /q "C:\DEV\GAMEGURUMAXREPO\SharingScripts\scriptbank\*.*"

echo Copy latest scripts to 'SharingScripts'
FOR /F "tokens=* delims=" %%x in (C:\DEV\GAMEGURUMAXREPO\2-CopyLatestTestScriptsForSHARING.txt) DO copy "C:\DEV\GAMEGURUMAXREPO\%%x" "C:\DEV\GAMEGURUMAXREPO\Sharing%%x"

cmd.exe
