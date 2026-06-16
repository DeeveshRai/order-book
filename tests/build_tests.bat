@echo off
setlocal

rem Compile the tests from the tests folder. Add parent dir to include path so
rem headers like "storage/nodeStorage.h" resolve correctly.
echo Compiling tests...

g++ -std=c++17 -I.. -I. -Wall -Wextra ^
	"%~dp0tests.cpp" ^
	"%~dp0..\storage\idMap.cpp" ^
	"%~dp0..\storage\nodeStorage.cpp" ^
	"%~dp0..\storage\priceStorage.cpp" ^
	"%~dp0..\matching\matching.cpp" ^
	"%~dp0..\cancel\cancel.cpp" ^
	-o "%~dp0tests.exe"

if errorlevel 1 (
	echo Build failed.
	endlocal
	exit /b 1
)

echo Build succeeded.
echo Running tests.exe...
"%~dp0tests.exe"

endlocal
exit /b 0