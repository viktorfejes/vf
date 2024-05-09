REM Build script for tests
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (SET cFilenames=!cFilenames! %%f)

REM echo "Files:" %cFilenames%

SET assembly=tests
SET compilerFlags=-g -Wno-missing-braces -Wall -ggdb -MD
REM -Wall -Werror -save-temps=obj -O0
SET includeFlags=-Isrc -I../
SET linkerFlags=
SET defines=

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o build/%assembly%.exe %defines% %includeFlags% %linkerFlags%