@echo off

set file_long=%1
set file=%file_long:~6,-4%.exe
call g++ -I.\include -L.\lib\win\ %1 -lcurl -o %file%
if not exist "out" mkdir out
move .\%file% .\out\%file% >nul
if "%2"=="--run" call .\out\%file%
