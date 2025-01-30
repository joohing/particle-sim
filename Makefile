all:
	gcc src/*.c -Wall -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -L/opt/homebrew/lib -lSDL2 -o sim
