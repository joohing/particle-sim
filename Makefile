all:
	gcc src/*.c -Wall `sdl2-config --cflags --libs` -D_THREAD_SAFE -lm -o sim -g
