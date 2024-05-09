@ECHO OFF
REM Build everything

ECHO "Building everything..."

REM Tests
mingw32-make -f "Makefile" all
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "Everything has been built successfully."