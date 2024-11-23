CC = zig c++
F = -std=c++23 src/main.cpp -o bin/c2cpp.exe
more_Flags = -target x86-windows-gnu

#small:
#	${CC} -Oz -fdata-sections -ffunction-sections -fvisibility=hidden -fno-exceptions -fno-rtti -march=native ${F} ${more_Flags}
#	strip --strip-all bin/c2cpp.exe

default:
	${CC} -O4 -fdata-sections -ffunction-sections -fvisibility=hidden -fno-exceptions -fno-rtti -march=native -fno-math-errno -ffast-math -funroll-loops ${F} ${more_Flags}
	strip --strip-all bin/c2cpp.exe
