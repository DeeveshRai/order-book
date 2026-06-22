@echo off
setlocal

rem Compile the benchmarks from benchmark folder. Add parent dir to include path so
rem headers like "storage/nodeStorage.h" resolve correctly.
echo Compiling Benchmarks...

@REM g++ -std=c++17 
g++ -O3 -march=native -std=c++17 -I.. -I. -Wall -Wextra ^
	"%~dp0benchmark.cpp" ^
	"%~dp0..\storage\idMap.cpp" ^
	"%~dp0..\storage\nodeStorage.cpp" ^
	"%~dp0..\storage\priceStorage.cpp" ^
	"%~dp0..\matching\matching.cpp" ^
	"%~dp0..\cancel\cancel.cpp" ^
	-o "%~dp0benchmark.exe"

if errorlevel 1 (
	echo Build failed.
	endlocal
	exit /b 1
)

echo Build succeeded.
echo Running benchmark.exe...
"%~dp0benchmark.exe"

endlocal
exit /b 0