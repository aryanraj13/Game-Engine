all:
	g++ -I src/include -L src/lib -o le level_editor.cpp -lmingw32 -lSDL2main -lSDL2