@echo off
setlocal

echo Compiling rigorous order book benchmark in release mode...

g++ -O3 -DNDEBUG -std=c++17 -I.. -I. -Wall -Wextra ^
    "%~dp0order_book_benchmark.cpp" ^
    "%~dp0..\storage\idMap.cpp" ^
    "%~dp0..\storage\nodeStorage.cpp" ^
    "%~dp0..\storage\priceStorage.cpp" ^
    "%~dp0..\matching\matching.cpp" ^
    "%~dp0..\cancel\cancel.cpp" ^
    -o "%~dp0order_book_benchmark.exe"

if errorlevel 1 (
    echo Build failed.
    endlocal
    exit /b 1
)

echo Build succeeded.
endlocal
exit /b 0
