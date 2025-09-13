@echo off
setlocal EnableExtensions

rem ===== CONFIG =====
set "PRG=hasm"

rem ===== MSYS2 location =====
if not defined MSYS2_HOME set "MSYS2_HOME=C:\msys64"
set "MSYS2_SHELL=%MSYS2_HOME%\msys2_shell.cmd"
if not exist "%MSYS2_SHELL%" (
  echo [ERROR] MSYS2 launcher not found: "%MSYS2_SHELL%"
  echo Set MSYS2_HOME to your MSYS2 folder ^(e.g. D:\msys64^) and re-run.
  exit /b 1
)

rem ===== Parse args: action + toolchain (any order) =====
set "ACTION="
set "ENVFLAG=-mingw64"
for %%A in (%*) do (
  if /I "%%~A"=="install"   set "ACTION=install"
  if /I "%%~A"=="uninstall" set "ACTION=uninstall"
  if /I "%%~A"=="mingw64"   set "ENVFLAG=-mingw64"
  if /I "%%~A"=="ucrt64"    set "ENVFLAG=-ucrt64"
  if /I "%%~A"=="clang64"   set "ENVFLAG=-clang64"
  if /I "%%~A"=="mingw32"   set "ENVFLAG=-mingw32"
)
if not defined ACTION set "ACTION=install"

rem ===== Resolve target bin for PATH display =====
set "WIN_BINDIR="
if /I "%ENVFLAG%"=="-mingw64"  set "WIN_BINDIR=%MSYS2_HOME%\mingw64\bin"
if /I "%ENVFLAG%"=="-ucrt64"   set "WIN_BINDIR=%MSYS2_HOME%\ucrt64\bin"
if /I "%ENVFLAG%"=="-clang64"  set "WIN_BINDIR=%MSYS2_HOME%\clang64\bin"
if /I "%ENVFLAG%"=="-mingw32"  set "WIN_BINDIR=%MSYS2_HOME%\mingw32\bin"

set "CURDIR=%CD%"

echo(
echo MSYS2_HOME : %MSYS2_HOME%
echo Toolchain  : %ENVFLAG%
echo Action     : %ACTION%
echo Working dir: %CURDIR%
echo Target bin : %WIN_BINDIR%
echo(

rem ===== Compose the MSYS2 command (no logging/redirection) =====
if /I "%ACTION%"=="install" (
  rem Build -> install; print everything to console
  set "MSYS_CMD=set -euxo pipefail; make clean; make; test -x bin/%PRG%.exe; install -d $MINGW_PREFIX/bin; install -m755 bin/%PRG%.exe $MINGW_PREFIX/bin/%PRG%.exe"
) else (
  rem Uninstall; print everything to console
  set "MSYS_CMD=set -euxo pipefail; rm -f $MINGW_PREFIX/bin/%PRG%.exe"
)

rem ===== UAC prompt =====
choice /M "Elevate (UAC) before running '%ACTION%'" /C YN
if errorlevel 2 goto runNoElevate

echo [*] Elevating and running in MSYS2...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p = Start-Process -FilePath '%MSYS2_SHELL%' -ArgumentList '%ENVFLAG% -here -c ""%MSYS_CMD%""' -WorkingDirectory '%CURDIR%' -Verb RunAs -PassThru -Wait; exit $p.ExitCode"
set "RC=%ERRORLEVEL%"
goto post

:runNoElevate
echo [*] Running in MSYS2 (no elevation)...
call "%MSYS2_SHELL%" %ENVFLAG% -here -c "%MSYS_CMD%"
set "RC=%ERRORLEVEL%"

:post
if not "%RC%"=="0" (
  echo [!] %ACTION% failed. See console output above for details.
  endlocal & exit /b %RC%
)

if /I "%ACTION%"=="install" (
  rem Offer to add the toolchain bin dir to USER PATH so `myprog` works from cmd/PowerShell
  echo(
  cmd /c "echo;%PATH%;| findstr /I /C:"%WIN_BINDIR%" >nul"
  if errorlevel 1 (
    choice /M "Add %WIN_BINDIR% to your USER PATH so '%PRG%' works everywhere?" /C YN
    if not errorlevel 2 (
      powershell -NoProfile -ExecutionPolicy Bypass -Command ^
        "$p='%WIN_BINDIR%'; $u=[Environment]::GetEnvironmentVariable('Path','User'); if([string]::IsNullOrEmpty($u)){ $u='' }; if($u -notlike ('*'+$p+'*')){ [Environment]::SetEnvironmentVariable('Path', ($u + ';' + $p), 'User'); Write-Host 'Added to User PATH.' } else { Write-Host 'Already on PATH.' }"
      echo [i] Restart terminals to pick up PATH changes.
    )
  ) else (
    echo [i] Already on PATH.
  )
)

echo [*] %ACTION% completed successfully.
endlocal & exit /b 0
