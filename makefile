all: compile_and_link run

compile_and_link:
	E:/programming_tools/msys64/ucrt64/bin/g++.exe -std=gnu++23 -O2 -Wall -o main "E:\C++\Projects\NEAT\main.cpp" -I"E:\programming_tools\SFML-2.5.1\include" -I"E:\C++\Projects\NEAT\include" -I"E:/C++/Tools/Custom_Libraries" -DSFML_STATIC -L"E:\programming_tools\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -lsfml-main

run:
	.\main

clean:
	rm main.exe
	rm evolutionResults.txt

debug:
	E:/programming_tools/msys64/ucrt64/bin/g++.exe -g -std=gnu++23 -O2 -Wall -o main "E:\C++\Projects\NEAT\main.cpp" -I"E:\programming_tools\SFML-2.5.1\include" -I"E:\C++\Projects\NEAT\include" -I"E:/C++/Tools/Custom_Libraries" -DSFML_STATIC -L"E:\programming_tools\SFML-2.5.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -lsfml-main
