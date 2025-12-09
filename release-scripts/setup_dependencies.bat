@echo off

set APP_EXE=steam-achievement-tracker.exe
set QT_VERSION=6.10.1
set QT_ARCH=msvc2022_64
set APP_DIR=

if not exist "%~dp0\%APP_EXE%" (
  echo Error: Wrong location - Please put this script next to the app executable and run it again.
  pause
  exit /b 1
)

set APP_DIR=%~dp0

echo Downloading aqt...
curl -L -o "%APP_DIR%\aqtinstall.exe" "https://github.com/miurahr/aqtinstall/releases/download/v3.3.0/aqt.exe" 2>nul
if not exist "%APP_DIR%\aqtinstall.exe" (
  echo Error: Failed to download aqt from 'https://github.com/miurahr/aqtinstall/releases/download/v3.3.0/aqt.exe'...
  pause
  exit /b 1
)

echo Downloading qt binaries (this might take a while)...
"%APP_DIR%\aqtinstall.exe" "install-qt" "windows" "desktop" "%QT_VERSION%" "win64_%QT_ARCH%" "--outputdir" "%APP_DIR%\qt_tmp" "--archives" "qtbase" 2>nul
if not exist "%APP_DIR%\qt_tmp\%QT_VERSION%\%QT_ARCH%" (
  echo Error: Failed to download qt binaries...
  pause
  exit /b 1
)

echo Copying files...
xcopy /Y /-I /Q "%APP_DIR%\qt_tmp\%QT_VERSION%\%QT_ARCH%\bin\Qt6Core.dll" "%APP_DIR%\Qt6Core.dll" >nul
xcopy /Y /-I /Q "%APP_DIR%\qt_tmp\%QT_VERSION%\%QT_ARCH%\bin\Qt6Gui.dll" "%APP_DIR%\Qt6Gui.dll" >nul
xcopy /Y /-I /Q "%APP_DIR%\qt_tmp\%QT_VERSION%\%QT_ARCH%\bin\Qt6Widgets.dll" "%APP_DIR%\Qt6Widgets.dll" >nul
mkdir "%APP_DIR%\plugins\platforms"
mkdir "%APP_DIR%\plugins\imageformats"
xcopy /Y /-I /Q "%APP_DIR%\qt_tmp\%QT_VERSION%\%QT_ARCH%\plugins\platforms\qwindows.dll" "%APP_DIR%\plugins\platforms\qwindows.dll" >nul
xcopy /Y /-I /Q "%APP_DIR%\qt_tmp\%QT_VERSION%\%QT_ARCH%\plugins\imageformats\qjpeg.dll" "%APP_DIR%\plugins\imageformats\qjpeg.dll" >nul

echo Removing temp files...
rmdir /S /Q "%APP_DIR%\qt_tmp" 2>nul
del "%APP_DIR%\aqtinstall.log" 2>nul
del "%APP_DIR%\aqtinstall.exe" 2>nul

echo Done!
pause