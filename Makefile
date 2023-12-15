all:
	g++ -I src/include -L src/lib -o main main_engine.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf
