rem all filenames must not contain spaces
@echo off
rem on the next line, you can specify the path to the prusa-slicer-console.exe file, 
rem if it is not specified in the PATH environment variable
set PRUSA_CONSOLE_PATH=C:\src\PrusaSlicer\build\src\Release
set CSVEXT=.csv
if "%~2"=="" (
	echo Usage: clitool file_with_models_list file_with_settings_list "prusa_options"
	echo Example: clitool models.txt settings.txt "--center 40,40"
	echo Warning: All filenames must be without spaces
	echo The path to the prusa-slicer-console.exe file must be specified in the PATH variable
	exit /b 0
)
for /f "delims=" %%I in (%2) do (
	for /f "delims=" %%F in (%1) do (
		echo.
		echo Processing %%F with settings from %%I ...
		if exist %PRUSA_CONSOLE_PATH%\prusa-slicer-console.exe (
			%PRUSA_CONSOLE_PATH%\prusa-slicer-console.exe  %~3 -g %%F --load %%I -o output.gcode >nul
		) else (
			prusa-slicer-console.exe  %~3 -g %%F --load %%I -o output.gcode >nul
		)

		if ERRORLEVEL 1 (
			echo Error converting %%F
		) else (
			echo Ok
			if exist output.csv (
				if exist %%~nF_%%~nI%CSVEXT% (
					del %%~nF_%%~nI%CSVEXT%
				)
				rem ren %%~nF%CSVEXT% %%~nF_%%~nI%CSVEXT%
				ren output.csv %%~nF_%%~nI%CSVEXT%
			)
		)
	)
)
if exist output.gcode del output.gcode
if exist output.csv del output.csv