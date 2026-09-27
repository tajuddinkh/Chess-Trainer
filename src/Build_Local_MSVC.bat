@echo off
setlocal
cd /d "%~dp0"

echo.
echo Chess Trainer 6.0.0 - local Microsoft C++ build
echo ================================================
echo.
where cl.exe >nul 2>&1 || (echo Microsoft C++ compiler was not found.& pause & exit /b 1)
where rc.exe >nul 2>&1 || (echo Windows Resource Compiler rc.exe was not found.& pause & exit /b 1)
copy /y Chess_Trainer_V6_0_0.cpp Chess_Trainer.cpp >nul
cl.exe /nologo /O2 /Oi- /GS- /Gs9999999 /GR- /Zl /utf-8 /c Chess_Trainer.cpp /Fo:Chess_Trainer.obj
if errorlevel 1 goto :fail
rc.exe /nologo /fo Chess_Trainer.res Chess_Trainer.rc
if errorlevel 1 goto :fail
link.exe /nologo /SUBSYSTEM:WINDOWS /MACHINE:X64 /ENTRY:wWinMainCRTStartup /NODEFAULTLIB /OPT:REF /OPT:ICF /DYNAMICBASE /HIGHENTROPYVA /NXCOMPAT /STACK:1048576,131072 Chess_Trainer.obj Chess_Trainer.res kernel32.lib user32.lib gdi32.lib /OUT:Chess_Trainer.exe
if errorlevel 1 goto :fail
del /q Chess_Trainer.obj Chess_Trainer.res Chess_Trainer.cpp 2>nul
echo Build complete: %CD%\Chess_Trainer.exe
exit /b 0
:fail
echo Build failed.
exit /b 1
