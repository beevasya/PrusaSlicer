set INSTALL_CC="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
set LARGIX_PATH=C:\MyProj\TeddyTools
set PRUSA_PATH=C:\MyProj\PrusaSlicer
set PRUSA_DEP_PATH=C:\MyProj\PrusaSlicer-deps
rem set INSTALL_DIR=C:\Temp\inst
set INSTALL_DIR=G:\Shared drives\Engineering\7. Software\Installs\PrusaLargix
set PRUSA_VER=1.0.2


%INSTALL_CC% %PRUSA_PATH%\installer\LargixSlicerScript.iss

pause