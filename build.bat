@echo off

set "vswhere_path=%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

@if not defined _echo echo off
for /f "usebackq delims=" %%i in (`"%vswhere_path%" -products * -requires Microsoft.VisualStudio.Workload.VCTools -property installationPath`) do (
  if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" (
    call "%%i\VC\Auxiliary\Build\vcvars64.bat" %*
    goto build
  )
)

echo Could not find MSVC Compiler + Libraries, please install them first: https://aka.ms/vs/17/release/vs_buildtools.exe
echo Select "Desktop Development with C++" in the installer.
echo.
pause
exit /b 1

:build
if exist "%~dp0\build" rmdir /S /Q "%~dp0\build"
mkdir "%~dp0\build\release"

pushd "%~dp0\build\release"
cmake --preset "release" -S "%~dp0." -B "%~dp0\build\release"
cmake --build "%~dp0\build\release"
tar -a -c -f "%~dp0\build\release\release-win.zip" -C "%~dp0\build\release\release-win" *

popd
echo Done!
echo.
pause