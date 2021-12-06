set LARGIX_PATH=C:\MyProj\TeddyTools
set PRUSA_PATH=C:\MyProj\PrusaSlicer

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

msbuild %PRUSA_PATH%\build\PrusaSlicer.sln /target:build /maxcpucount:4 /p:Configuration=Release /p:Platform="x64"
IF %ERRORLEVEL% NEQ 0 goto Done

msbuild %LARGIX_PATH%\PathNavigator\PathNavigator.sln /target:build /maxcpucount:4 /p:Configuration=Release /p:Platform="x64"
IF %ERRORLEVEL% NEQ 0 goto Done

msbuild %LARGIX_PATH%\ProgramBuilder\ProgramBuilder.sln /target:build /maxcpucount:4 /p:Configuration=Release /p:Platform="x64"
IF %ERRORLEVEL% NEQ 0 goto Done 

call buildInstaller.bat

:Done
pause
