TODO correct version based on makefile


PATH=C:\raylib\w64devkit\bin;%PATH%
del .\tool.exe
like this:         gcc -gdwarf -o tool.exe *.c  C:\raylib\raylib\src\raylib.rc.data -static -O2 -std=c99 -Wall -IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -lraylib -lopengl32 -lgdi32 -lwinmm -lm
.\tool.exe
pause
