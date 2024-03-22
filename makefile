all: build run

build:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -std=c++23 -O2 -Wall -o main "E:/C++/Projects/NEAT/main.cpp" -I"E:/C++/Projects/NEAT/include" -I"E:/C++/Tools/Custom_Libraries"

run:
	.\main

clean:
	rm main.exe
	rm evolutionResults.txt

debug:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -std=c++23 -O2 -Wall -o main  -g "E:/C++/Projects/NEAT/main.cpp" -I"E:/C++/Projects/NEAT/include" -I"E:/C++/Tools/Custom_Libraries" -DSFML_STATIC

graphics: graphics_compile graphics_link graphics_run

graphics_compile:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -std=c++23 -O2 -Wall -c "E:/C++/Projects/NEAT/main.cpp" -I"E:/programming_tools/SFML-2.6.1/include" -I"E:/C++/Projects/NEAT/include" -I"E:/C++/Tools/Custom_Libraries" -DSFML_STATIC

graphics_link:
	E:/programming_tools/msys64/mingw64/bin/g++.exe -o main "E:/C++/Projects/NEAT/main.o" -L"E:\programming_tools\SFML-2.6.1\lib" -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -lsfml-main-d -lopengl32 -lfreetype -lwinmm -lgdi32

graphics_run:
	.\main