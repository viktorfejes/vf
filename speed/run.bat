@ECHO "Building file..."
clang speed.c -Wall -Wextra -Werror -pedantic -O3 -o speed.exe

@ECHO "Running file..."
speed.exe
