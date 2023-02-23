@echo off

call g++ -I.\include -L.\lib\win\ src\a.cpp -lcurl -o a.exe
if not exist "out" mkdir out
move .\a.exe .\out\a.exe >nul
if "%1"=="--run" call .\out\a.exe
