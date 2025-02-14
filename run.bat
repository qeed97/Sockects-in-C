@echo off
cl /EHsc /Fe:server main.c /link /subsystem:console ws2_32.lib

if %errorlevel% neq 0 (
    echo Compilation failed.
    pause
    exit /b 1
)

server.exe

pause
cls