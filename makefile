all: build run

build:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -std=c++23 -O2 -Wall -o main "E:/C++/Projects/NEAT/main.cpp" -I"E:/C++/Projects/NEAT/include" -I"E:/C++/Tools/Custom_Libraries"

run:
	.\main

clean:
	rm main.o
	rm main.exe
	rm evolutionResults.txt

debug:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -std=c++23 -O2 -Wall -o main  -g "E:/C++/Projects/NEAT/main.cpp" -I"E:/C++/Projects/NEAT/include" -I"E:/C++/Tools/Custom_Libraries" -DSFML_STATIC

graphics: graphics_compile graphics_link graphics_run

graphics_compile:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -std=c++23 -O2 -Wall -c "E:/C++/Projects/NEAT/main.cpp" -I"E:\programming_tools\SFML\SFML_Sources\include" -I"E:/C++/Projects/NEAT/include" -I"E:/C++/Tools/Custom_Libraries" -DSFML_STATIC

graphics_link:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -o main "E:/C++/Projects/NEAT/main.o" -L"E:\programming_tools\SFML\SFML_Build\lib" -L"E:\programming_tools\SFML\SFML_Sources\extlibs\libs-msvc\x64" -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d -lsfml-main-d -lopengl32 -lwinmm -lgdi32 -lfreetype

graphics_run:
	.\main